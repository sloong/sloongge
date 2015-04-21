#pragma once
#include "SloongDraw.h"
using namespace SoaringLoong::Graphics;
class ITestRender
{
public:
	ITestRender();
	virtual ~ITestRender();

	virtual void Initialize( CDDraw* pDraw ) = 0;
	virtual void Render() = 0;
};

