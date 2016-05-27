#pragma once

#ifdef SLOONGENGINE_EXPORTS
#define SLOONGENGINE_API __declspec(dllexport)
#else
#define SLOONGENGINE_API __declspec(dllimport)
#endif

#include "SloongObject3D.h"
#include "SloongObject.h"
#include "SloongCamera.h"
#include "SloongDInput.h"
namespace Sloong
{
	namespace Universal
	{
		class CLua;
		class CLog;
		class CRect;
	}
	using namespace DirectX;
	using namespace Universal;
	namespace Graphics3D
	{
		class CCamera;
	}
	using namespace Graphics3D;
	namespace Graphics
	{
		class SLOONGENGINE_API CUserInterface
		{
		public:
			CUserInterface();
			~CUserInterface();

			void Initialize(const wstring& strPath, CDDraw* pDDraw, CDInput* pInput, CLua* pLua, CLog* pLog);

			const wstring& GetEventHandler() const;
			void SetEventHandler(const wstring& strName);
			void DeleteObject(UINT nID);
			void AddObject(UINT nID, CObject* pObject);
			void SetObjectPosition(UINT nID, const CRect& rcClient);
			void Render();
			void SetCamera(CCamera* pCamera);
			void Add3DObject(UINT nKey, UINT nID,CObject3D* pObject);
			void Move3DObject(UINT nKey, const CVector4D& vPos);

			void RegisterKeyboardEvent(const vector<size_t>& keyList);

		protected:
			CObject* FindObject(UINT nID);

		protected:
			map<UINT, CObject*>*		m_ObjectsMap;
			map<UINT, CObject3D*>*		m_p3DObjectMap;
			map<UINT, UINT>*			m_p3DKeyIDMap;
			vector<size_t>				m_vKeyboardEvent;
			wstring						m_strEventHandlerName;
			CLua*						m_pLua;
			CLog*					m_pLog;
			CDDraw*						m_pDDraw;
			CCamera*					m_pCamera;
			CDInput*				m_pInput;
		public:
			void Update( HWND hWnd );
		};
	}
}