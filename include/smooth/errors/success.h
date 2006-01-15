 /* The smooth Class Library
  * Copyright (C) 1998-2006 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_ERROR_SUCCESS_
#define _H_OBJSMOOTH_ERROR_SUCCESS_

namespace smooth
{
	namespace Errors
	{
		class Success;
	};
};

#include "error.h"

namespace smooth
{
	namespace Errors
	{
		class SMOOTHAPI Success : public Error
		{
			public:
						 Success();

				virtual String	 ToString();
		};
	};
};

#endif
