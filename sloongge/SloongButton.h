#pragma once
#include "SloongObject.h"

namespace Sloong
{
	namespace Graphics
	{
		class CButton : public CObject
		{
		public:
			CButton(CDDraw* pDDraw);
			~CButton();

			void SetTexture(vector<wstring>* vTexture);
			void Render(LPDIRECTDRAWSURFACE7 lpDDrawSurface);
		protected:

		};
	}
}


