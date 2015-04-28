#include "stdafx.h"
#include "SloongFile.h"
#include "SloongString.h"
#include "SloongException.h"
using namespace SoaringLoong::Universal;
// This function will load one line string from file.
// if the file is empty, return empty string.
LPCTSTR SoaringLoong::Universal::CFile::GetLine()
{
	int index = 0;
	int length = 0;

	while (true)
	{
		if (!fgetts(m_szBuffer, 256, m_pFileStream))
		{
			return NULL;
		}

		for (length = _tcslen(m_szBuffer), index = 0; isspace(m_szBuffer[index]); index++);

		if (index >= length || m_szBuffer[index] == '#')
		{
			continue;
		}

		return &m_szBuffer[index];
	}
}

SoaringLoong::Universal::CFile::CFile()
{

}

SoaringLoong::Universal::CFile::~CFile()
{
	if (m_pFileStream)
	{
		fclose(m_pFileStream);
		m_pFileStream = nullptr;
	}
	
}

HRESULT SoaringLoong::Universal::CFile::Open(LPCTSTR strFileName, OpenFileAccess emMode)
{
	if (!Access(strFileName, emMode))
		return S_FALSE;

	//CreateFile(strFileName,)
}

void SoaringLoong::Universal::CFile::Close()
{

}

errno_t SoaringLoong::Universal::CFile::OpenStream(LPCTSTR szFileName, LPCTSTR szMode)
{
	if (!Access(szFileName,Exist))
	{
		throw CException(CString(_T("Can't open file stream %s, the file is not existing."), szFileName));
	}
	return _tfopen_s(&m_pFileStream, szFileName, szMode);
}

bool SoaringLoong::Universal::CFile::Access(LPCTSTR szFileName, OpenFileAccess emMode)
{
	if (0 == _taccess_s(szFileName, emMode))
		return true;
	else
		return false;
}
