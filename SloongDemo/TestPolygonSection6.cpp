#include "stdafx.h"
#include "TestPolygonSection6.h"
#include "SloongMath2.h"
using namespace SoaringLoong::Math;
#include "Defines.h"
#include "SloongString.h"
using namespace SoaringLoong::Universal;
CTestPolygonSection6::CTestPolygonSection6()
{
}


CTestPolygonSection6::~CTestPolygonSection6()
{
}

void CTestPolygonSection6::Initialize(CDDraw* pDraw, DInputClass* pInput, RECT rcWindow)
{

	vscale = { 1, 1, 1, 1 }, vpos = { 0, 0, 0, 1 }, vrot = { 0, 0, 0, 1 };

	// initialize camera 
	POINT4D  cam_pos = { 0, 40, 0, 1 };
	POINT4D  cam_target = { 0, 0, 0, 1 };
	VECTOR4D cam_dir = { 0, 0, 0, 1 };

	m_pDraw = pDraw;
	m_pInput = pInput;
	m_rcWindow = rcWindow;
	
	m_cam.Initialize(CAMERA_ELUER, cam_pos, cam_dir, &cam_target, CAM_ROT_SEQ_XYZ, 200, 12000, 120, WINDOW_WIDTH, WINDOW_HEIGHT);
	//m_cam.Initialize(CAMERA_UVN, cam_pos, cam_dir, &cam_target, UVN_MODE_SPHERICAL, 200, 12000, 120, WINDOW_WIDTH, WINDOW_HEIGHT);

	obj_tank = new CObject3D(m_pDraw);
	obj_marker = new CObject3D(m_pDraw);
	obj_player = new CObject3D(m_pDraw);
	obj_tower = new CObject3D(m_pDraw);

	// load the master tank object
 	vscale.Initialize(0.75, 0.75, 0.75);
 	obj_tank->LoadPLGMode(_T("DXFile\\tank2.plg"), vscale, vpos, vrot);

	// load player object for 3rd person view
	vscale.Initialize(0.75, 0.75, 0.75);
	obj_player->LoadPLGMode(_T("DXFile\\tank3.plg"), vscale, vpos, vrot);

	// load the master tower object
	vscale.Initialize(1.0, 2.0, 1.0);
	obj_tower->LoadPLGMode(_T("DXFile\\tower1.plg"), vscale, vpos, vrot);

	// load the master ground marker
	vscale.Initialize(3.0, 3.0, 3.0);
	obj_marker->LoadPLGMode(_T("DXFile\\marker1.plg"), vscale, vpos, vrot);

#define UNIVERSE_RADIUS   4000

	// position the tanks
	for (int index = 0; index < NUM_TANKS; index++)
	{
		// randomly position the tanks
		tanks[index].x = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
		tanks[index].y = 0; // obj_tank.max_radius;
		tanks[index].z = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
		tanks[index].w = RAND_RANGE(0, 360);
	} // end for


	// position the towers
	for (int index = 0; index < NUM_TOWERS; index++)
	{
		// randomly position the tower
		towers[index].x = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
		towers[index].y = 0; // obj_tower.max_radius;
		towers[index].z = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
	} // end for
}

void CTestPolygonSection6::Render()
{
	m_pDraw->Start_Clock();
	static MATRIX4X4 mrot;   // general rotation matrix

	// these are used to create a circling camera
	static float view_angle = 0;
	static float camera_distance = 6000;
	static VECTOR4D pos = { 0, 0, 0, 0 };
	static float tank_speed;
	static float turning = 0;

	char work_string[256]; // temp string

	int index; // looping var

	// draw the sky
	m_pDraw->Draw_Rectangle(0, 0, WINDOW_WIDTH - 1, WINDOW_HEIGHT / 2, RGB(0, 140, 192), m_pDraw->GetBackSurface());

	// draw the ground
	m_pDraw->Draw_Rectangle(0, WINDOW_HEIGHT / 2, WINDOW_WIDTH - 1, WINDOW_HEIGHT - 1, RGB(103, 62, 3), m_pDraw->GetBackSurface());

	// read keyboard and other devices here
	m_pInput->GetInput();
	// game logic here...

	// allow user to move camera
#define TANK_SPEED        15
	tank_speed = TANK_SPEED;
	if (m_pInput->IsKeyDown(DIK_ESCAPE) || KEY_DOWN(VK_ESCAPE))
	{
		PostQuitMessage(0);
		return;
	}

	// turbo
	if (m_pInput->IsKeyDown(DIK_SPACE))
	{
		m_cam.WorldPos.y += 15;
	}
	// forward/backward
	if (m_pInput->IsKeyDown(DIK_UP))
	{
		// move forward
		m_cam.WorldPos.x += tank_speed*CMath2::Fast_Sin(m_cam.Direction.y);
		m_cam.WorldPos.z += tank_speed*CMath2::Fast_Cos(m_cam.Direction.y);
	} // end if

	if (m_pInput->IsKeyDown(DIK_DOWN))
	{
		// move backward
		m_cam.WorldPos.x -= tank_speed*CMath2::Fast_Sin(m_cam.Direction.y);
		m_cam.WorldPos.z -= tank_speed*CMath2::Fast_Cos(m_cam.Direction.y);
	} // end if

	// rotate
	if (m_pInput->IsKeyDown(DIK_RIGHT))
	{
		m_cam.Direction.y += 3;

		// add a little turn to object
		if ((turning += 2) > 15)
			turning = 15;

	} // end if

	if (m_pInput->IsKeyDown(DIK_LEFT))
	{
		m_cam.Direction.y -= 3;

		// add a little turn to object
		if ((turning -= 2) < -15)
			turning = -15;

	} // end if
	else // center heading again
	{
		if (turning > 0)
			turning -= 1;
		else
		if (turning < 0)
			turning += 1;

	} // end else

	// generate camera matrix
	m_cam.UpdateCameraMatrix();

	obj_player->Reset();

	
	obj_player->SetWorldPosition(CVector4D(
		m_cam.WorldPos.x + 300 * CMath2::Fast_Sin(m_cam.Direction.y), 
		m_cam.WorldPos.y - 70, 
		m_cam.WorldPos.z + 300 * CMath2::Fast_Cos(m_cam.Direction.y)));

	mrot.BuildRotateMatrix(0, m_cam.Direction.y + turning, 0);

	obj_player->Transform(mrot, TRANS_MODE::LocalToTrans, true);

	obj_player->ToWorld(TransOnly);

	obj_player->ToCamera(&m_cam);
	obj_player->ToProject(&m_cam);
	obj_player->PerspectiveToScreen(&m_cam);
	//obj_player->CameraToPerspectiveScreen(&m_cam);
	obj_player->Render(m_pDraw);

	// insert the tanks in the world
	for (index = 0; index < NUM_TANKS; index++)
	{
		// reset the object (this only matters for backface and object removal)
		
		obj_tank->Reset();

		// generate rotation matrix around y axis
		mrot.BuildRotateMatrix(0, tanks[index].w, 0);

		// rotate the local coords of the object
		obj_tank->Transform(mrot, TRANS_MODE::LocalToTrans, true);
		
		// set position of tank
		obj_tank->SetWorldPosition(CVector4D(tanks[index].x, tanks[index].y, tanks[index].z));
		obj_tank->Cull(&m_cam, CULL_ON_XYZ_PLANES);
		if (obj_tank->Visible())
		{
			obj_tank->ToWorld(TRANS_MODE::TransOnly);
			//obj_tank->RemoveBackface(&m_cam);
			obj_tank->ToCamera(&m_cam);
			obj_tank->ToProject(&m_cam);
			obj_tank->PerspectiveToScreen(&m_cam);
			//obj_tanke->CameraToPerspectiveScreen(&m_cam);
			obj_tank->Render(m_pDraw);

		}
		
	} // end for

	for (index = 0; index < NUM_TOWERS; index++)
	{
		// reset the object (this only matters for backface and object removal)

		obj_tower->Reset();

		// generate rotation matrix around y axis
		mrot.BuildRotateMatrix(0, towers[index].w, 0);

		// rotate the local coords of the object
		obj_tower->Transform(mrot, TRANS_MODE::LocalToTrans, true);

		// set position of tank
		obj_tower->SetWorldPosition(towers[index]);
		obj_tower->Cull(&m_cam, CULL_ON_XYZ_PLANES);
		if (obj_tower->Visible())
		{
			obj_tower->ToWorld(TRANS_MODE::TransOnly);
			//obj_tower->RemoveBackface(&m_cam);
			obj_tower->ToCamera(&m_cam);
			obj_tower->ToProject(&m_cam);
			obj_tower->PerspectiveToScreen(&m_cam);
			//obj_tanke->CameraToPerspectiveScreen(&m_cam);
			obj_tower->Render(m_pDraw);

		}

	} // end for

	
	// seed number generator so that modulation of markers is always the same
	srand(13);

#define POINT_SIZE        200
#define NUM_POINTS_X      (2*UNIVERSE_RADIUS/POINT_SIZE)
#define NUM_POINTS_Z      (2*UNIVERSE_RADIUS/POINT_SIZE)
#define NUM_POINTS        (NUM_POINTS_X*NUM_POINTS_Z)
	// insert the ground markers into the world
	for (int index_x = 0; index_x < NUM_POINTS_X; index_x++)
	for (int index_z = 0; index_z < NUM_POINTS_Z; index_z++)
	{
		// reset the object (this only matters for backface and object removal)
		obj_marker->Reset();

		// generate rotation matrix around y axis
		mrot.BuildRotateMatrix(0, 1, 0);

		// rotate the local coords of the object
		obj_marker->Transform(mrot, LocalToTrans, true);

		// set position of tower
		obj_marker->SetWorldPosition(CVector4D(
			RAND_RANGE(-100, 100) - UNIVERSE_RADIUS + index_x*POINT_SIZE,
		obj_marker->m_fMaxRadius,
		RAND_RANGE(-100, 100) - UNIVERSE_RADIUS + index_z*POINT_SIZE));

		// attempt to cull object   
		obj_marker->Cull(&m_cam, CULL_MODE::CULL_ON_XYZ_PLANES);
		if (obj_marker->Visible())
		{
			// if we get here then the object is visible at this world position
			// so we can insert it into the rendering list
			// perform local/model to world transform
			obj_marker->ToWorld(TRANS_MODE::TransOnly);

			obj_marker->ToCamera(&m_cam);
			obj_marker->ToProject(&m_cam);
			obj_marker->PerspectiveToScreen(&m_cam);
			obj_marker->Render(m_pDraw);
		} // end if

	} // end for
	

	sprintf_s(work_string, 256, "pos:[%f, %f, %f] heading:[%f] elev:[%f]",
		m_cam.WorldPos.x, m_cam.WorldPos.y, m_cam.WorldPos.z, m_cam.Direction.y, m_cam.Direction.x);

	CString str(work_string);
	m_pDraw->DrawText(str.GetString().c_str(), 0, WINDOW_HEIGHT - 20, RGB(0, 255, 0), m_pDraw->GetBackSurface());

	// draw instructions
	m_pDraw->DrawText(_T("Press ESC to exit. Press Arrow Keys to Move. Space for TURBO."), 0, 0, RGB(0, 255, 0), m_pDraw->GetBackSurface());

	// lock the back buffer
	LPBYTE pBackBuffer = m_pDraw->DDraw_Lock_Back_Surface();

	// unlock the back buffer
	m_pDraw->DDraw_Unlock_Back_Surface();

	// sync to 30ish fps
	m_pDraw->Wait_Clock(30);
}
