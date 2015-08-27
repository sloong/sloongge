#include "stdafx.h"
#include "SloongUI.h"
#include "SloongObject.h"
#include "univ/lua.h"
#include "univ/log.h"
#include "graphics/SloongGraphics.h"
#include "univ/exception.h"
#include "SloongEngine.h"
using namespace Sloong;
using namespace Sloong::Graphics;
using namespace Sloong::Universal;
using Sloong::Graphics3D::CULL_MODE;
using Sloong::Graphics3D::TRANS_MODE;

CUserInterface::CUserInterface()
{
	m_ObjectsMap = new map<UINT, CObject*>;
	m_p3DObjectMap = new map<UINT, CObject3D*>;
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
			//CSloongEngine::AddRenderTask(pObject, item.second, m_pCamera);
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

void CUserInterface::Initialize(const CString& strPath, CDDraw* pDDraw, CDInput* pInput, CLua* pLua, CLog* pLog)
{
	m_pLog = pLog;
	m_pLua = pLua;
	m_pDDraw = pDDraw;
	m_pInput = pInput;
	pLua->RunScript(strPath);
}

const CString& CUserInterface::GetEventHandler() const
{
	return m_strEventHandlerName;
}

void Sloong::Graphics::CUserInterface::SetObjectPosition(UINT nID, const CRect& rcClient)
{
	auto item = m_ObjectsMap->find(nID);
	if (item != m_ObjectsMap->end())
	{
		item->second->SetPosition(rcClient, 0);
	}
	
}


void Sloong::Graphics::CUserInterface::Update( HWND hWnd )
{
	try
	{
		m_pInput->GetInput();
		// Update the keyboard event
		for each (auto& item in m_vKeyboardEvent)
		{
			if (m_pInput->IsKeyDown((int)item))
			{
				CSloongEngine::SendEvent((int)item, UI_EVENT::KEY_PRESS);
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

void Sloong::Graphics::CUserInterface::SetEventHandler(const CString& strName)
{
	m_strEventHandlerName = strName;
}

void Sloong::Graphics::CUserInterface::Add3DObject(UINT nKey, UINT nID,CObject3D* pObject)
{
	(*m_p3DKeyIDMap)[nKey] = nID;
	(*m_p3DObjectMap)[nKey] = pObject;
}

void Sloong::Graphics::CUserInterface::SetCamera(CCamera* pCamera)
{
	m_pCamera = pCamera;
}

void Sloong::Graphics::CUserInterface::Move3DObject(UINT nKey, const CVector4D& vPos)
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

void Sloong::Graphics::CUserInterface::RegisterKeyboardEvent(const vector<size_t>& keyList)
{
	m_vKeyboardEvent = keyList;
}
