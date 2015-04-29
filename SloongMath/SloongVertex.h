#pragma once

namespace SoaringLoong
{
	namespace Math
	{
		namespace Vertex
		{
			typedef class CVertex2DInt
			{
			public:
				int x, y; // the vertex
			} VERTEX2DI, *VERTEX2DI_PTR;

			// a 2D vertex
			typedef class CVertex2DFloat
			{
			public:
				float x, y; // the vertex
			} VERTEX2DF, *VERTEX2DF_PTR;
		}
	}
}