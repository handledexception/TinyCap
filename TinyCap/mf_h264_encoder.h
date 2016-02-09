#ifndef _H_MF_H264_ENCODER
#define _H_MF_H264_ENCODER

#include <Windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <codecapi.h>
#include <Mferror.h>
#include <evr.h>
#include "rendercore.h"

const UINT32 ENCODED_VIDEO_WIDTH = 1920;
const UINT32 ENCODED_VIDEO_HEIGHT = 1080;
const UINT32 VIDEO_FPS_NUMERATOR = 30;
const UINT32 VIDEO_FPS_DENOMINATOR = 1;
const UINT64 VIDEO_FRAME_DURATION = 10 * 1000 * 1000 / VIDEO_FPS_NUMERATOR;
const UINT32 VIDEO_BIT_RATE = 1000000; // 1Mbps
const GUID VIDEO_ENCODING_FORMAT = MFVideoFormat_H264;
const GUID VIDEO_INPUT_FORMAT = MFVideoFormat_ARGB32;

class MF_H264_Encoder {

public:
	MF_H264_Encoder();
	MF_H264_Encoder(ID3D11Device *device, ID3D11DeviceContext *ctx);
	~MF_H264_Encoder();

	bool Init();	
	void WriteFrame(ID3D11Texture2D *videoSurface);
	void Shutdown();
	
	DWORD m_StreamIndex;
	LONGLONG m_TimeStamp;

private:
	bool InitSinkWriter();
	IMFSinkWriter *m_SinkWriter;

private:
	ID3D11Device *m_D3D11Device;
	ID3D11DeviceContext *m_D3D11Context;
	ID3D11Texture2D *m_DestTexture;
	
	
};

#endif