 /* The smooth Class Library
  * Copyright (C) 1998-2004 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth/gui/dialogs/dialog.h>

S::GUI::Dialogs::Dialog::Dialog()
{
	parentWindow = NIL;
}

S::GUI::Dialogs::Dialog::~Dialog()
{
}

S::Int S::GUI::Dialogs::Dialog::SetCaption(String newCaption)
{
	caption = newCaption;

	return Success;
}

S::Int S::GUI::Dialogs::Dialog::SetParentWindow(Window *newParent)
{
	parentWindow = newParent;

	return Success;
}