 /* IOLib-C++, Universal IO Library
  * Copyright (C) 1998-2003 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Library General Public
  * License as published by the Free Software Foundation; either
  * version 2 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Library General Public License for more details.
  *
  * You should have received a copy of the GNU Library General Public
  * License along with this library; if not, write to the Free
  * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
  * MA 02111-1307, USA */

#ifndef _H_IOLIB_IOLIB_
#define _H_IOLIB_IOLIB_

#include <stdio.h>
#include <memory.h>

#if defined WIN32 && !defined __WIN32__
	#define __WIN32__
#endif

#if defined __WIN32__
	#include <windows.h>
#endif

#if defined __WIN32__ && !defined __CYGWIN32__
	#include <io.h>
#else
	#if defined __CYGWIN32__
		#include <sys/ioctl.h>
	#endif
	#if defined __FreeBSD__
		#include <sys/types.h>
	#endif
#endif

#if defined __WIN32__ && !defined i386
	#define i386
#endif

#ifndef IOLIB_STREAM_TYPES
	#define IOLIB_STREAM_TYPES

	const int	 STREAM_NONE		= 0;
	const int	 STREAM_STREAM		= 1;
	const int	 STREAM_DRIVER		= 2;
	const int	 STREAM_FILE		= 3;
	const int	 STREAM_ANSI		= 4;
	const int	 STREAM_BUFFER		= 5;
#endif

#ifndef IOLIB_ERROR_CODES
	#define IOLIB_ERROR_CODES

	const int	 IOLIB_ERROR_OK		= 0;	// everything went well
	const int	 IOLIB_ERROR_INTERNAL	= 1;	// an internal error
	const int	 IOLIB_ERROR_BADPARAM	= 2;	// a function has been called with a bad parameter
	const int	 IOLIB_ERROR_UNEXPECTED	= 3;	// some function returned an unexpected value
	const int	 IOLIB_ERROR_UNKNOWN	= 4;	// an unknown error occurred
	const int	 IOLIB_ERROR_NOTOPEN	= 5;	// the connected stream is not open or no stream is connected
	const int	 IOLIB_ERROR_OPNOTAVAIL	= 6;	// the requested operation is not available on the connected stream
	const int	 IOLIB_ERROR_WRONGOS	= 7;	// the requested functionality is not available under the current operating system
	const int	 IOLIB_ERROR_NODATA	= 8;	// no data is available in the queue
#endif

#include "iolib/iolibdriver-cxx.h"
#include "iolib/iolibfilter-cxx.h"
#include "iolib/iolibstream-cxx.h"
#include "iolib/instream-cxx.h"
#include "iolib/outstream-cxx.h"

#if defined __CYGWIN32__ && defined IOLIB_DLL
	#include <windows.h>

	extern "C" BOOL APIENTRY _cygwin_dll_entry(HINSTANCE, DWORD, LPVOID);
#endif

#define IOLibGetByte(x, y) (y > 3)  ? 0 : ((x >> (8 * y)) & 255)
#define IOLibGetBit(x, y)  (y > 31) ? 0 : ((x >>      y ) & 1)

#endif
