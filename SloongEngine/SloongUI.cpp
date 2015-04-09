#include "stdafx.h"
#include "SloongUI.h"
#include "SloongObject.h"
#include "SloongLua.h"
#include "SloongDraw.h"
#include "SloongException.h"
#include "SloongGame.h"

using namespace SoaringLoong;
using namespace SoaringLoong::Graphics;

CUserInterface::CUserInterface()
{
}


CUserInterface::~CUserInterface()
{
}

void CUserInterface::DeleteObject(UINT nID)
{
	auto item = m_ObjectsMap.find(nID);

	if ( item != m_ObjectsMap.end())
	{
		SAFE_DELETE(item->second);
		m_ObjectsMap.erase(item);
	}
}

void CUserInterface::AddObject(UINT nID, CObject* pObject)
{
	m_ObjectsMap[nID] = pObject;
}

CObject* CUserInterface::FindObject(UINT nID)
{
	auto item = m_ObjectsMap.find(nID);
	if (item != m_ObjectsMap.end())
	{
		return item->second;
	}
	return nullptr;
}

void CUserInterface::Render()
{
	auto pLog = CSloongGame::GetLogSystem();
	try
	{
		if (m_ObjectsMap.size())
		{
			CDDraw* pDraw = CSloongGame::GetDDraw();

			pDraw->DDrawFillBackSurface(RGB(0, 0, 0));
			auto item = m_ObjectsMap.begin();
			while (item != m_ObjectsMap.end())
			{
				if (item->second->isDrawing())
				{
					try
					{
						item->second->Render(CSloongGame::GetDDraw()->GetBackSurface());
					}
					catch (CException& e)
					{
						pLog->Write(e.GetException().c_str());
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

void CUserInterface::Initialize(ctstring& strPath)
{
	CLua* pLua = CSloongGame::GetSloongLua();
	pLua->RunScript(strPath.c_str());
}

tstring CUserInterface::GetEventHandler() const
{
	return m_strEventHandlerName;
}

void SoaringLoong::Graphics::CUserInterface::SetObjectPosition(UINT nID, const CRect& rcClient)
{
	auto item = m_ObjectsMap.find(nID);
	if ( item != m_ObjectsMap.end() )
	{
		item->second->SetPosition(rcClient, 0);
	}
	
}


void SoaringLoong::Graphics::CUserInterface::Update()
{
	try
	{
		if ( m_ObjectsMap.size())
		{
			auto item = m_ObjectsMap.begin();
			while (item != m_ObjectsMap.end())
			{
				if (item->second->isDrawing())
				{
					try
					{
						item->second->Update();
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

void SoaringLoong::Graphics::CUserInterface::SetEventHandler(ctstring& strName)
{
	m_strEventHandlerName = strName;
}
