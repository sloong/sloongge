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
			POINT4D		m_WorldPos; // ��������������
			VECTOR4D	m_Direction; // ����ĳ������Ĭ�ϵĳ���
			CAMERA_TYPE emType; // �������
			VECTOR4D U, V, N; // UVN�����u,v,n����
			POINT4D		m_Target; // UVN�����Ŀ���
			int nMode;
			double ViewDistance; // �Ӿ�
			double FOV; // ��Ұ�Ƕ�
			double NearZ; // ���ü�����
			double FarZ; // Զ�ü�����
			PLANE3D  ClipPlaneLeft, ClipPlaneRight, ClipPlaneUp, ClipPlaneDown; // �������Ҳü�ƽ��
			double ViewPlaneWidth, ViewPlaneHeight; // ͸��ƽ��Ŀ�͸�
			double ScreenWidth, ScreenHeight; // ��Ļ���
			double ScreenCenterX, ScreenCenterY; // ��Ļ��������
			double AspectRatio; // ��߱�

			CMatrix4x4 MatrixCamera; // ����任����
			CMatrix4x4 MatrixProjection; // ͸��ͶӰ�任����
			CMatrix4x4 MatrixScreen; // ��Ļ�任����

		};
	}
}
