#ifndef _H_SYSTEM
#define _H_SYSTEM

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#include "rendercore.h"

#define SYSTEM_VIDEO_WIDTH 1280
#define SYSTEM_VIDEO_HEIGHT 720

#define SYSTEM_APP_NAME L"TinyCap"

class AppSystem
{
public:
	AppSystem();
	//AppSystem(const AppSystem &);
	~AppSystem();

	bool Init();
	void Refresh();
	void Shutdown();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int, int );
	void ShutdownWindows();

private:
	LPWSTR m_AppName;
	int m_AppWidth, m_AppHeight;
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	RenderCore *m_RenderCore;
};

// globals
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static AppSystem *g_ApplicationHandle = 0;


#endif