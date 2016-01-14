#ifndef _H_BITMAP
#define _H_BITMAP

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "texture.h"

struct vec2 {
	float x;
	float y;
};

class Bitmap
{
private:
	struct vertex_t {
		vec2 pos, tex;
	};

public:
	Bitmap();
	Bitmap(const Bitmap &);
	~Bitmap();

	bool Init(ID3D11Device *, int, int, WCHAR *, int, int);
	void Shutdown();
	bool Render(ID3D11DeviceContext *, int, int);

	int GetIndexCount();
	ID3D11ShaderResourceView *GetTexture();

private:
	bool InitBuffers(ID3D11Device *);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext *, int, int);
	void RenderBuffers(ID3D11DeviceContext *);

private:
	ID3D11Buffer *m_VertexBuffer, *m_IndexBuffer;
	int m_VertexCount, m_IndexCount;
	int m_PrevPosX, m_PrevPosY;
	Texture *m_Texture;
};
#endif