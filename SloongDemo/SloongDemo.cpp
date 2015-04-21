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
#include "SloongModelLoader.h"
#include "SloongString.h"
#include "DInputClass.h"
#include "SloongCamera.h"
#pragma comment(lib,"Universal.lib");
using namespace SoaringLoong;
using namespace SoaringLoong::Graphics;
using namespace SoaringLoong::Universal;
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
};

#define NUM_TOWERS        96
#define NUM_TANKS         24

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
		m_nHeight = SCREEN_HEIGHT;
		m_nWidth = SCREEN_WIDTH;
	}
	else
	{
		dwStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
		m_nHeight = WINDOW_HEIGHT;
		m_nWidth = WINDOW_WIDTH;
	}

	m_hMainWnd = CreateWindow(m_strWindowClass, m_strTitle, dwStyle,
		0, 0, m_nWidth, m_nHeight, NULL, NULL, hInstance, NULL);

	RECT winRect = { 0, 0, m_nWidth, m_nHeight };

	AdjustWindowRectEx(&winRect, GetWindowLong(m_hMainWnd, GWL_STYLE), GetMenu(m_hMainWnd) == NULL, GetWindowLong(m_hMainWnd, GWL_EXSTYLE));

	MoveWindow(m_hMainWnd, 0, 0, winRect.right - winRect.left, winRect.bottom - winRect.top, TRUE);

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

		//CWinConsole::StartConsole(m_hInst, g_pLua);

		//  		m_pD3D = new CSloongD3D();
		//  		m_pD3D->Init(m_hMainWnd, m_hInst);
		m_pInput = new DInputClass();
		m_pInput->Init(m_hMainWnd, m_hInst, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE); //前台，非独占模式
		m_pDraw = new CDDraw();
		m_pDraw->Initialize(m_hMainWnd, m_nWidth, m_nHeight, SCREEN_BPP, FULLSCREEN);

		g_pUIManager = new CUIManager();

		//g_pLua->RunScript(_T("Start.lua"));
		CMath2::Build_Sin_Cos_Tables();

		CPLGLoader plgLoader(m_pDraw);


		Section7Test3Init();
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
		GetSloongUIManager()->GetCurrentUI()->SetEventHandler(handlerName);
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
	Section7Test3Render();
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
using namespace SoaringLoong::Loader;

void CSloongGame::Section7Test1Init()
{
	// initialize a single polygon
	auto poly = &poly1[0];
	poly->state = POLY4DV1_STATE_ACTIVE;
	poly->attr = 0;
	poly->color = RGB(0, 255, 0);
	poly->worldPos = { 0, 0, 0, 1 };
	poly->vlist[0] = { 0, 50, 0, 1 };
	poly->vlist[1] = { 10, 0, 10, 1 };
	poly->vlist[2] = { 10, 0, -10, 1 };
	poly->prev = NULL;
	poly->next = &poly1[1];
	poly = &poly1[1];
	poly->state = POLY4DV1_STATE_ACTIVE;
	poly->attr = 0;
	poly->color = RGB(0, 255, 0);
	poly->worldPos = { 0, 0, 0, 1 };
	poly->vlist[0] = { 0, 50, 0, 1 };
	poly->vlist[1] = { 10, 0, 10, 1 };
	poly->vlist[2] = { -10, 0, 10, 1 };
	poly->prev = &poly1[0];
	poly->next = &poly1[2];
	poly = &poly1[2];
	poly->state = POLY4DV1_STATE_ACTIVE;
	poly->attr = 0;
	poly->color = RGB(0, 255, 0);
	poly->worldPos = { 0, 0, 0, 1 };
	poly->vlist[0] = { 0, 50, 0, 1 };
	poly->vlist[1] = { 10, 0, -10, 1 };
	poly->vlist[2] = { -10, 0, -10, 1 };
	poly->prev = &poly1[1];
	poly->next = &poly1[3];
	poly = &poly1[3];
	poly->state = POLY4DV1_STATE_ACTIVE;
	poly->attr = 0;
	poly->color = RGB(0, 255, 0);
	poly->worldPos = { 0, 0, 0, 1 };
	poly->vlist[0] = { 0, 50, 0, 1 };
	poly->vlist[1] = { -10, 0, -10, 1 };
	poly->vlist[2] = { -10, 0, 10, 1 };
	poly->prev = &poly1[2];
	poly->next = NULL;


	POINT4D  cam_pos = { 0, 0, -200, 1 };
	POINT4D  cam_target = { 0, 0, 0, 1 };
	VECTOR4D cam_dir = { 0, 0, 0, 1 };
	VECTOR4D cv = { 0, 0, 1, 1 };

	m_pCam = new CCamera();
	m_pCam->Initialize(CAMERA_TYPE::CAMERA_ELUER, cam_pos, cam_dir, &cam_target, &cv, false, 5, 50, 90, WINDOW_WIDTH, WINDOW_HEIGHT);
	m_pCam->UpdateCameraMatrix();

	// initialize the camera with 90 FOV, normalized coordinates
	/*CPLGLoader::Init_CAM4DV1(&cam,      // the camera object
	CAM_MODEL_EULER, // euler camera model
	&cam_pos,  // initial camera position
	&cam_dir,  // initial camera angles
	NULL,      // no initial target
	50.0,      // near and far clipping planes
	500.0,
	90.0,      // field of view in degrees
	WINDOW_WIDTH,   // size of final screen viewport
	WINDOW_HEIGHT);*/

}

void CSloongGame::Section7Test1Render()
{
	m_pDraw->Start_Clock();
	static MATRIX4X4 mrot; // general rotation matrix
	static float ang_y = 0;      // rotation angle

	int index; // looping var

	// read keyboard and other devices here
	m_pInput->GetInput();
	m_pCam->UpdateCameraMatrix();

	// initialize the renderlist
	CPLGLoader::Reset_RENDERLIST4DV1(&rend_list);

	// insert polygon into the renderlist
	for (int i = 0; i < ARRAYSIZE(poly1); i++)
	{
		CPLGLoader::Insert_POLYF4DV1_RENDERLIST4DV1(&rend_list, &poly1[i]);
	}


	// generate rotation matrix around y axis
	//CPLGLoader::Build_XYZ_Rotation_MATRIX4X4(0, ang_y, 0, &mrot);

	// rotate polygon slowly
	//if (++ang_y >= 360.0) ang_y = 0;



	// rotate the local coords of single polygon in renderlist
	// 对渲染列表的多边形执行旋转
	//CPLGLoader::Transform_RENDERLIST4DV1(&rend_list, &mrot, TRANSFORM_LOCAL_ONLY);

	//CPLGLoader::Transform_RENDERLIST4DV1(&rend_list, &m_pCam->MatrixCamera, TRANSFORM_TRANS_ONLY);

	// perform local/model to world transform
	// 模型坐标到世界坐标的转换
	CPLGLoader::Model_To_World_RENDERLIST4DV1(&rend_list, 2);


	// generate camera matrix
	// 构建相机矩阵
	//CPLGLoader::Build_CAM4DV1_Matrix_Euler(&cam, CAM_ROT_SEQ_ZYX);

	// apply world to camera transform
	// 世界坐标到相机坐标的转换
	CPLGLoader::World_To_Camera_RENDERLIST4DV1(&rend_list, &m_pCam->MatrixCamera);
	//CPLGLoader::World_To_Camera_RENDERLIST4DV1(&rend_list, &cam.mcam);

	// apply camera to perspective transformation
	// 应用相机坐标到透视的转换
	CPLGLoader::Camera_To_Perspective_RENDERLIST4DV1(&rend_list, &cam);

	// apply screen transform
	//	CPLGLoader::Perspective_To_Screen_RENDERLIST4DV1(&rend_list, &cam);

	for (int j = 0; j < 4; ++j)
	{
		auto obj = &rend_list.poly_data[j];
		for (int i = 0; i < 3; ++i)
		{

			obj->tvlist[i].x *= SCREEN_WIDTH;
			obj->tvlist[i].x += (SCREEN_WIDTH / 2);
			obj->tvlist[i].y *= SCREEN_WIDTH;
			obj->tvlist[i].y += (SCREEN_WIDTH / 2) - 100;
		}
	}

	// draw instructions
	m_pDraw->DrawText(_T("Press ESC to exit."), 0, 0, RGB(0, 255, 0), m_pDraw->GetBackSurface());

	// lock the back buffer
	LPBYTE lpBack = m_pDraw->DDraw_Lock_Back_Surface();

	// render the polygon list
	CPLGLoader::Draw_RENDERLIST4DV1_Wire16(&rend_list, lpBack, m_pDraw->GetBackPitch(), m_rcWindow);

	// unlock the back buffer
	m_pDraw->DDraw_Unlock_Back_Surface();

	m_pDraw->Wait_Clock(30);

	// check of user is trying to exit
	if (KEY_DOWN(VK_ESCAPE) || m_pInput->IsKeyDown(DIK_ESCAPE))
	{
		PostQuitMessage(0);

	} // end if

	if (m_pInput->IsKeyDown(DIK_SPACE))
	{
		m_pCam->WorldPos.y += 10;
	}

	if (m_pInput->IsKeyDown(DIK_NUMPAD0))
	{
		m_pCam->WorldPos.y -= 10;
	}

	if (m_pInput->IsKeyDown(DIK_UP))
	{
		// move forward
		m_pCam->WorldPos.z += 10;

	} // end if

	if (m_pInput->IsKeyDown(DIK_DOWN))
	{
		// move backward
		m_pCam->WorldPos.z -= 10;

	} // end if

	// rotate
	if (m_pInput->IsKeyDown(DIK_RIGHT))
	{
		m_pCam->WorldPos.x += 3;
		//m_pCam->WorldPos.x += CMath2::Fast_Sin(m_pCam->Direction.y);
		//m_pCam->WorldPos.z += CMath2::Fast_Cos(m_pCam->Direction.y);
		m_pCam->Direction.x += 3;

		// 		add a little turn to object
		// 		if ((turning += 2) > 15)
		// 			turning = 15;

	} // end if

	if (m_pInput->IsKeyDown(DIK_LEFT))
	{
		m_pCam->WorldPos.x -= 3;
		//m_pCam->WorldPos.x -= CMath2::Fast_Sin(m_pCam->Direction.y);
		//m_pCam->WorldPos.z -= CMath2::Fast_Cos(m_pCam->Direction.y);
		m_pCam->Direction.x -= 3;

		// add a little turn to object
		// 		if ((turning -= 2) < -15)
		// 			turning = -15;

	} // end if
}

void CSloongGame::Section7Test6Init()
{

	vscale = { 1, 1, 1, 1 }, vpos = { 0, 0, 0, 1 }, vrot = { 0, 0, 0, 1 };

	// initialize camera 
	POINT4D  cam_pos = { 0, 40, 0, 1 };
	POINT4D  cam_target = { 0, 0, 0, 1 };
	VECTOR4D cam_dir = { 0, 0, 0, 1 };

	CMath2::Build_Sin_Cos_Tables();



	/*	CPLGLoader::Init_CAM4DV1(&cam,      // the camera object
	CAM_MODEL_EULER, // the euler model
	&cam_pos,  // initial camera position
	&cam_dir,  // initial camera angles
	&cam_target,      // no target
	200.0,      // near and far clipping planes
	12000.0,
	120.0,      // field of view in degrees
	WINDOW_WIDTH,   // size of final screen viewport
	WINDOW_HEIGHT);*/


	CPLGLoader plgLoader(m_pDraw);

	// load the master tank object
	vscale.Initialize(0.75, 0.75, 0.75);
	plgLoader.LoadOBJECT4DV1(&obj_tank, _T("tank2.plg"), &vscale, &vpos, &vrot);

	// load player object for 3rd person view
	vscale.Initialize(0.75, 0.75, 0.75);
	plgLoader.LoadOBJECT4DV1(&obj_player, _T("tank3.plg"), &vscale, &vpos, &vrot);

	// load the master tower object
	vscale.Initialize(1.0, 2.0, 1.0);
	plgLoader.LoadOBJECT4DV1(&obj_tower, _T("tower1.plg"), &vscale, &vpos, &vrot);

	// load the master ground marker
	vscale.Initialize(3.0, 3.0, 3.0);
	plgLoader.LoadOBJECT4DV1(&obj_marker, _T("marker1.plg"), &vscale, &vpos, &vrot);

#define UNIVERSE_RADIUS   4000

	// position the tanks
	for (int index = 0; index < NUM_TANKS; index++)
	{
		// randomly position the tanks
		tanks[index].x = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
		tanks[index].y = 0; // obj_tank.max_radius;
		tanks[index].z = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
		tanks[index].w = RAND_RANGE(0, 360);
	} // end for


	// position the towers
	for (int index = 0; index < NUM_TOWERS; index++)
	{
		// randomly position the tower
		towers[index].x = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
		towers[index].y = 0; // obj_tower.max_radius;
		towers[index].z = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
	} // end for

}

void CSloongGame::Section7Test6Render()
{

	static MATRIX4X4 mrot;   // general rotation matrix

	// these are used to create a circling camera
	static float view_angle = 0;
	static float camera_distance = 6000;
	static VECTOR4D pos = { 0, 0, 0, 0 };
	static float tank_speed;
	static float turning = 0;

	char work_string[256]; // temp string

	int index; // looping var

	// draw the sky
	m_pDraw->Draw_Rectangle(0, 0, WINDOW_WIDTH - 1, WINDOW_HEIGHT / 2, RGB(0, 140, 192), m_pDraw->GetBackSurface());

	// draw the ground
	m_pDraw->Draw_Rectangle(0, WINDOW_HEIGHT / 2, WINDOW_WIDTH - 1, WINDOW_HEIGHT - 1, RGB(103, 62, 3), m_pDraw->GetBackSurface());

	// read keyboard and other devices here
	//DInput_Read_Keyboard();
	m_pInput->GetInput();
	// game logic here...

	// reset the render list
	CPLGLoader::Reset_RENDERLIST4DV1(&rend_list);

	// allow user to move camera
#define TANK_SPEED        15
	// turbo
	if (m_pInput->IsKeyDown(DIK_SPACE))
		tank_speed = 5 * TANK_SPEED;
	else
		tank_speed = TANK_SPEED;

	// forward/backward
	if (m_pInput->IsKeyDown(DIK_UP))
	{
		// move forward
		cam.pos.x += tank_speed*CMath2::Fast_Sin(cam.dir.y);
		cam.pos.z += tank_speed*CMath2::Fast_Cos(cam.dir.y);
	} // end if

	if (m_pInput->IsKeyDown(DIK_DOWN))
	{
		// move backward
		cam.pos.x -= tank_speed*CMath2::Fast_Sin(cam.dir.y);
		cam.pos.z -= tank_speed*CMath2::Fast_Cos(cam.dir.y);
	} // end if

	// rotate
	if (m_pInput->IsKeyDown(DIK_RIGHT))
	{
		cam.dir.y += 3;

		// add a little turn to object
		if ((turning += 2) > 15)
			turning = 15;

	} // end if

	if (m_pInput->IsKeyDown(DIK_LEFT))
	{
		cam.dir.y -= 3;

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
	CPLGLoader::Build_CAM4DV1_Matrix_Euler(&cam, CAM_ROT_SEQ_ZYX);

	// insert the tanks in the world
	for (index = 0; index < NUM_TANKS; index++)
	{
		// reset the object (this only matters for backface and object removal)
		CPLGLoader::Reset_OBJECT4DV1(&obj_tank);

		// generate rotation matrix around y axis
		CPLGLoader::Build_XYZ_Rotation_MATRIX4X4(0, tanks[index].w, 0, &mrot);

		// rotate the local coords of the object
		CPLGLoader::Transform_OBJECT4DV1(&obj_tank, &mrot, TRANSFORM_LOCAL_TO_TRANS, 1);

		// set position of tank
		obj_tank.world_pos.x = tanks[index].x;
		obj_tank.world_pos.y = tanks[index].y;
		obj_tank.world_pos.z = tanks[index].z;

		// attempt to cull object   
		if (!CPLGLoader::Cull_OBJECT4DV1(&obj_tank, &cam, CULL_OBJECT_XYZ_PLANES))
		{
			// if we get here then the object is visible at this world position
			// so we can insert it into the rendering list
			// perform local/model to world transform
			CPLGLoader::Model_To_World_OBJECT4DV1(&obj_tank, TRANSFORM_TRANS_ONLY);

			// insert the object into render list
			CPLGLoader::Insert_OBJECT4DV1_RENDERLIST4DV1(&rend_list, &obj_tank, 0);
		} // end if

	} // end for

	// insert the player into the world
	// reset the object (this only matters for backface and object removal)
	CPLGLoader::Reset_OBJECT4DV1(&obj_player);

	// set position of tank
	obj_player.world_pos.x = cam.pos.x + 300 * CMath2::Fast_Sin(cam.dir.y);
	obj_player.world_pos.y = cam.pos.y - 70;
	obj_player.world_pos.z = cam.pos.z + 300 * CMath2::Fast_Cos(cam.dir.y);

	// generate rotation matrix around y axis
	CPLGLoader::Build_XYZ_Rotation_MATRIX4X4(0, cam.dir.y + turning, 0, &mrot);

	// rotate the local coords of the object
	CPLGLoader::Transform_OBJECT4DV1(&obj_player, &mrot, TRANSFORM_LOCAL_TO_TRANS, 1);

	// perform world transform
	CPLGLoader::Model_To_World_OBJECT4DV1(&obj_player, TRANSFORM_TRANS_ONLY);

	// insert the object into render list
	CPLGLoader::Insert_OBJECT4DV1_RENDERLIST4DV1(&rend_list, &obj_player, 0);


	// insert the towers in the world
	for (index = 0; index < NUM_TOWERS; index++)
	{
		// reset the object (this only matters for backface and object removal)
		CPLGLoader::Reset_OBJECT4DV1(&obj_tower);

		// set position of tower
		obj_tower.world_pos.x = towers[index].x;
		obj_tower.world_pos.y = towers[index].y;
		obj_tower.world_pos.z = towers[index].z;

		// attempt to cull object   
		if (!CPLGLoader::Cull_OBJECT4DV1(&obj_tower, &cam, CULL_OBJECT_XYZ_PLANES))
		{
			// if we get here then the object is visible at this world position
			// so we can insert it into the rendering list
			// perform local/model to world transform
			CPLGLoader::Model_To_World_OBJECT4DV1(&obj_tower, 0);

			// insert the object into render list
			CPLGLoader::Insert_OBJECT4DV1_RENDERLIST4DV1(&rend_list, &obj_tower, 0);
		} // end if

	} // end for

	// seed number generator so that modulation of markers is always the same
	srand(13);

#define POINT_SIZE        200
#define NUM_POINTS_X      (2*UNIVERSE_RADIUS/POINT_SIZE)
#define NUM_POINTS_Z      (2*UNIVERSE_RADIUS/POINT_SIZE)
#define NUM_POINTS        (NUM_POINTS_X*NUM_POINTS_Z)
	// insert the ground markers into the world
	for (int index_x = 0; index_x < NUM_POINTS_X; index_x++)
	for (int index_z = 0; index_z < NUM_POINTS_Z; index_z++)
	{
		// reset the object (this only matters for backface and object removal)
		CPLGLoader::Reset_OBJECT4DV1(&obj_marker);

		// set position of tower
		obj_marker.world_pos.x = RAND_RANGE(-100, 100) - UNIVERSE_RADIUS + index_x*POINT_SIZE;
		obj_marker.world_pos.y = obj_marker.max_radius;
		obj_marker.world_pos.z = RAND_RANGE(-100, 100) - UNIVERSE_RADIUS + index_z*POINT_SIZE;

		// attempt to cull object   
		if (!CPLGLoader::Cull_OBJECT4DV1(&obj_marker, &cam, CULL_OBJECT_XYZ_PLANES))
		{
			// if we get here then the object is visible at this world position
			// so we can insert it into the rendering list
			// perform local/model to world transform
			CPLGLoader::Model_To_World_OBJECT4DV1(&obj_marker, 0);

			// insert the object into render list
			CPLGLoader::Insert_OBJECT4DV1_RENDERLIST4DV1(&rend_list, &obj_marker, 0);
		} // end if

	} // end for

	// remove backfaces
	CPLGLoader::Remove_Backfaces_RENDERLIST4DV1(&rend_list, &cam);

	// apply world to camera transform
	CPLGLoader::World_To_Camera_RENDERLIST4DV1(&rend_list, &cam);

	// apply camera to perspective transformation
	CPLGLoader::Camera_To_Perspective_RENDERLIST4DV1(&rend_list, &cam);

	// apply screen transform
	CPLGLoader::Perspective_To_Screen_RENDERLIST4DV1(&rend_list, &cam);

	sprintf_s(work_string, 256, "pos:[%f, %f, %f] heading:[%f] elev:[%f]",
		cam.pos.x, cam.pos.y, cam.pos.z, cam.dir.y, cam.dir.x);

	CString str(work_string);
	m_pDraw->DrawText(str.GetString().c_str(), 0, WINDOW_HEIGHT - 20, RGB(0, 255, 0), m_pDraw->GetBackSurface());

	// draw instructions
	m_pDraw->DrawText(_T("Press ESC to exit. Press Arrow Keys to Move. Space for TURBO."), 0, 0, RGB(0, 255, 0), m_pDraw->GetBackSurface());

	// lock the back buffer
	LPBYTE pBackBuffer = m_pDraw->DDraw_Lock_Back_Surface();

	// render the object
	CPLGLoader::Draw_RENDERLIST4DV1_Wire16(&rend_list, pBackBuffer, m_pDraw->GetBackPitch(), m_rcWindow);

	// unlock the back buffer
	m_pDraw->DDraw_Unlock_Back_Surface();

	// flip the surfaces
	//	DDraw_Flip();

	// sync to 30ish fps
	//	Wait_Clock(30);

	// check of user is trying to exit
	// 	if (KEY_DOWN(VK_ESCAPE) || keyboard_state[DIK_ESCAPE])
	// 	{
	// 		PostMessage(main_window_handle, WM_DESTROY, 0, 0);
	// 	} // end if
}

void CSloongGame::Section7Test2Init()
{
	// initialize a single polygon
	auto poly = &poly1[0];
	poly->state = POLY4DV1_STATE_ACTIVE;
	poly->attr = 0;
	poly->color = RGB(0, 255, 0);
	poly->worldPos = { 0, 0, 0, 1 };
	poly->vlist[0] = { 0, 50, 0, 1 };
	poly->vlist[1] = { 10, 0, 10, 1 };
	poly->vlist[2] = { 10, 0, -10, 1 };
	poly->prev = NULL;
	poly->next = &poly1[1];
	poly = &poly1[1];
	poly->state = POLY4DV1_STATE_ACTIVE;
	poly->attr = 0;
	poly->color = RGB(0, 255, 0);
	poly->worldPos = { 0, 0, 0, 1 };
	poly->vlist[0] = { 0, 50, 0, 1 };
	poly->vlist[1] = { 10, 0, 10, 1 };
	poly->vlist[2] = { -10, 0, 10, 1 };
	poly->prev = &poly1[0];
	poly->next = &poly1[2];
	poly = &poly1[2];
	poly->state = POLY4DV1_STATE_ACTIVE;
	poly->attr = 0;
	poly->color = RGB(0, 255, 0);
	poly->worldPos = { 0, 0, 0, 1 };
	poly->vlist[0] = { 0, 50, 0, 1 };
	poly->vlist[1] = { 10, 0, -10, 1 };
	poly->vlist[2] = { -10, 0, -10, 1 };
	poly->prev = &poly1[1];
	poly->next = &poly1[3];
	poly = &poly1[3];
	poly->state = POLY4DV1_STATE_ACTIVE;
	poly->attr = 0;
	poly->color = RGB(0, 255, 0);
	poly->worldPos = { 0, 0, 0, 1 };
	poly->vlist[0] = { 0, 50, 0, 1 };
	poly->vlist[1] = { -10, 0, -10, 1 };
	poly->vlist[2] = { -10, 0, 10, 1 };
	poly->prev = &poly1[2];
	poly->next = NULL;


	POINT4D  cam_pos = { 0, 0, -200, 1 };
	POINT4D  cam_target = { 0, 0, 5, 1 };
	VECTOR4D cam_dir = { 0, 0, 0, 1 };
	VECTOR4D cv = { 0, 0, 1, 1 };

	m_pCam = new CCamera();
	m_pCam->Initialize(CAMERA_TYPE::CAMERA_ELUER, cam_pos, cam_dir, &cam_target, &cv, false, 5, 50, 90, WINDOW_WIDTH, WINDOW_HEIGHT);
	m_pCam->UpdateCameraMatrix();

	// 缓存透视和屏幕变换矩阵
	// 透视变换矩阵
	m_pCam->UpdateProjectMatrix();
	// 屏幕变换矩阵
	m_pCam->UpdateScreenMatrix();

}

void CSloongGame::Section7Test2Render()
{
	if (KEY_DOWN(VK_DOWN))
	{
		m_pCam->WorldPos.z -= 1;
	}
	if (KEY_DOWN(VK_UP))
	{
		m_pCam->WorldPos.z += 1;
	}
	if (KEY_DOWN(VK_LEFT))
	{
		m_pCam->WorldPos.x -= 1;
	}
	if (KEY_DOWN(VK_RIGHT))
	{
		m_pCam->WorldPos.x += 1;
	}
	if (KEY_DOWN(VK_SPACE))
	{
		m_pCam->WorldPos.y += 1;
	}
	if (KEY_DOWN(VK_NUMPAD0))
	{
		m_pCam->WorldPos.y -= 1;
	}
	m_pCam->UpdateCameraMatrix();

	for (int i = 0; i < ARRAYSIZE(poly1); i++)
	{
		CPLGLoader::Insert_POLYF4DV1_RENDERLIST4DV1(&rend_list, &poly1[i]);
	}

	/*****************************
	3D流水线
	*****************************/
	// 世界变换
	CPLGLoader::Model_To_World_RENDERLIST4DV1(&rend_list, 2);


	// 每次旋转一下物体

	// 背面消除
	//ObjectDeleteBackface(&g_Obj, &g_Camera);

	// 相机变换
	CPLGLoader::World_To_Camera_RENDERLIST4DV1(&rend_list, &m_pCam->MatrixCamera);

	// 投影变换
	CPLGLoader::Camera_To_Perspective_RENDERLIST4DV1(&rend_list, m_pCam);

	// 视口变换
	CPLGLoader::Perspective_To_Screen_RENDERLIST4DV1(&rend_list, m_pCam);

	// 绘制物体
	LPBYTE lpBack = m_pDraw->DDraw_Lock_Back_Surface();

	// render the polygon list
	CPLGLoader::Draw_RENDERLIST4DV1_Wire16(&rend_list, lpBack, m_pDraw->GetBackPitch(), m_rcWindow);

	// unlock the back buffer
	m_pDraw->DDraw_Unlock_Back_Surface();

	m_pDraw->Wait_Clock(30);

}

SoaringLoong::Loader::RENDERLIST4DV1 CSloongGame::rend_list;

#include "SloongObject3D.h"
using namespace SoaringLoong::Graphics3D;

CObject3D g_obj;
CVector4D vWorldPos = { 0, 0, 0, 0 };
void CSloongGame::Section7Test3Init()
{
	CVector4D v1[] = {
		{ 10, 0, 10, 0 },
		{ 10, 0, -10, 0 },
		{ 0, 50, 0, 0 },
	};
	CVector4D v2[] = {
		{ 10, 0, -10, 0 },
		{ -10, 0, -10, 0 },
		{ 0, 50, 0, 0 },
	};
	CVector4D v3[] = {
		{ -10, 0, -10, 0 },
		{ -10, 0, 10, 0 },
		{ 0, 50, 0, 0 },
	};
	CVector4D v4[] = {
		{ -10, 0, 10, 0 },
		{ 10, 0, 10, 0 },
		{ 0, 50, 0, 0 },
	};
	CPolygon3D poly[4];
	poly[0].m_VectorList.push_back(v1[0]);
	poly[0].m_VectorList.push_back(v1[1]);
	poly[0].m_VectorList.push_back(v1[2]);

	poly[1].m_VectorList.push_back(v1[0]);
	poly[1].m_VectorList.push_back(v1[1]);
	poly[1].m_VectorList.push_back(v1[2]);

	poly[2].m_VectorList.push_back(v1[0]);
	poly[2].m_VectorList.push_back(v1[1]);
	poly[2].m_VectorList.push_back(v1[2]);

	poly[3].m_VectorList.push_back(v1[0]);
	poly[3].m_VectorList.push_back(v1[1]);
	poly[3].m_VectorList.push_back(v1[2]);

	g_obj.m_VertexList.push_back(poly[0]);
	g_obj.m_VertexList.push_back(poly[1]);
	g_obj.m_VertexList.push_back(poly[2]);
	g_obj.m_VertexList.push_back(poly[3]);

	CVector4D vPos = { 0, 0, 100, 0 };
	CVector4D vDir = { 0, 0, 0, 0 };

	m_pCam = new CCamera();
	m_pCam->Initialize(CAMERA_UVN, vPos, vDir, NULL, NULL, NULL, 5, 50, 90, SCREEN_WIDTH, SCREEN_HEIGHT);
	m_pCam->UpdateCameraMatrix();
	m_pCam->UpdateProjectMatrix();
	m_pCam->UpdateScreenMatrix();

	g_obj.m_pCamera = m_pCam;
	g_obj.m_pCameraMatrix = &m_pCam->MatrixCamera;
	g_obj.m_pProjectMatrix = &m_pCam->MatrixProjection;
	g_obj.m_pScreenMatrix = &m_pCam->MatrixScreen;
	g_obj.m_pWorldPos = &vWorldPos;
}

void CSloongGame::Section7Test3Render()
{
	m_pCam->UpdateCameraMatrix();
	m_pCam->UpdateProjectMatrix();
	m_pCam->UpdateScreenMatrix();

	g_obj.Update();
	g_obj.Render(m_pDraw);

}
