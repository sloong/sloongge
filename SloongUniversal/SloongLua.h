#pragma once

#ifdef SLOONGUNIVERSAL_EXPORTS
#define SLOONGUNIVERSAL_API __declspec(dllexport)
#else
#define SLOONGUNIVERSAL_API __declspec(dllimport)
#endif

#define LuaRes extern "C" int

namespace SoaringLoong
{
	namespace Universal
	{
		extern "C" {
#include "..\\SloongLua\\lua-5.3.0\\src\\lua.h"
#include "..\\SloongLua\\lua-5.3.0\\src\\lualib.h"
#include "..\\SloongLua\\lua-5.3.0\\src\\lauxlib.h"
		}
#pragma comment(lib,"SloongLua.lib")

		extern "C" {
			typedef int(*LuaFunctionType)(lua_State *pLuaState);
		};

		struct LuaFunctionRegistr
		{
			LPCTSTR strFunctionName;
			LuaFunctionType pFunction;
		};

		enum LuaType
		{
			TNONE = -1,

			TNIL = 0,
			TBOOLEAN = 1,
			TLIGHTUSERDATA = 2,
			TNUMBER = 3,
			TSTRING = 4,
			TTABLE = 5,
			TFUNCTION = 6,
			TUSERDATA = 7,
			TTHREAD = 8,
			NUMTAGS = 9,
		};

		class SLOONGUNIVERSAL_API CLua
		{
		public:
			CLua();
			virtual ~CLua();

			bool	RunScript(LPCTSTR strFileName);
			bool	RunBuffer(LPCSTR pBuffer, size_t sz);
			bool	RunString(LPCTSTR strCommand);
			bool	RunFunction(LPCTSTR strFunctionName, LPCTSTR args);
			tstring	GetErrorString();
			void	HandlerError(LPCTSTR strErrorType, LPCTSTR strCmd);
			bool	AddFunction(LPCTSTR strFunctionName, LuaFunctionType pFunction);
			tstring	GetStringArgument(int nNum, LPCTSTR pDefault = _T(""));
			double	GetNumberArgument(int nNum, double pDefault = -1.0f);
			void	PushString(LPCTSTR strString);
			void	PushNumber(double dValue);
			void	SetErrorHandle(void(*pErrHandler)(LPCTSTR strError));
			lua_State*	GetScriptContext();
			map<tstring, tstring> GetTableParam(int index);
			LuaType	CheckType(int index);
			size_t  StringToNumber(LPCTSTR string);
			void	InitializeWindow(HWND hWnd);
			void	ShowLuaWindow();
			void	RenderLuaWindow();

		private:
			lua_State *m_pScriptContext;
			void(*m_pErrorHandler)(LPCTSTR strError);
			HANDLE	m_pMutex;
		};

	}
	
}