 /* The smooth Class Library
  * Copyright (C) 1998-2006 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth/gui/widgets/basic/activearea.h>
#include <smooth/gui/widgets/hotspot/hotspot.h>
#include <smooth/graphics/surface.h>

const S::Int	 S::GUI::ActiveArea::classID = S::Object::RequestClassID();

S::GUI::ActiveArea::ActiveArea(const Color &iColor, const Point &iPos, const Size &iSize) : Widget(iPos, iSize)
{
	type		= classID;
	areaColor	= iColor;

	if (GetWidth() == 0) SetWidth(80);
	if (GetHeight() == 0) SetHeight(20);

	hotspot	= new Hotspot(Point(1, 1), GetSize() - Size(2, 2));

	hotspot->onLeftButtonClick.Connect(&onAction);

	RegisterObject(hotspot);

	onChangeSize.Connect(&ActiveArea::OnChangeSize, this);
}

S::GUI::ActiveArea::~ActiveArea()
{
	DeleteObject(hotspot);
}

S::Int S::GUI::ActiveArea::Paint(Int message)
{
	if (!IsRegistered())	return Error();
	if (!IsVisible())	return Success();

	EnterProtectedRegion();

	Surface	*surface = container->GetDrawSurface();
	Rect	 frame	 = Rect(GetRealPosition(), GetSize());

	switch (message)
	{
		case SP_SHOW:
		case SP_PAINT:
			surface->Box(frame, areaColor, FILLED);
			surface->Frame(frame, FRAME_DOWN);

			break;
	}

	LeaveProtectedRegion();

	return Success();
}

S::Int S::GUI::ActiveArea::SetColor(const Color &nColor)
{
	areaColor = nColor;

	Paint(SP_PAINT);

	return Success();
}

S::GUI::Color S::GUI::ActiveArea::GetColor()
{
	return areaColor;
}

S::Void S::GUI::ActiveArea::OnChangeSize(const Size &nSize)
{
	hotspot->SetSize(nSize - Size(2, 2));
}
