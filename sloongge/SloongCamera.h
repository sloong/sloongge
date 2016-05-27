#pragma once
#include "math\\SloongVector.h"
#include "math\\SloongPlane.h"
#include "math\\SloongMatrix.h"

// defines for camera rotation sequences
#define CAM_ROT_SEQ_XYZ  0
#define CAM_ROT_SEQ_YXZ  1
#define CAM_ROT_SEQ_XZY  2
#define CAM_ROT_SEQ_YZX  3
#define CAM_ROT_SEQ_ZYX  4
#define CAM_ROT_SEQ_ZXY  5

#define UVN_MODE_SIMPLE            0 
#define UVN_MODE_SPHERICAL         1

namespace Sloong
{
	using namespace Math;
	using namespace Math::Vector;
	using namespace Math::Matrix;
	namespace Graphics3D
	{
		enum CAMERA_TYPE
		{
			CAMERA_ELUER,
			CAMERA_UVN,
		};
		class SLOONGENGINE_API CCamera
		{
		public:
			CCamera();
			virtual ~CCamera();

		public:
			virtual void Initialize(CAMERA_TYPE emType, const POINT4D& Position, const POINT4D& Direction, LPPOINT4D Target,
								int nMode, double NearZ, double farZ, double FOV, double ScreenWidth, double ScreenHeight );

			virtual void UpdateCameraMatrix();
			virtual void UpdateProjectMatrix();
			virtual void UpdateScreenMatrix();

			virtual void Move(const POINT4D& Position, const POINT4D& Direction, LPPOINT4D Target);

		public:
			POINT4D		m_WorldPos; // 相机在世界的坐标
			VECTOR4D	m_Direction; // 相机的朝向，相机默认的朝向
			CAMERA_TYPE emType; // 相机类型
			VECTOR4D U, V, N; // UVN相机的u,v,n向量
			POINT4D		m_Target; // UVN相机的目标点
			int nMode;
			double ViewDistance; // 视距
			double FOV; // 视野角度
			double NearZ; // 近裁剪距离
			double FarZ; // 远裁剪距离
			PLANE3D  ClipPlaneLeft, ClipPlaneRight, ClipPlaneUp, ClipPlaneDown; // 上下左右裁剪平面
			double ViewPlaneWidth, ViewPlaneHeight; // 透视平面的宽和高
			double ScreenWidth, ScreenHeight; // 屏幕宽高
			double ScreenCenterX, ScreenCenterY; // 屏幕中心坐标
			double AspectRatio; // 宽高比

			CMatrix4x4 MatrixCamera; // 相机变换矩阵
			CMatrix4x4 MatrixProjection; // 透视投影变换矩阵
			CMatrix4x4 MatrixScreen; // 屏幕变换矩阵

		};
	}
}
