#include "stdafx.h"
#include "TestPolygonWithObject.h"
#include "SloongModelLoader.h"
#include "SloongPolygon.h"
SoaringLoong::Loader::RENDERLIST4DV1 rend_list;
using namespace SoaringLoong::Polygon;
#include "SloongObject3D.h"
using namespace SoaringLoong::Graphics3D;
#include "Defines.h"

CTestPolygonWithObject::CTestPolygonWithObject()
{
}


CTestPolygonWithObject::~CTestPolygonWithObject()
{
}

void CTestPolygonWithObject::Initialize(CDDraw* pDraw, DInputClass* pInput, RECT rcWindow)
{
	m_pDraw = pDraw;
	m_pInput = pInput;
	CVector4D v1[] = {
		{ 1, 0, 1, 0 },
		{ 1, 0, -1, 0 },
		{ 0, 5, 0, 0 },
	};
	CVector4D v2[] = {
		{ 1, 0, -1, 0 },
		{ -1, 0, -1, 0 },
		{ 0, 5, 0, 0 },
	};
	CVector4D v3[] = {
		{ -1, 0, -1, 0 },
		{ -1, 0, 1, 0 },
		{ 0, 5, 0, 0 },
	};
	CVector4D v4[] = {
		{ -1, 0, 1, 0 },
		{ 1, 0, 1, 0 },
		{ 0, 5, 0, 0 },
	};
	
	poly[0] = IPolygon::Create3D();
	poly[0]->Initialize(v1[0], v1[1], v1[2]);
// 	poly[0].m_VectorList->push_back(v1[0]);
// 	poly[0].m_VectorList->push_back(v1[1]);
// 	poly[0].m_VectorList->push_back(v1[2]);
	poly[0] = IPolygon::Create3D();
	poly[0]->Initialize(v2[0], v2[1], v2[2]);
// 	poly[1].m_VectorList->push_back(v1[0]);
// 	poly[1].m_VectorList->push_back(v1[1]);
// 	poly[1].m_VectorList->push_back(v1[2]);
	poly[0] = IPolygon::Create3D();
	poly[0]->Initialize(v3[0], v3[1], v3[2]);
// 	poly[2].m_VectorList->push_back(v1[0]);
// 	poly[2].m_VectorList->push_back(v1[1]);
// 	poly[2].m_VectorList->push_back(v1[2]);
	poly[0] = IPolygon::Create3D();
	poly[0]->Initialize(v4[0], v4[1], v4[2]);
// 	poly[3].m_VectorList->push_back(v1[0]);
// 	poly[3].m_VectorList->push_back(v1[1]);
// 	poly[3].m_VectorList->push_back(v1[2]);

 	g_obj.AddPolygon(poly[0]);
	g_obj.AddPolygon(poly[1]);
	g_obj.AddPolygon(poly[2]);
	g_obj.AddPolygon(poly[3]);

	CVector4D vPos = { 0, 0, 100, 0 };
	CVector4D vDir = { 0, 0, 0, 0 };

	m_pCam = new CCamera();
	m_pCam->Initialize(CAMERA_UVN, vPos, vDir, NULL, UVN_MODE_SIMPLE, 5, 50, 90, SCREEN_WIDTH, SCREEN_HEIGHT);
	m_pCam->UpdateCameraMatrix();
	m_pCam->UpdateProjectMatrix();
	m_pCam->UpdateScreenMatrix();

	g_obj.m_pCamera = m_pCam;
	g_obj.m_pCameraMatrix = &m_pCam->MatrixCamera;
	g_obj.m_pProjectMatrix = &m_pCam->MatrixProjection;
	g_obj.m_pScreenMatrix = &m_pCam->MatrixScreen;
	g_obj.m_pWorldPos = &vWorldPos;
}

void CTestPolygonWithObject::Render()
{
	m_pInput->GetInput();
	if (m_pInput->IsKeyDown(DIK_SPACE))
	{
		m_pCam->WorldPos.y += 10;
	}

	if (m_pInput->IsKeyDown(DIK_NUMPAD0))
	{
		m_pCam->WorldPos.y -= 10;
	}

	if (m_pInput->IsKeyDown(DIK_UP))
	{
		// move forward
		m_pCam->WorldPos.z += 10;

	} // end if

	if (m_pInput->IsKeyDown(DIK_DOWN))
	{
		// move backward
		m_pCam->WorldPos.z -= 10;

	} // end if

	// rotate
	if (m_pInput->IsKeyDown(DIK_RIGHT))
	{
		m_pCam->WorldPos.x += 3;
	} // end if

	if (m_pInput->IsKeyDown(DIK_LEFT))
	{
		m_pCam->WorldPos.x -= 3;
	} // end if
	m_pCam->UpdateCameraMatrix();
// 	m_pCam->UpdateProjectMatrix();
// 	m_pCam->UpdateScreenMatrix();

	g_obj.Update();
	g_obj.Render(m_pDraw);

}
