#include "stdafx.h"
#include "SloongCamera.h"
#include "SloongMathBase.h"
#include "SloongMath2.h"
using namespace SoaringLoong::Math;
using namespace SoaringLoong::Math::Vector;

SoaringLoong::Graphics3D::CCamera::CCamera()
{

}

SoaringLoong::Graphics3D::CCamera::~CCamera()
{

}


void SoaringLoong::Graphics3D::CCamera::Initialize(CAMERA_TYPE emType, const POINT4D& Position, const POINT4D& Direction, LPPOINT4D Target, int nMode, double NearZ, double farZ, double FOV, double ScreenWidth, double ScreenHeight)
{
	// this function initializes the camera object cam, the function
	// doesn't do a lot of error checking or sanity checking since 
	// I want to allow you to create projections as you wish, also 
	// I tried to minimize the number of parameters the functions needs

	// first set up parms that are no brainers
	// 设置类型
	this->emType = emType;
	// 设置位置和朝向
	this->m_WorldPos = Position;// positions
	this->m_Direction = Direction;// direction vector or angles for
	// euler camera
	// for UVN camera
	this->U.Initialize(1, 0, 0);
	this->V.Initialize(0, 1, 0);
	this->N.Initialize(0, 0, 1);

	this->nMode = nMode;
	// 设置相机目标点
	if ( Target )
	{
		this->m_Target = Target;
	}
	else
	{
		this->m_Target.Zero();
	}

	// 计算裁剪面和屏幕参数
	this->NearZ = NearZ; // near z=constant clipping plane
	this->FarZ = farZ;// far z=constant clipping plane
	this->ScreenWidth = ScreenWidth;// dimensions of viewport
	this->ScreenHeight = ScreenHeight;
	this->ScreenCenterX = (ScreenWidth - 1) / 2;// center of viewport
	this->ScreenCenterY = (ScreenHeight - 1) / 2;
	this->AspectRatio = ScreenWidth / ScreenHeight;

	// set all camera matrices to identity matrix
	this->MatrixCamera.Zero();
	this->MatrixProjection.Zero();
	this->MatrixScreen.Zero();

	// set independent vars
	this->FOV = FOV;
	// set the viewplane dimensions up, they will be 2 x (2/ar)

	this->ViewPlaneWidth = 2.0;
	this->ViewPlaneHeight = 2.0 / this->AspectRatio;

	// now we know fov and we know the viewplane dimensions plug into formula and
	// solve for view distance parameters

	
	// 根据FOV和视平面计算d
	this->ViewDistance = (0.5) * (this->ViewPlaneWidth) * tan(AngleToRadian(FOV / 2));

	// test for 90 fov first since it's easy :)
	// 所有裁剪面都过原点
	// set up the clipping planes -- easy for 90 degrees!
	POINT3D Pos;// point on the plane
	Pos.Initialize(0, 0, 0);
	// 面法线
	VECTOR3D vn;// normal to plane
	if (FOV == 90.0)
	{
		// 右裁剪面
		vn.Initialize(1, 0, -1);
		this->ClipPlaneRight.Initialize(Pos, vn, true);
		
		// 左裁剪面
		vn.Initialize(-1, 0, -1);
		this->ClipPlaneLeft.Initialize(Pos, vn, true);
		
		// 上裁剪面
		vn.Initialize(0, 1, -1);
		this->ClipPlaneUp.Initialize(Pos, vn, true);
		
		// 下裁剪面
		vn.Initialize(0, -1, -1);
		this->ClipPlaneDown.Initialize(Pos, vn, true);
	} // end if d=1
	else
	{
		// 如果视野不是90度，则在算某个裁剪面的法向量时，先去视平面上四个角上在该平面上的两个角作为该裁剪面上的两个向量，然后求叉乘，即可
		// 下面的法向量vn直接使用了结果
		// since we don't have a 90 fov, computing the normals
		// are a bit tricky, there are a number of geometric constructions
		// that solve the problem, but I'm going to solve for the
		// vectors that represent the 2D projections of the frustrum planes
		// on the x-z and y-z planes and then find perpendiculars to them

		// right clipping plane, check the math on graph paper 
		double dwTempWidth = ((this->ViewPlaneWidth) / 2.0);
		// 右裁剪面
		vn.Initialize(this->ViewDistance, 0, -dwTempWidth);
		this->ClipPlaneRight.Initialize(Pos, vn, true);

		// 左裁剪面
		vn.Initialize(-(this->ViewDistance), 0, -dwTempWidth);
		this->ClipPlaneLeft.Initialize(Pos, vn, true);

		// 上裁剪面
		vn.Initialize(0, this->ViewDistance, -dwTempWidth);
		this->ClipPlaneUp.Initialize(Pos, vn, true);

		// 下裁剪面
		vn.Initialize(0, -this->ViewDistance, -dwTempWidth);
		this->ClipPlaneDown.Initialize(Pos, vn, true);
	} // end else
}

void SoaringLoong::Graphics3D::CCamera::UpdateCameraMatrix()
{
	if ( emType == CAMERA_ELUER )
	{
		// this creates a camera matrix based on Euler angles 
		// and stores it in the sent camera object
		// if you recall from chapter 6 to create the camera matrix
		// we need to create a transformation matrix that looks like:

		// Mcam = mt(-1) * my(-1) * mx(-1) * mz(-1)
		// that is the inverse of the camera translation matrix mutilplied
		// by the inverses of yxz, in that order, however, the order of
		// the rotation matrices is really up to you, so we aren't going
		// to force any order, thus its programmable based on the value
		// of cam_rot_seq which can be any value CAM_ROT_SEQ_XYZ where 
		// XYZ can be in any order, YXZ, ZXY, etc.

		MATRIX4X4 mt_inv,  // inverse camera translation matrix
			mx_inv,  // inverse camera x axis rotation matrix
			my_inv,  // inverse camera y axis rotation matrix
			mz_inv,  // inverse camera z axis rotation matrix
			mrot,    // concatenated inverse rotation matrices
			mtmp;    // temporary working matrix


		// step 1: create the inverse translation matrix for the camera
		// position
		mt_inv.Initialize(1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			-this->m_WorldPos.x, -this->m_WorldPos.y, -this->m_WorldPos.z, 1);

		// step 2: create the inverse rotation sequence for the camera
		// rember either the transpose of the normal rotation matrix or
		// plugging negative values into each of the rotations will result
		// in an inverse matrix

		// first compute all 3 rotation matrices

		// extract out euler angles
		float theta_x = this->m_Direction.x;
		float theta_y = this->m_Direction.y;
		float theta_z = this->m_Direction.z;

		// compute the sine and cosine of the angle x
		float cos_theta = CMathBase::Fast_Cos(theta_x);  // no change since cos(-x) = cos(x)
		float sin_theta = -CMathBase::Fast_Sin(theta_x); // sin(-x) = -sin(x)

		// set the matrix up 
		mx_inv.Initialize(1, 0, 0, 0,
			0, cos_theta, sin_theta, 0,
			0, -sin_theta, cos_theta, 0,
			0, 0, 0, 1);

		// compute the sine and cosine of the angle y
		cos_theta = CMathBase::Fast_Cos(theta_y);  // no change since cos(-x) = cos(x)
		sin_theta = -CMathBase::Fast_Sin(theta_y); // sin(-x) = -sin(x)

		// set the matrix up 
		my_inv.Initialize(cos_theta, 0, -sin_theta, 0,
			0, 1, 0, 0,
			sin_theta, 0, cos_theta, 0,
			0, 0, 0, 1);

		// compute the sine and cosine of the angle z
		cos_theta = CMathBase::Fast_Cos(theta_z);  // no change since cos(-x) = cos(x)
		sin_theta = -CMathBase::Fast_Sin(theta_z); // sin(-x) = -sin(x)

		// set the matrix up 
		mz_inv.Initialize(cos_theta, sin_theta, 0, 0,
			-sin_theta, cos_theta, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);

		// now compute inverse camera rotation sequence
		switch (nMode)
		{
		case CAM_ROT_SEQ_XYZ:
		{
								mtmp.Multiply(&mx_inv, &my_inv);
								mrot.Multiply(&mtmp, &mz_inv);
		} break;

		case CAM_ROT_SEQ_YXZ:
		{
								mtmp.Multiply(&my_inv, &mx_inv);
								mrot.Multiply(&mtmp, &mz_inv);
		} break;

		case CAM_ROT_SEQ_XZY:
		{
								mtmp.Multiply(&mx_inv, &mz_inv);
								mrot.Multiply(&mtmp, &my_inv);
		} break;

		case CAM_ROT_SEQ_YZX:
		{
								mtmp.Multiply(&my_inv, &mz_inv);
								mrot.Multiply(&mtmp, &mx_inv);
		} break;

		case CAM_ROT_SEQ_ZYX:
		{
								mtmp.Multiply(&mz_inv, &my_inv);
								mrot.Multiply(&mtmp, &mx_inv);
		} break;

		case CAM_ROT_SEQ_ZXY:
		{
								mtmp.Multiply(&mz_inv, &mx_inv);
								mrot.Multiply(&mtmp, &my_inv);

		} break;

		default: break;
		} // end switch

		// now mrot holds the concatenated product of inverse rotation matrices
		// multiply the inverse translation matrix against it and store in the 
		// camera objects' camera transform matrix we are done!
		this->MatrixCamera.Multiply(&mt_inv, &mrot);

	}
	else
	{
		// this creates a camera matrix based on a look at vector n,
		// look up vector v, and a look right (or left) u
		// and stores it in the sent camera object, all values are
		// extracted out of the camera object itself
		// mode selects how uvn is computed
		// UVN_MODE_SIMPLE - low level simple model, use the target and view reference point
		// UVN_MODE_SPHERICAL - spherical mode, the x,y components will be used as the
		//     elevation and heading of the view vector respectively
		//     along with the view reference point as the position
		//     as usual

		MATRIX4X4 mt_inv,  // inverse camera translation matrix
			mt_uvn,  // the final uvn matrix
			mtmp;    // temporary working matrix

		// step 1: create the inverse translation matrix for the camera
		// position
		mt_inv.Initialize(1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			-this->m_WorldPos.x, -this->m_WorldPos.y, -this->m_WorldPos.z, 1);


		// step 2: determine how the target point will be computed
		if (nMode == UVN_MODE_SPHERICAL)
		{
			// use spherical construction
			// target needs to be recomputed

			// extract elevation and heading 
			float phi = this->m_Direction.x; // elevation
			float theta = this->m_Direction.y; // heading

			// compute trig functions once
			float sin_phi = CMathBase::Fast_Sin(phi);
			float cos_phi = CMathBase::Fast_Cos(phi);

			float sin_theta = CMathBase::Fast_Sin(theta);
			float cos_theta = CMathBase::Fast_Cos(theta);

			// now compute the target point on a unit sphere x,y,z
			this->m_Target.x = -1 * sin_phi*sin_theta;
			this->m_Target.y = 1 * cos_phi;
			this->m_Target.z = 1 * sin_phi*cos_theta;
		} // end else

		// at this point, we have the view reference point, the target and that's
		// all we need to recompute u,v,n
		// Step 1: n = <target position - view reference point>
		this->N.Subtract(&this->m_WorldPos, &this->m_Target);

		// Step 2: Let v = <0,1,0>
		this->V.Initialize(0, 1, 0);

		// Step 3: u = (v x n)
		this->U = CVector4D::Cross(this->V, this->N);

		// Step 4: v = (n x u)
		this->V = CVector4D::Cross(this->N, this->U);

		// Step 5: normalize all vectors
		this->U.Normalize();
		this->V.Normalize();
		this->N.Normalize();


		// build the UVN matrix by placing u,v,n as the columns of the matrix
		mt_uvn.Initialize(this->U.x, this->V.x, this->N.x, 0,
			this->U.y, this->V.y, this->N.y, 0,
			this->U.z, this->V.z, this->N.z, 0,
			0, 0, 0, 1);

		// now multiply the translation matrix and the uvn matrix and store in the 
		// final camera matrix mcam
		this->MatrixCamera.Multiply(&mt_inv, &mt_uvn);

	}
}

void SoaringLoong::Graphics3D::CCamera::UpdateProjectMatrix()
{
	// this function builds up a camera to perspective transformation
	// matrix, in most cases the camera would have a 2x2 normalized
	// view plane with a 90 degree FOV, since the point of the having
	// this matrix must be to also have a perspective to screen (viewport)
	// matrix that scales the normalized coordinates, also the matrix
	// assumes that you are working in 4D homogenous coordinates and at 
	// some point there will be a 4D->3D conversion, it might be immediately
	// after this transform is applied to vertices, or after the perspective
	// to screen transform

	MatrixProjection.Initialize(
		ViewDistance, 0, 0, 0,
		0, ViewDistance*AspectRatio, 0, 0,
		0, 0, 1, 1,
		0, 0, 0, 0); 
}

void SoaringLoong::Graphics3D::CCamera::UpdateScreenMatrix()
{
	// this function builds up a perspective to screen transformation
	// matrix, the function assumes that you want to perform the
	// transform in homogeneous coordinates and at raster time there will be 
	// a 4D->3D homogenous conversion and of course only the x,y points
	// will be considered for the 2D rendering, thus you would use this
	// function's matrix is your perspective coordinates were still 
	// in homgeneous form whene this matrix was applied, additionally
	// the point of this matrix to to scale and translate the perspective
	// coordinates to screen coordinates, thus the matrix is built up
	// assuming that the perspective coordinates are in normalized form for
	// a (2x2)/aspect_ratio viewplane, that is, x: -1 to 1, y:-1/aspect_ratio to 1/aspect_ratio

	float alpha = (0.5*ScreenWidth - 0.5);
	float beta = (0.5*ScreenHeight - 0.5);

	MatrixScreen.Initialize(
		alpha, 0, 0, 0,
		0, -beta, 0, 0,
		alpha, beta, 1, 0,
		0, 0, 0, 1);
}

void SoaringLoong::Graphics3D::CCamera::Move(const POINT4D& Position, const POINT4D& Direction, LPPOINT4D Target)
{
	m_WorldPos = Position;
	m_Direction = Direction;
	m_Target = Target;
	UpdateCameraMatrix();
}
