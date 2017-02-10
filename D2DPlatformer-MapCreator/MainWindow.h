#pragma once

#include "GameController.h"
#include "Window.h"

class MainWindow : public Window
{
public:
	static GameController* m_pGameController;

	bool bBlocked;

	MainWindow(float fWidth, float fHeight);
	~MainWindow() {}

	static LRESULT CALLBACK WindowProcMain(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lparam);
	static DWORD WINAPI StaticThreadStart(LPVOID lParam);
	void StartThread();

	DWORD Init();
};
