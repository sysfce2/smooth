 /* The smooth Class Library
  * Copyright (C) 1998-2004 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_BACKENDSDL_
#define _H_OBJSMOOTH_BACKENDSDL_

namespace smooth
{
	namespace Backends
	{
		class BackendSDL;
	};
};

#include "../backend.h"

namespace smooth
{
	namespace Backends
	{
		const Int	 BACKEND_SDL = 3;

		class SMOOTH_STATIC_EXPORT BackendSDL : public Backend
		{
			public:
					 BackendSDL();
				virtual	~BackendSDL();

				Int	 Init();
				Int	 Deinit();
		};
	};
};

#endif
