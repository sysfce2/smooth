 /* The smooth Class Library
  * Copyright (C) 1998-2004 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_EDITBOX_
#define _H_OBJSMOOTH_EDITBOX_

namespace smooth
{
	namespace GUI
	{
		class EditBox;
	};

	namespace System
	{
		class Timer;
	};
};

#include "../widget.h"
#include "../multi/list/list.h"
#include "../multi/list/combobox.h"

namespace smooth
{
	namespace GUI
	{
		const Int EDB_ALPHANUMERIC	= 0;
		const Int EDB_NUMERIC		= 1;
		const Int EDB_ASTERISK		= 2;

		class SMOOTHAPI EditBox : public Widget
		{
			private:
				System::Timer		*timer;
			slots:
				Void			 TimerProc();

				String			 GetVisibleText();
			protected:
				Int			 promptPos;
				Bool			 promptVisible;
				Int			 markStart;
				Int			 oMarkStart;
				Int			 markEnd;
				Bool			 marking;
				Int			 maxSize;
				Int			 invisibleChars;

				List			*dropDownList;
				ComboBox		*comboBox;

				Void			 SetCursor(Int);
				Void			 RemoveCursor();

				Void			 MarkText(Int, Int);
				Void			 DeleteSelectedText();
				Void			 InsertText(String);
			slots:
				Void			 DropDownListProc();
			public:
				static const Int	 classID;

							 EditBox(String, Point, Size, Int = 0);
							~EditBox();

				virtual Int		 Paint(Int);
				Int			 Process(Int, Int, Int);

				Int			 Show();
				Int			 Hide();

				Int			 Activate();
				Int			 Deactivate();

				Int			 SetText(const String &);

				Int			 SetDropDownList(List *);

				Int			 GetCursorPos();

				Int			 MarkAll();
			signals:
				Signal0<Void>		 onEnter;
		};
	};
};

#endif
