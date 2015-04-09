#pragma once

#define LuaRes extern "C" int

namespace SoaringLoong
{

	extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
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

	class CLua
	{
	public:
		CLua();
		virtual ~CLua();

		bool	RunScript(LPCTSTR strFileName);
		bool	RunBuffer(LPCSTR pBuffer, size_t sz);
		bool	RunString(LPCTSTR strCommand);
		bool	RunFunction(LPCTSTR strFunctionName, LPCTSTR args);
		tstring	GetErrorString();
		void	HandlerError(LPCTSTR strErrorType,LPCTSTR strCmd);
		bool	AddFunction(LPCTSTR strFunctionName, LuaFunctionType pFunction);
		tstring	GetStringArgument(int nNum, LPCTSTR pDefault = _T(""));
		double	GetNumberArgument(int nNum, double pDefault = 0.0);
		void	PushString(LPCTSTR strString);
		void	PushNumber(double dValue);
		void	SetErrorHandle(void(*pErrHandler)(LPCTSTR strError)) { m_pErrorHandler = pErrHandler; }
		lua_State*	GetScriptContext()	{ return m_pScriptContext; }

	public:
		static string UnicodeToANSI(LPCWSTR strWide);
		static wstring ANSIToUnicode(LPCSTR strMulti);
	private:
		lua_State *m_pScriptContext;
		void(*m_pErrorHandler)(LPCTSTR strError);
	};


}