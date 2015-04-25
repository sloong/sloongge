#pragma once
#include "ITestRender.h"
#include "SloongObject3D.h"
#include "SloongVector.h"
#include "SloongCamera.h"
#include "SloongPolygon.h"
#include "DInputClass.h"
using namespace SoaringLoong::Graphics3D;
using namespace SoaringLoong::Vector;
using namespace SoaringLoong::Polygon;
class CTestPolygonWithObject :
	public ITestRender
{
public:
	CTestPolygonWithObject();
	~CTestPolygonWithObject();

	virtual void Initialize(CDDraw* pDraw, DInputClass* pInput, RECT rcWindow);
	virtual void Render();

	CObject3D g_obj;
	CVector4D vWorldPos = { 0, 0, 0, 0 };
	CCamera*			m_pCam;
	CDDraw*		m_pDraw;
	IPolygon* poly[4];
	DInputClass* m_pInput;
};

