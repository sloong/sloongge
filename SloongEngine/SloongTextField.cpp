#include "stdafx.h"
#include "SloongTextField.h"
#include "Defines.h"

using namespace SoaringLoong;
using namespace SoaringLoong::Graphics;

CTextField::CTextField(CDDraw* pDDraw) : CObject(pDDraw)
{
	Initialize(pDDraw, 0, 0, 800, 600, 1, BOB_ATTR_MULTI_ANIM | BOB_ATTR_VISIBLE, DDSCAPS_SYSTEMMEMORY);
}

CTextField::~CTextField()
{
}

void SoaringLoong::Graphics::CTextField::Render(LPDIRECTDRAWSURFACE7 lpDDrawSurface)
{
	Draw(lpDDrawSurface);
}

