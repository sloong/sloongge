#include "stdafx.h"
#include "IUniversal.h"
#include "SloongString.h"

using SoaringLoong::Universal::CString;

#define MAX_BUFFER	2048

CString::CString()
{
	m_strString = new wstring();
}

string CString::UnicodeToANSI(LPCWSTR strWide)
{
	string strResult;
	int nLen = wcslen(strWide);
	LPSTR szMulti = new CHAR[nLen + 1];
	memset(szMulti, 0, nLen + 1);
	WideCharToMultiByte(CP_ACP, 0, strWide, wcslen(strWide), szMulti, nLen, NULL, FALSE);
	strResult = szMulti;
	delete[] szMulti;
	return strResult;
}

wstring CString::ANSIToUnicode(LPCSTR strMulti)
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

CString::~CString()
{
	SAFE_DELETE(m_strString);
}

string CString::GetStringA() const
{
	return UnicodeToANSI(m_strString->c_str());
}

wstring CString::GetStringW() const
{
	return (*m_strString);
}

void SoaringLoong::Universal::CString::FormatA(LPCSTR lpStr, ...)
{
	va_list args;
	va_start(args, lpStr);
	char szBuffer[MAX_BUFFER];
	vsprintf_s(szBuffer, MAX_BUFFER, lpStr, args);
	(*m_strString) = ANSIToUnicode(szBuffer);
	va_end(args);
}

void SoaringLoong::Universal::CString::FormatW(LPCWSTR lpStr, ...)
{
	va_list args;
	va_start(args, lpStr);
	WCHAR szBuffer[MAX_BUFFER];
	vswprintf_s(szBuffer, MAX_BUFFER, lpStr, args);
	va_end(args);
	(*m_strString) = szBuffer;
}

CString& SoaringLoong::Universal::CString::operator=(LPCSTR lpStr)
{
	(*m_strString) = ANSIToUnicode(lpStr);
	return *this;
}

CString& SoaringLoong::Universal::CString::operator=(LPCWSTR lpStr)
{
	(*m_strString) = lpStr;
	return *this;
}

SoaringLoong::Universal::CString::CString(LPCSTR lpStr, ...)
{
	new(this) CString();
	va_list args;
	va_start(args, lpStr);
	FormatA(lpStr, args);
	va_end(args);
}

SoaringLoong::Universal::CString::CString(LPCWSTR lpStr, ...)
{
	new(this) CString();
	va_list args;
	va_start(args, lpStr);
	FormatW(lpStr, args);
	va_end(args);
}

