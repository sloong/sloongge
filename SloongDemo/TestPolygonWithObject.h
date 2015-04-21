#pragma once
#include "ITestRender.h"
#include "SloongObject3D.h"
#include "SloongVector.h"
#include "SloongCamera.h"
using namespace SoaringLoong::Graphics3D;
using namespace SoaringLoong::Vector;
using namespace SoaringLoong::Graphics3D;
class CTestPolygonWithObject :
	public ITestRender
{
public:
	CTestPolygonWithObject();
	~CTestPolygonWithObject();

	virtual void Initialize();
	virtual void Render();

	CObject3D g_obj;
	CVector4D vWorldPos = { 0, 0, 0, 0 };
	CCamera*			m_pCam;
	CDDraw*		m_pDraw;
};

