 /* The smooth Class Library
  * Copyright (C) 1998-2003 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the "Artistic License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth/divider.h>
#include <smooth/objectmanager.h>
#include <smooth/toolkit.h>
#include <smooth/definitions.h>
#include <smooth/metrics.h>
#include <smooth/math.h>
#include <smooth/binary.h>
#include <smooth/stk.h>
#include <smooth/objectproperties.h>
#include <smooth/layer.h>

#ifdef __WIN32__
__declspec (dllexport)
#endif

S::Int	 S::OBJ_DIVIDER = S::Object::RequestObjectID();

S::GUI::Divider::Divider(Int pos, Int iOrientation)
{
	type				= OBJ_DIVIDER;
	orientation			= iOrientation;
	objectProperties->orientation	= OR_FREE;

	if (pos == 0)	pos = Math::Round(120 * Setup::FontSize);
	else		pos = Math::Round(pos * Setup::FontSize);

	possibleContainers.AddEntry(OBJ_WINDOW);
	possibleContainers.AddEntry(OBJ_LAYER);

	if (Binary::IsFlagSet(orientation, OR_HORZ))		{ objectProperties->pos.x = 0; objectProperties->pos.y = pos; }
	else if (Binary::IsFlagSet(orientation, OR_VERT))	{ objectProperties->pos.x = pos; objectProperties->pos.y = 0; }
}

S::GUI::Divider::~Divider()
{
	if (registered && myContainer != NIL) myContainer->UnregisterObject(this);
}

S::Int S::GUI::Divider::Paint(Int message)
{
	if (!registered)	return Error;
	if (!visible)		return Success;

	Object	*container = NIL;
	Window	*wnd = NIL;
	Layer	*layer = NIL;

	if (myContainer->GetContainerObject()->GetObjectType() == OBJ_WINDOW)
	{
		wnd = (Window *) myContainer->GetContainerObject();

		container = wnd;
	}
	else if (myContainer->GetContainerObject()->GetObjectType() == OBJ_LAYER)
	{
		layer = (Layer *) myContainer->GetContainerObject();
		wnd = (Window *) layer->GetContainer()->GetContainerObject();

		container = layer;
	}

	if (wnd == NIL) return Success;
	if (wnd->hwnd == NIL) return Success;

	Object	*object;
	Divider	*operat;
	HDC	 dc = GetContext(wnd);
	Point	 doubleBar1;
	Point	 doubleBar2;
	Bool	 afterMe = False;

	if (Binary::IsFlagSet(orientation, OR_HORZ))
	{
		if (container->GetObjectType() == OBJ_WINDOW)
		{
			doubleBar1.x = 4;

			if (Binary::IsFlagSet(orientation, OR_TOP))		doubleBar1.y = objectProperties->pos.y;
			else if	(Binary::IsFlagSet(orientation, OR_BOTTOM))	doubleBar1.y = wnd->GetObjectProperties()->size.cy - objectProperties->pos.y;

			doubleBar2.x = wnd->GetObjectProperties()->size.cx - 4;
			doubleBar2.y = doubleBar1.y;

			for (Int i = Object::objectCount - 1; i >= 0; i--)
			{
				object = mainObjectManager->RequestObject(i);

				if (object != NIL)
				{
					if (object->GetObjectType() == OBJ_DIVIDER && object->GetContainer() == myContainer)
					{
						operat = (Divider *) object;

						if (afterMe && Binary::IsFlagSet(operat->orientation, OR_VERT))
						{
							if (Binary::IsFlagSet(operat->orientation, OR_LEFT))
							{
								if (operat->GetObjectProperties()->pos.x >= doubleBar1.x-2) doubleBar1.x = operat->GetObjectProperties()->pos.x + 3;
							}
							else
							{
								if (operat->GetObjectProperties()->pos.x <= doubleBar2.x+1) doubleBar2.x = wnd->GetObjectProperties()->size.cx - operat->GetObjectProperties()->pos.x - 2;
							}
						}

						if (operat == this) afterMe = True;
					}
				}
			}
		}
		else if (container->GetObjectType() == OBJ_LAYER)
		{
			doubleBar1.x = layer->GetObjectProperties()->pos.x + 4;

			if (Binary::IsFlagSet(orientation, OR_TOP))		doubleBar1.y = layer->GetObjectProperties()->pos.y + objectProperties->pos.y;
			else if	(Binary::IsFlagSet(orientation, OR_BOTTOM))	doubleBar1.y = layer->GetObjectProperties()->pos.y + layer->GetObjectProperties()->size.cy - objectProperties->pos.y;

			doubleBar2.x = layer->GetObjectProperties()->pos.x + layer->GetObjectProperties()->size.cx - 4;
			doubleBar2.y = doubleBar1.y;

			for (Int i = Object::objectCount - 1; i >= 0; i--)
			{
				object = mainObjectManager->RequestObject(i);

				if (object != NIL)
				{
					if (object->GetObjectType() == OBJ_DIVIDER && object->GetContainer() == myContainer)
					{
						operat = (Divider *) object;

						if (afterMe && Binary::IsFlagSet(operat->orientation, OR_VERT))
						{
							if (Binary::IsFlagSet(operat->orientation, OR_LEFT))
							{
								if (layer->GetObjectProperties()->pos.x + operat->GetObjectProperties()->pos.x >= doubleBar1.x-2) doubleBar1.x = layer->GetObjectProperties()->pos.x + operat->GetObjectProperties()->pos.x + 3;
							}
							else
							{
								if (layer->GetObjectProperties()->pos.x + layer->GetObjectProperties()->size.cx - operat->GetObjectProperties()->pos.x <= doubleBar2.x+1) doubleBar2.x = layer->GetObjectProperties()->pos.x + layer->GetObjectProperties()->size.cx - operat->GetObjectProperties()->pos.x - 2;
							}
						}

						if (operat == this) afterMe = True;
					}
				}
			}
		}

		objectProperties->size.cx = doubleBar2.x - doubleBar1.x + 4;
		objectProperties->size.cy = 2;

		HBar(dc, doubleBar1, doubleBar2);
	}
	else if (Binary::IsFlagSet(orientation, OR_VERT))
	{
		if (container->GetObjectType() == OBJ_WINDOW)
		{
			if (Binary::IsFlagSet(orientation, OR_LEFT))		doubleBar1.x = objectProperties->pos.x;
			else if	(Binary::IsFlagSet(orientation, OR_RIGHT))	doubleBar1.x = wnd->GetObjectProperties()->size.cx - objectProperties->pos.x;

			doubleBar1.y = wnd->offset.top;
			doubleBar2.x = doubleBar1.x;
			doubleBar2.y = wnd->GetObjectProperties()->size.cy - wnd->offset.bottom - 2;

			for (Int i = Object::objectCount - 1; i >= 0; i--)
			{
				object = mainObjectManager->RequestObject(i);

				if (object != NIL)
				{
					if (object->GetObjectType() == OBJ_DIVIDER && object->GetContainer() == myContainer)
					{
						operat = (Divider *) object;

						if (afterMe && Binary::IsFlagSet(operat->orientation, OR_HORZ))
						{
							if (Binary::IsFlagSet(operat->orientation, OR_TOP))
							{
								if (operat->GetObjectProperties()->pos.y >= doubleBar1.y-2) doubleBar1.y = operat->GetObjectProperties()->pos.y + 3;
							}
							else
							{
								if (operat->GetObjectProperties()->pos.y <= doubleBar2.y+1) doubleBar2.y = wnd->GetObjectProperties()->size.cy - operat->GetObjectProperties()->pos.y - 2;
							}
						}

						if (operat == this) afterMe = True;
					}
				}
			}
		}
		else if (container->GetObjectType() == OBJ_LAYER)
		{
			if (Binary::IsFlagSet(orientation, OR_LEFT))		doubleBar1.x = layer->GetObjectProperties()->pos.x + objectProperties->pos.x;
			else if (Binary::IsFlagSet(orientation, OR_RIGHT))	doubleBar1.x = layer->GetObjectProperties()->pos.x + layer->GetObjectProperties()->size.cx - objectProperties->pos.x;

			doubleBar1.y = layer->GetObjectProperties()->pos.y + 4;
			doubleBar2.x = doubleBar1.x;
			doubleBar2.y = layer->GetObjectProperties()->pos.y + layer->GetObjectProperties()->size.cy - 4;

			for (Int i = Object::objectCount - 1; i >= 0; i--)
			{
				object = mainObjectManager->RequestObject(i);

				if (object != NIL)
				{
					if (object->GetObjectType() == OBJ_DIVIDER && object->GetContainer() == myContainer)
					{
						operat = (Divider *) object;

						if (afterMe && Binary::IsFlagSet(operat->orientation, OR_HORZ))
						{
							if (Binary::IsFlagSet(operat->orientation, OR_TOP))
							{
								if (layer->GetObjectProperties()->pos.y + operat->GetObjectProperties()->pos.y >= doubleBar1.y-2) doubleBar1.y = layer->GetObjectProperties()->pos.y + operat->GetObjectProperties()->pos.y + 3;
							}
							else
							{
								if (layer->GetObjectProperties()->pos.y + layer->GetObjectProperties()->size.cy - operat->GetObjectProperties()->pos.y <= doubleBar2.y+1) doubleBar2.y = layer->GetObjectProperties()->pos.y + layer->GetObjectProperties()->size.cy - operat->GetObjectProperties()->pos.y - 2;
							}
						}

						if (operat == this) afterMe = True;
					}
				}
			}
		}

		objectProperties->size.cy = doubleBar2.y - doubleBar1.y + 4;
		objectProperties->size.cx = 2;

		VBar(dc, doubleBar1, doubleBar2);
	}

	FreeContext(wnd, dc);

	return Success;
}
