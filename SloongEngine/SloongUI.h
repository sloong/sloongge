#pragma once

namespace SoaringLoong
{
	class CRect;
	namespace Graphics
	{
		class CObject;
		class CUserInterface
		{
		public:
			CUserInterface();
			~CUserInterface();

			void Initialize(ctstring& strPath);

			tstring GetEventHandler() const;
			void SetEventHandler(ctstring& strName);
			void DeleteObject(UINT nID);
			void AddObject(UINT nID, CObject* pObject);
			void SetObjectPosition(UINT nID, const CRect& rcClient);
			void Render();

		protected:
			CObject* FindObject(UINT nID);

		protected:
			map<UINT, CObject*> m_ObjectsMap;
			tstring m_strEventHandlerName;
		public:
			void Update();
		};
	}
}