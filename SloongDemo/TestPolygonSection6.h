#pragma once
#include "ITestRender.h"
#include "SloongVector.h"
#include "SloongDraw.h"
#include "SloongModelLoader.h"
#include "DInputClass.h"
#include "SloongCamera.h"
using namespace SoaringLoong::Vector;
using namespace SoaringLoong::Graphics;
using namespace SoaringLoong::Loader;
using namespace SoaringLoong::Graphics3D;

#define NUM_TOWERS        96
#define NUM_TANKS         24


class CTestPolygonSection6 :
	public ITestRender
{
public:
	CTestPolygonSection6();
	~CTestPolygonSection6();

	virtual void Initialize(CDDraw* pDraw, DInputClass* pInput, RECT rcWindow);
	virtual void Render();


	VECTOR4D vscale, vpos, vrot;
	CDDraw*		m_pDraw;
	OBJECT4DV1     obj_tower,    // used to hold the master tower
		obj_tank,     // used to hold the master tank
		obj_marker,   // the ground marker
		obj_player;   // the player object        

	POINT4D        towers[96], tanks[24]; 
	DInputClass*		m_pInput;
	RENDERLIST4DV1 rend_list;
	CAM4DV1        cam;       // the single camera
	CCamera        m_cam;       // the single camera
	RECT				m_rcWindow;
};

