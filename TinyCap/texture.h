#ifndef _H_TEXTURE
#define _H_TEXTURE

#include <Windows.h>
#include <d3d11.h>

class Texture
{
public:
	Texture();
	~Texture();

	bool Init(ID3D11Device *, ID3D11DeviceContext *, ID3D11Texture2D *);
	void Shutdown();
	
	ID3D11ShaderResourceView *GetSRV();
	ID3D11Texture2D *GetTexture2D();

private:
	unsigned char *m_TextureData;

	ID3D11ShaderResourceView *m_TextureView;
	ID3D11Texture2D *m_Texture;
};

#endif