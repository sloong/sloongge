#include "stdafx.h"
#include "SloongUIManager.h"
#include "SloongLua.h"
#include "SloongObject.h"
#include "SloongUI.h"
#include "SloongButton.h"
#include "SloongSprite.h"
#include "SloongTextField.h"
#include "SloongDraw.h"
#include "SloongEngine.h"
using namespace SoaringLoong;
using namespace SoaringLoong::Graphics;

CUIManager::CUIManager()
{
	m_UIMap = new map<tstring, CUserInterface*>;
	m_pModuleMap = new map<tstring, IObject*>;
}


CUIManager::~CUIManager()
{
	for each (auto& item in *m_UIMap)
	{
		delete item.second;
	}
	SAFE_DELETE(m_UIMap);
	for each (auto& item in *m_pModuleMap)
	{
		delete (item.second);
	}
	SAFE_DELETE(m_pModuleMap);
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

	if (strSpriteName == strType)
	{
		// Create Sprite
		CSprite* pSprite = new CSprite(m_pDDraw);
		pSprite->SetTexture(strTexture);
		pObject = pSprite;
	}
	else if (strTextFieldName == strType)
	{
		// Create Text field
		CTextField* pText = new CTextField(m_pDDraw);
		pText->SetTexture(strTexture);
		pObject = pText;
	}
	else if (strButtonName == strType)
	{
		// Create Button
		CButton* pButton = new CButton(m_pDDraw);
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
	auto item = m_UIMap->find(strFileName);
	if ( item == m_UIMap->end())
	{// the UI no run
		m_pCurrentUI = new CUserInterface();
		(*m_UIMap)[strFileName] = m_pCurrentUI;
		// TODO : build the full path
		tstring strFullPath = strFileName;
		m_pCurrentUI->Initialize(strFullPath, m_pDDraw, m_pLua, m_pLog);
		CSloongEngine::SendEvent(0,UI_EVENT::ENTER_INTERFACE);
	}
	else
	{
		m_pCurrentUI = item->second;

		if (m_pCurrentUI->GetEventHandler().empty())
		{
			m_pCurrentUI->Initialize(strFileName,m_pDDraw,m_pLua,m_pLog);
			CSloongEngine::SendEvent(0, UI_EVENT::ENTER_INTERFACE);
		}
		else
		{
			CSloongEngine::SendEvent(0, UI_EVENT::REENTER_INTERFACE);
		}
	}
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
	GetCurrentUI()->Update(m_hWnd);
}

void SoaringLoong::Graphics::CUIManager::Initialize(CDDraw* pDDraw, CLua* pLua, ILogSystem* pLog,HWND hWnd)
{
	m_pDDraw = pDDraw;
	m_pLua = pLua;
	m_pLog = pLog;
	m_hWnd = hWnd;
}

void SoaringLoong::Graphics::CUIManager::Load3DModule(const int& nID, const CString& strFileName, const CVector4D& vScale, const CVector4D& vPos, const CVector4D& vRotate)
{
	auto& item = m_pModuleMap->find(strFileName.GetString());
	if (item == m_pModuleMap->end())
	{
		IObject* pObject = IObject::Create3D(m_pDDraw);
		pObject->LoadPLGMode(strFileName.GetString().c_str());
	}
	else
	{
		item->second->AddObject(nID, vScale, vPos, vRotate);
	}
}

void SoaringLoong::Graphics::CUIManager::Move3DModule(const int& nID, const CVector4D& vPos)
{
	
}

void SoaringLoong::Graphics::CUIManager::Delete3DModule(const int& nID)
{

}
