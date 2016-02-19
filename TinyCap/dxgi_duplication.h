#ifndef _H_DXGI_DUPLICATION
#define _H_DXGI_DUPLICATION

#include "rendercore.h"

//todo: make this operate on its own thread
class DXGIDuplication {

public:
	DXGIDuplication();
	~DXGIDuplication();

	bool Init(int adapterNum, int outputNum, std::vector<IDXGIAdapter1*> *dxgiAdapters, ID3D11Device *device);
	bool GetFrame();
	bool FinishFrame();
	ID3D11Texture2D *GetTexture();	

private:	
	IDXGIOutputDuplication *m_DesktopDuplication;
	IDXGIResource *m_DesktopResource;
	ID3D11Texture2D *m_AcquiredDesktopImage;
};

#endif