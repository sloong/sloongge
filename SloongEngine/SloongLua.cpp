#include "stdafx.h"
#include "SloongLua.h"
#include "SloongString.h"
#include "SloongException.h"
using namespace SoaringLoong;
using SoaringLoong::Universal::CString;
using SoaringLoong::Universal::CException;
CLua::CLua()
{
	m_pErrorHandler = NULL;

	m_pScriptContext = luaL_newstate();
	luaL_openlibs(m_pScriptContext);
	m_pMutex = CreateMutex(NULL, FALSE, _T("SloongLuaMutex"));
}

CLua::~CLua()
{
	if (m_pScriptContext)
		lua_close(m_pScriptContext);

	CloseHandle(m_pMutex);
}

static string findScript(LPCTSTR strFullName)
{
	CString str(strFullName);
	
	FILE* fFind;

	char szDrive[MAX_PATH];
	char szDir[MAX_PATH];
	char szFileName[MAX_PATH];
	char szExtension[MAX_PATH];

	_splitpath_s(str.GetStringA().c_str(), szDrive, MAX_PATH, szDir, MAX_PATH, szFileName, MAX_PATH, szExtension, MAX_PATH);

	string strTestFile = (string)szDrive + szDir + ("Scripts\\") + szFileName + (".LUB");
	fopen_s(&fFind,strTestFile.c_str(), "r");
	if (!fFind)
	{
		strTestFile = (string)szDrive + szDir + ("Scripts\\") + szFileName + (".LUA");
		fopen_s(&fFind, strTestFile.c_str(), "r");
	}

	if (!fFind)
	{
		strTestFile = (string)szDrive + szDir + szFileName + (".LUB");
		fopen_s(&fFind, strTestFile.c_str(), "r");
	}

	if (!fFind)
	{
		strTestFile = (string)szDrive + szDir + szFileName + (".LUA");
		fopen_s(&fFind, strTestFile.c_str(), "r");
	}

	if (fFind)
	{
		fclose(fFind);
	}

	return strTestFile;
}

bool CLua::RunScript(LPCTSTR strFileName)
{
	WaitForSingleObject(m_pMutex, INFINITE);
	CString strFullName(findScript(strFileName).c_str());

	if ( 0 != luaL_loadfile(m_pScriptContext, strFullName.GetStringA().c_str()))
	{
		HandlerError(_T("Load Script"), strFullName.GetString().c_str());
		ReleaseMutex(m_pMutex);
		return false;
	}

	if ( 0 != lua_pcall(m_pScriptContext,0,LUA_MULTRET,0))
	{
		HandlerError(_T("Run Script"), strFullName.GetString().c_str());
		ReleaseMutex(m_pMutex);
		return false;
	}
	ReleaseMutex(m_pMutex);
	return true;
}


bool CLua::RunBuffer( LPCSTR pBuffer,size_t sz)
{
	if (0 != luaL_loadbuffer(m_pScriptContext, (LPCSTR)pBuffer, sz, NULL))
	{
		CString str(pBuffer);
		HandlerError(_T("Load Buffer"), str.GetString().c_str());
		return false;
	}

	if (0 != lua_pcall(m_pScriptContext, 0, LUA_MULTRET, 0))
	{
		CString str(pBuffer);
		HandlerError(_T("Run Buffer"), str.GetString().c_str());
		return false;
	}
	return true;
}

bool CLua::RunString(LPCTSTR strCommand)
{
	CString str(strCommand);

	WaitForSingleObject(m_pMutex, INFINITE);
	if (0 != luaL_loadstring(m_pScriptContext, str.GetStringA().c_str()))
	{
		HandlerError(_T("String Load"), strCommand);
		ReleaseMutex(m_pMutex);
		return false;
	}

	if (0 != lua_pcall(m_pScriptContext, 0, LUA_MULTRET, 0))
	{
		HandlerError(_T("Run String"), strCommand);
		ReleaseMutex(m_pMutex);
		return false;
	}

	ReleaseMutex(m_pMutex);
	return true;
}

tstring CLua::GetErrorString()
{
	CString strError(luaL_checkstring(m_pScriptContext, -1));

	return strError.GetString();
}


bool CLua::AddFunction( LPCTSTR pFunctionName, LuaFunctionType pFunction)
{
	CString FunctionName(pFunctionName);
	lua_register(m_pScriptContext, FunctionName.GetStringA().c_str(), pFunction);
	return true;
}


tstring CLua::GetStringArgument(int num, LPCTSTR pDefault)
{
	CString str(pDefault);

	str = luaL_optstring(m_pScriptContext, num, str.GetStringA().c_str());

	return str.GetString();
}

double CLua::GetNumberArgument(int num, double dDefault)
{
	return luaL_optnumber(m_pScriptContext, num, dDefault);
}

void CLua::PushString(LPCTSTR pString)
{
	CString str(pString);
	lua_pushstring(m_pScriptContext, str.GetStringA().c_str());
}

void CLua::PushNumber(double value)
{
	lua_pushnumber(m_pScriptContext, value);
}

bool CLua::RunFunction(LPCTSTR strFunctionName, LPCTSTR args)
{

	CString str;
	str.Format(_T("%s(%s)"), strFunctionName, args);
	return RunString(str.GetString().c_str());
}

void SoaringLoong::CLua::HandlerError(LPCTSTR strErrorType,LPCTSTR strCmd)
{
	if (m_pErrorHandler)
	{
		CString strMessage;
		strMessage.Format(_T("\n Error - %s:\n %s\n Error Message:%s"), strErrorType, strCmd, GetErrorString().c_str());
		m_pErrorHandler(strMessage.GetString().c_str());
	}
}

map<tstring, tstring> SoaringLoong::CLua::GetTableParam(int index)
{
	auto L = m_pScriptContext;
	map<tstring, tstring> data;
	lua_pushnil(L);
	// 现在的栈：-1 => nil; index => table
	if ( index >= lua_gettop(L))
	{
		throw CException(_T("The index is too big."));
	}

	while (lua_next(L, index))
	{
		// 现在的栈：-1 => value; -2 => key; index => table
		// 拷贝一份 key 到栈顶，然后对它做 lua_tostring 就不会改变原始的 key 值了
		lua_pushvalue(L, -2);
		// 现在的栈：-1 => key; -2 => value; -3 => key; index => table

		CString key = lua_tostring(L, -1);
		CString value = lua_tostring(L, -2);

		data[key.GetString()] = value.GetString();
		// 弹出 value 和拷贝的 key，留下原始的 key 作为下一次 lua_next 的参数
		lua_pop(L, 2);
		// 现在的栈：-1 => key; index => table
	}
	// 现在的栈：index => table （最后 lua_next 返回 0 的时候它已经把上一次留下的 key 给弹出了）
	// 所以栈已经恢复到进入这个函数时的状态
	return data;

}

SoaringLoong::LuaType SoaringLoong::CLua::CheckType(int index)
{
	int nType = lua_type(m_pScriptContext, index);
	return (LuaType)nType;
}

size_t SoaringLoong::CLua::StringToNumber(LPCTSTR string)
{
	CString str(string);
	lua_stringtonumber(m_pScriptContext, str.GetStringA().c_str());
	
	return lua_tonumber(m_pScriptContext, -1);
}
