 /* The smooth Class Library
  * Copyright (C) 1998-2003 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the "Artistic License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth/timer.h>
#include <smooth/objectmanager.h>

S::Void WINAPI TimerProc(HWND, unsigned int, unsigned int, unsigned long);

#ifdef __WIN32__
__declspec (dllexport)
#endif

S::Int	 S::OBJ_TIMER = S::Object::RequestObjectID();

S::Timer::Timer()
{
	type = OBJ_TIMER;

	timerid = -1;
}

S::Timer::~Timer()
{
	Stop();
}

S::Int S::Timer::Start(Int interval)
{
	if (timerid != -1) return Error;

	timerid = SetTimer(NIL, 0, interval, &TimerProc);

	return timerid;
}

S::Int S::Timer::Stop()
{
	if (timerid == -1) return Error;

	KillTimer(NIL, timerid);

	timerid = -1;

	return Success;
}

S::Int S::Timer::GetID()
{
	if (timerid == -1) return Error;

	return timerid;
}

S::Void WINAPI TimerProc(HWND wnd, unsigned int message, unsigned int timerid, unsigned long time)
{
	for (S::Int i = 0; i < S::Object::objectCount; i++)
	{
		S::Object *object = S::mainObjectManager->RequestObject(i);

		if (object != NIL)
		{
			if (object->GetObjectType() == S::OBJ_TIMER)
			{
				if (((S::Timer *) object)->GetID() == (signed int) timerid)
				{
					((S::Timer *) object)->onInterval.Emit();

					return;
				}
			}
		}
	}
}