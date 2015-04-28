#include "stdafx.h"
#include "SloongException.h"

using SoaringLoong::CException;

CException::CException( LPCTSTR lpStr )
{
	m_strMessage = lpStr;
}

CException::CException(LPCTSTR lpStr, HRESULT hRes)
{
	m_strMessage = lpStr;
	m_hResult = hRes;
}

SoaringLoong::CException::CException(CString strMessage)
{
	m_strMessage = strMessage;
}


CException::~CException()
{
}

ctstring CException::GetException() const
{
	return m_strMessage.GetString();
}

HRESULT CException::GetResult() const
{
	return m_hResult;
}
