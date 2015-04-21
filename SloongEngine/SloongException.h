#pragma once

namespace SoaringLoong
{
	class SLOONGENGINE_API CException : public exception
	{
	public:
		CException(LPCTSTR lpStr);
		CException(LPCTSTR lpStr, HRESULT hRes);
		~CException();

	public:
		ctstring GetException() const;
		HRESULT GetResult() const;

	protected:
		HRESULT m_hResult;
		tstring m_strMessage;
	};
}