#include "stdafx.h"
#include "SloongCamera.h"
#include "SloongMathBase.h"

using namespace SoaringLoong::Math;
using namespace SoaringLoong::Math::Vector;

SoaringLoong::Graphics3D::CCamera::CCamera()
{

}

SoaringLoong::Graphics3D::CCamera::~CCamera()
{

}


void SoaringLoong::Graphics3D::CCamera::Initialize(CAMERA_TYPE emType, const POINT4D& Position, const POINT4D& Direction, LPPOINT4D Target, LPVECTOR4D v, bool NeedTarget, double NearZ, double farZ, double FOV, double ScreenWidth, double ScreenHeight)
{
	this->emType = emType;

	// 设置位置和朝向
	this->WorldPos = Position;
	this->Direction = Direction;

	// 设置相机目标点
	if ( Target )
	{
		this->UVNTarget = Target;
	}
	else
	{
		this->UVNTarget.Initialize(0, 0, 0);
	}

	if ( v )
	{
		this->V = v;
	}

	this->bUVNTargetNeedCompute = NeedTarget;

	// 计算裁剪面和屏幕参数
	this->NearZ = NearZ;
	this->FarZ = FarZ;
	this->ScreenWidth = ScreenWidth;
	this->ScreenHeight = ScreenHeight;
	this->ScreenCenterX = (ScreenWidth - 1) / 2;
	this->ScreenCenterY = (ScreenHeight - 1) / 2;
	this->AspectRatio = ScreenWidth / ScreenHeight;
	this->FOV = FOV;
	this->ViewPlaneWidth = 2.0;
	this->ViewPlaneHeight = 2.0 / this->AspectRatio;

	// 根据FOV和视平面计算d
	this->ViewDistance = (0.5) * (this->ViewPlaneWidth) / tan(AngleToRadian(FOV / 2));

	// 所有裁剪面都过原点
	POINT3D Pos;
	Pos.Initialize(0, 0, 0);

	// 面法线
	VECTOR3D vn;

	if ( FOV == 90.0 )
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
	}
	else
	{
		// 如果视野不是90度，则在算某个裁剪面的法向量时，先去视平面上四个角上在该平面上的两个角作为该裁剪面上的两个向量，然后求叉乘，即可
		// 下面的法向量vn直接使用了结果

		double dwTempWidth = ((this->ViewPlaneWidth) / 2.0);
		// 右裁剪面
		vn.Initialize( this->ViewDistance, 0, -dwTempWidth);
		this->ClipPlaneRight.Initialize(Pos, vn, true);

		// 左裁剪面
		vn.Initialize( -(this->ViewDistance), 0, -dwTempWidth);
		this->ClipPlaneLeft.Initialize(Pos, vn, true);

		// 上裁剪面
		vn.Initialize( 0, this->ViewDistance, -dwTempWidth);
		this->ClipPlaneUp.Initialize(Pos, vn, true);

		// 下裁剪面
		vn.Initialize( 0, -this->ViewDistance, -dwTempWidth);
		this->ClipPlaneDown.Initialize(Pos, vn, true);
	}
}

void SoaringLoong::Graphics3D::CCamera::UpdateCameraMatrix()
{
	CVector4D vMove(-WorldPos.x, -WorldPos.y, -WorldPos.z);
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
	}
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
