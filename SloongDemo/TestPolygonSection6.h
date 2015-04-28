#pragma once
#include "ITestRender.h"
#include "SloongVector.h"
#include "SloongDraw.h"
#include "DInputClass.h"
#include "SloongCamera.h"
#include "SloongObject3D.h"
using namespace SoaringLoong::Vector;
using namespace SoaringLoong::Graphics;
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
	CObject3D*     obj_tower,    // used to hold the master tower
		*obj_tank,     // used to hold the master tank
		*obj_marker,   // the ground marker
		*obj_player;   // the player object        

	POINT4D        towers[96], tanks[24]; 
	DInputClass*		m_pInput;
	CCamera        m_cam;       // the single camera
	RECT				m_rcWindow;
};

