 /* The smooth Class Library
  * Copyright (C) 1998-2003 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the "Artistic License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth/string.h>
#include <smooth/math.h>
#include <iconv.h>
#include <string.h>
#include <iolib-cxx.h>

char	*S::String::inputFormat = NIL;
char	*S::String::previousInputFormat = NIL;

S::String::String()
{
	bString = NIL;
	wString = NIL;

	Clean();
}

S::String::String(const int nil)
{
	bString = NIL;
	wString = NIL;

	Clean();
}

S::String::String(const char *iString)
{
	if (iString == NIL)
	{
		bString = NIL;
		wString = NIL;

		Clean();
	}
	else
	{
		bString = NIL;
		wString = NIL;

		ImportFormat(iString, inputFormat);
	}
}

S::String::String(const wchar_t *iString)
{
	if (iString == NIL)
	{
		bString = NIL;
		wString = NIL;

		Clean();
	}
	else
	{
		stringSize = wcslen(iString) + 1;

		bString = new char [stringSize];
		wString = new wchar_t [stringSize];

		wcscpy(wString, iString);

		for (Int i = 0; i < stringSize - 1; i++)
		{
			bString[i] = wString[i];
		}

		bString[stringSize - 1] = 0;

		checkInconsistency = False;
	}
}

S::String::String(const String &iString)
{
	if (iString.wString == NIL)
	{
		bString = NIL;
		wString = NIL;

		Clean();
	}
	else
	{
		stringSize = iString.stringSize;

		bString = new char [stringSize];
		wString = new wchar_t [stringSize];

		strncpy(bString, iString.bString, stringSize);
		wcsncpy(wString, iString.wString, stringSize);

		checkInconsistency = iString.checkInconsistency;

		FixInconsistency();
	}
}

S::String::~String()
{
	Clean();
}

S::Void S::String::Clean()
{
	if (bString != NIL) delete [] bString;
	if (wString != NIL) delete [] wString;

	bString = NIL;
	wString = NIL;
	stringSize = 0;
	checkInconsistency = False;
}

char *S::String::SetInputFormat(const char *iFormat)
{
	delete [] previousInputFormat;

	previousInputFormat = inputFormat;

	inputFormat = new char [strlen(iFormat) + 1];

	strcpy(inputFormat, iFormat);

	return previousInputFormat;
}

S::Void S::String::FixInconsistency()
{
	if (!checkInconsistency) return;

	checkInconsistency = False;

	for (Int i = 0; i < stringSize; i++)
	{
		if (bString[i] != wString[i]) bString[i] = wString[i];
	}
}

S::Void S::String::ImportFormat(const char *str, const char *format)
{
	Clean();

	if (format == NIL)
	{
		SetInputFormat("UTF-8");

		format = inputFormat;
	}

	stringSize = ConvertString(str, strlen(str), format, NIL, 0, "UTF-16LE") / 2 + 1;

	if (stringSize == 0) return;

	bString = new char [stringSize];
	wString = new wchar_t [stringSize];

	ConvertString(str, strlen(str), format, (char *) wString, stringSize * 2, "UTF-16LE");

	for (Int i = 0; i < stringSize - 1; i++)
	{
		bString[i] = wString[i];
	}

	bString[stringSize - 1] = 0;
	wString[stringSize - 1] = 0;
}

char *S::String::ConvertTo(String encoding)
{
	if (stringSize == 0) return NIL;

	Int	 bufferSize	= ConvertString((char *) wString, stringSize * 2, "UTF-16LE", NIL, 0, encoding) + 1;
	char	*buffer		= new char [bufferSize * 4];

	ConvertString((char *) wString, stringSize * 2, "UTF-16LE", buffer, bufferSize, encoding);

	return buffer;
}

wchar_t &S::String::operator [](int n)
{
	FixInconsistency();

	char	*bBuffer;
	wchar_t	*wBuffer;

	if (n >= stringSize - 1)
	{
		if (stringSize > 0)
		{
			bBuffer = new char [stringSize];
			wBuffer = new wchar_t [stringSize];

			strncpy(bBuffer, bString, stringSize);
			wcsncpy(wBuffer, wString, stringSize);

			delete [] bString;
			delete [] wString;

			bString = new char [n + 2];
			wString = new wchar_t [n + 2];

			for (Int i = 0; i < (n + 1); i++)
			{
				bString[i] = 32;
				wString[i] = 32;
			}

			bString[n + 1] = 0;
			wString[n + 1] = 0;

			for (Int j = 0; j < stringSize - 1; j++)
			{
				bString[j] = bBuffer[j];
				wString[j] = wBuffer[j];
			}

			stringSize = n + 2;

			delete [] bBuffer;
			delete [] wBuffer;
		}
		else
		{
			Clean();

			bString = new char [n + 2];
			wString = new wchar_t [n + 2];

			for (Int i = 0; i < (n + 1); i++)
			{
				bString[i] = 32;
				wString[i] = 32;
			}

			bString[n + 1] = 0;
			wString[n + 1] = 0;

			stringSize = n + 2;
		}

		bString[n] = 0;
		wString[n] = 0;
	}

	checkInconsistency = True;

	return wString[n];
}

wchar_t &S::String::operator [](Int n)
{
	return (*this)[(int) n];
}

S::String::operator char *()
{
	FixInconsistency();

	return bString;
}

S::String::operator wchar_t *()
{
	FixInconsistency();

	return wString;
}

S::String &S::String::operator =(const int nil)
{
	Clean();

	return *this;
}

S::String &S::String::operator =(const char *newString)
{
	if (newString == NIL)
	{
		Clean();
	}
	else
	{
		ImportFormat(newString, inputFormat);
	}

	return *this;
}

S::String &S::String::operator =(const wchar_t *newString)
{
	if (newString == NIL)
	{
		Clean();
	}
	else
	{
		Clean();

		stringSize = wcslen(newString) + 1;

		bString = new char [stringSize];
		wString = new wchar_t [stringSize];

		wcscpy(wString, newString);

		for (Int i = 0; i < stringSize - 1; i++)
		{
			bString[i] = wString[i];
		}

		bString[stringSize - 1] = 0;
	}

	return *this;
}

S::String &S::String::operator =(const String &newString)
{
	if (newString.wString == NIL)
	{
		Clean();
	}
	else
	{
		String	 backup(newString);

		Clean();

		stringSize = backup.stringSize;

		bString = new char [stringSize];
		wString = new wchar_t [stringSize];

		strncpy(bString, backup.bString, stringSize);
		wcsncpy(wString, backup.wString, stringSize);

		checkInconsistency = newString.checkInconsistency;

		FixInconsistency();
	}

	return *this;
}

S::Bool S::String::operator ==(const int nil)
{
	if (wString == NIL)	return True;
	else			return False;
}

S::Bool S::String::operator ==(const char *str)
{
	if (bString == NIL && str == NIL)	return True;
	if (bString == NIL || str == NIL)	return False;

	if (!Compare(str))	return True;
	else			return False;
}

S::Bool S::String::operator ==(const wchar_t *str)
{
	if (wString == NIL && str == NIL)	return True;
	if (wString == NIL || str == NIL)	return False;

	if (!Compare(str))	return True;
	else			return False;
}

S::Bool S::String::operator ==(const String &str)
{
	if (wString == NIL && str.wString == NIL)	return True;
	if (wString == NIL || str.wString == NIL)	return False;

	if (!Compare(str))	return True;
	else			return False;
}

S::Bool S::String::operator !=(const int nil)
{
	if (wString == NIL)	return False;
	else			return True;
}

S::Bool S::String::operator !=(const char *str)
{
	if (bString == NIL && str == NIL)	return False;
	if (bString == NIL || str == NIL)	return True;

	if (Compare(str) != 0)	return True;
	else			return False;
}

S::Bool S::String::operator !=(const wchar_t *str)
{
	if (wString == NIL && str == NIL)	return False;
	if (wString == NIL || str == NIL)	return True;

	if (Compare(str) != 0)	return True;
	else			return False;
}

S::Bool S::String::operator !=(const String &str)
{
	if (wString == NIL && str.wString == NULL)	return False;
	if (wString == NIL || str.wString == NULL)	return True;

	if (Compare(str) != 0)	return True;
	else			return False;
}

S::Int S::String::Length()
{
	if (stringSize == 0) return 0;

	stringSize = 1;

	for (Int i = 0; i >= 0; i++)
	{
		if (wString[i] == 0) break;

		stringSize++;
	}

	return stringSize - 1;
}

S::String &S::String::Append(const char *str)
{
	String	 str2 = str;

	return Append(str2);
}

S::String &S::String::Append(const wchar_t *str)
{
	String	 str2 = str;

	return Append(str2);
}

S::String &S::String::Append(const String &str)
{
	Int	 len1 = Length();
	Int	 len2 = String(str).Length();

	wchar_t	*composed = new wchar_t [len1 + len2 + 1];

	for (Int i = 0; i < (len1 + len2 + 1); i++)
	{
		composed[i] = 0;
	}

	for (Int j = 0; j < len1; j++)
	{
		composed[j] = wString[j];
	}

	for (Int k = len1; k < (len1 + len2); k++)
	{
		composed[k] = str.wString[k - len1];
	}

	*this = composed;

	delete [] composed;

	return *this;
}

S::String &S::String::Copy(const char *str)
{
	String 	 str2 = str;

	return Copy(str2);
}

S::String &S::String::Copy(const wchar_t *str)
{
	String 	 str2 = str;

	return Copy(str2);
}

S::String &S::String::Copy(const String &str)
{
	*this = str;

	return *this;
}

S::String &S::String::CopyN(const char *str, const Int n)
{
	String 	 str2 = str;

	return CopyN(str2, n);
}

S::String &S::String::CopyN(const wchar_t *str, const Int n)
{
	String 	 str2 = str;

	return CopyN(str2, n);
}

S::String &S::String::CopyN(const String &str, const Int n)
{
	String	 backup(str);

	Clean();

	for (Int i = n - 1; i >= 0; i--)
	{
		(*this)[i] = backup[i];
	}

	return *this;
}

S::Int S::String::Compare(const char *str)
{
	String 	 str2 = str;

	return Compare(str2);
}

S::Int S::String::Compare(const wchar_t *str)
{
	String 	 str2 = str;

	return Compare(str2);
}

S::Int S::String::Compare(const String &str)
{
	FixInconsistency();

	Int	 len1 = Length();
	Int	 len2 = String(str).Length();

	if (len1 != len2)
	{
		return len1 - len2;
	}
	else
	{
		for (Int i = 0; i <= len1; i++)
		{
			if (wString[i] != str.wString[i]) return 1;
		}
	}

	return 0;
}

S::Int S::String::CompareN(const char *str, Int n)
{
	String 	 str2 = str;

	return CompareN(str2, n);
}

S::Int S::String::CompareN(const wchar_t *str, Int n)
{
	String 	 str2 = str;

	return CompareN(str2, n);
}

S::Int S::String::CompareN(const String &str, Int n)
{
	FixInconsistency();

	if (Length() < n) return 1;

	for (int i = 0; i < n; i++)
	{
		if (wString[i] != str.wString[i]) return 1;
	}

	return 0;
}

S::String &S::String::Fill(const Int value)
{
	for (Int i = 0; i < Length(); i++)
	{
		(*this)[i] = value;
	}

	FixInconsistency();

	return *this;
}

S::String &S::String::FillN(const Int value, const Int count)
{
	Clean();

	for (Int i = count - 1; i >= 0; i--)
	{
		(*this)[i] = value;
	}

	FixInconsistency();

	return *this;
}

S::Int S::String::ToInt()
{
	if (Length() == 0) return 0;

	Bool	 neg = False;
	Int	 first = 0;
	Int	 n = 0;
	Int	 size = Length();

	for (Int i = size - 1; i >= 0; i--)
	{
		if ((wString[i] < 48 || wString[i] > 57) && wString[i] != 45) size = i;
	}

	if (wString[0] == '-')
	{
		neg = True;
		first = 1;
		size--;
	}

	for (Int j = first; j < (first + size); j++)
	{
		n += (Int) Math::Pow(10, size - (j - first) - 1) * (wString[j] - 48);
	}

	if (!neg)	return n;
	else		return 0 - n;
}

S::Float S::String::ToDouble()
{
	if (Length() == 0) return 0;

	Bool	 neg = False;
	Int	 first = 0;
	Float	 n = 0;
	Int	 size = Length();
	Int	 afpsize = 0;
	Int	 firstafp = 0;

	for (Int i = size - 1; i >= 0; i--)
	{
		if ((wString[i] < 48 || wString[i] > 57) && wString[i] != 45) size = i;
	}

	if (wString[size] == '.')
	{
		firstafp = size + 1;

		for (Int i = size + 1; i < Length(); i++)
		{
			if (wString[i] >= 48 && wString[i] <= 57)	afpsize++;
			else						break;
		}
	}

	if (wString[0] == '-')
	{
		neg = True;
		first = 1;
		size--;
	}

	for (Int j = first; j < (first + size); j++)
	{
		n += (Int) Math::Pow(10, size - (j - first) - 1) * (wString[j] - 48);
	}

	for (Int k = firstafp; k < (firstafp + afpsize); k++)
	{
		n += (Float) Math::Pow(10, 0 - (k - firstafp) - 1) * (wString[k] - 48);
	}

	if (!neg)	return n;
	else		return 0 - n;
}

S::String S::String::IntToString(const Int value)
{
	String	 newString;
	Int	 sz;

	if (value == 0)		sz = 1;
	else if (value < 0)	sz = (Int) Math::Log10(-value) + 2;
	else			sz = (Int) Math::Log10(value) + 1;

	if (value < 0)
	{
		newString[0] = 45;

		for (Int i = 0; i < sz-1; i++)
		{
			newString[i+1] = (wchar_t) Math::Floor(((-value) % (Int) Math::Pow(10, sz - i - 1)) / Math::Pow(10, sz - (i + 1) - 1)) + 48;
		}
	}
	else
	{
		for (Int i = 0; i < sz; i++)
		{
			newString[i] = (wchar_t) Math::Floor((value % (Int) Math::Pow(10, sz - i)) / Math::Pow(10, sz - (i + 1))) + 48;
		}
	}

	newString.FixInconsistency();

	return newString;
}

S::String S::String::DoubleToString(Float value)
{
	String	 newString;
	Int	 sz;
	Int	 afpslen;
	Int	 lastnn = 0;
	String	 afps;

	if ((Int) value == 0)	sz = 1;
	else if (value < 0)	sz = (Int) Math::Log10(-value) + 1;
	else			sz = (Int) Math::Log10(value) + 1;

	if (value < 0)
	{
		sz++;

		newString[0] = 45;

		for (Int i = 0; i < sz-1; i++)
		{
			newString[i+1] = (wchar_t) Math::Floor(((Int) (-value) % (Int) Math::Pow(10, sz - i - 1)) / Math::Pow(10, sz - (i + 1) - 1)) + 48;
		}

		afps = IntToString((Int) (-(value - (Int) value) * (Int) Math::Pow(10, 9)));
	}
	else
	{
		for (Int i = 0; i < sz; i++)
		{
			newString[i] = (wchar_t) Math::Floor(((Int) value % (Int) Math::Pow(10, sz - i)) / Math::Pow(10, sz - (i + 1))) + 48;
		}

		afps = IntToString((Int) ((value - (Int) value) * (Int) Math::Pow(10, 9)));
	}

	afpslen = afps.Length();

	for (Int i = 0; i < afpslen; i++)
	{
		if (afps[i] != 48) lastnn = i + 1;
	}

	afps.CopyN(afps, lastnn);

	lastnn = 0;

	for (Int j = 0; j < afpslen; j++)
	{
		if (afps[j] != 57) lastnn = j + 1;
	}

	if (lastnn == 0)	afps.CopyN(afps, 1);
	else			afps.CopyN(afps, lastnn);

	if (lastnn != 0) afps[lastnn - 1] = afps[lastnn - 1] + 1;

	if (value - (Int) value != 0)
	{
		newString.Append(".");

		for (Int i = 0; i < (9 - afpslen); i++)
		{
			newString.Append("0");
		}

		newString.Append(afps);
	}

	return newString;
}

S::Int S::ConvertString(const char *inBuffer, Int inBytes, const char *inEncoding, char *outBuffer, Int outBytes, const char *outEncoding)
{
	Bool	 delBuffer = False;

	if (outBuffer == NIL)
	{
		delBuffer	= True;
		outBytes	= inBytes * 8;
		outBuffer	= new char [outBytes];
	}

	for (Int i = 0; i < outBytes; i++) outBuffer[i] = 0;

	InStream	*in	= new InStream(STREAM_BUFFER, (void *) inBuffer, inBytes);
	OutStream	*out	= new OutStream(STREAM_BUFFER, (void *) outBuffer, outBytes);
	iconv_t		 cd	= iconv_open(outEncoding, inEncoding);
	Int		 size	= 0;

	iconv(cd, NULL, NULL, NULL, NULL);

	char		 inBuf[4096 + 4096];
	size_t		 inBufRest = 0;
	char		 outBuf[4096];

	for (;;)
	{
		size_t	 inBufSize = min(in->Size() - in->GetPos(), 4096);

		in->InputData((void *) (inBuf + 4096), inBufSize);

		if (inBufSize == 0)
		{
			if (inBufRest == 0)
			{
				break;
			}
			else
			{
				iconv_close(cd);

				delete in;
				delete out;

				return 0;
			}
		}
		else
		{
			const char	*inPtr	= inBuf + 4096 - inBufRest;
			size_t		 inSize	= inBufRest + inBufSize;

			inBufRest = 0;

			while (inSize > 0)
			{
				char	*outPtr		= outBuf;
				size_t	 outSize	= sizeof(outBuf);

				iconv(cd, (const char **) &inPtr, &inSize, &outPtr, &outSize);

				if (outPtr != outBuf)
				{
					out->OutputData((void *) outBuf, outPtr - outBuf);
					size += (outPtr - outBuf);
				}
			}
		}
	}

	char	*outPtr		= outBuf;
	size_t	 outSize	= sizeof(outBuf);

	iconv(cd, NULL, NULL, &outPtr, &outSize);

	if (outPtr != outBuf)
	{
		out->OutputData((void *) outBuf, outPtr - outBuf);
		size += (outPtr - outBuf);
	}

	iconv_close(cd);

	if (size >= outBytes) size = 0;

	delete in;
	delete out;

	if (delBuffer) delete [] outBuffer;

	return size;
}