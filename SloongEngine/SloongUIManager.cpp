#include "stdafx.h"
#include "SloongUIManager.h"
#include "univ/lua.h"
#include "SloongObject.h"
#include "SloongUI.h"
#include "SloongButton.h"
#include "SloongSprite.h"
#include "SloongTextField.h"
#include "graphics/SloongGraphics.h"
#include "SloongEngine.h"
using namespace Sloong;
using namespace Sloong::Graphics;




CUIManager::CUIManager()
{
	m_UIMap = new map<CString, CUserInterface*>;
	m_pModuleMap = new map<CString, CObject3D*>;
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

void CUIManager::CreateGUIItem(const UINT nID, const CString& strType, vector<CString>* strTexture)
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

	if (strType == strSpriteName)
	{
		// Create Sprite
		CSprite* pSprite = new CSprite(m_pDDraw);
		pSprite->SetTexture(strTexture);
		pObject = pSprite;
	}
	else if (strType == strTextFieldName)
	{
		// Create Text field
		CTextField* pText = new CTextField(m_pDDraw);
		pText->SetTexture(strTexture);
		pObject = pText;
	}
	else if (strType == strButtonName)
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

void CUIManager::RunGUI(const CString& strFileName)
{
	auto item = m_UIMap->find(strFileName);
	if ( item == m_UIMap->end())
	{// the UI no run
		m_pCurrentUI = new CUserInterface();
		(*m_UIMap)[strFileName] = m_pCurrentUI;
		// TODO : build the full path
		CString strFullPath = strFileName;
		m_pCurrentUI->Initialize(strFullPath, m_pDDraw, m_pInput, m_pLua, m_pLog);
		CSloongEngine::SendEvent(0,UI_EVENT::ENTER_INTERFACE);
	}
	else
	{
		m_pCurrentUI = item->second;

		if (m_pCurrentUI->GetEventHandler().empty())
		{
			m_pCurrentUI->Initialize(strFileName, m_pDDraw, m_pInput, m_pLua, m_pLog);
			CSloongEngine::SendEvent(0, UI_EVENT::ENTER_INTERFACE);
		}
		else
		{
			CSloongEngine::SendEvent(0, UI_EVENT::REENTER_INTERFACE);
		}
	}
}

void Sloong::Graphics::CUIManager::DeleteItem(const UINT nID)
{

}

void Sloong::Graphics::CUIManager::MoveItem(const UINT nID, const CRect& rcRect)
{
	CUserInterface* pUI = GetCurrentUI();;
	if (nullptr == pUI)
	{
		throw tstring(_T("Get current UI field."));
	}
	pUI->SetObjectPosition(nID, rcRect);
}


void Sloong::Graphics::CUIManager::Update()
{
	GetCurrentUI()->Update(m_hWnd);
}

void Sloong::Graphics::CUIManager::Initialize(CDDraw* pDDraw, CLua* pLua,CDInput* pInput, CLog* pLog,HWND hWnd)
{
	m_pDDraw = pDDraw;
	m_pLua = pLua;
	m_pInput = pInput;
	m_pLog = pLog;
	m_hWnd = hWnd;
}

void Sloong::Graphics::CUIManager::Load3DModule(const int& nID, const CString& strFileName, const CVector4D& vScale, const CVector4D& vPos, const CVector4D& vRotate)
{
	auto& item = m_pModuleMap->find(strFileName);
	if (item == m_pModuleMap->end())
	{
		CObject3D* pObject = new CObject3D(m_pDDraw);
		pObject->LoadPLGMode(strFileName);
		(*m_pModuleMap)[strFileName] = pObject;
		item = m_pModuleMap->find(strFileName);
	}
	
	// 添加當前模型引用
	int nIndex = item->second->AddObject(vScale, vPos, vRotate);
	auto pUI = GetCurrentUI();
	// 添加當前模型指針和ID索引到當前界面
	pUI->Add3DObject(nID, nIndex, item->second);
	
}

void Sloong::Graphics::CUIManager::Move3DModule(const int& nID, const CVector4D& vPos)
{
	GetCurrentUI()->Move3DObject(nID, vPos);
}

void Sloong::Graphics::CUIManager::Delete3DModule(const int& nID)
{

}

void Sloong::Graphics::CUIManager::SetCamera(CCamera* pCamera)
{
	m_pCamera = pCamera;
	GetCurrentUI()->SetCamera(m_pCamera);
}

void Sloong::Graphics::CUIManager::MoveCamera(const POINT4D& Position, const POINT4D& Direction, LPPOINT4D Target)
{
	m_pCamera->Move(Position, Direction, Target);
}
