#ifndef _H_DXGI_DUPLICATION
#define _H_DXGI_DUPLICATION

#include "rendercore.h"

class DXGIDuplication {
public:
	DXGIDuplication();
	~DXGIDuplication();

	bool Init(IDXGIOutput1 *, ID3D11Device *);

private:
	IDXGIOutputDuplication *m_DesktopDuplication;
};
#endif