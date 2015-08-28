#include "stdafx.h"
#include "ConsoleWindow.h"
#include "univ\\lua.h"
#include "Resource.h"

using namespace Sloong::Universal;

#ifndef GWL_WNDPROC
#define GWL_WNDPROC         (-4)
#endif // !GWL_WNDPROC

LuaFunctionRegistr DebugGlue[] = 
{
	{ _T("Print"), Debug_Print },
	{ _T("print"), Debug_Print },

	{ NULL, NULL }
};

CWinConsole* Sloong::g_Console = NULL;


volatile bool CWinConsole::m_bWinIsActive = true;
volatile HWND CWinConsole::m_hWnd = NULL;
volatile HWND CWinConsole::m_hParentWnd = NULL;
volatile HWND CWinConsole::m_hEditControl = NULL;
CRect CWinConsole::m_rcWindow;

TCHAR CWinConsole::m_CommandBuffer[4096];

WNDPROC lpfnInputEdit;  // Storage for subclassed edit control 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HWND CWinConsole::StartConsole(HINSTANCE hInstance, HWND hWnd, CRect rcWindow, CLua *pScriptContext)
{
	if (!g_Console)
	{
		g_Console = new CWinConsole();
	}

	m_hParentWnd = hWnd;
	m_rcWindow = rcWindow;
	if (!m_hWnd)
		g_Console->Init(hInstance);

	g_Console->m_pScriptContext = pScriptContext;
	// init the glue functions required for the debug window
	for (int i = 0; DebugGlue[i].strFunctionName; i++)
	{
		pScriptContext->AddFunction(DebugGlue[i].strFunctionName, DebugGlue->pFunction);
	}

	return (m_hWnd);
}

void CWinConsole::StopConsole()
{
	if (!g_Console)
	{
		delete g_Console;
		g_Console = NULL;
	}
}


CWinConsole::CWinConsole()
{
	m_hWnd = NULL;
	memset(m_CommandBuffer, 0, 4096);

}

CWinConsole::~CWinConsole()
{
	if (!m_hWnd)
	{
		CloseWindow(m_hWnd);
	}
}

void CWinConsole::AdjustScrollBar(void)
{
	SCROLLINFO si;

	si.cbSize = sizeof(si);
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	si.nMin = 0;
	si.nMax = m_stringList.size();
	si.nPage = m_textAreaHeight;
	si.nPos = m_stringList.size() - m_ScrollyPos;
	SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);
}


void CWinConsole::ResizeControls(void)
{
	RECT r = m_rcWindow;

	m_textAreaHeight = (r.bottom - r.top) / 16;

	SetWindowPos(m_hEditControl, HWND_TOP, r.left + 2, r.bottom - 18, r.right - r.left - 4, 16, SWP_NOZORDER);

	AdjustScrollBar();
	InvalidateRect(m_hWnd, m_rcWindow, TRUE);
}

void CWinConsole::Paint(HDC hDC)
{
	SetTextColor(hDC, RGB(255, 255, 255));
	SetBkColor(hDC, RGB(0, 0, 0));

	RECT r = m_rcWindow;

	int x = 2;
	int y = r.bottom - 40;

	auto it = m_stringList.begin();
	int skip = m_ScrollyPos;
	while (skip)
	{
		++it;
		--skip;
	}

	while (it != m_stringList.end())
	{
		TextOut(hDC, x, y, (*it).c_str(), _tcslen((*it).c_str()));
		y -= 16;
		++it;
	}
}


LRESULT WINAPI CWinConsole::MsgProc(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (uMsg)
	{
	case WM_ACTIVATEAPP:
		m_bWinIsActive = (wParam != 0);
		return 0L;

	case WM_ACTIVATE:
		// Pause if minimized or not the top window
		m_bWinIsActive = (wParam == WA_ACTIVE) || (wParam == WA_CLICKACTIVE);
		return 0L;

	case WM_DESTROY:
		m_bWinIsActive = false;
		m_hWnd = NULL;
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		g_Console->Paint(hdc);
		EndPaint(hWnd, &ps);
		break;

	case WM_CHAR:
		break;

	case WM_LBUTTONDOWN:
		break;

	case WM_RBUTTONDOWN:
		break;

	case WM_LBUTTONUP:
		break;

	case WM_RBUTTONUP:
		break;

	case WM_MOUSEMOVE:
		break;

	case WM_COMMAND:
		break;

	case WM_SIZING:
	case WM_SIZE:
		g_Console->ResizeControls();
		break;

	case WM_SETCURSOR:
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		ShowCursor(TRUE);
		break;

	case WM_VSCROLL:
		switch (wParam & 0xFFFF)//LOWORD (wParam)) 
		{
			// User clicked the shaft above the scroll box. 

		case SB_PAGEUP:
			g_Console->m_ScrollyPos = min((int)(g_Console->m_ScrollyPos + g_Console->m_textAreaHeight), (int)(g_Console->m_stringList.size() - g_Console->m_textAreaHeight) + 1);
			//yInc = min(-1, -yClient / yChar); 
			break;

			// User clicked the shaft below the scroll box. 

		case SB_PAGEDOWN:
			// yInc = max(1, yClient / yChar); 
			break;

			// User clicked the top arrow. 

		case SB_LINEUP:
			g_Console->m_ScrollyPos = min(g_Console->m_ScrollyPos + 1, (int)(g_Console->m_stringList.size() - g_Console->m_textAreaHeight) + 1);
			break;

			// User clicked the bottom arrow. 

		case SB_LINEDOWN:
			//yInc = 1; 
			g_Console->m_ScrollyPos = max(g_Console->m_ScrollyPos - 1, 0);
			break;

			// User dragged the scroll box. 

		case SB_THUMBTRACK:
			//yInc = HIWORD(wParam) - yPos; 
			break;

		default:
			//yInc = 0;
			break;

		}
		{
			SCROLLINFO si;
			si.cbSize = sizeof(si);
			si.fMask = SIF_POS;
			si.nPos = g_Console->m_stringList.size() - g_Console->m_ScrollyPos;
			SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
		}
		InvalidateRect(m_hWnd, NULL, TRUE);

		break;

	case WM_USER:
		// command ready from edit control
		// string should be in m_CommandBuffer
		Write(m_CommandBuffer);

		CString str(m_CommandBuffer);
		
		if (0 != luaL_loadbuffer(g_Console->m_pScriptContext->GetScriptContext(), str.GetStringA().c_str(), strlen(str.GetStringA().c_str()), NULL))
		{
			Write(_T("Error loading Command\n"));
		}
		if (0 != lua_pcall(g_Console->m_pScriptContext->GetScriptContext(), 0, LUA_MULTRET, 0))
		{
			Write(_T("Error in Command\n"));

			str = luaL_checkstring(g_Console->m_pScriptContext->GetScriptContext(), -1);
			Write(str.GetString().c_str());
		}
		// clear buffer when done processing
		memset(m_CommandBuffer, 0, 4096*sizeof(TCHAR));
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


void CWinConsole::Write( LPCTSTR pString)
{
	if (g_Console && m_hWnd)
	{
		//remove any linefeed chars (look goofy in log)
		LPTSTR buf = new TCHAR[_tcslen(pString) + 1];
		int indx = 0;
		for (int i = 0; i < (int)_tcslen(pString); i++)
		{
			if (pString[i] != 10)
			{
				buf[indx++] = pString[i];
			}
			else
			{
				buf[indx] = 0;
				g_Console->m_stringList.push_front(buf);
				indx = 0;
			}


		}
		if (indx > 0)
		{
			buf[indx] = 0;
			g_Console->m_stringList.push_front(buf);
		}
		InvalidateRect(m_hWnd, NULL, TRUE);
		delete buf;
	}

	g_Console->AdjustScrollBar();
}


LRESULT CALLBACK CWinConsole::SubclassInputEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CHAR:              // Found a RETURN keystroke!             
		if ((TCHAR)wParam == VK_RETURN)
		{
			// get the command string
			long lSizeofString;
			// Get the size of the string
			lSizeofString = SendMessage(hWnd, WM_GETTEXTLENGTH, 0, 0);

			// Get the string                 
			SendMessage(hWnd, WM_GETTEXT, lSizeofString + 1, (LPARAM)m_CommandBuffer);

			// send message to parent that command was entered
			SendMessage(m_hWnd, WM_USER, 0, lSizeofString);

			// clear the edit string
			SendMessage(hWnd, WM_SETTEXT, 0, (long) "");
			return 1;
		}
	}
	return CallWindowProc(lpfnInputEdit, hWnd, message, wParam, lParam);
}

void CWinConsole::Init(HINSTANCE hInstance)
{
	m_hInstance = hInstance;
	m_ScrollyPos = 0;
	// create application handler and link to our WindowProc
	WNDCLASS wc;

	// Set up and register window class
	wc.style = 0;
	wc.lpfnWndProc = (WNDPROC)MsgProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(DWORD);
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_SLOONG));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = _T("WinConsole");
#if 0
	if (!RegisterClass(&wc))
		ThrowError();
#else
	RegisterClass(&wc);
#endif

	m_hWnd = CreateWindow(_T("WinConsole"), // class
		_T("LUA WinConsole"), // caption
		WS_OVERLAPPEDWINDOW, // style 
		CW_USEDEFAULT, // left
		CW_USEDEFAULT, // top
		640, // width
		480, // height
		m_hParentWnd, // parent window
		NULL, // menu 
		m_hInstance, // instance
		NULL); // parms

	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);
	SetFocus(m_hWnd);

	m_hEditControl = CreateWindow(_T("EDIT"), // class
		_T(""), // caption
		ES_LEFT | WS_CHILD, // style 
		2, // left
		404, // top
		228, // width
		16, // height
		m_hWnd, // parent window
		(HMENU)0xa7, // menu 
		m_hInstance, // instance
		NULL); // parms

	ShowWindow(m_hEditControl, SW_SHOW);
	UpdateWindow(m_hEditControl);
	SetFocus(m_hEditControl);
	m_ScrollyPos = 0;

	lpfnInputEdit = (WNDPROC)SetWindowLong(m_hEditControl, GWL_WNDPROC, (long)SubclassInputEditProc);
	g_Console->ResizeControls();
	m_stringList.push_back(_T("Sloong Lua Commander."));
}

static int Debug_Print(lua_State *L)
{
#ifdef _DEBUG
	int n = lua_gettop(L);  /* number of arguments */
	int i;
	lua_getglobal(L, "tostring");
	for (i = 1; i <= n; i++) {
		lua_pushvalue(L, -1);  /* function to be called */
		lua_pushvalue(L, i);   /* value to print */
		lua_call(L, 1, 1);
		CString str = lua_tostring(L, -1);  /* get result */
		if (str.GetString().empty())
			return luaL_error(L, "`tostring' must return a string to `print'");
		if (i > 1) CWinConsole::Write(_T("\t"));
		CWinConsole::Write(str.GetString().c_str());
		lua_pop(L, 1);  /* pop result */
	}
	CWinConsole::Write(_T("\n"));
#endif
	return 0;
}