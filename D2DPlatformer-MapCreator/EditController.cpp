#include "EditController.h"
#include "MainWindow.h"

EditController::EditController(EditWindow* pEditWindow)
{
	m_pEditWindow = pEditWindow;
	isEditModeActive = false;
	m_bCurrentCreationMode = 0;
	m_pCurrentlySelected = NULL;
}

EditController::~EditController()
{
	if (m_pEditWindow)
		delete m_pEditWindow;
}

LRESULT EditController::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		HMENU hMenu = CreateMenu();
		//AppendMenu(hMenu, MF_STRING, B_BACKGROUND, "&Background");
		AppendMenu(hMenu, MF_STRING, B_BRICK, "&Brick");
		AppendMenu(hMenu, MF_STRING, B_DUMMY, "&Dummy");
		SetMenu(hWnd, hMenu);

		CreateStartingControls(hWnd);
	}
	break;
	case WM_DESTROY:
	case WM_QUIT:
	case WM_CLOSE:
	{
		PostQuitMessage(0);
	}
	break;
	case WM_LBUTTONDOWN:
	{
		SetFocus(m_pEditWindow->GetHwnd());
	}
	break;
	case WM_CREATESELECTED:
	{
		ClearSelectedControls(); 

		if (Object::instanceof<Background>(m_pCurrentlySelected))
			CreateSelectedBackgroundControls((Background*)m_pCurrentlySelected);
		else if (Object::instanceof<Brick>(m_pCurrentlySelected))
			CreateSelectedBrickControls((Brick*)m_pCurrentlySelected);

		UpdateWindow(m_pEditWindow->GetHwnd());
	}
	break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case B_BROWSE:
		{
			Browse(hWnd);
		}
		break;
		case B_EDITMODE:
		{
			InitEditMode();
		}
		break;
		case B_NORMALMODE:
		{
			InitNormalMode();
		}
		break;
		case B_DRAW:
		{
			if (m_bCurrentCreationMode == 0)
				break;
			/*
			if (isBackGroundActive)
				GetBackGroundInput();
				*/
			if (m_bCurrentCreationMode == OT_BRICK)
				GetBrickInput(WT_OBJECT);
			else if (m_bCurrentCreationMode == OT_DUMMY)
				GetDummyInput();
		}
		break;
		case B_BACKGROUND:
		{
			if (!isEditModeActive)
			{
				MessageBox(NULL, "Chose a mode first!", "Error", MB_OK);
				break;
			}

			m_bCurrentCreationMode = OT_BACKGROUND;

			ClearControls();
			CreateBackgroundControls(hWnd);
		}
		break;
		case B_BRICK:
		{
			if (!isEditModeActive)
			{
				MessageBox(NULL, "Chose a mode first!", "Error", MB_OK);
				break;
			}

			m_bCurrentCreationMode = OT_BRICK;

			ClearControls();
			CreateBrickControls(hWnd);
		}
		break;
		case B_DUMMY:
		{
			if (!isEditModeActive)
			{
				MessageBox(NULL, "Chose a mode first!", "Error", MB_OK);
				break;
			}

			m_bCurrentCreationMode = OT_DUMMY;

			ClearControls();
			CreateDummyControls(hWnd);
		}
		break;
		case B_DELETE:
		{
			if (m_pCurrentlySelected == NULL)
			{
				MessageBox(NULL, "No Object selected!", "Error", MB_OK);
				break;
			}

			m_pGameController->EditControllerMessageHandler(MSG_DELETE, OT_BRICK, m_pCurrentlySelected);
		    m_pCurrentlySelected  = NULL;
		}
		break;
		case B_CHANGE:
		{
			if (m_pCurrentlySelected == NULL)
			{
				MessageBox(NULL, "No Object selected!", "Error", MB_OK);
				break;
			}

			//LAYERS: ADD WIDTH AND HEIGHT ?, Different images or always 1? Things to make clear before adding layer support.
			if (Object::instanceof<Background>(m_pCurrentlySelected) && !src.empty())
				m_pGameController->EditControllerMessageHandler(MSG_CHANGE, OT_BACKGROUND, src);
			else if (Object::instanceof<Brick>(m_pCurrentlySelected))
				GetBrickInput(WT_OBJECTSELECTED);	
		}
		break;
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void EditController::GameControllerMessageHandler(int bMessage, ...)
{
	va_list arguments;
	va_start(arguments, bMessage);
	int bType = va_arg(arguments, int);

	switch (bMessage)
	{
	case MSG_CLOSE:
	{
		if (m_pEditWindow != NULL)
			SendMessage(m_pEditWindow->GetHwnd(), WM_CLOSE, NULL, NULL);
	}
	break;
	case MSG_CHANGE:
	{
		PostMessage(m_pEditWindow->GetHwnd(), WM_CREATESELECTED, NULL, NULL);
	}
	break;
	case MSG_SELECTEDOBJECT:
	{
		if (bType == OT_BACKGROUND)
		{
			Background* pBackground = va_arg(arguments, Background*);

			if (m_pCurrentlySelected != NULL && pBackground)
				if (m_pCurrentlySelected == pBackground)
				{
					printf("equal\n");
					break;
				}
			int bTemp = OT_BACKGROUND;
			m_pCurrentlySelected = pBackground;
			PostMessage(m_pEditWindow->GetHwnd(), WM_CREATESELECTED, reinterpret_cast<WPARAM &>(bTemp), NULL);
		}
		else if (bType == OT_BRICK)
		{
			Brick* pBrick = va_arg(arguments, Brick*);
			
			if (m_pCurrentlySelected != NULL && pBrick)
				if (m_pCurrentlySelected == pBrick)
				{
					printf("equal\n");
					break;
				}
			int bTemp = OT_BRICK;
			m_pCurrentlySelected = pBrick;
			PostMessage(m_pEditWindow->GetHwnd(), WM_CREATESELECTED, reinterpret_cast<WPARAM &>(bTemp), NULL);
		}
		else if (bType == OT_DUMMY)
		{
			Dummy* pDummy = va_arg(arguments, Dummy*);
		}
	}
	break;
	}
}

void EditController::InitNormalMode()
{
	MainWindow* pMainWindow = new MainWindow(1400.0, 700.0);
	pMainWindow->StartThread();

	DestroyWindow(m_pEditWindow->GetHwnd());

	pMainWindow->m_pGameController->EditControllerMessageHandler(MSG_SETCONTROLLER, this, false);
}

void EditController::InitEditMode()
{
	MainWindow* pMainWindow = new MainWindow(1400.0, 700.0);
	pMainWindow->StartThread();

	m_pGameController = pMainWindow->m_pGameController;
	m_pGameController->EditControllerMessageHandler(MSG_SETCONTROLLER, this, true);

	ClearControls();
	CreateMainControls(m_pEditWindow->GetHwnd());
	isEditModeActive = true;
}

void EditController::ClearControls()
{
	for (auto &a : m_pControls)
		DestroyWindow(a->m_hWnd);

	m_pControls.clear();
}

void EditController::ClearSelectedControls()
{
	for (auto &a : m_pSelectedControls)
		DestroyWindow(a->m_hWnd);
	
	m_pSelectedControls.clear();
}

void EditController::CreateStartingControls(HWND hWnd)
{
	HWND temp = NULL;
	Control* pControl = NULL;

	// will always stay.
	CreateWindowEx(NULL,
		"Static",
		"MapCreator",
		WS_CHILD | WS_VISIBLE,
		200,
		20,
		100,
		20,
		hWnd,
		NULL,
		GetModuleHandle(NULL),
		NULL);

	temp = CreateWindowEx(NULL,
		"Static",
		"Which mode do you want to open?",
		WS_CHILD | WS_VISIBLE,
		150,
		50,
		250,
		20,
		hWnd,
		NULL,
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OTHER);
	m_pControls.push_back(pControl);

	temp = CreateWindowEx(NULL,
		"Button",
		"NormalMode",
		WS_VISIBLE | WS_CHILD,
		300,
		110,
		150,
		20,
		hWnd,
		(HMENU)B_NORMALMODE,
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OTHER);
	m_pControls.push_back(pControl);

	temp = CreateWindowEx(NULL,
		"BUTTON",
		"EditMode",
		WS_VISIBLE | WS_CHILD,
		100,
		110,
		150,
		20,
		hWnd,
		(HMENU)B_EDITMODE,
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OTHER);
	m_pControls.push_back(pControl);
}

void EditController::CreateMainControls(HWND hWnd)
{
	HWND temp = NULL;
	Control* pControl = NULL;

	temp = CreateWindowEx(NULL,
		"Static",
		"Chose what you want to add from the menu bar",
		WS_CHILD | WS_VISIBLE,
		100,
		200,
		300,
		20,
		hWnd,
		NULL,
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OTHER);
	m_pControls.push_back(pControl);

		CreateWindowEx(NULL,
		"Static",
		"Currently selected:",
		WS_CHILD | WS_VISIBLE,
		10,
		410,
		150,
		20,
		hWnd,
		NULL,
		GetModuleHandle(NULL),
		NULL);

		CreateWindowEx(NULL,
			"Button",
			"Delete",
			WS_VISIBLE | WS_CHILD,
			370,
			410,
			100,
			20,
			hWnd,
			(HMENU)B_DELETE,
			GetModuleHandle(NULL),
			NULL);

		CreateWindowEx(NULL,
			"Button",
			"Change",
			WS_VISIBLE | WS_CHILD,
			260,
			410,
			100,
			20,
			hWnd,
			(HMENU)B_CHANGE,
			GetModuleHandle(NULL),
			NULL);
}

void EditController::CreateBackgroundControls(HWND hWnd)
{
	HWND temp = NULL;
	Control* pControl = NULL;

	temp =CreateWindowEx(NULL,
		"BUTTON",
		"Browse",
		WS_VISIBLE | WS_CHILD,
		100,
		100,
		100,
		20,
		hWnd,
		(HMENU)B_BROWSE,
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OTHER);
	m_pControls.push_back(pControl);

	temp = CreateWindowEx(NULL,
		"BUTTON",
		"Draw",
		WS_VISIBLE | WS_CHILD,
		300,
		100,
		100,
		20,
		hWnd,
		(HMENU)B_DRAW,
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OTHER);
	m_pControls.push_back(pControl);
}

void EditController::CreateBrickControls(HWND hWnd)
{
	HWND temp = NULL;
	Control* pControl = NULL;

	temp = CreateWindowEx(NULL,
		"Static",
		"X",
		WS_VISIBLE | WS_CHILD,
		100,
		100,
		100,
		20,
		hWnd,
		NULL,
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OTHER);
	m_pControls.push_back(pControl);

	temp = CreateWindowEx(NULL,
		"Edit",
		"",
		WS_VISIBLE | WS_CHILD,
		100,
		120,
		100,
		20,
		hWnd,
		HMENU(WT_OBJECT_X),
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OBJECT_X);
	m_pControls.push_back(pControl);

	temp = CreateWindowEx(NULL,
		"Static",
		"Y",
		WS_VISIBLE | WS_CHILD,
		300,
		100,
		100,
		20,
		hWnd,
		NULL,
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OTHER);
	m_pControls.push_back(pControl);

	temp = CreateWindowEx(NULL,
		"Edit",
		"",
		WS_VISIBLE | WS_CHILD,
		300,
		120,
		100,
		20,
		hWnd,
		HMENU (WT_OBJECT_Y),
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OBJECT_Y);
	m_pControls.push_back(pControl);

	temp = CreateWindowEx(NULL,
		"Static",
		"Width",
		WS_VISIBLE | WS_CHILD,
		50,
		200,
		100,
		20,
		hWnd,
		NULL,
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OTHER);
	m_pControls.push_back(pControl);

	temp = CreateWindowEx(NULL,
		"Edit",
		"",
		WS_VISIBLE | WS_CHILD,
		50,
		220,
		100,
		20,
		hWnd,
		HMENU(WT_OBJECT_WIDTH),
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OBJECT_WIDTH);
	m_pControls.push_back(pControl);

	temp = CreateWindowEx(NULL,
		"Static",
		"Height",
		WS_VISIBLE | WS_CHILD,
		200,
		200,
		100,
		20,
		hWnd,
		NULL,
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OTHER);
	m_pControls.push_back(pControl);

	temp = CreateWindowEx(NULL,
		"Edit",
		"",
		WS_VISIBLE | WS_CHILD,
		200,
		220,
		100,
		20,
		hWnd,
		HMENU(WT_OBJECT_HEIGHT),
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OBJECT_HEIGHT);
	m_pControls.push_back(pControl);

	temp = CreateWindowEx(NULL,
		"Static",
		"Angle",
		WS_VISIBLE | WS_CHILD,
		350,
		200,
		100,
		20,
		hWnd,
		NULL,
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OTHER);
	m_pControls.push_back(pControl);

	temp = CreateWindowEx(NULL,
		"Edit",
		"",
		WS_VISIBLE | WS_CHILD,
		350,
		220,
		100,
		20,
		hWnd,
		HMENU(WT_OBJECT_ANGLE),
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OBJECT_ANGLE);
	m_pControls.push_back(pControl);

	temp = CreateWindowEx(NULL,
		"BUTTON",
		"Browse",
		WS_VISIBLE | WS_CHILD,
		100,
		300,
		100,
		20,
		hWnd,
		(HMENU)B_BROWSE,
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OTHER);
	m_pControls.push_back(pControl);

	temp = CreateWindowEx(NULL,
		"BUTTON",
		"Draw",
		WS_VISIBLE | WS_CHILD,
		300,
		300,
		100,
		20,
		hWnd,
		(HMENU)B_DRAW,
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OTHER);
	m_pControls.push_back(pControl);
}

void EditController::CreateDummyControls(HWND hWnd)
{
	HWND temp = NULL;
	Control* pControl = NULL;

	temp = CreateWindowEx(NULL,
		"Static",
		"X",
		WS_VISIBLE | WS_CHILD,
		100,
		100,
		100,
		20,
		hWnd,
		NULL,
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OTHER);
	m_pControls.push_back(pControl);

	temp = CreateWindowEx(NULL,
		"Edit",
		"",
		WS_VISIBLE | WS_CHILD,
		100,
		120,
		100,
		20,
		hWnd,
		HMENU(WT_OBJECT_X),
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OBJECT_X);
	m_pControls.push_back(pControl);

	temp = CreateWindowEx(NULL,
		"Static",
		"Y",
		WS_VISIBLE | WS_CHILD,
		300,
		100,
		100,
		20,
		hWnd,
		NULL,
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OTHER);
	m_pControls.push_back(pControl);

	temp = CreateWindowEx(NULL,
		"Edit",
		"",
		WS_VISIBLE | WS_CHILD,
		300,
		120,
		100,
		20,
		hWnd,
		HMENU(WT_OBJECT_Y),
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OBJECT_Y);
	m_pControls.push_back(pControl);

	temp = CreateWindowEx(NULL,
		"BUTTON",
		"Browse",
		WS_VISIBLE | WS_CHILD,
		100,
		200,
		100,
		20,
		hWnd,
		(HMENU)B_BROWSE,
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OTHER);
	m_pControls.push_back(pControl);

	temp = CreateWindowEx(NULL,
		"BUTTON",
		"Draw",
		WS_VISIBLE | WS_CHILD,
		300,
		200,
		100,
		20,
		hWnd,
		(HMENU)B_DRAW,
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OTHER);
	m_pControls.push_back(pControl);
}

void EditController::CreateSelectedBackgroundControls(Background* pBackground)
{
	HWND temp = NULL;
	Control* pControl = NULL;

	HWND hWnd = m_pEditWindow->GetHwnd();

	temp = CreateWindowEx(NULL,
		"Static",
		"BACKGROUND",
		WS_CHILD | WS_VISIBLE,
		150,
		410,
		100,
		20,
		hWnd,
		NULL,
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OBJECTSELECTED_TYPE);
	m_pSelectedControls.push_back(pControl);

	temp = CreateWindowEx(NULL,
		"BUTTON",
		"Browse",
		WS_VISIBLE | WS_CHILD,
		100,
		450,
		100,
		20,
		hWnd,
		(HMENU)B_BROWSE,
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_SELECTEDOTHER);
	m_pSelectedControls.push_back(pControl);
}

void EditController::CreateSelectedBrickControls(Brick* pBrick)
{
	HWND temp = NULL;
	Control* pControl = NULL;

	HWND hWnd = m_pEditWindow->GetHwnd();
	
	const std::string Height = std::to_string(pBrick->m_fHeight);
	const std::string Width = std::to_string(pBrick->m_fWidth);
	const std::string PosX = std::to_string(pBrick->m_fX);
	const std::string PosY = std::to_string(pBrick->m_fY);
	const std::string Angle = std::to_string(pBrick->m_fAngle);

	temp = CreateWindowEx(NULL,
		"Static",
		"BRICK",
		WS_CHILD | WS_VISIBLE,
		150,
		410,
		100,
		20,
		hWnd,
		NULL,
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OBJECTSELECTED_TYPE);
	m_pSelectedControls.push_back(pControl);

	temp = CreateWindowEx(NULL,
		"Static",
		"X",
		WS_VISIBLE | WS_CHILD,
		100,
		450,
		100,
		20,
		hWnd,
		NULL,
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_SELECTEDOTHER);
	m_pSelectedControls.push_back(pControl);

	temp = CreateWindowEx(NULL,
		"Edit",
		"",
		WS_VISIBLE | WS_CHILD,
		100,
		470,
		100,
		20,
		hWnd,
		HMENU(WT_OBJECTSELECTED_X),
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OBJECTSELECTED_X);
	m_pSelectedControls.push_back(pControl);
	SetWindowText(temp, PosX.c_str());

	temp = CreateWindowEx(NULL,
		"Static",
		"Y",
		WS_VISIBLE | WS_CHILD,
		300,
		450,
		100,
		20,
		hWnd,
		NULL,
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_SELECTEDOTHER);
	m_pSelectedControls.push_back(pControl);

	temp = CreateWindowEx(NULL,
		"Edit",
		"",
		WS_VISIBLE | WS_CHILD,
		300,
		470,
		100,
		20,
		hWnd,
		HMENU(WT_OBJECTSELECTED_Y),
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OBJECTSELECTED_Y);
	m_pSelectedControls.push_back(pControl);
	SetWindowText(temp, PosY.c_str());

	temp = CreateWindowEx(NULL,
		"Static",
		"Width",
		WS_VISIBLE | WS_CHILD,
		50,
		550,
		100,
		20,
		hWnd,
		NULL,
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_SELECTEDOTHER);
	m_pSelectedControls.push_back(pControl);

	temp = CreateWindowEx(NULL,
		"Edit",
		"",
		WS_VISIBLE | WS_CHILD,
		50,
		570,
		100,
		20,
		hWnd,
		HMENU(WT_OBJECTSELECTED_WIDTH),
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OBJECTSELECTED_WIDTH);
	m_pSelectedControls.push_back(pControl);
	SetWindowText(temp, Width.c_str());

	temp = CreateWindowEx(NULL,
		"Static",
		"Height",
		WS_VISIBLE | WS_CHILD,
		200,
		550,
		100,
		20,
		hWnd,
		NULL,
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_SELECTEDOTHER);
	m_pSelectedControls.push_back(pControl);

	temp = CreateWindowEx(NULL,
		"Edit",
		"",
		WS_VISIBLE | WS_CHILD,
		200,
		570,
		100,
		20,
		hWnd,
		HMENU(WT_OBJECTSELECTED_HEIGHT),
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OBJECTSELECTED_HEIGHT);
	m_pSelectedControls.push_back(pControl);
	SetWindowText(temp, Height.c_str());

	temp = CreateWindowEx(NULL,
		"Static",
		"Angle",
		WS_VISIBLE | WS_CHILD,
		350,
		550,
		100,
		20,
		hWnd,
		NULL,
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_SELECTEDOTHER);
	m_pSelectedControls.push_back(pControl);

	temp = CreateWindowEx(NULL,
		"Edit",
		"",
		WS_VISIBLE | WS_CHILD,
		350,
		570,
		100,
		20,
		hWnd,
		HMENU(WT_OBJECTSELECTED_ANGLE),
		GetModuleHandle(NULL),
		NULL);
	pControl = new Control(temp, WT_OBJECTSELECTED_ANGLE);
	m_pSelectedControls.push_back(pControl);
	SetWindowText(temp, Angle.c_str());
}

void EditController::CreateSelectedDummyControls(Dummy* pDummy)
{

}

/*
void EditController::GetBackGroundInput()
{
	if (src.empty())
	{
		MessageBox(NULL, "U did not pass in an image!", "Error", MB_OK);
		return;
	}
	m_pGameController->EditControllerMessageHandler(MSG_CREATE, OT_BACKGROUND, src);
}
*/

void EditController::GetBrickInput(int bType)
{
	if (bType == WT_OBJECT && src.empty())
	{
		MessageBox(NULL, "Put in all the parameters please!", "Error", MB_OK);
		return;
	}

	HWND hXInput = NULL;
	HWND hYInput = NULL;
	HWND hWidthInput = NULL;
	HWND hHeightInput = NULL;
	HWND hAngleInput = NULL;

	if (bType == WT_OBJECT)
	{
		for (auto &a : m_pControls)
		{
			if (a->m_bType == WT_OBJECT_X)
				hXInput = a->m_hWnd;
			else if (a->m_bType == WT_OBJECT_Y)
				hYInput = a->m_hWnd;
			else if (a->m_bType == WT_OBJECT_WIDTH)
				hWidthInput = a->m_hWnd;
			else if (a->m_bType == WT_OBJECT_HEIGHT)
				hHeightInput = a->m_hWnd;
			else if (a->m_bType == WT_OBJECT_ANGLE)
				hAngleInput = a->m_hWnd;
		}
	}

	else if (bType == WT_OBJECTSELECTED)
	{
		for (auto &a : m_pSelectedControls)
		{
			if (a->m_bType == WT_OBJECTSELECTED_X)
				hXInput = a->m_hWnd;
			else if (a->m_bType == WT_OBJECTSELECTED_Y)
				hYInput = a->m_hWnd;
			else if (a->m_bType == WT_OBJECTSELECTED_WIDTH)
				hWidthInput = a->m_hWnd;
			else if (a->m_bType == WT_OBJECTSELECTED_HEIGHT)
				hHeightInput = a->m_hWnd;
			else if (a->m_bType == WT_OBJECTSELECTED_ANGLE)
				hAngleInput = a->m_hWnd;
		}
	}

	if (hXInput == NULL || hYInput == NULL || hWidthInput == NULL || hHeightInput == NULL || hAngleInput == NULL)
	{
		MessageBox(NULL, "Put in all the parameters please!", "Error", MB_OK);
		return;
	}
	int TextlenX = GetWindowTextLength(hXInput);
	int TextlenY = GetWindowTextLength(hYInput);
	int TextlenWidth = GetWindowTextLength(hWidthInput);
	int TextlenHeight = GetWindowTextLength(hHeightInput);
	int TextlenAngle = GetWindowTextLength(hAngleInput);

	char* XInput = new char[TextlenX + 1];
	char* YInput = new char[TextlenY + 1];
	char* WidthInput = new char[TextlenWidth + 1];
	char* HeightInput = new char[TextlenHeight + 1];
	char* AngleInput = new char[TextlenAngle + 1];

	GetWindowText(hXInput, XInput, TextlenX + 1);
	GetWindowText(hYInput, YInput, TextlenY + 1);
	GetWindowText(hWidthInput, WidthInput, TextlenWidth + 1);
	GetWindowText(hHeightInput, HeightInput, TextlenHeight + 1);
	GetWindowText(hAngleInput, AngleInput, TextlenAngle + 1);

	long X = strtol(XInput, NULL, 10);
	long Y = strtol(YInput, NULL, 10);
	long Width = strtol(WidthInput, NULL, 10);
	long Height = strtol(HeightInput, NULL, 10);
	long Angle = strtol(AngleInput, NULL, 10);

	if (!IsDigits(XInput) || !IsDigits(YInput) || !IsDigits(WidthInput) || !IsDigits(HeightInput) || !IsDigits(AngleInput))
	{
		MessageBox(NULL, "Only numbers please!", "Error", MB_OK);
		return;
	}

	if (bType == WT_OBJECT)
		m_pGameController->EditControllerMessageHandler(MSG_CREATE, OT_BRICK, src, X, Y, Width, Height, Angle);
	else if (bType == WT_OBJECTSELECTED)
		m_pGameController->EditControllerMessageHandler(MSG_CHANGE, OT_BRICK, m_pCurrentlySelected, X, Y, Width, Height, Angle);

	delete XInput;
	delete YInput;
	delete WidthInput;
	delete HeightInput;
	delete AngleInput;
}

void EditController::GetDummyInput()
{
	HWND hXInput = NULL;
	HWND hYInput = NULL;

	for (auto &a : m_pControls)
	{
		if (a->m_bType == WT_OBJECT_X)
			hXInput = a->m_hWnd;
		else if (a->m_bType == WT_OBJECT_Y)
			hYInput = a->m_hWnd;
	}

	if (hXInput == NULL || hYInput == NULL || src.empty())
	{
		MessageBox(NULL, "Put in all the parameters please!", "Error", MB_OK);
		return;
	}

	int TextlenX = GetWindowTextLength(hXInput);
	int TextlenY = GetWindowTextLength(hYInput);

	char* XInput = new char[TextlenX + 1];
	char* YInput = new char[TextlenY + 1];

	GetWindowText(hXInput, XInput, TextlenX + 1);
	GetWindowText(hYInput, YInput, TextlenY + 1);

	long X = strtol(XInput, NULL, 10);
	long Y = strtol(YInput, NULL, 10);

	if (X == 0 || Y == 0)
	{
		MessageBox(NULL, "Only numbers please!", "Error", MB_OK);
		return;
	}
	
	m_pGameController->EditControllerMessageHandler(MSG_CREATE, OT_DUMMY, src, X, Y);

	delete XInput;
	delete YInput;
}

void EditController::Browse(HWND hWnd)
{
	OPENFILENAME ofn;
	char szFile[260];

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (!GetOpenFileName(&ofn) == TRUE)
		return;

	src = ofn.lpstrFile;
}

std::wstring EditController::CharToWChar(const char* ptr)
{
	std::string temp = ptr;

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &temp[0], (int)temp.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &temp[0], (int)temp.size(), &wstrTo[0], size_needed);

	return wstrTo;
}