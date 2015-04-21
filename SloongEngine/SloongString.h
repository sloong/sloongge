#pragma once

namespace SoaringLoong
{
	namespace Universal
	{
		class SLOONGENGINE_API CString //: wstring
		{
		public:
			CString();
			CString(LPCSTR lpStr);
			CString(LPCWSTR lpStr);
			~CString();

			CString& operator=(LPCSTR lpStr);
			CString& operator=(LPCWSTR lpStr);
			CString& operator=(string lpStr);
			CString& operator=(wstring lpstr);

			void FormatA(LPCSTR lpStr, ...);
			void FormatW(LPCWSTR lpStr, ...);
			void Format(LPCTSTR lpStr, ...);

			string GetStringA();
			wstring GetStringW();

			tstring GetString();

		public:
			static string UnicodeToANSI(LPCWSTR strWide);
			static wstring ANSIToUnicode(LPCSTR strMulti);
		protected:
			wstring m_strString;
		};
	}
	
#ifdef UNICODE
#define  GetString  GetStringW
#define  Format		FormatW
#else
#define GetString  GetStringA
#define  Format		FormatA
#endif // !UNICODE

}
