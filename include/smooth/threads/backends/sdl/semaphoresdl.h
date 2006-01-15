 /* The smooth Class Library
  * Copyright (C) 1998-2006 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_SEMAPHORESDL_
#define _H_OBJSMOOTH_SEMAPHORESDL_

namespace smooth
{
	namespace Threads
	{
		class SemaphoreSDL;
	};
};

#include "../semaphorebackend.h"

#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>

namespace smooth
{
	namespace Threads
	{
		const Int	 SEMAPHORE_SDL	= 3;

		class SemaphoreSDL : public SemaphoreBackend
		{
			protected:
				SDL_sem	*semaphore;
				Bool	 mySemaphore;
			public:
					 SemaphoreSDL(Void * = NIL);
					~SemaphoreSDL();

				Void	*GetSystemSemaphore();

				Int	 Wait();
				Int	 Release();
		};
	};
};

#endif
