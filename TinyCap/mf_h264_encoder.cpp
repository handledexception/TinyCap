#include "mf_h264_encoder.h"
#include "util.h"

unsigned long pixelbuf[640 * 360];

static int counter = 0;

MF_H264_Encoder::MF_H264_Encoder(ID3D11Device *device, ID3D11DeviceContext *ctx)
:
m_SinkWriter(0), m_DestTexture(0)
{ 
	m_D3D11Device = device;
	m_D3D11Context = ctx;
	m_StreamIndex = 0;
	m_TimeStamp = 0;
	m_DestTexture = nullptr;
};

MF_H264_Encoder::~MF_H264_Encoder()
{ 
	if (m_SinkWriter) { m_SinkWriter->Release(); m_SinkWriter = nullptr; }
};

bool MF_H264_Encoder::Init()
{	
	HRESULT hr = MFStartup(MF_VERSION);
	m_SinkWriter = nullptr;
	
	// fill pixel buffer
	//for (unsigned long i = 0; i < (640 * 360); i++) {
	//	pixelbuf[i] = 0x00FF0000;
	//}

	if (InitSinkWriter()) {
		return true;
	}
	else { return false; }
};

bool MF_H264_Encoder::InitSinkWriter()
{
	IMFMediaType *pMediaTypeOut = nullptr;
	IMFMediaType *pMediaTypeIn = nullptr;

	HRESULT hr = MFCreateSinkWriterFromURL(L"test.mp4", nullptr, nullptr, &m_SinkWriter);
	hr = MFCreateMediaType(&pMediaTypeOut);
	hr = pMediaTypeOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
	hr = pMediaTypeOut->SetGUID(MF_MT_SUBTYPE, VIDEO_ENCODING_FORMAT);
	hr = pMediaTypeOut->SetUINT32(MF_MT_MPEG2_PROFILE, eAVEncH264VProfile_Main);
	hr = pMediaTypeOut->SetUINT32(MF_MT_MPEG2_LEVEL, eAVEncH264VLevel4_1);
	hr = pMediaTypeOut->SetUINT32(MF_MT_AVG_BITRATE, VIDEO_BIT_RATE);
	hr = pMediaTypeOut->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
	hr = MFSetAttributeSize(pMediaTypeOut, MF_MT_FRAME_SIZE, ENCODED_VIDEO_WIDTH, ENCODED_VIDEO_HEIGHT);
	hr = MFSetAttributeRatio(pMediaTypeOut, MF_MT_FRAME_RATE, VIDEO_FPS_NUMERATOR, VIDEO_FPS_DENOMINATOR);
	hr = MFSetAttributeRatio(pMediaTypeOut, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
	hr = m_SinkWriter->AddStream(pMediaTypeOut, &m_StreamIndex);

	hr = MFCreateMediaType(&pMediaTypeIn);
	hr = pMediaTypeIn->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
	hr = pMediaTypeIn->SetGUID(MF_MT_SUBTYPE, VIDEO_INPUT_FORMAT);
	hr = pMediaTypeIn->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
	hr = MFSetAttributeSize(pMediaTypeIn, MF_MT_FRAME_SIZE, ENCODED_VIDEO_WIDTH, ENCODED_VIDEO_HEIGHT);
	hr = MFSetAttributeRatio(pMediaTypeIn, MF_MT_FRAME_RATE, VIDEO_FPS_NUMERATOR, VIDEO_FPS_DENOMINATOR);
	hr = MFSetAttributeRatio(pMediaTypeIn, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
	hr = m_SinkWriter->SetInputMediaType(m_StreamIndex, pMediaTypeIn, nullptr);

	hr = m_SinkWriter->BeginWriting();
	m_SinkWriter->AddRef();

	pMediaTypeOut->Release();
	pMediaTypeIn->Release();

	return true;
}

void MF_H264_Encoder::WriteFrame(ID3D11Texture2D *videoSurface)
{
	HRESULT hr;
	IMFSample *pSample = nullptr;
	IMFMediaBuffer *pMediaBuffer = nullptr;	
	IMF2DBuffer2 *pBuffer2D = nullptr; 
	DWORD length;
	
	D3D11_TEXTURE2D_DESC srcDesc;
	D3D11_TEXTURE2D_DESC destDesc;
	m_DestTexture = nullptr;

	if (videoSurface) {
		videoSurface->GetDesc(&srcDesc);

		ZeroMemory(&destDesc, sizeof(destDesc));
		destDesc.ArraySize = 1;
		destDesc.BindFlags = 0;
		destDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		destDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		destDesc.Height = srcDesc.Height;
		destDesc.MipLevels = 1;
		destDesc.MiscFlags = 0;
		destDesc.SampleDesc = srcDesc.SampleDesc;
		destDesc.Usage = D3D11_USAGE_STAGING;
		destDesc.Width = srcDesc.Width;

		m_D3D11Device->CreateTexture2D(&destDesc, nullptr, &m_DestTexture);
		m_D3D11Context->CopyResource(m_DestTexture, videoSurface);



		hr = MFCreateDXGISurfaceBuffer(__uuidof(ID3D11Texture2D), m_DestTexture, 0, false, &pMediaBuffer);
		hr = pMediaBuffer->QueryInterface(__uuidof(IMF2DBuffer), reinterpret_cast<void **>(&pBuffer2D));
		hr = pBuffer2D->GetContiguousLength(&length);
		hr = pMediaBuffer->SetCurrentLength(length);

		hr = MFCreateVideoSampleFromSurface(nullptr, &pSample);
		//hr = MFCreateSample(&pSample);	
		hr = pSample->AddBuffer(pMediaBuffer);
		hr = pSample->SetSampleTime(m_TimeStamp);
		hr = pSample->SetSampleDuration(VIDEO_FRAME_DURATION);

		hr = m_SinkWriter->WriteSample(m_StreamIndex, pSample);

		m_TimeStamp += VIDEO_FRAME_DURATION;

		// relinquish and null out objects
		pSample->Release();			pSample = nullptr;
		pBuffer2D->Release();		pBuffer2D = nullptr;
		pMediaBuffer->Release();	pMediaBuffer = nullptr;
		m_DestTexture->Release();	m_DestTexture = nullptr;

		counter++;
		DebugOut("Wrote %d MF frame(s)!\n", counter);
	}
	
	
	/* BYTE *pData = nullptr;
	const LONG cbWidth = 4 * 640;
	const DWORD cbBuffer = cbWidth * 360;

	hr = MFCreateMemoryBuffer(cbBuffer, &pMediaBuffer);
	hr = pMediaBuffer->Lock(&pData, nullptr, nullptr);
	hr = MFCopyImage(pData, cbWidth, (BYTE *)pixelbuf, cbWidth, cbWidth, 360);
	if (pMediaBuffer) {
		pMediaBuffer->Unlock();
	}
	hr = pMediaBuffer->SetCurrentLength(cbBuffer);
	hr = MFCreateSample(&pSample);
	hr = pSample->AddBuffer(pMediaBuffer);
	hr = pSample->SetSampleTime(m_TimeStamp);
	hr = pSample->SetSampleDuration(VIDEO_FRAME_DURATION);
	hr = m_SinkWriter->WriteSample(m_StreamIndex, pSample);
	m_TimeStamp += VIDEO_FRAME_DURATION;
	pSample->Release();
	pMediaBuffer->Release(); */
}

void MF_H264_Encoder::Shutdown()
{ 
	m_SinkWriter->Finalize();
	MFShutdown();	
}
