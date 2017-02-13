#pragma once
#include <algorithm>  
#include <vector>
#include <windows.h>

#include "Common.h"
#include "Control.h"
#include "EditWindow.h"

#define WM_CREATESELECTED (WM_USER + 0x0001)

class MainWindow;
class GameController;
class Brick;
class Dummy;
class Object;
class Background;

class EditController
{
	std::vector<Control*>m_pControls;
	std::vector<Control*>m_pSelectedControls;
	
	EditWindow* m_pEditWindow;

	GameController* m_pGameController;

    std::string src;

	bool isEditModeActive;

	int m_bCurrentCreationMode;
	Object* m_pCurrentlySelected;

public:

	EditController(EditWindow* pEditWindow);
	~EditController();

	LRESULT MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void GameControllerMessageHandler(int bMessage, ...);

	void InitNormalMode();
	void InitEditMode();

	void CreateStartingControls(HWND hWnd);
	void CreateMainControls(HWND hWnd);
	void CreateBackgroundControls(HWND hWnd);
	void CreateBrickControls(HWND hWnd);
	void CreateDummyControls(HWND hWnd);

	void CreateSelectedBrickControls(Brick* pBrick);
	void CreateSelectedBackgroundControls(Background* pBackground);
	void CreateSelectedDummyControls(Dummy* pDummy);

	//void GetBackGroundInput();
	void GetBrickInput(int bType);
	void GetDummyInput();

	void ClearControls();
	void ClearSelectedControls();

	void Browse(HWND hWnd);
	std::wstring CharToWChar(const char* ptr);

	bool IsDigits(const std::string &str)
	{
		return str.find_first_not_of("0123456789+-.") == std::string::npos;
	}
};