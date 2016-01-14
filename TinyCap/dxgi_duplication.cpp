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

bool DXGIDuplication::GetFrame()
{
	HRESULT hr;
	IDXGIResource *desktopResource = nullptr;
	DXGI_OUTDUPL_FRAME_INFO frameInfo;
	ZeroMemory(&frameInfo, sizeof(DXGI_OUTDUPL_FRAME_INFO));
	//if (desktopResource) { desktopResource->Release(); desktopResource = nullptr; }
	hr = m_DesktopDuplication->AcquireNextFrame(500, &frameInfo, &desktopResource);
	if (FAILED(hr)) {
		DebugOut("IDXGIOutputDuplication::AcquireNextFrame failed!\n");
		return false;
	}

	/* if (m_AcquiredDesktopImage) {
		m_AcquiredDesktopImage->Release();
		m_AcquiredDesktopImage = nullptr;
	} */

	hr = desktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&m_AcquiredDesktopImage));
	desktopResource->Release();
	desktopResource = nullptr;
	if (FAILED(hr)) {
		DebugOut("Failed to QueryInterface for ID3D11Texture2D in DXGI Output Duplication!\n");
		return false;
	}

	return true;
}

ID3D11Texture2D *DXGIDuplication::GetTexture()
{
	return m_AcquiredDesktopImage;
}