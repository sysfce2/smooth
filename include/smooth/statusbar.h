 /* The smooth Class Library
  * Copyright (C) 1998-2003 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the "Artistic License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_STATUSBAR_
#define _H_OBJSMOOTH_STATUSBAR_

namespace smooth
{
	namespace GUI
	{
		class Statusbar;
	};
};

#include "widget.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI Statusbar : public Widget
		{
			public:
					 Statusbar(String);
					~Statusbar();

				Int	 Paint(Int);

				Int	 SetText(String);
		};
	};

	SMOOTHVAR Int OBJ_STATUSBAR;
};

#endif