#include "stdafx.h"
#include "SloongUIManager.h"
#include "SloongLua.h"
#include "SloongObject.h"
#include "SloongUI.h"
#include "SloongButton.h"
#include "SloongSprite.h"
#include "SloongTextField.h"
#include "SloongDraw.h"
#include "SloongGame.h"

using namespace SoaringLoong;
using namespace SoaringLoong::Graphics;

CUIManager::CUIManager()
{
}


CUIManager::~CUIManager()
{
}

void CUIManager::CreateGUIItem(const UINT nID, const tstring strType, const vector<tstring>& strTexture)
{
	if ( strTexture[0].empty() )
	{
		return;
	}
	CUserInterface* pUI = GetCurrentUI();;
	if (nullptr == pUI)
	{
		throw tstring(_T("Get current UI field."));
	}

	CObject* pObject = nullptr;
	CDDraw* pDDraw = CSloongGame::GetDDraw();

	if (strSpriteName == strType)
	{
		// Create Sprite
		CSprite* pSprite = new CSprite(pDDraw);
		pSprite->SetTexture(strTexture);
		pObject = pSprite;
	}
	else if (strTextFieldName == strType)
	{
		// Create Text field
		CTextField* pText = new CTextField(pDDraw);
		pText->SetTexture(strTexture);
		pObject = pText;
	}
	else if (strButtonName == strType)
	{
		// Create Button
		CButton* pButton = new CButton(pDDraw);
		pButton->SetTexture(strTexture);
		pObject = pButton;
	}
	else
	{
		throw tstring(_T("Type error"));
	}

	if (pObject)
	{
		pUI->DeleteObject(nID);
		pObject->SetID(nID);
		pUI->AddObject(nID, pObject);
	}
}

CUserInterface* CUIManager::GetCurrentUI() const
{
	return m_pCurrentUI;
}

void CUIManager::RunGUI(ctstring& strFileName)
{
	auto item = m_UIMap.find(strFileName);
	if ( item == m_UIMap.end())
	{// the UI no run
		m_pCurrentUI = new CUserInterface();
		m_UIMap[strFileName] = m_pCurrentUI;
		// TODO : build the full path
		tstring strFullPath = strFileName;
		m_pCurrentUI->Initialize(strFullPath);
		SendEvent(UI_EVENT::ENTER_INTERFACE);
	}
	else
	{
		m_pCurrentUI = item->second;

		if (m_pCurrentUI->GetEventHandler().empty())
		{
			m_pCurrentUI->Initialize(strFileName);
			SendEvent(UI_EVENT::ENTER_INTERFACE);
		}
		else
		{
			SendEvent(UI_EVENT::REENTER_INTERFACE);
		}
	}
}

void CUIManager::SendEvent(UI_EVENT emEvent, float arg1 /*= 0.0f*/, float arg2 /*= 0.0f*/, float arg3 /*= 0.0f*/, float arg4 /*= 0.0f*/)
{
	CSloongGame::SendEvent(0, emEvent);
}

void SoaringLoong::Graphics::CUIManager::DeleteItem(const UINT nID)
{

}

void SoaringLoong::Graphics::CUIManager::MoveItem(const UINT nID, const CRect& rcRect)
{
	CUserInterface* pUI = GetCurrentUI();;
	if (nullptr == pUI)
	{
		throw tstring(_T("Get current UI field."));
	}
	pUI->SetObjectPosition(nID, rcRect);
}


void SoaringLoong::Graphics::CUIManager::Update()
{
	GetCurrentUI()->Update();
}
