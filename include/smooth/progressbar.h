 /* The smooth Class Library
  * Copyright (C) 1998-2003 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the "Artistic License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_PROGRESSBAR_
#define _H_OBJSMOOTH_PROGRESSBAR_

namespace smooth
{
	namespace GUI
	{
		class Progressbar;
	};
};

#include "widget.h"

namespace smooth
{
	namespace GUI
	{
		const Int PB_NOTEXT	= 0;
		const Int PB_VALUE	= 1;
		const Int PB_PERCENT	= 2;

		class SMOOTHAPI Progressbar : public Widget
		{
			protected:
				Int		 value;

				Int		 startValue;
				Int		 endValue;

				Int		 textFlag;
			public:
						 Progressbar(Point, Size, Int, Int, Int, Int, Int);
						~Progressbar();

				virtual Int	 Paint(Int);

				Int		 SetValue(Int);
				Int		 GetValue();
		};
	};

	SMOOTHVAR Int OBJ_PROGRESSBAR;
};

#endif