#pragma once
#include "SloongDraw.h"
#include "DInputClass.h"
using namespace SoaringLoong::Graphics;
class ITestRender
{
public:
	ITestRender();
	virtual ~ITestRender();

	virtual void Initialize( CDDraw* pDraw, DInputClass* pInput, RECT rcWindow ) = 0;
	virtual void Render() = 0;
};

