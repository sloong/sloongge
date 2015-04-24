#pragma once
#include "ITestRender.h"
#include "SloongModelLoader.h"
#include "SloongCamera.h"
#include "DInputClass.h"
#include "IUniversal.h"
using namespace SoaringLoong;
using namespace SoaringLoong::Loader;
using namespace SoaringLoong::Graphics3D;
class CTestSinglePolygon :
	public ITestRender
{
public:
	CTestSinglePolygon();
	~CTestSinglePolygon();

	virtual void Initialize(CDDraw* pDraw, DInputClass* pInput, RECT rcWindow);
	virtual void Render();

	POLYF4DV1 poly1[4];
	CCamera*			m_pCam;
	CDDraw*		m_pDraw;
	DInputClass*		m_pInput;
	RENDERLIST4DV1 rend_list; // the render list
	CAM4DV1        cam;       // the single camera
	CRect		m_rcWindow;
};

