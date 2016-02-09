#include "mf_h264_encoder.h"

unsigned long pixelbuf[640 * 360];


MF_H264_Encoder::MF_H264_Encoder(ID3D11Device *device, ID3D11DeviceContext *ctx)
:
m_SinkWriter(0)
{ 
	m_D3D11Device = device;
	m_D3D11Context = ctx;
	m_StreamIndex = 0;
	m_TimeStamp = 0;
};

MF_H264_Encoder::~MF_H264_Encoder()
{ 
	if (m_SinkWriter) { m_SinkWriter->Release(); m_SinkWriter = nullptr; }
};

bool MF_H264_Encoder::Init()
{
	HRESULT hr;

	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	
	hr = MFStartup(MF_VERSION);
	m_SinkWriter = NULL;
	
	// fill pixel buffer
	for (unsigned long i = 0; i < (640 * 360); i++) {
		pixelbuf[i] = 0x00FF0000;
	}

	if (InitSinkWriter()) {
		return true;
	}
	else { return false; }
};

bool MF_H264_Encoder::InitSinkWriter()
{
	//IMFSinkWriter *pSinkWriter = NULL;
	IMFMediaType *pMediaTypeOut = NULL;
	IMFMediaType *pMediaTypeIn = NULL;

	HRESULT hr = MFCreateSinkWriterFromURL(L"test.mp4", NULL, NULL, &m_SinkWriter);
	hr = MFCreateMediaType(&pMediaTypeOut);
	hr = pMediaTypeOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
	hr = pMediaTypeOut->SetGUID(MF_MT_SUBTYPE, VIDEO_ENCODING_FORMAT);
	hr = pMediaTypeOut->SetUINT32(MF_MT_MPEG2_PROFILE, eAVEncH264VProfile::eAVEncH264VProfile_Main);
	hr = pMediaTypeOut->SetUINT32(MF_MT_MPEG2_LEVEL, eAVEncH264VLevel::eAVEncH264VLevel4_1);
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
	hr = m_SinkWriter->SetInputMediaType(m_StreamIndex, pMediaTypeIn, NULL);

	hr = m_SinkWriter->BeginWriting();
	m_SinkWriter->AddRef();

	//m_SinkWriter->Release();
	pMediaTypeOut->Release();
	pMediaTypeIn->Release();

	return true;
}

void MF_H264_Encoder::WriteFrame(ID3D11Texture2D *videoSurface)
{
	HRESULT hr;
	IMFSample *pSample = nullptr;
	IMFMediaBuffer *pMediaBuffer = nullptr;	
	IMF2DBuffer *pBuffer2D = nullptr; 
	DWORD length;
	
	D3D11_TEXTURE2D_DESC srcDesc;
	D3D11_TEXTURE2D_DESC destDesc;
	ID3D11Texture2D *destTexture = nullptr;

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
			m_D3D11Device->CreateTexture2D(&destDesc, nullptr, &destTexture);
			m_D3D11Context->CopyResource(destTexture, videoSurface);
			//D3D11_MAPPED_SUBRESOURCE mapResource;
			//m_D3D11Context->Map(videoSurface, 0, D3D11_MAP_READ, 0, &mapResource);
		

		hr = MFCreateDXGISurfaceBuffer(__uuidof(ID3D11Texture2D), destTexture, 0, false, &pMediaBuffer);
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

		destTexture->Release();
		destTexture = nullptr;
		pSample->Release();
		pMediaBuffer->Release();
		pBuffer2D->Release();
	}
	/* BYTE *pData = NULL;
	const LONG cbWidth = 4 * 640;
	const DWORD cbBuffer = cbWidth * 360;

	hr = MFCreateMemoryBuffer(cbBuffer, &pMediaBuffer);
	hr = pMediaBuffer->Lock(&pData, NULL, NULL);
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
	CoUninitialize();
}
