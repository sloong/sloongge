#include "stdafx.h"
#include "SloongObject3D.h"
#include "SloongVector.h"
#include "SloongMatrix.h"
#include "SloongPolygon.h"
#include "SloongCamera.h"
#include "SloongDraw.h"
using namespace SoaringLoong::Graphics3D;
using namespace SoaringLoong::Math::Vector;
using namespace SoaringLoong::Math::Matrix;
using namespace SoaringLoong::Math::Polygon;
using namespace SoaringLoong::Graphics;

SoaringLoong::Graphics3D::CObject3D::CObject3D()
{
	m_pWorldPos = nullptr;
	m_pCamera = nullptr;
	m_pCameraMatrix = nullptr;
	m_pProjectMatrix = nullptr;
	m_pScreenMatrix = nullptr;

	m_VertexList = new vector<IPolygon*>;
}

SoaringLoong::Graphics3D::CObject3D::~CObject3D()
{
	m_pWorldPos = nullptr;
	m_pCamera = nullptr;
	m_pCameraMatrix = nullptr;
	m_pProjectMatrix = nullptr;
	m_pScreenMatrix = nullptr;
	SAFE_DELETE(m_VertexList);
}

void SoaringLoong::Graphics3D::CObject3D::Update()
{
	UpdateWorldVertex(*m_pWorldPos);
	//DeleteBackface()
	UpdateCameraVertex(*m_pCameraMatrix);
	UpdateProjectVertex(*m_pProjectMatrix);
	UpdateScreenVertex(*m_pScreenMatrix);
}

void SoaringLoong::Graphics3D::CObject3D::Render( CDDraw* pDDraw )
{
	LPBYTE pBuffer = pDDraw->DDraw_Lock_Back_Surface();
	//pDDraw->DrawClipLine();
	auto list = *m_VertexList;
	auto len = list.size();
	for (int i = 0; i < len; i++)
	{
		if (list[i])
		{
			list[i]->Render(pDDraw);
		}
		
	}
	pDDraw->DDraw_Unlock_Back_Surface();
}

void SoaringLoong::Graphics3D::CObject3D::DeleteBackface( const CCamera& pCam )
{
	auto len = m_VertexList->size();
	for (int i = 0; i < len; i++ )
	{
		
		auto x = (*m_VertexList)[i]->GetX();
		auto y = (*m_VertexList)[i]->GetY();
		auto z = (*m_VertexList)[i]->GetZ();
		CVector4D u, v, n;

		u = CVector4D::Subtract(*x, *y);
		v = CVector4D::Subtract(*x, *z);
		n = CVector4D::Cross(u, v);

		CVector4D view = { 0, 0, 1, 0 };
		auto dp = n.Dot(pCam.N);
		if (dp <= 0.0)
		{
			(*m_VertexList)[i]->AddAttribute(POLY_STATE_BACKFACE);
		}
		else
		{
			(*m_VertexList)[i]->AddAttribute(POLY_STATE_ACTIVE);
		}
	}
}

void SoaringLoong::Graphics3D::CObject3D::UpdateWorldVertex(const POINT4D& mWorld)
{
	if ( !m_pWorldPos )
	{
		m_pWorldPos = new CVector4D();
	}
	m_pWorldPos->Copy(mWorld);
	auto len = m_VertexList->size();
	// 对每一个多边形进行转换
	for (int i = 0; i < len; i++)
	{
		if ((*m_VertexList)[i])
		{
			(*m_VertexList)[i]->ToWorld(mWorld);
		}
		
	}
}


void SoaringLoong::Graphics3D::CObject3D::UpdateCameraVertex(const CMatrix4x4& mCamera)
{
	if ( !m_pCameraMatrix )
	{
		m_pCameraMatrix = new CMatrix4x4();
	}
	m_pCameraMatrix->Copy(mCamera);
	UpdateVertex(mCamera,false);
}

void SoaringLoong::Graphics3D::CObject3D::UpdateProjectVertex(const CMatrix4x4& mProject)
{
	if (!m_pProjectMatrix)
	{
		m_pProjectMatrix = new CMatrix4x4();
	}
	m_pProjectMatrix->Copy(mProject);
	UpdateVertex(mProject,true);
}

void SoaringLoong::Graphics3D::CObject3D::UpdateScreenVertex(const CMatrix4x4& mScreen)
{
	if ( !m_pScreenMatrix )
	{
		m_pScreenMatrix = new CMatrix4x4();
	}
	m_pScreenMatrix->Copy(mScreen);
	UpdateVertex(mScreen,true);
}

void SoaringLoong::Graphics3D::CObject3D::UpdateVertex(const CMatrix4x4& mMarix,bool bNormal)
{
	auto len = m_VertexList->size();
	for (int i = 0; i < len; i++)
	{
		if ((*m_VertexList)[i])
		{
			(*m_VertexList)[i]->Transform(mMarix, bNormal);
		}
		
	}
}

void SoaringLoong::Graphics3D::CObject3D::AddPolygon(IPolygon* pPoly)
{
	m_VertexList->push_back(pPoly);
}
