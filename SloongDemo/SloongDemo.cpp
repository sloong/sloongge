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
#include "SloongEngine.h"
#pragma comment(lib,"Universal.lib")
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
		m_pEngine = new CSloongEngine();
		m_pEngine->SetEnentHandler(SendEvent);

		//CWinConsole::StartConsole(m_hInst, g_pLua);

		//  		m_pD3D = new CSloongD3D();
		//  		m_pD3D->Init(m_hMainWnd, m_hInst);
		m_pInput = new DInputClass();
		m_pInput->Init(m_hMainWnd, m_hInst, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE); //前台，非独占模式
		m_pDraw = new CDDraw();
		m_pDraw->Initialize(m_hMainWnd, m_nWidth, m_nHeight, SCREEN_BPP, FULLSCREEN);

		g_pUIManager = new CUIManager();
		g_pUIManager->Initialize(m_pDraw, g_pLua, m_pLog,m_hMainWnd);

		g_pLua->RunScript(_T("Start.lua"));
		CMath2::Build_Sin_Cos_Tables();
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
	GetSloongUIManager()->Update();
	m_pDraw->DDrawFillBackSurface(0);

	if (KEY_DOWN(VK_ESCAPE))
	{
		PostMessage(m_hMainWnd, WM_DESTROY, 0, 0);

		m_pDraw->Screen_Transitions(SCREEN_DARKNESS, NULL, 0);

	}
	GetSloongUIManager()->GetCurrentUI()->Render();
	//Section7Test3Render();
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

