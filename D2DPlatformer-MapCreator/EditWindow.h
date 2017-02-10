#pragma once

#include "Window.h"

class EditController;

class EditWindow : public Window {
public:
	static EditController* m_pEditController;

	EditWindow(float fWidth, float fHeight);
	~EditWindow() {}

	static LRESULT CALLBACK EditWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	DWORD Init();
};
