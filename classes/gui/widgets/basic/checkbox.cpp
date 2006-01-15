 /* The smooth Class Library
  * Copyright (C) 1998-2006 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth/gui/widgets/basic/checkbox.h>
#include <smooth/gui/widgets/hotspot/simplebutton.h>
#include <smooth/misc/math.h>
#include <smooth/graphics/surface.h>
#include <smooth/gui/window/window.h>

const S::Int		 S::GUI::CheckBox::classID = S::Object::RequestClassID();

S::Signal0<S::Void>	 S::GUI::CheckBox::internalCheckValues;

S::GUI::CheckBox::CheckBox(const String &iText, const Point &iPos, const Size &iSize, Bool *iVariable) : Widget(iPos, iSize)
{
	type		= classID;
	text		= iText;
	variable	= iVariable;
	state		= *variable;

	font.SetColor(Setup::ClientTextColor);

	if (GetWidth() == 0) SetWidth(80);
	if (GetHeight() == 0) SetHeight(17);

	GetTextSize();

	internalCheckValues.Connect(&CheckBox::InternalCheckValues, this);

	hotspot	= new HotspotSimpleButton(Point(0, 0), GetSize());
	hotspot->onLeftButtonClick.Connect(&CheckBox::OnLeftButtonClick, this);
	hotspot->onLeftButtonClick.Connect(&onAction);

	onChangeSize.Connect(&HotspotSimpleButton::SetSize, hotspot);

	RegisterObject(hotspot);
}

S::GUI::CheckBox::~CheckBox()
{
	DeleteObject(hotspot);

	internalCheckValues.Disconnect(&CheckBox::InternalCheckValues, this);
}

S::Int S::GUI::CheckBox::Paint(Int message)
{
	if (!IsRegistered())	return Error();
	if (!IsVisible())	return Success();

	EnterProtectedRegion();

	Surface	*surface	= container->GetDrawSurface();
	Rect	 frame		= Rect(GetRealPosition() + Point(3, 3), Size(11, 11));
	
	switch (message)
	{
		case SP_SHOW:
		case SP_PAINT:
			if (IsActive())	surface->Box(frame, Setup::ClientColor, FILLED);
			else		surface->Box(frame, Setup::BackgroundColor, FILLED);

			surface->Frame(frame, FRAME_DOWN);

			if (*variable == True)
			{
				Point p1 = Point(frame.left + 3 - (Setup::rightToLeft ? 1 : 0), frame.top + 3);
				Point p2 = Point(frame.left + 10 - (Setup::rightToLeft ? 1 : 0), frame.bottom - 1);

				for (Int i = 0; i < 2; i++)
				{
					Int	 color = IsActive() ? Setup::DividerDarkColor : Setup::DividerDarkColor.Average(Setup::BackgroundColor);

					if (i == 1)
					{
						color = IsActive() ? Setup::ClientTextColor : Setup::GrayTextColor;

						p1 -= Point((Setup::rightToLeft ? -i : i), i);
						p2 -= Point((Setup::rightToLeft ? -i : i), i);
					}

					surface->Line(p1 + Point(0, 0), p2 + Point(0, 0), color);
					surface->Line(p1 + Point(1, 0), p2 + Point(0, -1), color);
					surface->Line(p1 + Point(0, 1), p2 + Point(-1, 0), color);
					surface->Line(p1 + Point(6, 0), p2 + Point(-8, 0), color);
					surface->Line(p1 + Point(6, 1), p2 + Point(-7, 0), color);
					surface->Line(p1 + Point(5, 0), p2 + Point(-8, -1), color);
				}
			}

			Rect	 textRect;

			textRect.left	= frame.right + 3;
			textRect.top	= frame.top - 1;
			textRect.right	= textRect.left + GetWidth();
			textRect.bottom	= textRect.top + 20;

			Font	 nFont = font;

			if (!IsActive()) nFont.SetColor(Setup::GrayTextColor);

			surface->SetText(text, textRect, nFont);

			break;
	}

	LeaveProtectedRegion();

	return Success();
}

S::Void S::GUI::CheckBox::OnLeftButtonClick()
{
	if (*variable == False)	*variable = True;
	else			*variable = False;

	internalCheckValues.Emit();
}

S::Void S::GUI::CheckBox::InternalCheckValues()
{
	if (state != *variable)
	{
		state = *variable;

		Paint(SP_PAINT);
	}
}
