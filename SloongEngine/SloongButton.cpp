#include "stdafx.h"
#include "SloongButton.h"
#include "SloongBitmap.h"

using namespace SoaringLoong;
using namespace SoaringLoong::Graphics;

CButton::CButton(CDDraw* pDDraw) : CObject(pDDraw)
{
	Initialize(pDDraw, 0, 0, 800, 600, 4, BOB_ATTR_MULTI_ANIM | BOB_ATTR_VISIBLE, DDSCAPS_SYSTEMMEMORY);
}


CButton::~CButton()
{
}

void SoaringLoong::Graphics::CButton::SetTexture(const vector<tstring>& vTexture)
{
	m_vTexture.clear();
	m_vTexture = vTexture;

	CBitmap oBitmap;
	oBitmap.LoadBitmapFromFile(vTexture[0].c_str());
	LoadFrame(&oBitmap, 0, 0, 0, BITMAP_EXTRACT_MODE_ABS);
	oBitmap.Shutdown();

	oBitmap.LoadBitmapFromFile(vTexture[1].c_str());
	LoadFrame(&oBitmap, 1, 0, 0, BITMAP_EXTRACT_MODE_ABS);
	oBitmap.Shutdown();

	oBitmap.LoadBitmapFromFile(vTexture[2].c_str());
	LoadFrame(&oBitmap, 2, 0, 0, BITMAP_EXTRACT_MODE_ABS);
	oBitmap.Shutdown();

	oBitmap.LoadBitmapFromFile(vTexture[3].c_str());
	LoadFrame(&oBitmap, 3, 0, 0, BITMAP_EXTRACT_MODE_ABS);
	oBitmap.Shutdown();
}

void SoaringLoong::Graphics::CButton::Render(LPDIRECTDRAWSURFACE7 lpDDrawSurface)
{
	this->curr_frame = m_stStatus;
	CObject::Render(lpDDrawSurface);
}
