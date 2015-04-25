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
	this->WorldPos = Position;
	this->Direction = Direction;
// 	cam->pos.Copy(*cam_pos); // positions
// 	cam->dir.Copy(*cam_dir); // direction vector or angles for
	// euler camera
	// for UVN camera
	this->U.Initialize(1, 0, 0);
	this->V.Initialize(0, 1, 0);
	this->N.Initialize(0, 0, 1);
// 	cam->u.Initialize(1, 0, 0);  // set to +x
// 	cam->v.Initialize(0, 1, 0);  // set to +y
// 	cam->n.Initialize(0, 0, 1);  // set to +z        

	this->nMode = nMode;
	// 设置相机目标点
	if ( Target )
	{
		this->UVNTarget = Target;
	}
	else
	{
		this->UVNTarget.Zero();
	}
// 	if (cam_target != NULL)
// 		cam->target.Copy(*cam_target); // UVN target
// 	else
// 		cam->target.VECTOR4D_ZERO(&cam->target);



	// 计算裁剪面和屏幕参数
	this->NearZ = NearZ;
	this->FarZ = FarZ;
	this->ScreenWidth = ScreenWidth;
	this->ScreenHeight = ScreenHeight;
	this->ScreenCenterX = (ScreenWidth - 1) / 2;
	this->ScreenCenterY = (ScreenHeight - 1) / 2;
	this->AspectRatio = ScreenWidth / ScreenHeight;
// 	cam->near_clip_z = near_clip_z;     // near z=constant clipping plane
// 	cam->far_clip_z = far_clip_z;      // far z=constant clipping plane
// 
// 	cam->viewport_width = viewport_width;   // dimensions of viewport
// 	cam->viewport_height = viewport_height;
// 
// 	cam->viewport_center_x = (viewport_width - 1) / 2; // center of viewport
// 	cam->viewport_center_y = (viewport_height - 1) / 2;
// 
// 	cam->aspect_ratio = (float)viewport_width / (float)viewport_height;

	// set all camera matrices to identity matrix
	this->MatrixCamera.Zero();
	this->MatrixProjection.Zero();
	this->MatrixScreen.Zero();
// 	cam->mcam.Zero();
// 	cam->mper.Zero();
// 	cam->mscr.Zero();

	// set independent vars
	this->FOV = FOV;
//	cam->fov = fov;

	// set the viewplane dimensions up, they will be 2 x (2/ar)

	this->ViewPlaneWidth = 2.0;
	this->ViewPlaneHeight = 2.0 / this->AspectRatio;
// 	cam->viewplane_width = 2.0;
// 	cam->viewplane_height = 2.0 / cam->aspect_ratio;

	// now we know fov and we know the viewplane dimensions plug into formula and
	// solve for view distance parameters

	
	// 根据FOV和视平面计算d
	this->ViewDistance = (0.5) * (this->ViewPlaneWidth) * tan(AngleToRadian(FOV / 2));
// 	float tan_fov_div2 = tan(DEG_TO_RAD(fov / 2));
// 	cam->view_dist = (0.5)*(cam->viewplane_width)*tan_fov_div2;

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
		// right clipping plane 
// 		vn.Initialize(1, 0, -1); // x=z plane
// 		cam->rt_clip_plane.Initialize(pt_origin, vn, 1);
// 
// 		// left clipping plane
// 		vn.Initialize(-1, 0, -1); // -x=z plane
// 		cam->lt_clip_plane.Initialize(pt_origin, vn, 1);
// 
// 		// top clipping plane
// 		vn.Initialize(0, 1, -1); // y=z plane
// 		cam->tp_clip_plane.Initialize(pt_origin, vn, 1);
// 
// 		// bottom clipping plane
// 		vn.Initialize(0, -1, -1); // -y=z plane
// 		cam->bt_clip_plane.Initialize(pt_origin, vn, 1);
	} // end if d=1
	else
	{
		// 如果视野不是90度，则在算某个裁剪面的法向量时，先去视平面上四个角上在该平面上的两个角作为该裁剪面上的两个向量，然后求叉乘，即可
		// 下面的法向量vn直接使用了结果

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
		// since we don't have a 90 fov, computing the normals
		// are a bit tricky, there are a number of geometric constructions
		// that solve the problem, but I'm going to solve for the
		// vectors that represent the 2D projections of the frustrum planes
		// on the x-z and y-z planes and then find perpendiculars to them

		// right clipping plane, check the math on graph paper 
// 		vn.Initialize(cam->view_dist, 0, -cam->viewplane_width / 2.0);
// 		cam->rt_clip_plane.Initialize(pt_origin, vn, 1);
// 
// 		// left clipping plane, we can simply reflect the right normal about
// 		// the z axis since the planes are symetric about the z axis
// 		// thus invert x only
// 		vn.Initialize(-cam->view_dist, 0, -cam->viewplane_width / 2.0);
// 		cam->lt_clip_plane.Initialize(pt_origin, vn, 1);
// 
// 		// top clipping plane, same construction
// 		vn.Initialize(0, cam->view_dist, -cam->viewplane_width / 2.0);
// 		cam->tp_clip_plane.Initialize(pt_origin, vn, 1);
// 
// 		// bottom clipping plane, same inversion
// 		vn.Initialize(0, -cam->view_dist, -cam->viewplane_width / 2.0);
// 		cam->bt_clip_plane.Initialize(pt_origin, vn, 1);
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
			-this->WorldPos.x, -this->WorldPos.y, -this->WorldPos.z, 1);

		// step 2: create the inverse rotation sequence for the camera
		// rember either the transpose of the normal rotation matrix or
		// plugging negative values into each of the rotations will result
		// in an inverse matrix

		// first compute all 3 rotation matrices

		// extract out euler angles
		float theta_x = this->Direction.x;
		float theta_y = this->Direction.y;
		float theta_z = this->Direction.z;

		// compute the sine and cosine of the angle x
		float cos_theta = CMath2::Fast_Cos(theta_x);  // no change since cos(-x) = cos(x)
		float sin_theta = -CMath2::Fast_Sin(theta_x); // sin(-x) = -sin(x)

		// set the matrix up 
		mx_inv.Initialize(1, 0, 0, 0,
			0, cos_theta, sin_theta, 0,
			0, -sin_theta, cos_theta, 0,
			0, 0, 0, 1);

		// compute the sine and cosine of the angle y
		cos_theta = CMath2::Fast_Cos(theta_y);  // no change since cos(-x) = cos(x)
		sin_theta = -CMath2::Fast_Sin(theta_y); // sin(-x) = -sin(x)

		// set the matrix up 
		my_inv.Initialize(cos_theta, 0, -sin_theta, 0,
			0, 1, 0, 0,
			sin_theta, 0, cos_theta, 0,
			0, 0, 0, 1);

		// compute the sine and cosine of the angle z
		cos_theta = CMath2::Fast_Cos(theta_z);  // no change since cos(-x) = cos(x)
		sin_theta = -CMath2::Fast_Sin(theta_z); // sin(-x) = -sin(x)

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
			-this->WorldPos.x, -this->WorldPos.y, -this->WorldPos.z, 1);


		// step 2: determine how the target point will be computed
		if (nMode == UVN_MODE_SPHERICAL)
		{
			// use spherical construction
			// target needs to be recomputed

			// extract elevation and heading 
			float phi = this->Direction.x; // elevation
			float theta = this->Direction.y; // heading

			// compute trig functions once
			float sin_phi = CMath2::Fast_Sin(phi);
			float cos_phi = CMath2::Fast_Cos(phi);

			float sin_theta = CMath2::Fast_Sin(theta);
			float cos_theta = CMath2::Fast_Cos(theta);

			// now compute the target point on a unit sphere x,y,z
			this->UVNTarget.x = -1 * sin_phi*sin_theta;
			this->UVNTarget.y = 1 * cos_phi;
			this->UVNTarget.z = 1 * sin_phi*cos_theta;
		} // end else

		// at this point, we have the view reference point, the target and that's
		// all we need to recompute u,v,n
		// Step 1: n = <target position - view reference point>
		this->WorldPos.VECTOR4D_Build(&this->WorldPos, &this->UVNTarget, &this->N);

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
	/*CVector4D vMove(-WorldPos.x, -WorldPos.y, -WorldPos.z);
	CMatrix4x4 mMove;
	mMove.BuildMoveMatrix(vMove);

	switch (emType)
	{
	case SoaringLoong::Graphics3D::CAMERA_ELUER:
	{
		CMatrix4x4 mRotation;
		mRotation.BuildRotateMartix(-this->Direction.x, -this->Direction.y, -this->Direction.z);
		this->MatrixCamera.Multiply(&mMove, &mRotation);
	}break;
	case SoaringLoong::Graphics3D::CAMERA_UVN:
	{
		if (this->bUVNTargetNeedCompute)
		{
			// 方向角，仰角
			double phi = this->Direction.x;
			double theta = this->Direction.y;

			double sin_phi = CMathBase::Fast_Sin(phi);
			double cos_phi = CMathBase::Fast_Cos(phi);
			double sin_theta = CMathBase::Fast_Sin(theta);
			double cos_theta = CMathBase::Fast_Cos(theta);

			this->UVNTarget.x = -1 * sin_phi * sin_theta;
			this->UVNTarget.y = 1 * cos_phi;
			this->UVNTarget.z = 1 * sin_phi * cos_theta;
		}

		// 临时的UVN
		CVector4D u, v, n;

		n = CVector4D::Subtract(this->UVNTarget, this->WorldPos);
		v = this->V;

		// 应为N和V可以组成一个平面，所以可求法向量U  
		u = CVector4D::Cross(v, n);

		// 因为V和N可能不垂直，所以反求V，使得V和U、N都垂直  
		v = CVector4D::Cross(n, u);

		this->U.VECTOR4D_Normalize(&u);
		this->V.VECTOR4D_Normalize(&v);
		this->N.VECTOR4D_Normalize(&n);

		// UVN变换矩阵  
		CMatrix4x4 mUVN;
		mUVN.Initialize(
			U.x, V.x, N.x, 0,
			U.y, V.y, N.y, 0,
			U.z, V.z, N.z, 0,
			0, 0, 0, 1);

		MatrixCamera.Multiply(&mMove, &mUVN);

	}break;
	default:
		break;
	}*/
}

void SoaringLoong::Graphics3D::CCamera::UpdateProjectMatrix()
{
	MatrixProjection.Initialize(
		ViewDistance, 0, 0, 0,
		0, -ViewDistance, 0, 0,
		0, 0, 1, 1,
		0, 0, 0, 0); 
}

void SoaringLoong::Graphics3D::CCamera::UpdateScreenMatrix()
{
	MatrixScreen.Initialize(
		ScreenWidth / 2, 0, 0, 0,
		0, ScreenWidth / 2, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
}
