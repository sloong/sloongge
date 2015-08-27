#pragma once
#include "SloongObject.h"

namespace Sloong
{
	namespace Graphics
	{
		class CTextField : public CObject
		{
		public:
			CTextField(CDDraw* pDDraw);
			~CTextField();

		public:
			virtual void Render(LPDIRECTDRAWSURFACE7 lpDDrawSurface);
		};
	}
	
}
