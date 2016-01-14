#include <Windows.h>

#include "main.h"
#include "appsystem.h"

// WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{	
	AppSystem *sys = new AppSystem();
	if (!sys) {
		return 0;
	}

	if (sys->Init()) {
		sys->Refresh();
	}

	sys->Shutdown();
	delete sys;	

	return 0;
}
