#include "texture.h"
#include "util.h"

Texture::Texture() : m_Texture(0), m_TextureView(0) { };

Texture::~Texture() { };

bool Texture::Init(ID3D11Device *device, ID3D11DeviceContext *context, ID3D11Texture2D *texture)
{
	HRESULT hr;
	//D3D11_TEXTURE2D_DESC copyDesc;
	D3D11_TEXTURE2D_DESC frameDesc;
	texture->GetDesc(&frameDesc);
		
	/*
	ZeroMemory(&copyDesc, sizeof(D3D11_TEXTURE2D_DESC));
	copyDesc.Width = frameDesc.Width;
	copyDesc.Height = frameDesc.Height;
	copyDesc.Format = frameDesc.Format;
	copyDesc.MipLevels = frameDesc.MipLevels;
	copyDesc.ArraySize = frameDesc.ArraySize;
	copyDesc.Usage = D3D11_USAGE_DEFAULT;
	copyDesc.SampleDesc = frameDesc.SampleDesc;
	copyDesc.CPUAccessFlags = 0;
	copyDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	
	hr = device->CreateTexture2D(&copyDesc, nullptr, &m_Texture); */

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderDesc;
	shaderDesc.Format = frameDesc.Format;
	shaderDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderDesc.Texture2D.MostDetailedMip = frameDesc.MipLevels - 1;
	shaderDesc.Texture2D.MipLevels = frameDesc.MipLevels;

	hr = device->CreateShaderResourceView(texture, &shaderDesc, &m_TextureView);
	if (FAILED(hr)) {
		DebugOut("ID3D11Device::CreateShaderResourceView from ID3D11Texture2D failed!\n");
		return false;
	}
	
	return true;
};

ID3D11ShaderResourceView *Texture::GetSRV()
{
	return m_TextureView;
};

ID3D11Texture2D *Texture::GetTexture2D()
{
	return m_Texture;
};