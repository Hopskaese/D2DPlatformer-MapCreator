#include "GameController.h"
#include "MainWindow.h"
#include <iostream>
#include "DIPScale.h"

GameController::GameController(MainWindow* pMainWindow)
{
	m_pMainWindow = pMainWindow;
	m_pGraphics = new Graphics();
	if (!m_pGraphics->Init(m_pMainWindow->GetHwnd()))
	{
		delete m_pGraphics;
		m_pGraphics = NULL;
	}

	m_pDPIScale = new DPIScale();
	m_pDPIScale->Initialize(m_pGraphics->GetFatory());

	m_pHPTimer = new HPTimer();

	m_bActive = true;

	m_D2D1PFLastCenter = D2D1::Point2F(0, 0);
	
	Loader::Init(m_pGraphics);
	m_pMap = Loader::LoadMap();

	m_pMainPlayer = new Player(200, 200);

	m_pMainPlayer->m_pSprites[Player::PSPRITE_STAND][Player::DIRECTION::LEFT] =	new SpriteSheet(L"data\\player\\PlayerStandLeft.png",	m_pGraphics, 43, 60, 20);
	m_pMainPlayer->m_pSprites[Player::PSPRITE_STAND][Player::DIRECTION::RIGHT] =new SpriteSheet(L"data\\player\\PlayerStandRight.png",	m_pGraphics, 43, 60, 20);
	m_pMainPlayer->m_pSprites[Player::PSPRITE_RUN]	[Player::DIRECTION::LEFT] =	new SpriteSheet(L"data\\player\\PlayerRunLeft.png",		m_pGraphics, 43, 60, 20);
	m_pMainPlayer->m_pSprites[Player::PSPRITE_RUN]	[Player::DIRECTION::RIGHT] =new SpriteSheet(L"data\\player\\PlayerRunRight.png",	m_pGraphics, 43, 60, 20);
	m_pMainPlayer->m_pSprites[Player::PSPRITE_JUMP]	[Player::DIRECTION::LEFT] =	new SpriteSheet(L"data\\player\\PlayerJumpLeft.png",	m_pGraphics, 43, 60, 20, false);
	m_pMainPlayer->m_pSprites[Player::PSPRITE_JUMP]	[Player::DIRECTION::RIGHT] =new SpriteSheet(L"data\\player\\PlayerJumpRight.png",	m_pGraphics, 43, 60, 35, false);
	
	m_pMap->Add(m_pMainPlayer);

	m_pCurrentlySelected = NULL;
	OldPosX = 0;
	OldPosY = 0;
	isDragging = false;
}

GameController::~GameController()
{
	if (m_pGraphics) delete m_pGraphics;
	if (m_pMap)		 delete m_pMap;
	if (m_pHPTimer)  delete m_pHPTimer;
}

LRESULT GameController::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
	case WM_QUIT:
	case WM_CLOSE:
		{
			if(m_pEditController != NULL)
				m_pEditController->GameControllerMessageHandler(MSG_CLOSE);
			
			PostQuitMessage(0);
		    return 0;
		}
		break;
		
	case WM_GETMINMAXINFO:
	{
		MINMAXINFO *info = (MINMAXINFO*)lParam;
		info->ptMaxTrackSize.x = 1400;

		return 0;
	}
	
	case WM_ACTIVATEAPP:
		{
			m_bActive = wParam != 0;
			if (!m_bActive) OnRunStop();
		}
		break;
	case WM_KEYDOWN:
	{
		OnKeyDown(wParam);
	}
	break;
	case WM_KEYUP:
	{
		OnKeyUp(wParam);
	}
	break;
	case WM_SIZE:
		{
			OnResize(LOWORD(lParam), HIWORD(lParam));
		}
		break;
	case WM_MOUSEMOVE:
		{
			if (isDragging && m_pCurrentlySelected != NULL)
			{
				int PosX = GET_X_LPARAM(lParam);
				int PosY = GET_Y_LPARAM(lParam);
				int xChange = 0;
				int yChange = 0;

				CalculateMouseCoordinates(&PosX, &PosY);

				xChange = (OldPosX - PosX) * -1;
				yChange = (OldPosY - PosY) * -1;

				m_pCurrentlySelected->m_fX += xChange;
				m_pCurrentlySelected->m_fY += yChange;

				OldPosX = m_pCurrentlySelected->m_fX;
				OldPosY = m_pCurrentlySelected->m_fY;
			}
		}
	break;
	case WM_LBUTTONDOWN:
		{
			isDragging = true;
			int PosX = GET_X_LPARAM(lParam);
			int PosY = GET_Y_LPARAM(lParam);

			CalculateMouseCoordinates(&PosX, &PosY);
			GetSelectedObject(PosX, PosY);	
		}
		break;
	case WM_LBUTTONUP:
		{
			isDragging = false;
			m_pEditController->GameControllerMessageHandler(MSG_CHANGE);
			m_pCurrentlySelected = NULL;
		}
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void GameController::EditControllerMessageHandler(int bMessage, ...)
{
	va_list arguments;
	va_start(arguments, bMessage);

	switch (bMessage)
	{
	case MSG_CREATE:
	{
		int bType = va_arg(arguments, int);

		/*
		if (bType == OT_BACKGROUND)
			NewBackground(arguments);
			*/
		 if (bType == OT_BRICK)
			NewBrick(arguments);
		else if (bType == OT_DUMMY)
			NewDummy(arguments);
	}
	break;
	case MSG_CHANGE:
	{
		int bType = va_arg(arguments, int);

		if (bType == OT_BACKGROUND)
			ChangeBackground(arguments);
		else if (bType == OT_BRICK)
			ChangeBrick(arguments);
		
	}
	break;
	case MSG_DELETE:
	{
		DeleteObject(arguments);
	}
	break;
	case MSG_SETCONTROLLER:
	{
		EditController* pEditController = va_arg(arguments, EditController*);
		bool isSet = va_arg(arguments, bool);

		if (isSet == false)
		{
			delete pEditController;
			m_pEditController = NULL;
		}
		else
		{
			m_pEditController = pEditController;
		}
	}
	break;
	}
}

void GameController::DeleteObject(va_list arguments)
{
	int bType = va_arg(arguments, int);
	
	if (bType == OT_BRICK)
	{
		Brick* pBrick = va_arg(arguments, Brick*);
		m_pMap->DeleteFromObjects(pBrick);
		m_pMap->DeleteFromBricks(pBrick);

		delete pBrick;
	}
	va_end(arguments);
}

/*
void GameController::NewBackground(va_list arguments)
{
	m_pMainWindow->bBlocked = true;
	std::string src;

	float fHeight = m_pMap->GetBackground()->m_pSpriteSheet->GetHeight();
	float fWidth = m_pMap->GetBackground()->m_pSpriteSheet->GetWidth();

	for (int i = 0; i < 1; i++)
	{
		src = va_arg(arguments, std::string);
		va_end(arguments);
		std::wstring temp = CharToWChar(src.c_str());

		m_pMap->m_objects.erase(m_pMap->m_objects.begin());
		delete 	m_pMap->GetBackground()->m_pSpriteSheet;
		m_pMap->GetBackground()->m_pSpriteSheet = NULL;
	    m_pMap->GetBackground()->m_pSpriteSheet = new SpriteSheet((wchar_t*)temp.c_str(), m_pGraphics, fWidth, fHeight, 0, false);
		m_pMap->m_objects.insert(m_pMap->m_objects.begin(), m_pMap->GetBackground());
	}
	m_pMainWindow->bBlocked = false;
}
*/

void GameController::ChangeBackground(va_list arguments)
{
	m_pMainWindow->bBlocked = true;

	std::string src = va_arg(arguments, std::string);
	va_end(arguments);

	float fHeight = m_pMap->GetBackground()->m_pSpriteSheet->GetHeight();
	float fWidth = m_pMap->GetBackground()->m_pSpriteSheet->GetWidth();

	std::wstring temp = CharToWChar(src.c_str());

	m_pMap->m_objects.erase(m_pMap->m_objects.begin());
	delete 	m_pMap->GetBackground()->m_pSpriteSheet;
	m_pMap->GetBackground()->m_pSpriteSheet = NULL;
	m_pMap->GetBackground()->m_pSpriteSheet = new SpriteSheet((wchar_t*)temp.c_str(), m_pGraphics, fWidth, fHeight, 0, false);
	m_pMap->m_objects.insert(m_pMap->m_objects.begin(), m_pMap->GetBackground());

	m_pMainWindow->bBlocked = false;

}

void GameController::InsertBrick(Brick* pBrick)
{
	// Has to be changed if more than 1 background.
	int InsertPosition = 1 + m_pMap->m_bricks.size();

	m_pMap->m_objects.insert(m_pMap->m_objects.begin()+ InsertPosition, pBrick);
	m_pMap->m_bricks.push_back(pBrick);
}

void GameController::NewBrick(va_list arguments)
{
	printf("New brick\n");
	m_pMainWindow->bBlocked = true;
	std::string src;
	float fX = 0.0f;
	float fY = 0.0f;
	float fWidth = 0.0f;
	float fHeight = 0.0f;
	float fAngle = 0.0f;

	for (int i = 0; i < 6; i++)
	{
		switch (i)
		{
		case 0:
			src = va_arg(arguments, std::string);
			break;
		case 1:
			fX = va_arg(arguments, long);
			break;
		case 2:
			fY = va_arg(arguments, long);
			break;
		case 3:
			fWidth = va_arg(arguments, long);
			break;
		case 4:
			fHeight = va_arg(arguments, long);
			break;
		case 5:
			fAngle = va_arg(arguments, long);
			break;
		}
	}
	va_end(arguments);

	std::wstring temp = CharToWChar(src.c_str());
	Brick* pBrick = new Brick(fX, fY, fWidth, fHeight, fAngle);
	pBrick->m_pSpriteSheet = new SpriteSheet((wchar_t*)temp.c_str(), m_pGraphics);
	printf("new brick stats X [%f] Y [%f]\n", pBrick->m_fX, pBrick->m_fY);
	InsertBrick(pBrick);

	m_pMainWindow->bBlocked = false;
}

void GameController::ChangeBrick(va_list arguments)
{
	m_pMainWindow->bBlocked = true;
	Brick* pBrick = NULL;
	float fX = 0.0f;
	float fY = 0.0f;
	float fWidth = 0.0f;
	float fHeight = 0.0f;
	float fAngle = 0.0f;

	for (int i = 0; i < 6; i++)
	{
		switch (i)
		{
		case 0:
			pBrick = va_arg(arguments, Brick*);
			break;
		case 1:
			fX = va_arg(arguments, long);
			break;
		case 2:
			fY = va_arg(arguments, long);
			break;
		case 3:
			fWidth = va_arg(arguments, long);
			break;
		case 4:
			fHeight = va_arg(arguments, long);
			break;
		case 5:
			fAngle = va_arg(arguments, long);
			break;
		}
	}
	va_end(arguments);

	pBrick->m_fX = fX;
	pBrick->m_fY = fY;
	pBrick->m_fWidth = fWidth;
	pBrick->m_fHeight = fHeight;
	pBrick->m_fAngle = fAngle;

	m_pMainWindow->bBlocked = false;
}

void GameController::InsertDummy(Dummy* pDummy)
{
	m_pMap->m_objects.insert(m_pMap->m_objects.end(), pDummy);
	m_pMap->m_dummies.push_back(pDummy);
}

void GameController::NewDummy(va_list arguments)
{
	m_pMainWindow->bBlocked = true;
	std::string src;
	long fX = 0.0f;
	long fY = 0.0f;

	for (int i = 0; i < 3; i++)
	{
		switch (i)
		{
		case 0:
			src = va_arg(arguments, std::string);
			break;
		case 1:
			fX = va_arg(arguments, long);
			break;
		case 2:
			fY = va_arg(arguments, long);
			break;
		}
	}

	va_end(arguments);

	std::wstring temp = CharToWChar(src.c_str());
	Dummy* pDummy = new Dummy(fX, fY);
	pDummy->m_pSpriteSheet = new SpriteSheet((wchar_t*)temp.c_str(), m_pGraphics);
	m_pMap->Add(pDummy);

	m_pMainWindow->bBlocked = false;
}

void GameController::GetSelectedObject(int PosX, int PosY)
{
	int bType = -1;
	Object* pObject = NULL;
	for (auto &a : m_pMap->m_objects)
	{
		// to add: layer support
		if (Object::instanceof<Background>(a))
		{
			Background *pBackground = NULL;
			pBackground = (Background*)a;

			bType = OT_BACKGROUND;
			pObject = pBackground;

		}
		else if (Object::instanceof<Brick>(a))
		{
			Brick* pBrick = NULL;
			pBrick = (Brick*)a;

			if (pBrick->m_fX <= PosX && PosX <=pBrick->m_fX + pBrick->m_fWidth && pBrick->m_fY <= PosY && PosY <= pBrick->m_fY + pBrick->m_fHeight)
			{
				bType = OT_BRICK;
				pObject = pBrick;

				m_pCurrentlySelected = pBrick;
				OldPosX = pBrick->m_fX;
				OldPosY = pBrick->m_fY;
			}
		}
		/*
		else if (Object::instanceof<Dummy>(a))
		{
			Dummy* pDummy = (Dummy*)a;
		}
		*/
	}

	if (bType != -1 && pObject != NULL)
		m_pEditController->GameControllerMessageHandler(MSG_SELECTEDOBJECT, bType, pObject);
}

void GameController::CalculateMouseCoordinates(int* bPosX, int* bPosY)
{
	Player* pPlayer = m_pMap->m_players[0];
	D2D1_SIZE_F size = m_pGraphics->GetRenderTarget()->GetSize();
	int bMidWidth = size.width / 2;
	int bMidHeight = size.height / 2;
	int bHeight = size.height;
	int bPositionX = 0;
	int bPositionY = 0;

	//Overall I am always trying to get the coordinates of the left border so I can add the mouse coordinates to it.
	//ViewXChange(shifting) starts at size.width /2
	if (pPlayer->m_fX < 0)
	{
		bPositionX = abs(pPlayer->m_fX);
		bPositionX -= bMidWidth;
		//Substracting mid value from positive X-Coordinate results in >0 if no shifting has happened yet
		if (bPositionX > 0)
			bPositionX = *bPosX + (-1400);
		else
		{
			//Shifting has happened. Position currently holds Shift-Amount.
			bPositionX = abs(bPositionX);
			bPositionX += *bPosX;
			bPositionX -= 1400;
		}
	}
	else
	{
		//Get shifting Amount and just add posX
		bPositionX = pPlayer->m_fX;
		//adjust coordinates when shifting stops at the end of map.
		if (bPositionX >= (m_pMap->m_fMaxX - bMidWidth))
		{
			bPositionX -= bPositionX - (m_pMap->m_fMaxX - bMidWidth);
			bPositionX -= bMidWidth;
			bPositionX += *bPosX;
		}
		else
		{
			bPositionX -= bMidWidth;
			bPositionX += *bPosX;
		}
	}

	if (pPlayer->m_fY < 0)
	{
		bPositionY = bMidHeight - pPlayer->m_fY;
		bPositionY = 700 - bPositionY - *bPosY;
	}
	else
	{
		bPositionY = bMidHeight - pPlayer->m_fY;
		if (bPositionY > 0)
		{
			bPositionY = *bPosY - bPositionY;
		}
		else
		{
			bPositionY = *bPosY;
			bPositionY += m_pMap->m_fMaxY - bHeight;
		}

	}
	
	//windowresizing fix.

	*bPosX = bPositionX;
	*bPosY = bPositionY;
}

void GameController::Update()
{
	double dt = m_pHPTimer->GetTimeDelta();

	if (m_pMap)
	{
		for (auto o : m_pMap->m_players)
		{
			Move(o, dt);
			Gravity(o, dt);
		}
	}

	auto POINTCenter = GetCenter();

	if (auto pBackground = m_pMap->GetBackground())
	{
		pBackground->OnViewXChange(m_D2D1PFLastCenter.x - POINTCenter.x);
		pBackground->OnViewYChange(m_D2D1PFLastCenter.y - POINTCenter.y);
	}

	// Save last center coordinates
	m_D2D1PFLastCenter.x = POINTCenter.x;
	m_D2D1PFLastCenter.y = POINTCenter.y;

	m_pGraphics->SetCenter(POINTCenter.x, POINTCenter.y);

	m_pHPTimer->UpdateFramestate();
}

void GameController::Render()
{
	m_pGraphics->BeginDraw();
	m_pGraphics->ClearScreen(255, 255, 255);

	if (m_pMap) 
	{
		for (auto o : m_pMap->m_objects)
			m_pGraphics->Draw(o);
			//o->Draw(m_pGraphics);
	}

	m_pGraphics->ResetTransform();
	Debug();
	m_pGraphics->EndDraw();
}

void GameController::OnRunStart(bool bDir)
{
	if (!m_pMainPlayer) return;

	m_pMainPlayer->m_bDirection = bDir;
	m_pMainPlayer->AddPState(Player::PS_RUN);

	// Send packet
}

void GameController::OnJump()
{
	if (!m_pMainPlayer) return;

	if (!m_pMap->GetGround(m_pMainPlayer)) return;

	m_pMainPlayer->Jump();
}

void GameController::OnRunStop()
{
	if (!m_pMainPlayer) return;

	m_pMainPlayer->SubPState(Player::PS_RUN);
}

void GameController::Move(Player *pPlayer, double dt)
{
	pPlayer->Move(dt);

	if (m_pMainPlayer->m_fX + PLAYER_WIDTH / 2 > m_pMap->m_fMaxX)
		m_pMainPlayer->m_fX = m_pMap->m_fMaxX - PLAYER_WIDTH / 2;

	if (m_pMainPlayer->m_fX - PLAYER_WIDTH / 2 < m_pMap->m_fMinX)
		m_pMainPlayer->m_fX = m_pMap->m_fMinX + PLAYER_WIDTH / 2;
}

void GameController::OnResize(UINT width, UINT height)
{
	D2D1_SIZE_U size;
	size.width = width;
	size.height = height;

	m_pGraphics->GetRenderTarget()->Resize(size);
}

void GameController::OnKeyDown(WPARAM wParam)
{
	if (wParam == VK_KEY_A || wParam == VK_KEY_D)
	{
		if (m_pMainPlayer && m_pMainPlayer->IsPState(Player::PS_RUN)) return;
		OnRunStart(wParam == VK_KEY_D);
	}

	if (wParam == VK_SPACE)
		OnJump();
}

void GameController::OnKeyUp(WPARAM wParam)
{
	if (wParam == VK_KEY_A && !(GetKeyState(VK_KEY_D) & 0x8000))
		OnRunStop();
	else if (wParam == VK_KEY_A && (GetKeyState(VK_KEY_D) & 0x8000))
		OnRunStart(Player::RIGHT);

	if (wParam == VK_KEY_D && !(GetKeyState(VK_KEY_A) & 0x8000))
		OnRunStop();
	else if (wParam == VK_KEY_D && (GetKeyState(VK_KEY_A) & 0x8000))
		OnRunStart(Player::LEFT);
}

void GameController::Gravity(Player *pPlayer, double dt)
{
	pPlayer->m_fSpeedY += GRAVITY * (float)dt;

	if (auto pBrick = m_pMap->GetGround(pPlayer, dt))
	{
		pPlayer->m_fSpeedY = 0;
		pPlayer->SubPState(Player::PS_JUMP);
		pPlayer->m_byJump = 0;
		pPlayer->m_fY = pBrick->GetHeight(pPlayer->m_fX);
	}
	else
	{
		pPlayer->m_fY += pPlayer->m_fSpeedY * (float)dt;
	}
}

void GameController::Debug()
{
	if (m_pMainPlayer)
	{
		/*
		m_pGraphics->DrawString(10, 10, "X: " + std::to_string(m_pMainPlayer->m_fX));
		m_pGraphics->DrawString(10, 22, "Y: " + std::to_string(m_pMainPlayer->m_fY));
		m_pGraphics->DrawString(10, 34, "SpeedY: " + std::to_string(m_pMainPlayer->m_fSpeedY));
		*/
	}
}

D2D1_POINT_2F GameController::GetCenter()
{
	auto center = D2D1::Point2F(0, 0);

	if (!m_pMainPlayer) return center;
	if (!m_pMap)		return center;

	center.x = m_pMainPlayer->m_fX;
	center.y = m_pMainPlayer->m_fY - PLAYER_HEIGHT / 2;

	/*
	if (m_pMap->m_fMaxY - center.y < SCREEN_HEIGHT / 2) 
		center.y = m_pMap->m_fMaxY - SCREEN_HEIGHT / 2;

	if (m_pMap->m_fMaxX - center.x < SCREEN_WIDTH / 2) 
		center.x = m_pMap->m_fMaxX - SCREEN_WIDTH / 2;

	if (center.x - m_pMap->m_fMinX < SCREEN_WIDTH / 2) 
		center.x = m_pMap->m_fMinX + SCREEN_WIDTH / 2;
	*/

	D2D1_SIZE_F size = m_pGraphics->GetRenderTarget()->GetSize();

	if (m_pMap->m_fMaxY - center.y < size.height / 2)
		center.y = m_pMap->m_fMaxY - size.height / 2;

	if (m_pMap->m_fMaxX - center.x < size.width / 2)
		center.x = m_pMap->m_fMaxX - size.width / 2;
	

	if (center.x - m_pMap->m_fMinX < size.width / 2)
		center.x = m_pMap->m_fMinX + size.width / 2;
	
	return center;
}

std::wstring GameController::CharToWChar(const char* ptr)
{
	std::string temp = ptr;

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &temp[0], (int)temp.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &temp[0], (int)temp.size(), &wstrTo[0], size_needed);

	return wstrTo;
}
