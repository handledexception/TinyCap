#ifndef _H_RENDERCORE
#define _H_RENDERCORE

/*
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
*/ 

#include <vector>
#include <Windows.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "textureshader.h"

const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class RenderCore
{
public:
	RenderCore();
	//RenderCore(const RenderCore &);
	~RenderCore();

	bool Init(int, int, HWND);
	void Shutdown();
	bool Render();
	void ZBufferState(int);

	IDXGISwapChain *GetSwapChain();
	ID3D11Device *GetDevice();
	ID3D11DeviceContext *GetDeviceContext();	

	//void GetProjectionMatrix(DirectX::XMMATRIX &projectionMatrix);
	//void GetWorldMatrix(DirectX::XMMATRIX &worldMatrix);

private:
	void BeginScene(float, float, float, float);
	void EndScene();

	bool EnumerateDisplayAdapters(std::vector<IDXGIAdapter1*> *);

private:
	IDXGISwapChain *m_SwapChain;
	ID3D11Device *m_d3d11Device;
	ID3D11DeviceContext *m_d3d11DeviceContext;
	ID3D11RenderTargetView *m_d3d11RenderTargetView;
	ID3D11Texture2D *m_d3d11DepthStencilBuffer;
	ID3D11DepthStencilState *m_d3d11DepthStencilState;
	ID3D11DepthStencilState *m_d3d11DepthStencilDisabledState;
	ID3D11DepthStencilView *m_d3d11DepthStencilView;
	ID3D11RasterizerState *m_d3d11RasterState;

	TextureShader *m_TextureShader;

	DirectX::XMMATRIX m_WorldMatrix;
	DirectX::XMMATRIX m_ProjectionMatrix;	
	DirectX::XMMATRIX m_OrthoMatrix;	

	//std::vector<IDXGIAdapter*> m_VideoAdapterList;
	int m_VideoMemory;
	char m_VideoCardDesc[128];
};

static std::vector<IDXGIAdapter1*> gDXGIAdapters;
#endif
