#pragma once
#include <Windows.h>
#include <stdio.h>

class Window
{
protected:
	HWND m_hWindow;
	float m_fWidth;
	float m_fHeight;
public:
	Window(float fWidth, float fHeight) { m_fWidth = fWidth, m_fHeight = fHeight; }
	~Window() {}

	float GetWidth() { return m_fWidth; }
	float GetHeight() { return m_fHeight; }

	HWND GetHwnd() { return m_hWindow; }
};
