#pragma once

#ifdef SLOONGENGINE_EXPORTS
#define SLOONGENGINE_API __declspec(dllexport)
#else
#define SLOONGENGINE_API __declspec(dllimport)
#endif

#include "SloongString.h"
#include "ISloongObject.h"
#include "SloongCamera.h"
#include "SloongDInput.h"
namespace SoaringLoong
{
	class CRect;
	class CLua;
	class ILogSystem;
	using Universal::CString;
	using Graphics3D::IObject;
	using Graphics3D::CCamera;
	using DirectX::CDInput;
	namespace Graphics
	{
		class CObject;
		class CDDraw;
		class SLOONGENGINE_API CUserInterface
		{
		public:
			CUserInterface();
			~CUserInterface();

			void Initialize(ctstring& strPath, CDDraw* pDDraw, CDInput* pInput, CLua* pLua, ILogSystem* pLog);

			tstring GetEventHandler() const;
			void SetEventHandler(LPCTSTR strName);
			void DeleteObject(UINT nID);
			void AddObject(UINT nID, CObject* pObject);
			void SetObjectPosition(UINT nID, const CRect& rcClient);
			void Render();
			void SetCamera(CCamera* pCamera);
			void Add3DObject(UINT nKey, UINT nID,IObject* pObject);
			void Move3DObject(UINT nKey, const CVector4D& vPos);

			void RegisterKeyboardEvent(const vector<size_t>& keyList);

		protected:
			CObject* FindObject(UINT nID);

		protected:
			map<UINT, CObject*>*		m_ObjectsMap;
			map<UINT, IObject*>*		m_p3DObjectMap;
			map<UINT, UINT>*			m_p3DKeyIDMap;
			vector<size_t>				m_vKeyboardEvent;
			CString						m_strEventHandlerName;
			CLua*						m_pLua;
			ILogSystem*					m_pLog;
			CDDraw*						m_pDDraw;
			CCamera*					m_pCamera;
			CDInput*				m_pInput;
		public:
			void Update( HWND hWnd );
		};
	}
}