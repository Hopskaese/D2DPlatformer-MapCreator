#pragma once
#include <windows.h>

class Control {
public:
	HWND m_hWnd;
	int m_bType;
	Control(HWND hWnd, int bType) { m_hWnd = hWnd, m_bType = bType; }
};