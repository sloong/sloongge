// LuaGame.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "IUniversal.h"
#include "SloongDemo.h"
#include "Defines.h"
#include "SloongUIManager.h"
#include "SloongUI.h"
#include "SloongObject.h"
#include "SloongDraw.h"
#include "SloongException.h"
#include "ConsoleWindow.h"
#include "SloongSprite.h"
#include "SloongD3D.h"
#include "SloongString.h"
#include "DInputClass.h"
#include "SloongCamera.h"
#include "SloongEngine.h"
#include "SloongObject3D.h"
#include "SloongMath2.h"
#pragma comment(lib,"Universal.lib")
#pragma comment(lib,"SloongMath.lib")
#pragma comment(lib,"SloongGraphic.lib")
using namespace SoaringLoong;
using namespace SoaringLoong::Graphics;
using namespace SoaringLoong::Universal;
using namespace SoaringLoong::Graphics3D;
// Global Variables:							
CLua* CSloongGame::g_pLua;
CSloongGame* CSloongGame::g_pApp = NULL;
CUIManager* CSloongGame::g_pUIManager = NULL;
LuaFunctionRegistr CSloongGame::g_LuaFunctionList[] =
{
	{ _T("Version"), CSloongGame::Version },
	{ _T("RegisterEvent"), CSloongGame::RegisterEvent },
	{ _T("CreateGUIItem"), CSloongGame::CreateGUIItem },
	{ _T("MoveGUIItem"), CSloongGame::MoveGUIItem },
	{ _T("DeleteGUIItem"), CSloongGame::DeleteGUIItem },
	{ _T("EnableItem"), CSloongGame::EnableItem },
	{ _T("RunGUI"), CSloongGame::RunGUI },
	{ _T("RunItemCommand"), CSloongGame::RunItemCommand },
	{ _T("SendEvent"), CSloongGame::SendEvent },
	{ _T("SetItemFont"), CSloongGame::SetItemFont },
	{ _T("StartTimer"), CSloongGame::StartTimer },
	{ _T("Exit"), CSloongGame::Exit },
	{ _T("Load3DModule"), CSloongGame::Load3DModule},
};

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CSloongGame theApp;
	CSloongGame::SetAppMain(&theApp);

	if (!theApp.InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	return theApp.Run();
}


CSloongGame::CSloongGame()
{
	g_pLua = NULL;
	m_hMainWnd = NULL;
	m_strTitle = _T("SloongGame");
	m_strWindowClass = _T("SLOONGGAME");
	CoInitialize(NULL);
}


CSloongGame::~CSloongGame()
{
	SAFE_RELEASE(m_pLog);
	SAFE_RELEASE(m_pUniversal);
	SAFE_DELETE(g_pLua);
	SAFE_DELETE(g_pUIManager);
	CoUninitialize();
}

ATOM CSloongGame::MyRegisterClass(const HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = CSloongGame::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SLOONG));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = m_strWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SLOONG));

	return RegisterClassEx(&wcex);
}


BOOL CSloongGame::InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	CSloongGame::MyRegisterClass(hInstance);

	if (SUCCEEDED(CreateUniversal((LPVOID*)&m_pUniversal)))
	{
		if (SUCCEEDED(m_pUniversal->CreateLogSystem(m_pUniversal, &m_pLog)))
		{
			CString str;
			str.Format(_T("%s.log"), m_strTitle);
			m_pLog->Initialize(m_pUniversal, str.GetString().c_str());
		}
		else
		{
			MessageBox(NULL, _T("Create LogSystem fialed."), NULL, MB_OK);
			ExitProcess(-1);
		}
	}
	else
	{
		MessageBox(NULL, _T("Create Universal fialed."), NULL, MB_OK);
		ExitProcess(-1);
	}


	m_hInst = hInstance; // Store instance handle in our global variable

	DWORD dwStyle;
	if (FULLSCREEN)
	{
		dwStyle = WS_POPUP | WS_VISIBLE;
		m_rcWindow.SetRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	}
	else
	{
		dwStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
		m_rcWindow.SetRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	}

	m_hMainWnd = CreateWindow(m_strWindowClass, m_strTitle, dwStyle,
		0, 0, m_rcWindow.Width(), m_rcWindow.Height(), NULL, NULL, hInstance, NULL);

	AdjustWindowRectEx(&m_rcWindow, GetWindowLong(m_hMainWnd, GWL_STYLE), GetMenu(m_hMainWnd) == NULL, GetWindowLong(m_hMainWnd, GWL_EXSTYLE));

	m_rcWindow.SetRect(0, 0, m_rcWindow.Width(), m_rcWindow.Height());

	MoveWindow(m_hMainWnd, 0, 0, m_rcWindow.Width(), m_rcWindow.Height(), TRUE);

	if (!m_hMainWnd)
	{
		return FALSE;
	}

	try
	{
		g_pLua = new CLua();
		g_pLua->SetErrorHandle(CSloongGame::ErrorHandler);
		for (int i = 0; i < ARRAYSIZE(g_LuaFunctionList); i++)
		{
			g_pLua->AddFunction(g_LuaFunctionList[i].strFunctionName, g_LuaFunctionList[i].pFunction);
		}
		m_pEngine = new CSloongEngine();
		m_pEngine->SetEnentHandler(SendEvent);

		//CWinConsole::StartConsole(m_hInst, g_pLua);

		//  		m_pD3D = new CSloongD3D();
		//  		m_pD3D->Init(m_hMainWnd, m_hInst);
		m_pInput = new DInputClass();
		m_pInput->Init(m_hMainWnd, m_hInst, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE); //前台，非独占模式
		m_pDraw = new CDDraw();
		m_pDraw->Initialize(m_hMainWnd, m_rcWindow.Width(), m_rcWindow.Height(), SCREEN_BPP, FULLSCREEN);

		g_pUIManager = new CUIManager();
		g_pUIManager->Initialize(m_pDraw, g_pLua, m_pLog, m_hMainWnd);

		g_pLua->RunScript(_T("Start.lua"));

		InitTest();
	}
	catch (CException& e)
	{
		m_pLog->Log(LOGLEVEL::FATAL, e.GetResult(), e.GetException().c_str());
	}

	ShowWindow(m_hMainWnd, nCmdShow);
	UpdateWindow(m_hMainWnd);

	return TRUE;
}


LRESULT CALLBACK CSloongGame::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PRINT:
		GetAppMain()->Render();
		ValidateRect(GetAppMain()->m_hMainWnd, NULL);
		break;
	case WM_KEYDOWN:                // 键盘按下消息
		if (wParam == VK_ESCAPE)    // ESC键
			DestroyWindow(GetAppMain()->m_hMainWnd);    // 销毁窗口, 并发送一条WM_DESTROY消息
		break;
	case WM_DESTROY:
		CSloongGame::g_pApp->Shutdown();
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void CSloongGame::Shutdown()
{
	SAFE_DELETE(g_pLua);

	PostQuitMessage(0);
}

int CSloongGame::Run()
{
	MSG msg = { 0 };
	// Main message loop:
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Render();
			//m_pD3D->Loop();
		}
	}

	Shutdown();

	return (int)msg.wParam;
}

LuaRes CSloongGame::Version(lua_State* l)
{
	tstring str = g_pLua->GetStringArgument(1);
	MessageBox(NULL, str.c_str(), _T("Test"), MB_OK);
	return 0;
}

LuaRes CSloongGame::RegisterEvent(lua_State* l)
{
	if (g_pLua)
	{
		auto handlerName = g_pLua->GetStringArgument(1);
		GetSloongUIManager()->GetCurrentUI()->SetEventHandler(handlerName.c_str());
	}
	return 0;
}

LuaRes CSloongGame::SendEvent(int id, LPCTSTR args)
{
	auto strEventHandler = GetSloongUIManager()->GetCurrentUI()->GetEventHandler();
	if (g_pLua && !strEventHandler.empty())
	{
		TCHAR buf[256] = { 0 };
		if (args)
		{
			_stprintf_s(buf, 256, _T("%d,%s"), id, args);
		}
		else
		{
			_stprintf_s(buf, 256, _T("%d"), id);
		}
		g_pLua->RunFunction(strEventHandler.c_str(), buf);
	}
	return 0;
}

int CSloongGame::SendEvent(lua_State* l)
{
	return 0;
}

int CSloongGame::SendEvent(int id, UI_EVENT event)
{
	CString str;
	str.Format(_T("%d"), event);
	SendEvent(id, str.GetString().c_str());
	return 0;
}

void CSloongGame::ErrorHandler(LPCTSTR strError)
{
	GetAppMain()->m_pLog->WriteLine(strError);
}


int CSloongGame::CreateGUIItem(lua_State* l)
{
	CUIManager* pGUIManager = CSloongGame::GetSloongUIManager();
	if (pGUIManager)
	{
		CLua* pLua = GetSloongLua();

		UINT nID = pLua->GetNumberArgument(1);
		tstring strType = pLua->GetStringArgument(2);
		vector<tstring> strTexture;

		if (strSpriteName == strType || strTextFieldName == strType)
		{
			strTexture.push_back(pLua->GetStringArgument(3));
		}
		else if (strButtonName == strType)
		{
			strTexture.push_back(pLua->GetStringArgument(3));
			strTexture.push_back(pLua->GetStringArgument(4));
			strTexture.push_back(pLua->GetStringArgument(5));
			strTexture.push_back(pLua->GetStringArgument(6));
		}
		else
		{
			throw tstring(_T("Type error"));
		}

		pGUIManager->CreateGUIItem(nID, strType, strTexture);
	}

	return 0;
}

int CSloongGame::DeleteGUIItem(lua_State* l)
{
	g_pUIManager->DeleteItem(g_pLua->GetNumberArgument(1));
	return 0;
}

int CSloongGame::MoveGUIItem(lua_State* l)
{
	UINT nID = g_pLua->GetNumberArgument(1);
	CRect rc;
	rc.SetRect(g_pLua->GetNumberArgument(2),
		g_pLua->GetNumberArgument(3),
		g_pLua->GetNumberArgument(4) + g_pLua->GetNumberArgument(2),
		g_pLua->GetNumberArgument(5) + g_pLua->GetNumberArgument(3));
	g_pUIManager->MoveItem(nID, rc);
	return 0;
}

int CSloongGame::RunItemCommand(lua_State* l)
{
	return 0;
}

int CSloongGame::RunGUI(lua_State* l)
{
	auto pLug = GetSloongLua();
	GetSloongUIManager()->RunGUI(pLug->GetStringArgument(1));
	return 0;
}

int CSloongGame::SetItemFont(lua_State* l)
{
	return 0;
}

int CSloongGame::EnableItem(lua_State* l)
{
	return 0;
}

int CSloongGame::StartTimer(lua_State* l)
{
	return 0;
}

int CSloongGame::GetMousePos(lua_State* l)
{
	return 0;
}

int CSloongGame::Exit(lua_State* l)
{
	PostQuitMessage(GetSloongLua()->GetNumberArgument(1));
	return 0;
}


int CSloongGame::Load3DModule(lua_State* l)
{
	
	return 0;
}

CLua* CSloongGame::GetSloongLua()
{
	return g_pLua;
}

void CSloongGame::SetSloongLua(CLua* pLua)
{
	g_pLua = pLua;
}

CSloongGame* CSloongGame::GetAppMain()
{
	return g_pApp;
}

void CSloongGame::SetAppMain(CSloongGame* pApp)
{
	g_pApp = pApp;
}

CUIManager* CSloongGame::GetSloongUIManager()
{
	return g_pUIManager;
}

void CSloongGame::Render()
{
	//GetSloongUIManager()->Update();
	m_pDraw->DDrawFillBackSurface(0);

	if (KEY_DOWN(VK_ESCAPE))
	{
		PostMessage(m_hMainWnd, WM_DESTROY, 0, 0);

		m_pDraw->Screen_Transitions(SCREEN_DARKNESS, NULL, 0);

	}
	//GetSloongUIManager()->GetCurrentUI()->Render();
	RenderTest();
	// Flip
	m_pDraw->DDraw_Flip();
}

CDDraw* CSloongGame::GetDDraw()
{
	return g_pApp->m_pDraw;
}

ILogSystem* CSloongGame::GetLogSystem()
{
	return g_pApp->m_pLog;
}

IUniversal* CSloongGame::GetUniversal()
{
	return g_pApp->m_pUniversal;
}

bool CSloongGame::InRect(const CSize& pos, const CRect& rc)
{
	if ((pos.cx < rc.left) || (pos.cx > rc.right))
		return false;
	if ((pos.cy < rc.top) || (pos.cy > rc.bottom))
		return false;

	return true;
}

void CSloongGame::InitTest()
{
	vscale = { 1, 1, 1, 1 }, vpos = { 0, 0, 0, 1 }, vrot = { 0, 0, 0, 1 };

	// initialize camera 
	POINT4D  cam_pos = { 0, 40, 0, 1 };
	POINT4D  cam_target = { 0, 0, 0, 1 };
	VECTOR4D cam_dir = { 0, 0, 0, 1 };

	m_cam.Initialize(CAMERA_ELUER, cam_pos, cam_dir, &cam_target, CAM_ROT_SEQ_XYZ, 200, 12000, 120, WINDOW_WIDTH, WINDOW_HEIGHT);
	//m_cam.Initialize(CAMERA_UVN, cam_pos, cam_dir, &cam_target, UVN_MODE_SPHERICAL, 200, 12000, 120, WINDOW_WIDTH, WINDOW_HEIGHT);

	obj_tank = IObject::Create3D(m_pDraw);
	obj_marker = IObject::Create3D(m_pDraw);
	obj_player = IObject::Create3D(m_pDraw);
	obj_tower = IObject::Create3D(m_pDraw);

#define UNIVERSE_RADIUS   4000

#define NUM_TOWERS        96
#define NUM_TANKS         24


	// load the master tank object
	vscale.Initialize(0.75, 0.75, 0.75);
	obj_tank->LoadPLGMode(_T("DXFile\\tank2.plg"));

	// position the tanks
	for (int index = 0; index < NUM_TANKS; index++)
	{
		obj_tank->AddObject(index, CVector4D(
			RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS),
			0,
			RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS)),vscale,vrot);
	} // end for


	// load player object for 3rd person view
	vscale.Initialize(0.75, 0.75, 0.75);
	obj_player->LoadPLGMode(_T("DXFile\\tank3.plg") );
	obj_player->AddObject(0, vpos, vscale, vrot);

	// load the master tower object
	vscale.Initialize(1.0, 2.0, 1.0);
	obj_tower->LoadPLGMode(_T("DXFile\\tower1.plg") );

	// position the towers
	for (int index = 0; index < NUM_TOWERS; index++)
	{
		obj_tower->AddObject(index, CVector4D(
			RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS),
			0,
			RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS)), vscale,vrot);
	} // end for

	// load the master ground marker
	vscale.Initialize(3.0, 3.0, 3.0);
	obj_marker->LoadPLGMode(_T("DXFile\\marker1.plg"));


#define POINT_SIZE        200
#define NUM_POINTS_X      (2*UNIVERSE_RADIUS/POINT_SIZE)
#define NUM_POINTS_Z      (2*UNIVERSE_RADIUS/POINT_SIZE)
#define NUM_POINTS        (NUM_POINTS_X*NUM_POINTS_Z)
	// insert the ground markers into the world
	int loopIndex = 0;
	double avg, max = 0.0;
	
	for (int index_x = 0; index_x < NUM_POINTS_X; index_x++)
	for (int index_z = 0; index_z < NUM_POINTS_Z; index_z++)
	{
		loopIndex++;
		
		// set position of tower
 		obj_marker->AddObject(loopIndex, CVector4D(
 			RAND_RANGE(-100, 100) - UNIVERSE_RADIUS + index_x*POINT_SIZE,
 			max,
 			RAND_RANGE(-100, 100) - UNIVERSE_RADIUS + index_z*POINT_SIZE), vscale,vrot);
		obj_marker->GetRadius(avg, max);
	}

}

void CSloongGame::RenderTest()
{
	m_pDraw->Start_Clock();
	static MATRIX4X4 mrot;   // general rotation matrix

	// these are used to create a circling camera
	static float view_angle = 0;
	static float camera_distance = 6000;
	static VECTOR4D pos = { 0, 0, 0, 0 };
	static float tank_speed;
	static float turning = 0;

	char work_string[256]; // temp string

	// draw the sky
	m_pDraw->Draw_Rectangle(0, 0, WINDOW_WIDTH - 1, WINDOW_HEIGHT / 2, RGB(0, 140, 192), m_pDraw->GetBackSurface());

	// draw the ground
	m_pDraw->Draw_Rectangle(0, WINDOW_HEIGHT / 2, WINDOW_WIDTH - 1, WINDOW_HEIGHT - 1, RGB(103, 62, 3), m_pDraw->GetBackSurface());

	// read keyboard and other devices here
	m_pInput->GetInput();
	// game logic here...

	if (m_pInput->IsKeyDown(DIK_ESCAPE) || KEY_DOWN(VK_ESCAPE))
	{
		PostQuitMessage(0);
		return;
	}

	// allow user to move camera
#define TANK_SPEED        15
	tank_speed = TANK_SPEED;

	// turbo
	if (m_pInput->IsKeyDown(DIK_SPACE))
	{
		m_cam.WorldPos.y += 15;
	}
	// forward/backward
	if (m_pInput->IsKeyDown(DIK_UP))
	{
		// move forward
		m_cam.WorldPos.x += tank_speed*CMathBase::Fast_Sin(m_cam.Direction.y);
		m_cam.WorldPos.z += tank_speed*CMathBase::Fast_Cos(m_cam.Direction.y);
	} // end if

	if (m_pInput->IsKeyDown(DIK_DOWN))
	{
		// move backward
		m_cam.WorldPos.x -= tank_speed*CMathBase::Fast_Sin(m_cam.Direction.y);
		m_cam.WorldPos.z -= tank_speed*CMathBase::Fast_Cos(m_cam.Direction.y);
	} // end if

	// rotate
	if (m_pInput->IsKeyDown(DIK_RIGHT))
	{
		m_cam.Direction.y += 3;

		// add a little turn to object
		if ((turning += 2) > 15)
			turning = 15;

	} // end if

	if (m_pInput->IsKeyDown(DIK_LEFT))
	{
		m_cam.Direction.y -= 3;

		// add a little turn to object
		if ((turning -= 2) < -15)
			turning = -15;

	} // end if
	else // center heading again
	{
		if (turning > 0)
			turning -= 1;
		else
		if (turning < 0)
			turning += 1;

	} // end else

	// generate camera matrix
	m_cam.UpdateCameraMatrix();

	obj_player->Reset();


	obj_player->SetWorldPosition(CVector4D(
		m_cam.WorldPos.x + 300 * CMathBase::Fast_Sin(m_cam.Direction.y),
		m_cam.WorldPos.y - 70,
		m_cam.WorldPos.z + 300 * CMathBase::Fast_Cos(m_cam.Direction.y)));

	mrot.BuildRotateMatrix(0, m_cam.Direction.y + turning, 0);

	obj_player->Transform(mrot, TRANS_MODE::LocalToTrans, true);

	obj_player->ToWorld(TransOnly);

	obj_player->ToCamera(&m_cam);
	obj_player->ToProject(&m_cam);
	obj_player->PerspectiveToScreen(&m_cam);
	//obj_player->CameraToPerspectiveScreen(&m_cam);
	obj_player->Render();

	mrot.BuildRotateMatrix(0, 1, 0);
	obj_tank->RenderAll(&m_cam, &mrot);
	obj_tower->RenderAll(&m_cam, &mrot);
	obj_marker->RenderAll(&m_cam, &mrot);

	sprintf_s(work_string, 256, "pos:[%f, %f, %f] heading:[%f] elev:[%f]",
		m_cam.WorldPos.x, m_cam.WorldPos.y, m_cam.WorldPos.z, m_cam.Direction.y, m_cam.Direction.x);

	CString str(work_string);
	m_pDraw->DrawText(str.GetString().c_str(), 0, WINDOW_HEIGHT - 20, RGB(0, 255, 0), m_pDraw->GetBackSurface());

	// draw instructions
	m_pDraw->DrawText(_T("Press ESC to exit. Press Arrow Keys to Move. Space for TURBO."), 0, 0, RGB(0, 255, 0), m_pDraw->GetBackSurface());

	// lock the back buffer
	LPBYTE pBackBuffer = m_pDraw->DDraw_Lock_Back_Surface();

	// unlock the back buffer
	m_pDraw->DDraw_Unlock_Back_Surface();

	// sync to 30ish fps
	m_pDraw->Wait_Clock(30);
}
