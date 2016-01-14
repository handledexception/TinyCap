#ifndef _H_DXGI_DUPLICATION
#define _H_DXGI_DUPLICATION

#include "rendercore.h"

class DXGIDuplication {
public:
	DXGIDuplication();
	~DXGIDuplication();

	bool Init(IDXGIOutput1 *, ID3D11Device *);
	bool GetFrame();
	bool FinishFrame();
	ID3D11Texture2D *GetTexture();

private:
	IDXGIOutputDuplication *m_DesktopDuplication;
	ID3D11Texture2D *m_AcquiredDesktopImage;
};

#endif