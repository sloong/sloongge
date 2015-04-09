#pragma once
#include "SloongLua.h"

namespace SoaringLoong
{

	class CWinConsole
	{
	public:
		static HWND StartConsole(HINSTANCE hInstance, CLua *pScriptContext);
		static void StopConsole();
		static void Write( LPCTSTR pString);

		static LRESULT WINAPI MsgProc(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK SubclassInputEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		bool	ConsoleReady(void)	{ return m_hWnd != NULL; }

	private:
		CWinConsole();
		virtual ~CWinConsole();

		void	Init(HINSTANCE hInstance);
		void	ResizeControls(void);
		void	AdjustScrollBar(void);
		void	Paint(HDC hDC);


	private:
		static volatile bool	m_bWinIsActive;
		static volatile HWND m_hWnd;
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
using namespace SoaringLoong;
static int Debug_Print(lua_State *L);

