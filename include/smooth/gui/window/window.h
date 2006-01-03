 /* The smooth Class Library
  * Copyright (C) 1998-2006 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_WINDOW_
#define _H_OBJSMOOTH_WINDOW_

namespace smooth
{
	namespace GUI
	{
		class Window;
		class WindowBackend;
		class PopupMenu;
		class Layer;
	};

	namespace System
	{
		class Timer;
	};
};

#include "../widgets/widget.h"
#include "../../graphics/forms/rect.h"
#include "../../loop.h"
#include "../widgets/multi/menu/menu.h"
#include "../../graphics/bitmap.h"
#include "../../templates/callbacks.h"

namespace smooth
{
	namespace GUI
	{
		const Int	 WF_MODAL		= 1;
		const Int	 WF_SYSTEMMODAL		= 2;
		const Int	 WF_TOPMOST		= 4;
		const Int	 WF_APPTOPMOST		= 8;
		const Int	 WF_NORESIZE		= 16;
		const Int	 WF_NOTASKBUTTON	= 32;
		const Int	 WF_DELAYPAINT		= 64;
		const Int	 WF_THINBORDER		= 128;

		const Int	 WO_SEPARATOR	= 1;
		const Int	 WO_NOSEPARATOR	= 2;

		class SMOOTHAPI Window : public Widget
		{
			protected:
				WindowBackend			*backend;

				Int				 stay;
				Bool				 maximized;
				Bool				 minimized;

				Bool				 created;
				Bool				 destroyed;

				Bool				 firstPaint;

				Bitmap				 icon;

				Rect				 innerOffset;
				Rect				 updateRect;
				Rect				 timedUpdateRect;

				String				 defaultStatus;

				PopupMenu			*trackMenu;

				Layer				*mainLayer;
				System::Timer			*paintTimer;

				Bool				 Create();

				Void				 CalculateOffsets();

				Void				 PopupProc();
			public:
				static const Int		 classID;

				static Int			 nOfActiveWindows;

				Bool				 initshow;

								 Window(const String &, const Point &, const Size &, Void * = NIL);
				virtual				~Window();

				Int				 SetIcon(const Bitmap &);
				Bitmap				&GetIcon();

				Int				 SetMetrics(const Point &, const Size &);

				Int				 SetText(const String &);

				Layer				*GetMainLayer();

				Int				 SetStatusText(const String &);
				const String			&GetStatusText();

				Int				 SetDefaultStatusText(const String &);
				Int				 RestoreDefaultStatusText();

				virtual Point			 GetRealPosition();

				Rect				 GetWindowRect();
				const Rect			&GetClientRect();
				Rect				 GetRestoredWindowRect();

				const Rect			&GetUpdateRect();
				Int				 SetUpdateRect(const Rect &);

				Int				 SetMinimumSize(const Size &);
				Int				 SetMaximumSize(const Size &);

				virtual Int			 Show();
				virtual Int			 Hide();

				Int				 Maximize();
				Int				 Minimize();

				Int				 Restore();

				Bool				 IsMaximized();
				Bool				 IsMinimized();

				Int				 Stay();
				Int				 Close();

				Bool				 IsInUse();

				virtual Int			 Paint(Int);
				virtual Int			 Process(Int, Int, Int);

				Int				 MouseX();
				Int				 MouseY();

				Bool				 IsMouseOn(const Rect &);

				virtual Surface			*GetDrawSurface();
				Void				*GetSystemWindow();

				static Window			*GetWindow(Void *);

				virtual Int			 RegisterObject(Widget *);
				virtual Int			 UnregisterObject(Widget *);
			signals:
				Signal0<Void>			 onCreate;

				Signal0<Void>			 onPaint;
				Signal0<Void>			 onResize;
				Signal0<Void>			 onPeek;
				Signal3<Void, Int, Int, Int>	 onEvent;
			callbacks:
				Callback2<PopupMenu *, Int, Int> getTrackMenu;
				Callback0<Bool>			 doQuit;
			slots:
				Void				 PaintTimer();
		};
	};
};

#endif
