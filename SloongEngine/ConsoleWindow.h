#pragma once
#include "univ\\lua.h"
#include "univ\\univ.h"
using namespace Sloong;
namespace Sloong
{
	using namespace Universal;
	class CWinConsole
	{
	public:
		static HWND StartConsole(HINSTANCE hInstance, HWND hWnd, CRect rcWindow, CLua* pScriptContext);
		static void StopConsole();
		static void Write( LPCTSTR pString);

		static LRESULT WINAPI MsgProc(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK SubclassInputEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		bool	ConsoleReady(void)	{ return m_hWnd != NULL; }
		void	Paint(HDC hDC);
	private:
		CWinConsole();
		virtual ~CWinConsole();

		void	Init(HINSTANCE hInstance);
		void	ResizeControls(void);
		void	AdjustScrollBar(void);
		


	private:
		static volatile bool	m_bWinIsActive;
		static volatile HWND m_hWnd;
		static volatile HWND m_hParentWnd;
		static CRect	m_rcWindow;
		static volatile HWND m_hEditControl;
		static TCHAR m_CommandBuffer[4096];

		list<tstring> m_stringList;
		HINSTANCE m_hInstance;

		CLua *m_pScriptContext;

		int m_ScrollyPos;
		int m_textAreaHeight;
	};
	extern CWinConsole *g_Console;
}
using namespace Sloong;
static int Debug_Print(lua_State *L);

