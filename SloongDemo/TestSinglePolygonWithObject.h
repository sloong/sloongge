#pragma once
#include "ITestRender.h"
#include "SloongModelLoader.h"
#include "SloongCamera.h"
#include "IUniversal.h"
using namespace SoaringLoong;
using namespace SoaringLoong::Loader;
using namespace SoaringLoong::Graphics3D;
class CTestSinglePolygonWithObject :
	public ITestRender
{
public:
	CTestSinglePolygonWithObject();
	~CTestSinglePolygonWithObject();

	virtual void Initialize();
	virtual void Render();

	//OBJECT4DV1 obj;
	POLYF4DV1 poly1[4];
	CCamera*			m_pCam;
	// Test
	RENDERLIST4DV1 rend_list; // the render list
	CDDraw*		m_pDraw;
	CRect		m_rcWindow;
};

