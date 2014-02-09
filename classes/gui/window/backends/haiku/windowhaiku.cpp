 /* The smooth Class Library
  * Copyright (C) 1998-2014 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth/gui/window/backends/haiku/windowhaiku.h>
#include <smooth/gui/window/window.h>
#include <smooth/input/pointer.h>
#include <smooth/misc/math.h>
#include <smooth/system/backends/haiku/eventhaiku.h>
#include <smooth/system/system.h>
#include <smooth/init.h>
#include <smooth/foreach.h>

#include <Application.h>
#include <Message.h>
#include <Region.h>

S::GUI::WindowBackend *CreateWindowHaiku()
{
	return new S::GUI::WindowHaiku();
}

S::Int	 windowHaikuTmp = S::GUI::WindowBackend::SetBackend(&CreateWindowHaiku);

S::Array<S::GUI::WindowHaiku *, S::Void *>	 S::GUI::WindowHaiku::windowBackends;

namespace smooth
{
	namespace GUI
	{
		const Int	 B_WINDOW_CREATED     = 'WCRE';
		const Int	 B_WINDOW_DESTROYED   = 'WDES';
		const Int	 B_WINDOW_DEACTIVATED = 'WDEA';
		const Int	 B_PAINT	      = 'PINT';

		class HaikuView : public BView
		{
			public:
				HaikuView(BRect frame) : BView(frame, NULL, B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_FRAME_EVENTS)
				{
					SetViewColor(Setup::BackgroundColor.GetRed(), Setup::BackgroundColor.GetGreen(), Setup::BackgroundColor.GetBlue());
				}

				void Draw(BRect updateRect)
				{
					System::EventHaiku::EnqueueMessage(Window(), *(Window()->CurrentMessage()), B_PAINT, 0, 0);
				}

				void MouseMoved(BPoint point, uint32 transit, const BMessage *message)
				{
					System::EventHaiku::EnqueueMessage(Window(), *(Window()->CurrentMessage()), B_MOUSE_MOVED, point.x, point.y);
				}

				void MouseDown(BPoint point)
				{
					System::EventHaiku::EnqueueMessage(Window(), *(Window()->CurrentMessage()), B_MOUSE_DOWN, point.x, point.y);
				}

				void MouseUp(BPoint point)
				{
					System::EventHaiku::EnqueueMessage(Window(), *(Window()->CurrentMessage()), B_MOUSE_UP, point.x, point.y);
				}
		};

		class HaikuWindow : public BWindow
		{
			private:
				BView	*view;
			public:
				HaikuWindow(BRect frame, String title, Int look, Int feel, S::Int flags) : BWindow(frame, title, (window_look) look, (window_feel) feel, flags)
				{
					view = new HaikuView(Bounds());

					AddChild(view);
				}

				void WindowActivated(bool active)
				{
					if (active) System::EventHaiku::EnqueueMessage(this, *CurrentMessage(), B_WINDOW_ACTIVATED, 0, 0);
					else	    System::EventHaiku::EnqueueMessage(this, *CurrentMessage(), B_WINDOW_DEACTIVATED, 0, 0);
				}

				void FrameMoved(BPoint origin)
				{
					System::EventHaiku::EnqueueMessage(this, *CurrentMessage(), B_WINDOW_MOVED, origin.x, origin.y);
				}

				void FrameResized(float width, float height)
				{
					System::EventHaiku::EnqueueMessage(this, *CurrentMessage(), B_WINDOW_RESIZED, width, height);
				}

				bool QuitRequested()
				{
					WindowHaiku	*backend = WindowHaiku::GetWindowBackend(this);

					if (backend != NIL)
					{
						System::EventHaiku::ProcessMessage(this, *CurrentMessage(), B_QUIT_REQUESTED, 0, 0);

						if (backend->GetSystemWindow() == NIL) return true;
						else				       return false;
					}

					return true;
				}

				void MessageReceived(BMessage *message)
				{
					System::EventHaiku::EnqueueMessage(this, *CurrentMessage(), message->what, 0, 0);

					BWindow::MessageReceived(message);
				}
		};
	};
};

S::GUI::WindowHaiku::WindowHaiku(Void *iWindow)
{
	type	 = WINDOW_HAIKU;

	wnd	 = NIL;
	view	 = NIL;

	id	 = windowBackends.Add(this);

	minSize	 = Size(160, 24);
	maxSize	 = Size(32768, 32768);

	fontSize = Surface().GetSurfaceDPI() / 96.0;

	flags	 = 0;
}

S::GUI::WindowHaiku::~WindowHaiku()
{
	windowBackends.Remove(id);
}

S::Void *S::GUI::WindowHaiku::GetSystemWindow() const
{
	return (Void *) wnd;
}

S::GUI::WindowHaiku *S::GUI::WindowHaiku::GetWindowBackend(BWindow *wnd)
{
	if (wnd == NIL) return NIL;

	for (Int i = 0; i < windowBackends.Length(); i++)
	{
		WindowHaiku	*window = windowBackends.GetNth(i);

		if (window != NIL)
		{
			if (window->wnd == wnd) return window;
		}
	}

	return NIL;
}

S::Input::Keyboard::Key S::GUI::WindowHaiku::ConvertKey(Int keyCode, const BMessage &currentMessage)
{
	Input::Keyboard::Key	 key = Input::Keyboard::KeyOther;

	switch (keyCode)
	{
		case B_LEFT_ARROW:  key = Input::Keyboard::KeyLeft;   break;
		case B_UP_ARROW:    key = Input::Keyboard::KeyUp;     break;
		case B_RIGHT_ARROW: key = Input::Keyboard::KeyRight;  break;
		case B_DOWN_ARROW:  key = Input::Keyboard::KeyDown;   break;

		case B_HOME:	    key = Input::Keyboard::KeyHome;   break;
		case B_END:	    key = Input::Keyboard::KeyEnd;    break;
		case B_INSERT:	    key = Input::Keyboard::KeyInsert; break;
		case B_DELETE:	    key = Input::Keyboard::KeyDelete; break;
		case B_PAGE_UP:	    key = Input::Keyboard::KeyPrior;  break;
		case B_PAGE_DOWN:   key = Input::Keyboard::KeyNext;   break;

		case B_RETURN:	    key = Input::Keyboard::KeyReturn; break;
		case B_BACKSPACE:   key = Input::Keyboard::KeyBack;   break;
		case B_TAB:	    key = Input::Keyboard::KeyTab;    break;

		case B_SPACE:	    key = Input::Keyboard::KeySpace;  break;

		case B_ESCAPE:	    key = Input::Keyboard::KeyEscape; break;

		case B_FUNCTION_KEY:
			{
				int32	  keyCode;

				currentMessage.FindInt32("key", &keyCode);

				switch (keyCode)
				{
					case B_F1_KEY:	key = Input::Keyboard::KeyF1;  break;
					case B_F2_KEY:	key = Input::Keyboard::KeyF2;  break;
					case B_F3_KEY:	key = Input::Keyboard::KeyF3;  break;
					case B_F4_KEY:	key = Input::Keyboard::KeyF4;  break;
					case B_F5_KEY:	key = Input::Keyboard::KeyF5;  break;
					case B_F6_KEY:	key = Input::Keyboard::KeyF6;  break;
					case B_F7_KEY:	key = Input::Keyboard::KeyF7;  break;
					case B_F8_KEY:	key = Input::Keyboard::KeyF8;  break;
					case B_F9_KEY:	key = Input::Keyboard::KeyF9;  break;
					case B_F10_KEY:	key = Input::Keyboard::KeyF10; break;
					case B_F11_KEY:	key = Input::Keyboard::KeyF11; break;
					case B_F12_KEY:	key = Input::Keyboard::KeyF12; break;
				}
			}
	}

	if	(keyCode >= '0' && keyCode <= '9') key = (Input::Keyboard::Key)  keyCode;
	else if	(keyCode >= 'a' && keyCode <= 'z') key = (Input::Keyboard::Key) (keyCode - 0x20);
	else if	(keyCode >= 'A' && keyCode <= 'Z') key = (Input::Keyboard::Key)  keyCode;

	return key;
}

S::Int S::GUI::WindowHaiku::ProcessSystemMessages(Int message, Int wParam, Int lParam, const BMessage &currentMessage)
{
	/* Process system messages not relevant
	 * to portable Window implementation.
	 */
	switch (message)
	{
		case B_QUIT_REQUESTED:
			if (doClose.Call()) Close();

			return Break;

		case B_WINDOW_CREATED:
			onCreate.Emit();

			return Break;

		case B_WINDOW_DESTROYED:
			onDestroy.Emit();

			return Break;
	}

	/* Update cursor position when receiving mouse messages.
	 */
	if (message == B_MOUSE_MOVED	||
	    message == B_MOUSE_DOWN	||
	    message == B_MOUSE_UP)
	{
		BPoint	 point = wnd->ConvertToScreen(BPoint(wParam, lParam));

		Input::Pointer::UpdatePosition(Window::GetWindow(wnd), point.x, point.y);
	}

	static int32	 buttons = -1;

	int32		 clicks	 =  0;
	float		 amount	 =  0;

	const char	*bytes	 = NIL;
	String		 string	 = NIL;

	/* Convert Windows messages to smooth messages.
	 */
	switch (message)
	{
		/* Mouse messages:
		 */
		case B_MOUSE_MOVED:
			return onEvent.Call(SM_MOUSEMOVE, 0, 0);

		case B_MOUSE_WHEEL_CHANGED:
			currentMessage.FindFloat("be:wheel_delta_y", &amount);

			/* Pass message to smooth window.
			 */
			onEvent.Call(SM_MOUSEWHEEL, amount * -120.0, 0);

			/* Send an event to update widget under cursor if necessary.
			 */
			onEvent.Call(SM_MOUSEMOVE, 0, 0);

			break;

		case B_MOUSE_DOWN:
			currentMessage.FindInt32("buttons", &buttons);
			currentMessage.FindInt32("clicks", &clicks);

			if	(buttons == B_PRIMARY_MOUSE_BUTTON)   onEvent.Call(SM_LBUTTONDOWN, 0, 0);
			else if (buttons == B_SECONDARY_MOUSE_BUTTON) onEvent.Call(SM_RBUTTONDOWN, 0, 0);
			else if (buttons == B_TERTIARY_MOUSE_BUTTON)  onEvent.Call(SM_MBUTTONDOWN, 0, 0);

			if (clicks == 2)
			{
				if	(buttons == B_PRIMARY_MOUSE_BUTTON)   onEvent.Call(SM_LBUTTONDBLCLK, 0, 0);
				else if (buttons == B_SECONDARY_MOUSE_BUTTON) onEvent.Call(SM_RBUTTONDBLCLK, 0, 0);
				else if (buttons == B_TERTIARY_MOUSE_BUTTON)  onEvent.Call(SM_MBUTTONDBLCLK, 0, 0);
			}

			break;

		case B_MOUSE_UP:
			if	(buttons == B_PRIMARY_MOUSE_BUTTON)   onEvent.Call(SM_LBUTTONUP, 0, 0);
			else if (buttons == B_SECONDARY_MOUSE_BUTTON) onEvent.Call(SM_RBUTTONUP, 0, 0);
			else if (buttons == B_TERTIARY_MOUSE_BUTTON)  onEvent.Call(SM_MBUTTONUP, 0, 0);

			break;

		/* Keyboard messages:
		 */
		case B_KEY_DOWN:
			currentMessage.FindString("bytes", &bytes);

			string.ImportFrom("UTF-8", bytes);

			onEvent.Call(SM_CHAR, string[0], 0);

			if (bytes[1] == 0)
			{
				Input::Keyboard::UpdateKeyState(ConvertKey(bytes[0], currentMessage), True);

				onEvent.Call(SM_KEYDOWN, ConvertKey(bytes[0], currentMessage), 0);
			}

			break;

		case B_KEY_UP:
			currentMessage.FindString("bytes", &bytes);

			string.ImportFrom("UTF-8", bytes);

			if (bytes[1] == 0)
			{
				Input::Keyboard::UpdateKeyState(ConvertKey(bytes[0], currentMessage), False);

				onEvent.Call(SM_KEYUP, ConvertKey(bytes[0], currentMessage), 0);
			}

			break;

		/* Paint messages:
		 */
		case B_PAINT:
			{
				BRegion	 clippingRegion;

				wnd->Lock();
				view->GetClippingRegion(&clippingRegion);
				wnd->Unlock();

				BRect	 clippingRect = clippingRegion.Frame();

				updateRect = Rect(Point(clippingRect.left, clippingRect.top), Size(clippingRect.Width(), clippingRect.Height()) + Size(1, 1));

				onEvent.Call(SM_PAINT, 0, 0);
			}

			break;

		/* Window state change messages:
		 */
		case B_WINDOW_RESIZED:
			{
				BRect	 windowRect = wnd->Frame();

				if (drawSurface != NIL) drawSurface->SetSize(Size(windowRect.Width(), windowRect.Height()) + Size(1, 1));

				onEvent.Call(SM_WINDOWMETRICS, ((	(int) windowRect.left			  + 32768) << 16) | (	    (int) windowRect.top		       + 32768),
							       ((Math::Floor((windowRect.Width() + 1) / fontSize) + 32768) << 16) | (Math::Floor((windowRect.Height() + 1) / fontSize) + 32768));

				updateRect = Rect(Point(0, 0), Size(windowRect.Width(), windowRect.Height()) + Size(1, 1));

				onEvent.Call(SM_PAINT, 0, 0);
			}

			break;

		case B_WINDOW_MOVED:
			{
				BRect	 windowRect = wnd->Frame();

				onEvent.Call(SM_WINDOWMETRICS, ((	(int) windowRect.left			  + 32768) << 16) | (	    (int) windowRect.top		       + 32768),
							       ((Math::Floor((windowRect.Width() + 1) / fontSize) + 32768) << 16) | (Math::Floor((windowRect.Height() + 1) / fontSize) + 32768));
			}

			return Success();

		case B_WINDOW_ACTIVATED:
			onEvent.Call(SM_GETFOCUS, 0, 0);

			break;

		case B_WINDOW_DEACTIVATED:
			Input::Keyboard::ResetKeyState();

			System::System::Sleep(50);

			/* Get the window that now has the focus.
			 */
			{
				Window	*focusWnd = NIL;

				foreach (WindowHaiku *backend, windowBackends)
				{
					if (backend->wnd != NIL && backend->wnd->IsActive())
					{
						focusWnd = Window::GetWindow((Void *) backend->wnd);

						break;
					}
				}

				onEvent.Call(SM_LOSEFOCUS, focusWnd != NIL ? focusWnd->GetHandle() : -1, 0);
			}

			break;
	}

	return Success();
}

S::Int S::GUI::WindowHaiku::Open(const String &title, const Point &pos, const Size &size, Int iFlags)
{
	flags = iFlags;

	Int	 windowLook  = B_TITLED_WINDOW_LOOK;
	Int	 windowFeel  = B_NORMAL_WINDOW_FEEL;
	Int	 windowFlags = 0;

	if (flags & WF_NORESIZE)	windowFlags = windowFlags | B_NOT_RESIZABLE;
	if (flags & WF_THINBORDER)	windowLook  = B_NO_BORDER_WINDOW_LOOK;
	if (flags & WF_TOPMOST)		windowFeel  = B_FLOATING_ALL_WINDOW_FEEL;
	if (flags & WF_MODAL)		windowFeel  = B_MODAL_APP_WINDOW_FEEL;

	wnd = new HaikuWindow(BRect(pos.x, pos.y, pos.x + Math::Round(size.cx * fontSize) + sizeModifier.cx, pos.y + Math::Round(size.cy * fontSize) + sizeModifier.cy), title, windowLook, windowFeel, windowFlags);

	if (wnd != NIL)
	{
		view = wnd->ChildAt(0);

		/* Send the Create message.
		 */
		ProcessSystemMessages(B_WINDOW_CREATED, 0, 0, BMessage());

		/* Create drawing surface.
		 */
		if ((flags & WF_THINBORDER) || (flags & WF_NORESIZE)) drawSurface = new Surface(view, size * fontSize + sizeModifier);
		else						      drawSurface = new Surface(view);

		drawSurface->SetSize(size * fontSize + sizeModifier);

		/* Set minimum and maximum size.
		 */
		if (!(flags & WF_NORESIZE || flags & WF_THINBORDER)) wnd->SetSizeLimits(Math::Round(minSize.cx * fontSize) + sizeModifier.cx, Math::Round(maxSize.cx * fontSize) + sizeModifier.cx, Math::Round(minSize.cy * fontSize) + sizeModifier.cy, Math::Round(maxSize.cy * fontSize) + sizeModifier.cy);

		return Success();
	}

	return Error();
}

S::Int S::GUI::WindowHaiku::Close()
{
	if (wnd == NIL) return Success();

	/* Send the Destroy message.
	 */
	ProcessSystemMessages(B_WINDOW_DESTROYED, 0, 0, BMessage());

	if (Window::nOfActiveWindows == 0) be_app->PostMessage(B_QUIT_REQUESTED);

	/* Delete surface.
	 */
	if (drawSurface != NIL) delete drawSurface;

	drawSurface = NIL;

	/* Destroy window.
	 */
	BWindow	*oldwnd = wnd;

	wnd  = NIL;
	view = NIL;

	oldwnd->Lock();
	oldwnd->Quit();

	return Success();
}

S::Int S::GUI::WindowHaiku::RequestClose()
{
	if (doClose.Call()) return Close();

	return Success();
}

S::Int S::GUI::WindowHaiku::SetTitle(const String &nTitle)
{
	if (wnd == NIL) return Error();

	wnd->SetTitle(nTitle);

	return Success();
}

S::Int S::GUI::WindowHaiku::SetMinimumSize(const Size &nMinSize)
{
	minSize = nMinSize;

	if (wnd == NIL) return Success();

	wnd->SetSizeLimits(Math::Round(minSize.cx * fontSize) + sizeModifier.cx, Math::Round(maxSize.cx * fontSize) + sizeModifier.cx, Math::Round(minSize.cy * fontSize) + sizeModifier.cy, Math::Round(maxSize.cy * fontSize) + sizeModifier.cy);

	BRect	 windowRect = wnd->Frame();

	SetMetrics(Point(windowRect.left, windowRect.top),
		   Size(Math::Max((Int) Math::Round((windowRect.right  - windowRect.left - sizeModifier.cx) / fontSize), minSize.cx),
			Math::Max((Int) Math::Round((windowRect.bottom - windowRect.top  - sizeModifier.cy) / fontSize), minSize.cy)));

	return Success();
}

S::Int S::GUI::WindowHaiku::SetMaximumSize(const Size &nMaxSize)
{
	maxSize = nMaxSize;

	if (wnd == NIL) return Success();

	wnd->SetSizeLimits(Math::Round(minSize.cx * fontSize) + sizeModifier.cx, Math::Round(maxSize.cx * fontSize) + sizeModifier.cx, Math::Round(minSize.cy * fontSize) + sizeModifier.cy, Math::Round(maxSize.cy * fontSize) + sizeModifier.cy);

	BRect	 windowRect = wnd->Frame();

	SetMetrics(Point(windowRect.left, windowRect.top),
		   Size(Math::Min((Int) Math::Round((windowRect.right  - windowRect.left - sizeModifier.cx) / fontSize), maxSize.cx),
			Math::Min((Int) Math::Round((windowRect.bottom - windowRect.top  - sizeModifier.cy) / fontSize), maxSize.cy)));

	return Success();
}

S::Int S::GUI::WindowHaiku::Show()
{
	if (wnd == NIL) return Success();

	wnd->Show();

	return Success();
}

S::Int S::GUI::WindowHaiku::Hide()
{
	if (wnd == NIL) return Success();

	wnd->Hide();

	return Success();
}

S::Int S::GUI::WindowHaiku::SetMetrics(const Point &nPos, const Size &nSize)
{
	wnd->MoveTo(nPos.x, nPos.y);
	wnd->ResizeTo(Math::Round(nSize.cx * fontSize) + sizeModifier.cx, Math::Round(nSize.cy * fontSize) + sizeModifier.cy);

	return Success();
}
