#include "dxgi_duplication.h"
#include "util.h"

DXGIDuplication::DXGIDuplication()
: 
m_AcquiredDesktopImage(0), m_DesktopResource(0), m_DesktopDuplication(0) 
{
	m_AcquiredDesktopImage = nullptr;
	m_DesktopResource = nullptr;
	m_DesktopDuplication = nullptr;
};

DXGIDuplication::~DXGIDuplication() 
{
	if (m_AcquiredDesktopImage) { m_AcquiredDesktopImage->Release(); m_AcquiredDesktopImage = nullptr; }
	if (m_DesktopResource) { m_DesktopResource->Release(); m_DesktopResource = nullptr; }
	if (m_DesktopDuplication) { m_DesktopDuplication->Release(); m_DesktopDuplication = nullptr; }	
};

bool DXGIDuplication::Init(IDXGIOutput1 *output, ID3D11Device *device)
{
	HRESULT hr;

	// start DXGI Desktop Duplication API
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
	DXGI_OUTDUPL_FRAME_INFO frameInfo;

	m_DesktopResource = nullptr;

	// Get frame from Desktop Duplication API
	ZeroMemory(&frameInfo, sizeof(frameInfo));
	hr = m_DesktopDuplication->AcquireNextFrame(500, &frameInfo, &m_DesktopResource);
	// bail out if the timeout for AcquireNextFrame is reached
	if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
		_com_error err(hr);
		const wchar_t *errorString = err.ErrorMessage();
		DebugOutW(L"IDXGIOutputDuplication::AcquireNextFrame failed! %s\n", errorString);

		return false;
	}	

	// release any existing ID3D11Texture2D
	if (m_AcquiredDesktopImage) {
		m_AcquiredDesktopImage->Release();
		m_AcquiredDesktopImage = nullptr;
	}

	// Query DXGIResource for ID3D11Texture2D interface
	hr = m_DesktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&m_AcquiredDesktopImage));	
	if (FAILED(hr)) {
		DebugOut("Failed to QueryInterface for ID3D11Texture2D in DXGI Output Duplication!\n");
		return false;
	}
	if (m_DesktopResource) {
		m_DesktopResource->Release();
		m_DesktopResource = nullptr;
	}

	return true;
}

bool DXGIDuplication::FinishFrame()
{
	HRESULT hr;

	hr = m_DesktopDuplication->ReleaseFrame();
	if (FAILED(hr)) {
		DebugOut("IDXGIOutputDuplication::ReleaseFrame failed!\n");
		return false;
	}

	if (m_AcquiredDesktopImage) {
		m_AcquiredDesktopImage->Release();
		m_AcquiredDesktopImage = nullptr;
	}

	return true;
};


ID3D11Texture2D *DXGIDuplication::GetTexture()
{
	return m_AcquiredDesktopImage;
}