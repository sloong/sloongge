#include "stdafx.h"
#include "SloongSprite.h"

using namespace Sloong;
using namespace Sloong::Graphics;

CSprite::CSprite(CDDraw* pDDraw) : CObject(pDDraw)
{
	Initialize(pDDraw, 0, 0, 800, 600, 1, BOB_ATTR_MULTI_ANIM | BOB_ATTR_VISIBLE, DDSCAPS_SYSTEMMEMORY);
}


CSprite::~CSprite()
{
}
