#include "stdafx.h"
#include "SloongLua.h"
#include "SloongString.h"

using namespace SoaringLoong;
using SoaringLoong::Universal::CString;
CLua::CLua()
{
	m_pErrorHandler = NULL;

	m_pScriptContext = luaL_newstate();
	luaL_openlibs(m_pScriptContext);
}
string CLua::UnicodeToANSI(LPCWSTR strWide)
{
	string strResult;
	int nLen = wcslen(strWide);
	LPSTR szMulti = new CHAR[nLen + 1];
	memset(szMulti, 0, nLen+1);
	WideCharToMultiByte(CP_ACP, 0, strWide, wcslen(strWide), szMulti, nLen, NULL, FALSE);
	strResult = szMulti;
	delete[] szMulti;
	return strResult;
}

wstring CLua::ANSIToUnicode(LPCSTR strMulti)
{
	wstring strResult;
	int nLen = strlen(strMulti);
	LPWSTR strWide = new WCHAR[nLen + 1];
	memset(strWide, 0, sizeof(TCHAR)*(nLen + 1));
	MultiByteToWideChar(CP_ACP, 0, strMulti, strlen(strMulti), strWide, nLen);
	strResult = strWide;
	delete[] strWide;
	return strResult;
}

CLua::~CLua()
{
	if (m_pScriptContext)
		lua_close(m_pScriptContext);
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
	CString strFullName(findScript(strFileName).c_str());

	if ( 0 != luaL_loadfile(m_pScriptContext, strFullName.GetStringA().c_str()))
	{
		HandlerError(_T("Load Script"), strFullName.GetString().c_str());
		return false;
	}

	if ( 0 != lua_pcall(m_pScriptContext,0,LUA_MULTRET,0))
	{
		HandlerError(_T("Run Script"), strFullName.GetString().c_str());
		return false;
	}
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

	if (0 != luaL_loadstring(m_pScriptContext, str.GetStringA().c_str()))
	{
		HandlerError(_T("String Load"),strCommand);
		return false;
	}

	if (0 != lua_pcall(m_pScriptContext, 0, LUA_MULTRET, 0))
	{
		HandlerError(_T("Run String"), strCommand);
		return false;
	}
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
