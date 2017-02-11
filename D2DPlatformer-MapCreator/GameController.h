#pragma once

#include <vector>
#include <Windowsx.h>

#include "Graphics.h"
#include "Map.h"
#include <stdarg.h>
#include "HPTimer.h"
#include "Loader.h"
#include "EditController.h"

#define VK_KEY_A 0x41
#define VK_KEY_D 0x44

class DPIScale;

class GameController
{
	Graphics			*m_pGraphics;
	Map					*m_pMap;
	HPTimer				*m_pHPTimer;

	DPIScale            *m_pDPIScale;

	Player				*m_pMainPlayer;

	bool				m_bActive;

	D2D1_POINT_2F		m_D2D1PFLastCenter;

	EditController      *m_pEditController;

	MainWindow          *m_pMainWindow;

	Brick*				m_pCurrentlySelected;
	
	int					OldPosX;
	int					OldPosY;
	bool				isDragging;

public:

	GameController(MainWindow* pMainWindow);
	~GameController();

	LRESULT MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void EditControllerMessageHandler(int bType, ...);

	void OnKeyDown(WPARAM wParam);
	void OnKeyUp(WPARAM wParam);

	void Update();
	void Render();

	void OnJump();
	void OnRunStart(bool bDir);
	void OnRunStop();
	void Move(Player *pPlayer, double dt);
	void Gravity(Player *pPlayer, double dt);

	void OnResize(UINT width, UINT height);
	
	D2D1_POINT_2F GetCenter();

	void Debug();

	bool IsGrahpics() { return m_pGraphics != NULL; }

	//void NewBackground(va_list arguments);
	void NewBrick(va_list arguments);
	void NewDummy(va_list arguments);

	void ChangeBrick(va_list arguments);
	void ChangeBackground(va_list arguments);

	void InsertBrick(Brick* pBrick);
	void InsertDummy(Dummy* pDummy);

	void DeleteObject(va_list arguments);

	void CalculateMouseCoordinates(int *bPosX, int *bPosY);
	void GetSelectedObject(int PosX, int PosY);

	std::wstring CharToWChar(const char* ptr);
};