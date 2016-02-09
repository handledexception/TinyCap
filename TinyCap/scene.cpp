#include <Windows.h>
#include <stdio.h>
#include "scene.h"

Scene::Scene()
{	
	m_VertexLayout = nullptr;
	m_VertexBuffer = nullptr;
};

Scene::Scene(ID3D11Device *device, ID3D11DeviceContext *context, int displayWidth, int displayHeight)
{	
	m_ScreenWidth = displayWidth;
	m_ScreenHeight = displayHeight;

	m_VertexLayout = nullptr;
	m_VertexBuffer = nullptr;

	m_D3D11Device = device;
	m_D3D11Context = context;	

	m_SceneTexture = new Texture();
	m_SceneTextureShader = new TextureShader(m_D3D11Device, m_D3D11Context);

	if (!InitVertexBuffer()) {
		DebugOut("Error initializing Vertex Buffer!\n");
		return;
	}
};

Scene::~Scene()
{
	if (m_VertexLayout) { m_VertexLayout->Release(); }
	if (m_VertexBuffer) { m_VertexBuffer->Release(); }

	m_VertexLayout = nullptr;
	m_VertexBuffer = nullptr;
};


void Scene::Render(ID3D11Texture2D *texture, const DirectX::XMMATRIX &world, const DirectX::XMMATRIX &projection)
{
	if (m_D3D11Context && m_VertexBuffer) {
		// set vertex buffer
		uint32_t vertexStride = sizeof(vertex_t);
		uint32_t vertexOffset = 0;

		m_D3D11Context->IASetVertexBuffers(0, 1, &m_VertexBuffer, &vertexStride, &vertexOffset);
		m_D3D11Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	
	}
	else {
		DebugOut("Error rendering Scene! No D3D11 Device Context or no Vertex Buffer Created!\n");
	}

	m_SceneTexture->Init(m_D3D11Device, m_D3D11Context, texture);	
	m_SceneTextureShader->Render(m_D3D11Context, world, projection, m_SceneTexture->GetSRV());
	
};

bool Scene::InitVertexBuffer()
{
	HRESULT hr;	
	vertex_t *vertices;

	// some temporary vertex data
	/* vertex3f vertices[] = {		
		{ 0.0f, 1.0f, 0.0f },
		{ 1.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
	}; */	

	vertices = new vertex_t[6];
	memset(vertices, 0, (sizeof(vertex_t) * 6));

	D3D11_BUFFER_DESC vertexBufferDesc;
	memset(&vertexBufferDesc, 0, sizeof(vertexBufferDesc));	
	vertexBufferDesc.ByteWidth = sizeof(vertex_t) * 6;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	memset(&vertexBufferData, 0, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = vertices;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	
	hr = m_D3D11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_VertexBuffer);
	if (FAILED(hr)) {
		DebugOut("ID3D11Device::CreateBuffer failed when attempting to create vertex buffer!\n");

		return false;
	}	

	delete[] vertices;

	return true;
};

bool Scene::UpdateVertexBuffer(int posx, int posy, int width, int height)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;	
	vertex_t *vertices;
	rectf bitmapRect;

	m_PixelWidth = width;
	m_PixelHeight = height;

	bitmapRect.left  = (float)((m_ScreenWidth / 2) * -1.f) + (float)posx;	
	bitmapRect.right = bitmapRect.left + (float)m_PixelWidth;
	bitmapRect.top = (float)(m_ScreenHeight / 2) - (float)posy;
	bitmapRect.bottom = bitmapRect.top - (float)m_PixelHeight;

	vertices = new vertex_t[6];

	// triangle 1
	vertices[0].pos = vec3f(bitmapRect.left, bitmapRect.top, 0.0f); // top left
	//vertices[0].pos = vec3f(-1.0f, 0.0f, 0.0f);
	vertices[0].tex = vec2f(0.0f, 0.0f);

	vertices[1].pos = vec3f(bitmapRect.right, bitmapRect.bottom, 0.0f); // bottom right
	//vertices[1].pos = vec3f(-1.0f, 1.0f, 0.0f);
	vertices[1].tex = vec2f(1.0f, 1.0f);

	vertices[2].pos = vec3f(bitmapRect.left, bitmapRect.bottom, 0.0f); // bottom left
	//vertices[2].pos = vec3f(0.0f, 1.0f, 0.0f);
	vertices[2].tex = vec2f(0.0f, 1.0f);

	//triangle 2
	vertices[3].pos = vec3f(bitmapRect.left, bitmapRect.top, 0.0f); // top left
	//vertices[3].pos = vec3f(-1.0f, 0.0f, 0.0f);
	vertices[3].tex = vec2f(0.0f, 0.0f);

	vertices[4].pos = vec3f(bitmapRect.right, bitmapRect.top, 0.0f); // top right
	//vertices[4].pos = vec3f(0.0f, 1.0f, 0.0f);
	vertices[4].tex = vec2f(1.0f, 0.0f);

	vertices[5].pos = vec3f(bitmapRect.right, bitmapRect.bottom, 0.0f); // bottom right
	//vertices[5].pos = vec3f(0.0f, 0.0f, 0.0f);
	vertices[5].tex = vec2f(1.0f, 1.0f);

	/* vertex3f vertices[] = {
		{ -vx, 0.0f, 0.0f },
		{ -vx, vy, 0.0f },
		{ 0.0f, vy, 0.0f },
		{ -vx, 0.0f, 0.0f },
		{ 0.0f, vy, 0.0f },
		{ 0.0f, 0.0f, 0.0f },
	}; */

	//ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	hr = m_D3D11Context->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr)) {
		DebugOut("Scene::UpdateVertexBuffer failed!\n");
		return false;
	}
	memcpy((vertex_t *)mappedResource.pData, (void *)vertices, (sizeof(vertex_t) * 6));
	m_D3D11Context->Unmap(m_VertexBuffer, 0);	

	delete[] vertices;

	return true;
};