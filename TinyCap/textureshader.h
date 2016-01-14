#ifndef _H_TEXTURESHADER
#define _H_TEXTURESHADER

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include "tcmath.h"



class TextureShader {

private:
	struct matrix_buffer_t {
		DirectX::XMMATRIX world;
		//DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

public:
	TextureShader();
	TextureShader(ID3D11Device *, ID3D11DeviceContext *);
	~TextureShader();

	void Render(ID3D11DeviceContext*, const DirectX::XMMATRIX &, const DirectX::XMMATRIX &, ID3D11ShaderResourceView *);


private:
	bool CompileShaders();
	bool CreateShaders(ID3D11Device *);
	bool MakeBuffers(ID3D11Device *, ID3D11DeviceContext *);
	bool SetParameters(ID3D11DeviceContext *, const DirectX::XMMATRIX &, const DirectX::XMMATRIX &, ID3D11ShaderResourceView *);

private:
	ID3DBlob *m_vsBlob;
	ID3DBlob *m_psBlob;

	ID3D11VertexShader *m_VertexShader;
	ID3D11PixelShader *m_PixelShader;
	ID3D11InputLayout *m_VertexLayout;
	ID3D11Buffer *m_MatrixBuffer;
	ID3D11SamplerState *m_SamplerState;
};
#endif