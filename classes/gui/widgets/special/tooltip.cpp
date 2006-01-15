 /* The smooth Class Library
  * Copyright (C) 1998-2006 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth/gui/widgets/special/tooltip.h>
#include <smooth/gui/window/toolwindow.h>
#include <smooth/system/timer.h>
#include <smooth/graphics/surface.h>
#include <smooth/misc/math.h>

const S::Int	 S::GUI::Tooltip::classID = S::Object::RequestClassID();

S::GUI::Tooltip::Tooltip() : Widget(Point(), Size())
{
	type		= classID;
	orientation	= OR_FREE;
	toolWindow	= NIL;
	timeOut		= 5000;
	timer		= NIL;

	font.SetColor(Setup::TooltipTextColor);
}

S::GUI::Tooltip::~Tooltip()
{
	if (timer != NIL)
	{
		timer->Stop();

		DeleteObject(timer);
	}

	if (toolWindow != NIL) DeleteObject(toolWindow);
}

S::Int S::GUI::Tooltip::Show()
{
	if (visible) return Success();

	visible = True;

	if (!IsRegistered()) return Success();

	Window	*window	 = container->GetContainerWindow();
	Size	 tSize	 = Size(font.GetTextSizeX(text) + 6, font.GetTextSizeY(text) + 4);
	Point	 tPos	 = Point(Math::Max(2, GetX() + window->GetX()), GetY() + window->GetY() - tSize.cy + 1);

	if (tPos.x + tSize.cx > LiSAGetDisplaySizeX() - 2 && !Setup::rightToLeft)					tPos.x = Math::Max(2, LiSAGetDisplaySizeX() - 2 - tSize.cx);
	if (window->GetWidth() - ((tPos.x - window->GetX()) + tSize.cx) + window->GetX() < 2 && Setup::rightToLeft)	tPos.x = Math::Max(2, GetX() + window->GetX() - tSize.cx);
	if (tPos.y < 2)													tPos.y = GetY() + window->GetY() + 2;

	toolWindow = new ToolWindow(tPos - window->GetPosition(), tSize);
	toolWindow->onPaint.Connect(&Tooltip::OnToolWindowPaint, this);

	RegisterObject(toolWindow);

	if (timeOut != 0)
	{
		timer = new System::Timer();

		timer->onInterval.Connect(&Tooltip::OnTimer, this);
		timer->Start(timeOut);
	}

	onShow.Emit();

	return Success();
}

S::Int S::GUI::Tooltip::Hide()
{
	if (!visible) return Success();

	visible = False;

	if (!IsRegistered()) return Success();

	if (toolWindow != NIL)
	{
		UnregisterObject(toolWindow);

		DeleteObject(toolWindow);

		toolWindow = NIL;
	}

	onHide.Emit();

	return Success();
}

S::Void S::GUI::Tooltip::OnToolWindowPaint()
{
	Surface	*surface = toolWindow->GetDrawSurface();
	Rect	 tRect	 = Rect(Point(0, 0), Size(font.GetTextSizeX(text) + 6, font.GetTextSizeY(text) + 4));

	surface->Box(tRect, Setup::TooltipColor, FILLED);
	surface->Box(tRect, Color(0, 0, 0), OUTLINED);

	surface->SetText(text, tRect + Point(2, 1), font);
}

S::Int S::GUI::Tooltip::SetTimeout(Int mSeconds)
{
	timeOut = mSeconds;

	return Success();
}

S::Void S::GUI::Tooltip::OnTimer()
{
	Hide();

	timer->Stop();

	DeleteObject(timer);

	timer = NIL;
}
