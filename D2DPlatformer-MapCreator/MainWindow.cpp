#include "MainWindow.h"

GameController* MainWindow::m_pGameController;

MainWindow::MainWindow(float fWidth, float fHeight) : Window(fWidth, fHeight)
{
	bBlocked = false;
}

LRESULT CALLBACK MainWindow::WindowProcMain(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return m_pGameController->MsgProc(hWnd, uMsg, wParam, lParam);

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

DWORD MainWindow::StaticThreadStart(LPVOID lParam)
{
	MainWindow *pMainWindow = (MainWindow*)lParam;
	return pMainWindow->Init();
}

void MainWindow::StartThread()
{
	DWORD ThreadId;
	HANDLE hThread = CreateThread(NULL, 0, StaticThreadStart, (void*) this, 0, &ThreadId);

	WaitForSingleObject(hThread, 1000);
}

DWORD MainWindow::Init()
{
	WNDCLASSEX windowClass;
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.lpfnWndProc = WindowProcMain;
	windowClass.lpszClassName = "MainWindow";
	windowClass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClassEx(&windowClass);

	RECT rect = { 0, 0, m_fWidth, m_fHeight };
	
	AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW);
	
	float width = rect.right - rect.left;
	float height = rect.bottom - rect.top;
		
	m_hWindow = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, "MainWindow", "MapBuilder", WS_OVERLAPPEDWINDOW, 200, 100, 1600,798 , NULL, NULL, GetModuleHandle(NULL), 0);
	if (!m_hWindow)
	{
		printf("Error creating window\n");
		return 1;
	}
	
	m_pGameController = new GameController(this);

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	ShowWindow(m_hWindow, SW_SHOW);

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, m_hWindow, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (!bBlocked)
			{
				m_pGameController->Update();
				m_pGameController->Render();
			}
		}
	}

	delete m_pGameController;
	return 0;
}


