 /* The smooth Class Library
  * Copyright (C) 1998-2004 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth/xml/xul/button.h>
#include <smooth/xml/node.h>
#include <smooth/gui/widgets/basic/button.h>

S::XML::XUL::Button::Button(Node *node)
{
	button = NIL;

	if (node != NIL)
	{
		id = GetXMLAttributeValue(node, "id");

		button = new GUI::Button(GetXMLAttributeValue(node, "label"), NIL, GUI::Point(0, 0), GUI::Size(0, 0));

		if (GetXMLAttributeValue(node, "disabled") == "true") button->Deactivate();
	}
}

S::XML::XUL::Button::~Button()
{
	if (button != NIL) Object::DeleteObject(button);
}

S::GUI::Widget *S::XML::XUL::Button::GetWidget()
{
	return button;
}