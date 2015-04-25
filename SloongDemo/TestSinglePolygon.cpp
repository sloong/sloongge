#include "stdafx.h"
#include "TestSinglePolygon.h"
#include "Defines.h"

CTestSinglePolygon::CTestSinglePolygon()
{
}


CTestSinglePolygon::~CTestSinglePolygon()
{
}

void CTestSinglePolygon::Initialize(CDDraw* pDraw, DInputClass* pInput, RECT rcWindow)
{
	m_pDraw = pDraw;
	m_pInput = pInput;
	m_rcWindow = rcWindow;

	// initialize a single polygon
	auto poly = &poly1[0];
	poly->state = POLY4DV1_STATE_ACTIVE;
	poly->attr = 0;
	poly->color = RGB(0, 255, 0);
	poly->worldPos = { 0, 0, 0, 1 };
	poly->vlist[0] = { 0, 50, 0, 1 };
	poly->vlist[1] = { 10, 0, 10, 1 };
	poly->vlist[2] = { 10, 0, -10, 1 };
	poly->prev = NULL;
	poly->next = &poly1[1];
	poly = &poly1[1];
	poly->state = POLY4DV1_STATE_ACTIVE;
	poly->attr = 0;
	poly->color = RGB(0, 255, 0);
	poly->worldPos = { 0, 0, 0, 1 };
	poly->vlist[0] = { 0, 50, 0, 1 };
	poly->vlist[1] = { 10, 0, 10, 1 };
	poly->vlist[2] = { -10, 0, 10, 1 };
	poly->prev = &poly1[0];
	poly->next = &poly1[2];
	poly = &poly1[2];
	poly->state = POLY4DV1_STATE_ACTIVE;
	poly->attr = 0;
	poly->color = RGB(0, 255, 0);
	poly->worldPos = { 0, 0, 0, 1 };
	poly->vlist[0] = { 0, 50, 0, 1 };
	poly->vlist[1] = { 10, 0, -10, 1 };
	poly->vlist[2] = { -10, 0, -10, 1 };
	poly->prev = &poly1[1];
	poly->next = &poly1[3];
	poly = &poly1[3];
	poly->state = POLY4DV1_STATE_ACTIVE;
	poly->attr = 0;
	poly->color = RGB(0, 255, 0);
	poly->worldPos = { 0, 0, 0, 1 };
	poly->vlist[0] = { 0, 50, 0, 1 };
	poly->vlist[1] = { -10, 0, -10, 1 };
	poly->vlist[2] = { -10, 0, 10, 1 };
	poly->prev = &poly1[2];
	poly->next = NULL;


	POINT4D  cam_pos = { 0, 0, -200, 1 };
	POINT4D  cam_target = { 0, 0, 0, 1 };
	VECTOR4D cam_dir = { 0, 0, 0, 1 };
	VECTOR4D cv = { 0, 0, 1, 1 };

	m_pCam = new CCamera();
	m_pCam->Initialize(CAMERA_TYPE::CAMERA_ELUER, cam_pos, cam_dir, &cam_target, CAM_ROT_SEQ_ZYX, 5, 50, 90, WINDOW_WIDTH, WINDOW_HEIGHT);
	m_pCam->UpdateCameraMatrix();

	// initialize the camera with 90 FOV, normalized coordinates
	/*CPLGLoader::Init_CAM4DV1(&cam,      // the camera object
	CAM_MODEL_EULER, // euler camera model
	&cam_pos,  // initial camera position
	&cam_dir,  // initial camera angles
	NULL,      // no initial target
	50.0,      // near and far clipping planes
	500.0,
	90.0,      // field of view in degrees
	WINDOW_WIDTH,   // size of final screen viewport
	WINDOW_HEIGHT);*/

}

void CTestSinglePolygon::Render()
{
	m_pDraw->Start_Clock();
	static MATRIX4X4 mrot; // general rotation matrix
	static float ang_y = 0;      // rotation angle

	// read keyboard and other devices here
	m_pInput->GetInput();
	m_pCam->UpdateCameraMatrix();

	// initialize the renderlist
	CPLGLoader::Reset_RENDERLIST4DV1(&rend_list);

	// insert polygon into the renderlist
	for (int i = 0; i < ARRAYSIZE(poly1); i++)
	{
		CPLGLoader::Insert_POLYF4DV1_RENDERLIST4DV1(&rend_list, &poly1[i]);
	}


	// generate rotation matrix around y axis
	//CPLGLoader::Build_XYZ_Rotation_MATRIX4X4(0, ang_y, 0, &mrot);

	// rotate polygon slowly
	//if (++ang_y >= 360.0) ang_y = 0;



	// rotate the local coords of single polygon in renderlist
	// 对渲染列表的多边形执行旋转
	//CPLGLoader::Transform_RENDERLIST4DV1(&rend_list, &mrot, TRANSFORM_LOCAL_ONLY);

	//CPLGLoader::Transform_RENDERLIST4DV1(&rend_list, &m_pCam->MatrixCamera, TRANSFORM_TRANS_ONLY);

	// perform local/model to world transform
	// 模型坐标到世界坐标的转换
	CPLGLoader::Model_To_World_RENDERLIST4DV1(&rend_list, 2);


	// generate camera matrix
	// 构建相机矩阵
	//CPLGLoader::Build_CAM4DV1_Matrix_Euler(&cam, CAM_ROT_SEQ_ZYX);

	// apply world to camera transform
	// 世界坐标到相机坐标的转换
	CPLGLoader::World_To_Camera_RENDERLIST4DV1(&rend_list, &m_pCam->MatrixCamera);
	//CPLGLoader::World_To_Camera_RENDERLIST4DV1(&rend_list, &cam.mcam);

	// apply camera to perspective transformation
	// 应用相机坐标到透视的转换
	CPLGLoader::Camera_To_Perspective_RENDERLIST4DV1(&rend_list, &cam);

	// apply screen transform
	//	CPLGLoader::Perspective_To_Screen_RENDERLIST4DV1(&rend_list, &cam);

	for (int j = 0; j < 4; ++j)
	{
		auto obj = &rend_list.poly_data[j];
		for (int i = 0; i < 3; ++i)
		{

			obj->tvlist[i].x *= SCREEN_WIDTH;
			obj->tvlist[i].x += (SCREEN_WIDTH / 2);
			obj->tvlist[i].y *= SCREEN_WIDTH;
			obj->tvlist[i].y += (SCREEN_WIDTH / 2) - 100;
		}
	}

	// draw instructions
	m_pDraw->DrawText(_T("Press ESC to exit."), 0, 0, RGB(0, 255, 0), m_pDraw->GetBackSurface());

	// lock the back buffer
	LPBYTE lpBack = m_pDraw->DDraw_Lock_Back_Surface();

	// render the polygon list
	CPLGLoader::Draw_RENDERLIST4DV1_Wire16(&rend_list, lpBack, m_pDraw->GetBackPitch(), m_rcWindow);

	// unlock the back buffer
	m_pDraw->DDraw_Unlock_Back_Surface();

	m_pDraw->Wait_Clock(30);

	// check of user is trying to exit
	if (KEY_DOWN(VK_ESCAPE) || m_pInput->IsKeyDown(DIK_ESCAPE))
	{
		PostQuitMessage(0);

	} // end if

	if (m_pInput->IsKeyDown(DIK_SPACE))
	{
		m_pCam->WorldPos.y += 10;
	}

	if (m_pInput->IsKeyDown(DIK_NUMPAD0))
	{
		m_pCam->WorldPos.y -= 10;
	}

	if (m_pInput->IsKeyDown(DIK_UP))
	{
		// move forward
		m_pCam->WorldPos.z += 10;

	} // end if

	if (m_pInput->IsKeyDown(DIK_DOWN))
	{
		// move backward
		m_pCam->WorldPos.z -= 10;

	} // end if

	// rotate
	if (m_pInput->IsKeyDown(DIK_RIGHT))
	{
		m_pCam->WorldPos.x += 3;
		//m_pCam->WorldPos.x += CMath2::Fast_Sin(m_pCam->Direction.y);
		//m_pCam->WorldPos.z += CMath2::Fast_Cos(m_pCam->Direction.y);
		m_pCam->Direction.x += 3;

		// 		add a little turn to object
		// 		if ((turning += 2) > 15)
		// 			turning = 15;

	} // end if

	if (m_pInput->IsKeyDown(DIK_LEFT))
	{
		m_pCam->WorldPos.x -= 3;
		//m_pCam->WorldPos.x -= CMath2::Fast_Sin(m_pCam->Direction.y);
		//m_pCam->WorldPos.z -= CMath2::Fast_Cos(m_pCam->Direction.y);
		m_pCam->Direction.x -= 3;

		// add a little turn to object
		// 		if ((turning -= 2) < -15)
		// 			turning = -15;

	} // end if
}
