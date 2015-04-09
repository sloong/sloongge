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
}

SoaringLoong::Graphics3D::CObject3D::~CObject3D()
{
	m_pWorldPos = nullptr;
	m_pCamera = nullptr;
	m_pCameraMatrix = nullptr;
	m_pProjectMatrix = nullptr;
	m_pScreenMatrix = nullptr;
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
	//LPBYTE pBuffer = pDDraw->DDraw_Lock_Back_Surface();
	//pDDraw->DrawClipLine();
	auto len = m_VertexList.size();
	for (int i = 0; i < len; i++)
	{
		m_VertexList[i].Render(pDDraw);
	}
}

void SoaringLoong::Graphics3D::CObject3D::DeleteBackface( const CCamera& pCam )
{
	auto len = m_VertexList.size();
	for (int i = 0; i < len; i++ )
	{
		auto& item = m_VertexList[i];

		auto x = item.m_VectorList[0];
		auto y = item.m_VectorList[1];
		auto z = item.m_VectorList[2];
		CVector4D u, v, n;

		u = CVector4D::Subtract(x, y);
		v = CVector4D::Subtract(x, z);
		n = CVector4D::Cross(u, v);

		CVector4D view = { 0, 0, 1, 0 };
		auto dp = n.Dot(pCam.N);
		if (dp <= 0.0)
		{
			item.m_dwAttribute |= POLY_STATE_BACKFACE;
		}
		else
		{
			item.m_dwAttribute |= POLY_STATE_ACTIVE;
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
	auto len = m_VertexList.size();
	for (int i = 0; i < len; i++)
	{
		auto& item = m_VertexList[i];
		item.m_WorldVertex.clear();
		for (int i = 0; i < item.m_VectorList.size(); i++)
		{
			item.m_WorldVertex.push_back( CVector4D::Add(item.m_VectorList[i], mWorld));
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
	auto len = m_VertexList.size();
	for (int i = 0; i < len; i++)
	{
		auto& item = m_VertexList[i];
		item.m_CameraVertexList.clear();
		for (int i = 0; i < item.m_WorldVertex.size(); i++)
		{
			item.m_CameraVertexList.push_back( CVector4D::Multiply(item.m_WorldVertex[i], mCamera));
		}
	}
}

void SoaringLoong::Graphics3D::CObject3D::UpdateProjectVertex(const CMatrix4x4& mProject)
{
	if (!m_pProjectMatrix)
	{
		m_pProjectMatrix = new CMatrix4x4();
	}
	m_pProjectMatrix->Copy(mProject);
	auto len = m_VertexList.size();
	for (int i = 0; i < len; i++)
	{
		auto& item = m_VertexList[i];
		item.m_ProjectVertexList.clear();
		for (int i = 0; i < item.m_CameraVertexList.size(); i++)
		{
			auto& temp = CVector4D::Multiply(item.m_CameraVertexList[i], mProject);
			temp.x /= temp.w;
			temp.y /= temp.w;
			temp.z /= temp.w;
			temp.w = 1;

			item.m_ProjectVertexList.push_back(temp);
		}
	}
}

void SoaringLoong::Graphics3D::CObject3D::UpdateScreenVertex(const CMatrix4x4& mScreen)
{
	if ( !m_pScreenMatrix )
	{
		m_pScreenMatrix = new CMatrix4x4();
	}
	m_pScreenMatrix->Copy(mScreen);
	auto len = m_VertexList.size();
	for (int i = 0; i < len; i++)
	{
		auto& item = m_VertexList[i];
		item.m_ScreenVertexList.clear();
		for (int i = 0; i < item.m_ProjectVertexList.size(); i++)
		{
			auto& temp = CVector4D::Multiply(item.m_ProjectVertexList[i], mScreen);
			temp.x /= temp.w;
			temp.y /= temp.w;
			temp.z /= temp.w;
			temp.w = 1;

			item.m_ScreenVertexList.push_back(temp);
		}
	}
}
