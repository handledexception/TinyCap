#ifndef _H_TEXTURE
#define _H_TEXTURE

#include <Windows.h>
#include <d3d11.h>

class Texture
{
public:
	Texture();
	Texture(const Texture &);
	~Texture();

	bool Init(ID3D11Device *, WCHAR *);
	void Shutdown();
	
	ID3D11ShaderResourceView *GetTexture();

private:
	unsigned char *m_TextureData;
	ID3D11Texture2D *m_Texture;
	ID3D11ShaderResourceView *m_TextureView;
};

#endif