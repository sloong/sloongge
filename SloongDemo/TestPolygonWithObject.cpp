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

void CTestPolygonWithObject::Initialize()
{

	CVector4D v1[] = {
		{ 10, 0, 10, 0 },
		{ 10, 0, -10, 0 },
		{ 0, 50, 0, 0 },
	};
	CVector4D v2[] = {
		{ 10, 0, -10, 0 },
		{ -10, 0, -10, 0 },
		{ 0, 50, 0, 0 },
	};
	CVector4D v3[] = {
		{ -10, 0, -10, 0 },
		{ -10, 0, 10, 0 },
		{ 0, 50, 0, 0 },
	};
	CVector4D v4[] = {
		{ -10, 0, 10, 0 },
		{ 10, 0, 10, 0 },
		{ 0, 50, 0, 0 },
	};
	CPolygon3D poly[4];
	poly[0].m_VectorList->push_back(v1[0]);
	poly[0].m_VectorList->push_back(v1[1]);
	poly[0].m_VectorList->push_back(v1[2]);
						
	poly[1].m_VectorList->push_back(v1[0]);
	poly[1].m_VectorList->push_back(v1[1]);
	poly[1].m_VectorList->push_back(v1[2]);
						
	poly[2].m_VectorList->push_back(v1[0]);
	poly[2].m_VectorList->push_back(v1[1]);
	poly[2].m_VectorList->push_back(v1[2]);
						
	poly[3].m_VectorList->push_back(v1[0]);
	poly[3].m_VectorList->push_back(v1[1]);
	poly[3].m_VectorList->push_back(v1[2]);

	g_obj.m_VertexList->push_back(poly[0]);
	g_obj.m_VertexList->push_back(poly[1]);
	g_obj.m_VertexList->push_back(poly[2]);
	g_obj.m_VertexList->push_back(poly[3]);

	CVector4D vPos = { 0, 0, 100, 0 };
	CVector4D vDir = { 0, 0, 0, 0 };

	m_pCam = new CCamera();
	m_pCam->Initialize(CAMERA_UVN, vPos, vDir, NULL, NULL, NULL, 5, 50, 90, SCREEN_WIDTH, SCREEN_HEIGHT);
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
	m_pCam->UpdateCameraMatrix();
	m_pCam->UpdateProjectMatrix();
	m_pCam->UpdateScreenMatrix();

	g_obj.Update();
	g_obj.Render(m_pDraw);

}
