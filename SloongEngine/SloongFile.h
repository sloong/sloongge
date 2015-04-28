#pragma once
#include "SloongString.h"
namespace SoaringLoong
{
	namespace Universal
	{
		typedef enum OpenFileAccess
		{
			Exist = 0,
			Write = 2,
			Read = 4,
			All = 6,
		};

		class CFile
		{
		public:
			CFile();
			virtual ~CFile();

			virtual LPCTSTR GetLine();
			virtual HRESULT Open(LPCTSTR szFileName, OpenFileAccess emMode);
			virtual errno_t OpenStream(LPCTSTR szFileName, LPCTSTR szMode );
			virtual void Close();
		public:
			static bool Access(LPCTSTR szFileName, OpenFileAccess emMode);
		public:
			TCHAR m_szBuffer[256];
			FILE* m_pFileStream;
			CString m_strFileName;
		};
	}
}

