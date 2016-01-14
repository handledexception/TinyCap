#ifndef _H_SCENE
#define _H_SCENE

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "tcmath.h"
#include "textureshader.h"
#include "util.h"

class Scene {

private:
	struct vertex_t {
		vec3f pos;
		vec2f tex;
	};

public:
	Scene();
	Scene(ID3D11Device *, ID3D11DeviceContext *);
	~Scene();

	void Render();
	bool UpdateVertexBuffer(int, int, int, int);

private:
	bool InitVertexBuffer();

private:
	uint32_t m_ScreenWidth;
	uint32_t m_ScreenHeight;
	uint32_t m_PixelWidth;
	uint32_t m_PixelHeight;

	ID3D11InputLayout *m_VertexLayout;
	ID3D11Buffer *m_VertexBuffer;

	ID3D11Device *m_D3D11Device;
	ID3D11DeviceContext *m_D3D11Context;
};

#endif
