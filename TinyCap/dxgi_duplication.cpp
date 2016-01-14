#include "dxgi_duplication.h"
#include "util.h"

DXGIDuplication::DXGIDuplication() { };

DXGIDuplication::~DXGIDuplication() { };

bool DXGIDuplication::Init(IDXGIOutput1 *output, ID3D11Device *device)
{
	HRESULT hr;
	
	hr = output->DuplicateOutput(device, &m_DesktopDuplication);
	if (FAILED(hr)) {
		DebugOut("DXGI Desktop Duplication not supported on this system.\n");
		output->Release();

		return false;
	}

	

	return true;
};