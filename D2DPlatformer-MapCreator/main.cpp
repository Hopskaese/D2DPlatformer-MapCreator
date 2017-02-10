#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <time.h>
#include "EditWindow.h"
#include <stdio.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmd, int nCmdShow)
{
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);

	EditWindow* pEditWindow = new EditWindow(500, 700);
	pEditWindow->Init();
	
	return 0;
}

