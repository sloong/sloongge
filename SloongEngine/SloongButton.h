#pragma once
#include "SloongObject.h"

namespace SoaringLoong
{
	namespace Graphics
	{
		class CButton : public CObject
		{
		public:
			CButton(CDDraw* pDDraw);
			~CButton();

			void SetTexture(const vector<tstring>& vTexture);
			void Render(LPDIRECTDRAWSURFACE7 lpDDrawSurface);
		protected:

		};
	}
}


