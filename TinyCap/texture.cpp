#include "texture.h"

Texture::Texture() { m_Texture = 0; };
Texture::Texture(const Texture &other) { };
Texture::~Texture() { };

bool Texture::Init(ID3D11Device *device, WCHAR *filename)
{
	HRESULT hr;

	hr = D3D11CreateShader
};