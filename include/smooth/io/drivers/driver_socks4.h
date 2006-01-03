 /* The smooth Class Library
  * Copyright (C) 1998-2006 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_DRIVER_SOCKS4_
#define _H_OBJSMOOTH_DRIVER_SOCKS4_

namespace smooth
{
	namespace IO
	{
		class DriverSOCKS4;
	};
};

#include "../../definitions.h"
#include "../driver.h"

#if defined __WIN32__ && !defined __CYGWIN32__
	#include <winsock2.h>
#else
	#include <netinet/in.h>
	#include <sys/socket.h>
	#include <netdb.h>
#endif

namespace smooth
{
	namespace IO
	{
		class SMOOTHAPI DriverSOCKS4 : public Driver
		{
			private:
				SOCKET		 stream;
				Bool		 closeStream;

				Void		 CloseSocket();
			public:
						 DriverSOCKS4(const String &, Int, const String &, Int);
						~DriverSOCKS4();

				Int		 ReadData(UnsignedByte *, Int);
				Int		 WriteData(UnsignedByte *, Int);
		};
	};
};

#endif
