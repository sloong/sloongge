#include "stdafx.h"
#include "SloongUI.h"
#include "SloongObject.h"
#include "SloongLua.h"
#include "SloongDraw.h"
#include "SloongException.h"

using namespace SoaringLoong;
using namespace SoaringLoong::Graphics;

CUserInterface::CUserInterface()
{
	m_ObjectsMap = new map<UINT, CObject*>;
}


CUserInterface::~CUserInterface()
{
	SAFE_DELETE(m_ObjectsMap);
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
	}
	catch (CException& e)
	{
		
	}
}

void CUserInterface::Initialize(ctstring& strPath, CDDraw* pDDraw, CLua* pLua, ILogSystem* pLog)
{
	m_pLog = pLog;
	m_pLua = pLua;
	m_pDDraw = pDDraw;
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
