#include "EditWindow.h"
#include "EditController.h"

//combo box http://www.functionx.com/visualc/controls/combobox.htm

EditController* EditWindow::m_pEditController;

EditWindow::EditWindow(float fWidth, float fHeight) : Window(fWidth, fHeight) 
{
	m_pEditController = new EditController(this);
}

LRESULT CALLBACK EditWindow::EditWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return m_pEditController->MsgProc(hWnd, uMsg, wParam, lParam);

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

DWORD EditWindow::Init()
{
	WNDCLASSEX windowClass;
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.lpfnWndProc = EditWindowProc;
	windowClass.lpszClassName = "ControlWindow";
	windowClass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClassEx(&windowClass);

	RECT rect = { 0, 0, m_fWidth, m_fHeight };
	AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW);

	m_hWindow = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, "ControlWindow", "Mapcreator", WS_OVERLAPPEDWINDOW | WS_DLGFRAME, 100, 100, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, GetModuleHandle(NULL), 0);
	if (!m_hWindow)
	{
		printf("Error creating window\n");
		return -1;
	}

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	ShowWindow(m_hWindow, SW_SHOW);

	while(msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, m_hWindow, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return 0;
}