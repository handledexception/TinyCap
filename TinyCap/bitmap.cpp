#include "bitmap.h"

Bitmap::Bitmap()
{
	m_VertexBuffer = 0;
	m_IndexBuffer = 0;
	m_Texture = 0;
};

Bitmap::Bitmap(const Bitmap &other) { };
Bitmap::~Bitmap() { };