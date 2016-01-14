#include "textureshader.h"
#include "util.h"

TextureShader::TextureShader() { };

TextureShader::TextureShader(ID3D11Device *device, ID3D11DeviceContext *context)
{
	if (!CompileShaders()) {
		DebugOut("TextureShader::CompileShaders failed!\n");
		return;
	}

	if (!CreateShaders(device)) {
		DebugOut("TextureShader::CreateShaders failed!\n");
		return;
	}

	if (!MakeBuffers(device, context)) {
		DebugOut("TextureShader::MakeBuffers failed!\n");
		return;
	}
};

TextureShader::~TextureShader() { };

void TextureShader::Render(ID3D11DeviceContext *context, const DirectX::XMMATRIX &world, const DirectX::XMMATRIX &projection, ID3D11ShaderResourceView *texture)
{	
	SetParameters(context, world, projection, texture);

	context->IASetInputLayout(m_VertexLayout);
	context->VSSetShader(m_VertexShader, NULL, 0);
	context->PSSetShader(m_PixelShader, NULL, 0);
	context->PSSetSamplers(0, 1, &m_SamplerState);
	context->Draw(6, 0); // always drawing two triangles, can update this to DrawIndexed eventually
};

bool TextureShader::CompileShaders()
{
	HRESULT hr;
	ID3DBlob *errorsBlob = NULL;
	
	m_vsBlob = NULL;
	m_psBlob = NULL;

	// compile vertex shader from file
	hr = D3DCompileFromFile(L"simple.vs", NULL, NULL, "TextureVertexShader", "vs_4_0", D3DCOMPILE_DEBUG, 0, &m_vsBlob, &errorsBlob);
	if (FAILED(hr)) {
		DebugOut("Vertex Shader compilation failed!\n");
		
		if (errorsBlob != NULL) {
			DebugOut("ID3DBlob error: %s\n", (char *)errorsBlob->GetBufferPointer());
			errorsBlob->Release();
			errorsBlob = NULL;
		}

		return false;
	}

	// compile pixel shader	from file
	hr = D3DCompileFromFile(L"simple.ps", NULL, NULL, "TexturePixelShader", "ps_4_0", D3DCOMPILE_DEBUG, 0, &m_psBlob, &errorsBlob);
	if (FAILED(hr)) {
		DebugOut("Pixel shader compilation failed!\n");
		if (errorsBlob != NULL) {
			DebugOut("ID3DBlob error: %s\n", (char *)errorsBlob->GetBufferPointer());
			errorsBlob->Release();
			errorsBlob = NULL;
		}

		return false;
	}

	return true;
};

bool TextureShader::CreateShaders(ID3D11Device *device)
{
	HRESULT hr;

	// create vertex shader object from compiled shader code
	hr = device->CreateVertexShader(m_vsBlob->GetBufferPointer(), m_vsBlob->GetBufferSize(), NULL, &m_VertexShader);
	if (FAILED(hr)) {
		DebugOut("ID3D11Device::CreateVertexShader failed!\n");
		m_vsBlob->Release();
		m_psBlob = NULL;

		return false;
	}

	// create pixel shader
	hr = device->CreatePixelShader(m_psBlob->GetBufferPointer(), m_psBlob->GetBufferSize(), NULL, &m_PixelShader);	
	if (FAILED(hr)) {
		DebugOut("ID3D11Device::CreatePixelShader failed!\n");
		m_psBlob->Release();
		m_psBlob = NULL;

		return false;
	}

	return true;
};

bool TextureShader::MakeBuffers(ID3D11Device *device, ID3D11DeviceContext *context)
{
	HRESULT hr;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	// create input layout
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	uint32_t layoutElems = ARRAYSIZE(layout);

	hr = device->CreateInputLayout(layout, layoutElems, m_vsBlob->GetBufferPointer(), m_vsBlob->GetBufferSize(), &m_VertexLayout);	
	if (FAILED(hr)) {
		DebugOut("ID3D11Device::CreateInputLayout failed!\n");
		return false;
	}
	
	m_vsBlob->Release();
	m_vsBlob = NULL;

	m_psBlob->Release();
	m_psBlob = NULL;

	// setup description for dynamic matrix constant buffer in the vertex shader
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(matrix_buffer_t);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	hr = device->CreateBuffer(&matrixBufferDesc, NULL, &m_MatrixBuffer);
	if (FAILED(hr)) {
		DebugOut("ID3D11Device::CreateBuffer failed when attempting to create Matrix Constant Buffer!\n");
		return false;
	}

	// create texture sampler state description
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = device->CreateSamplerState(&samplerDesc, &m_SamplerState);
	if (FAILED(hr)) {
		DebugOut("ID3D11Device::CreateSamplerState failed!\n");
		return false;
	}

	return true;
};

bool TextureShader::SetParameters(ID3D11DeviceContext *context, const DirectX::XMMATRIX &world, const DirectX::XMMATRIX &projection, ID3D11ShaderResourceView *texture)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	matrix_buffer_t *matrixData;
	unsigned int bufferNumber;

	DirectX::XMMATRIX tmpWorld;
	//DirectX::XMMATRIX tmpView;
	DirectX::XMMATRIX tmpProjection;

	// transpose the matrices to prepare them for the shader.
	tmpWorld = DirectX::XMMatrixTranspose(world);
	//tmpView = DirectX::XMMatrixTranspose(view);
	tmpProjection = DirectX::XMMatrixTranspose(projection);

	// lock the matrix constant buffer for writing
	hr = context->Map(m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr)) {
		DebugOut("Failed to lock CBuffer for writing!\n");
		return false;
	}

	// get a pointer to data in matrix constant buffer
	matrixData = (matrix_buffer_t *)mappedResource.pData;
	
	matrixData->world = tmpWorld;
	//matrixData->view = tmpView;
	matrixData->projection = tmpProjection;

	context->Unmap(m_MatrixBuffer, 0);

	bufferNumber = 0;
		
	context->VSSetConstantBuffers(bufferNumber, 1, &m_MatrixBuffer);
	context->PSSetShaderResources(0, 1, &texture);

	return true;
};