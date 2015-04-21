#pragma once

#ifdef SLOONGENGINE_EXPORTS
#define SLOONGENGINE_API __declspec(dllexport)
#else
#define SLOONGENGINE_API __declspec(dllimport)
#endif

#include "SloongString.h"

namespace SoaringLoong
{
	class CRect;
	class CLua;
	class ILogSystem;
	using Universal::CString;
	namespace Graphics
	{
		class CObject;
		class CDDraw;
		class SLOONGENGINE_API CUserInterface
		{
		public:
			CUserInterface();
			~CUserInterface();

			void Initialize(ctstring& strPath, CDDraw* pDDraw, CLua* pLua, ILogSystem* pLog);

			tstring GetEventHandler() const;
			void SetEventHandler(LPCTSTR strName);
			void DeleteObject(UINT nID);
			void AddObject(UINT nID, CObject* pObject);
			void SetObjectPosition(UINT nID, const CRect& rcClient);
			void Render();

		protected:
			CObject* FindObject(UINT nID);

		protected:
			map<UINT, CObject*>* m_ObjectsMap;
			CString m_strEventHandlerName;
			CLua*				m_pLua;
			ILogSystem*			m_pLog;
			CDDraw*				m_pDDraw;
		public:
			void Update( HWND hWnd );
		};
	}
}