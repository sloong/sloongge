#include "stdafx.h"
#include "SloongUI.h"
#include "SloongObject.h"
#include "SloongLua.h"
#include "SloongDraw.h"
#include "SloongException.h"
#include "SloongEngine.h"
using namespace SoaringLoong;
using namespace SoaringLoong::Graphics;
using namespace SoaringLoong::Universal;
using SoaringLoong::Graphics3D::CULL_MODE;
using SoaringLoong::Graphics3D::TRANS_MODE;

CUserInterface::CUserInterface()
{
	m_ObjectsMap = new map<UINT, CObject*>;
	m_p3DObjectMap = new map<UINT, IObject*>;
	m_p3DKeyIDMap = new map<UINT, UINT>;
}


CUserInterface::~CUserInterface()
{
	SAFE_DELETE(m_ObjectsMap);
	SAFE_DELETE(m_p3DKeyIDMap);
	SAFE_DELETE(m_p3DObjectMap);
}

void CUserInterface::DeleteObject(UINT nID)
{
	auto item = m_ObjectsMap->find(nID);

	if (item != m_ObjectsMap->end())
	{
		SAFE_DELETE(item->second);
		m_ObjectsMap->erase(item);
	}
}

void CUserInterface::AddObject(UINT nID, CObject* pObject)
{
	(*m_ObjectsMap)[nID] = pObject;
}

CObject* CUserInterface::FindObject(UINT nID)
{
	auto item = m_ObjectsMap->find(nID);
	if (item != m_ObjectsMap->end())
	{
		return item->second;
	}
	return nullptr;
}

void CUserInterface::Render()
{
	try
	{
		if (m_ObjectsMap->size())
		{
			m_pDDraw->DDrawFillBackSurface(RGB(0, 0, 0));
			auto item = m_ObjectsMap->begin();
			while (item != m_ObjectsMap->end())
			{
				if (item->second->isDrawing())
				{
					try
					{
						item->second->Render(m_pDDraw->GetBackSurface());
					}
					catch (CException& e)
					{
						m_pLog->Write(e.GetException().c_str());
					}
				}
				item++;
			}
		}

		for each (auto item in *m_p3DKeyIDMap)
		{
			auto pObject = (*m_p3DObjectMap)[item.first];
			pObject->SetCurrentIndex(item.second);
			pObject->Reset();
			pObject->Cull(m_pCamera, CULL_MODE::CULL_ON_XYZ_PLANES);
			if ( pObject->Visible())
			{
				pObject->ToWorld(TRANS_MODE::LocalToTrans);
				pObject->ToCamera(m_pCamera);
				pObject->ToProject(m_pCamera);
				pObject->PerspectiveToScreen(m_pCamera);
				pObject->Render();
			}
		}
	}
	catch (CException& e)
	{
		
	}
}

void CUserInterface::Initialize(ctstring& strPath, CDDraw* pDDraw, DInputClass* pInput, CLua* pLua, ILogSystem* pLog)
{
	m_pLog = pLog;
	m_pLua = pLua;
	m_pDDraw = pDDraw;
	m_pInput = pInput;
	pLua->RunScript(strPath.c_str());
}

tstring CUserInterface::GetEventHandler() const
{
	return m_strEventHandlerName.GetString();
}

void SoaringLoong::Graphics::CUserInterface::SetObjectPosition(UINT nID, const CRect& rcClient)
{
	auto item = m_ObjectsMap->find(nID);
	if (item != m_ObjectsMap->end())
	{
		item->second->SetPosition(rcClient, 0);
	}
	
}


void SoaringLoong::Graphics::CUserInterface::Update( HWND hWnd )
{
	try
	{
		m_pInput->GetInput();
		// Update the keyboard event
		for each (auto& item in m_vKeyboardEvent)
		{
			if (m_pInput->IsKeyDown(item))
			{
				CSloongEngine::SendEvent(item, UI_EVENT::KEY_PRESS);
			}
		}


		if (m_ObjectsMap->size())
		{
			CPoint pos;
			GetCursorPos(&pos);
			ScreenToClient(hWnd, &pos);

			auto item = m_ObjectsMap->begin();
			while (item != m_ObjectsMap->end())
			{
				if (item->second->isDrawing())
				{
					try
					{
						item->second->Update(pos);
					}
					catch (CException& e)
					{
						//pLog->Write(e.GetException().c_str());
					}
				}
				item++;
			}
		}
	}
	catch (CException& e)
	{
		
	}
}

void SoaringLoong::Graphics::CUserInterface::SetEventHandler(LPCTSTR strName)
{
	m_strEventHandlerName = strName;
}

void SoaringLoong::Graphics::CUserInterface::Add3DObject(UINT nKey, UINT nID,IObject* pObject)
{
	(*m_p3DKeyIDMap)[nKey] = nID;
	(*m_p3DObjectMap)[nKey] = pObject;
}

void SoaringLoong::Graphics::CUserInterface::SetCamera(CCamera* pCamera)
{
	m_pCamera = pCamera;
}

void SoaringLoong::Graphics::CUserInterface::Move3DObject(UINT nKey, const CVector4D& vPos)
{
	// 根據參數Key從KeyIDMap中找到對應的Object ID
	int nID = (*m_p3DKeyIDMap)[nKey];
	// 根據參數Key從ObjectMap中找到對應的Object對象
	auto pObj = (*m_p3DObjectMap)[nKey];
	// 設置對象的當前的數據索引
	pObj->SetCurrentIndex(nID);
	// 設置對象的世界座標
	pObj->SetWorldPosition(vPos);

}

void SoaringLoong::Graphics::CUserInterface::RegisterKeyboardEvent(const vector<size_t>& keyList)
{
	m_vKeyboardEvent = keyList;
}
