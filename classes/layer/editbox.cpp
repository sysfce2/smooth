 /* The smooth Class Library
  * Copyright (C) 1998-2003 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the "Artistic License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

/* TODO: rewrite EditBox from scratch */

#include <smooth/editbox.h>
#include <smooth/toolkit.h>
#include <smooth/definitions.h>
#include <smooth/loop.h>
#include <smooth/i18n.h>
#include <smooth/binary.h>
#include <smooth/string.h>
#include <smooth/metrics.h>
#include <smooth/math.h>
#include <smooth/stk.h>
#include <smooth/objectproperties.h>
#include <smooth/layer.h>
#include <smooth/timer.h>
#include <smooth/surface.h>

#include <imm.h>

#ifdef __WIN32__
__declspec (dllexport)
#endif

S::Int	 S::OBJ_EDITBOX = S::Object::RequestObjectID();

S::GUI::EditBox::EditBox(String text, Point pos, Size size, Int subType, Int iMaxSize)
{
	type				= OBJ_EDITBOX;
	objectProperties->text		= text;
	isRight				= False;
	isAsterisk			= False;
	markStart			= 0;
	markEnd				= 0;
	leftCut				= 0;
	objectProperties->fontColor	= Setup::ClientTextColor;
	subtype				= subType;
	maxSize				= iMaxSize;
	currLine			= 0;
	nOfLines			= CountLines();
	promptPos			= 0;
	linePromptPos			= 0;
	timer				= NIL;

	visText = NIL;

	possibleContainers.AddEntry(OBJ_LAYER);

	for (int i = 0; i < objectProperties->text.Length(); i++)
	{
		if (objectProperties->text[i] == 13 && objectProperties->text[i+1] != 10)	break;
		else if (objectProperties->text[i] == 10 && objectProperties->text[i+1] != 13)	break;

		visText[i] = objectProperties->text[i];
	}

	if (maxSize <= 0) maxSize = 32768;

	objectProperties->pos.x = Math::Round(pos.x * Setup::FontSize);
	objectProperties->pos.y = Math::Round(pos.y * Setup::FontSize);

	if (size.cx == 0)	objectProperties->size.cx = Math::Round(80 * Setup::FontSize);
	else			objectProperties->size.cx = Math::Round(size.cx * Setup::FontSize);
	if (size.cy == 0)	objectProperties->size.cy = Math::Round(19 * Setup::FontSize);
	else			objectProperties->size.cy = Math::Round(size.cy * Setup::FontSize);
}

S::GUI::EditBox::~EditBox()
{
	if (registered && myContainer != NIL) myContainer->UnregisterObject(this);
}

S::Int S::GUI::EditBox::Paint(Int message)
{
	if (!registered)	return Error;
	if (!visible)		return Success;

	Surface	*surface = myContainer->GetDrawSurface();

	EnterProtectedRegion();

	Rect	 frame;
	Rect	 textRect;
	Point	 realPos = GetRealPosition();

	switch (message)
	{
		default:
		case SP_PAINT:
			frame.left	= realPos.x;
			frame.top	= realPos.y;
			frame.right	= realPos.x + objectProperties->size.cx - 1;
			frame.bottom	= realPos.y + objectProperties->size.cy - 1;

			if (active)	surface->Box(frame, Setup::ClientColor, FILLED);
			else		surface->Box(frame, Setup::BackgroundColor, FILLED);

			surface->Frame(frame, FRAME_DOWN);

			textRect.left	= frame.left + 3;
			textRect.top	= frame.top + 3;
			textRect.right	= textRect.left + objectProperties->size.cx - 6;

			if (Binary::IsFlagSet(subtype, EDB_MULTILINE))	textRect.bottom	= textRect.top + objectProperties->size.cy - 6;
			else						textRect.bottom = textRect.top + 16;

			for (Int i = 0; i < nOfLines; i++)
			{
				if (active)	surface->SetText(GetLine(i), textRect, objectProperties->font, objectProperties->fontSize, objectProperties->fontColor, objectProperties->fontWeight);
				else		surface->SetText(GetLine(i), textRect, objectProperties->font, objectProperties->fontSize, Setup::TextColor, objectProperties->fontWeight);

				textRect.top += METRIC_EDITBOXLINEHEIGHT;

				if (textRect.top >= textRect.bottom) break;
			}

			break;
	}

	LeaveProtectedRegion();

	return Success;
}

S::Int S::GUI::EditBox::Process(Int message, Int wParam, Int lParam)
{
	if (!registered)		return Error;
	if (!active || !visible)	return Success;

	Window	*wnd = myContainer->GetContainerWindow();

	if (wnd == NIL) return Success;

	Surface	*surface = myContainer->GetDrawSurface();
	Point	 realPos = GetRealPosition();
	Int	 retVal = Success;
	Rect	 frame;
	int	 nOfChars = objectProperties->text.Length();
	Point	 p1;
	Point	 p2;
	String	 newtext;
	String	 lrcaddtext;
	int	 newpos;
	int	 i;

#ifdef __WIN32__
	int	 prevPromptPos;
	int	 prevLinePromptPos;
	int	 prevCurrLine;
#endif

	frame.left	= realPos.x;
	frame.top	= realPos.y;
	frame.right	= realPos.x + objectProperties->size.cx - 1;
	frame.bottom	= realPos.y + objectProperties->size.cy - 1;

	switch (message)
	{
		case SM_LOOSEFOCUS:
			if (objectProperties->clicked)
			{
				objectProperties->clicked = False;
				objectProperties->checked = False;

				if (!Binary::IsFlagSet(subtype, EDB_ASTERISK))	p1.x = frame.left + 3 + GetTextSizeX(visText, linePromptPos - leftCut, objectProperties->font, objectProperties->fontSize, objectProperties->fontWeight);
				else						p1.x = frame.left + 3 + GetTextSizeX(String().FillN('*', linePromptPos - leftCut), linePromptPos - leftCut, objectProperties->font, objectProperties->fontSize, objectProperties->fontWeight);

				p1.y = frame.top + 2 + currLine * METRIC_EDITBOXLINEHEIGHT;
				p2.x = p1.x;
				p2.y = p1.y + METRIC_EDITBOXLINEHEIGHT;

				surface->Line(p1, p2, Setup::ClientColor);

				promptPos = 0;
				linePromptPos = 0;

				if (timer != NIL)
				{
					timer->Stop();

					delete timer;

					timer = NIL;
				}

				Process(SM_MOUSEMOVE, 0, 0);
			}

			break;
		case SM_LBUTTONDOWN:
			if (!active) break;

			if (objectProperties->clicked)
			{
				objectProperties->clicked = False;
				objectProperties->checked = False;

				if (!Binary::IsFlagSet(subtype, EDB_ASTERISK))	p1.x = frame.left + 3 + GetTextSizeX(visText, linePromptPos - leftCut, objectProperties->font, objectProperties->fontSize, objectProperties->fontWeight);
				else						p1.x = frame.left + 3 + GetTextSizeX(String().FillN('*', linePromptPos - leftCut), linePromptPos - leftCut, objectProperties->font, objectProperties->fontSize, objectProperties->fontWeight);

				p1.y = frame.top + 2 + currLine * METRIC_EDITBOXLINEHEIGHT;
				p2.x = p1.x;
				p2.y = p1.y + METRIC_EDITBOXLINEHEIGHT;

				surface->Line(p1, p2, Setup::ClientColor);

				promptPos = 0;
				linePromptPos = 0;

				{
					HIMC		 hImc = ImmGetContext(wnd->hwnd);
					COMPOSITIONFORM	 info;

					info.dwStyle = CFS_POINT;
					info.ptCurrentPos.x = p1.x - 3;
					info.ptCurrentPos.y = p1.y - 2;

					ImmSetCompositionWindow(hImc, &info);

					ImmDestroyContext(hImc);
				}

				if (timer != NIL)
				{
					timer->Stop();

					delete timer;

					timer = NIL;
				}

				if (wnd->IsMouseOn(frame) && lParam != 1)
				{
					objectProperties->checked = True;
					Process(SM_LBUTTONDOWN, 0, 0);
				}
				else
				{
					Process(SM_MOUSEMOVE, 0, 0);
				}
			}
			else if (objectProperties->checked)
			{
				objectProperties->clicked = True;
				surface->Frame(frame, FRAME_DOWN);

				if (!Binary::IsFlagSet(subtype, EDB_MULTILINE))
				{
					for (Int i = 0; i <= visText.Length(); i++)
					{
						if (!Binary::IsFlagSet(subtype, EDB_ASTERISK))	newpos = frame.left + 3 + GetTextSizeX(visText, i, objectProperties->font,objectProperties->fontSize, objectProperties->fontWeight);
						else						newpos = frame.left + 3 + GetTextSizeX(String().FillN('*', i), i, objectProperties->font, objectProperties->fontSize, objectProperties->fontWeight);

						if (i > 0 && wnd->MouseX() < (p1.x + newpos) / 2)
						{
							promptPos = i - 1 + leftCut;
							linePromptPos = i - 1 + leftCut;

							break;
						}
						else
						{
							p1.x = newpos;

							if (i == nOfChars)
							{
								promptPos = i + leftCut;
								linePromptPos = i + leftCut;
							}
						}
					}
				}
				else
				{
// insert multiline code here
				}

				if (!Binary::IsFlagSet(subtype, EDB_ASTERISK))	p1.x = frame.left + 3 + GetTextSizeX(visText, linePromptPos - leftCut, objectProperties->font, objectProperties->fontSize, objectProperties->fontWeight);
				else						p1.x = frame.left + 3 + GetTextSizeX(String().FillN('*', linePromptPos - leftCut), linePromptPos - leftCut, objectProperties->font, objectProperties->fontSize, objectProperties->fontWeight);

				p1.y = frame.top + 2 + currLine * METRIC_EDITBOXLINEHEIGHT;
				p2.x = p1.x;
				p2.y = p1.y + METRIC_EDITBOXLINEHEIGHT;

				surface->Line(p1, p2, Setup::TextColor);

				promptVisible = True;

				{
					HIMC		 hImc = ImmGetContext(wnd->hwnd);
					COMPOSITIONFORM	 info;

					info.dwStyle = CFS_POINT;
					info.ptCurrentPos.x = p1.x - 4;
					info.ptCurrentPos.y = p1.y - 2;

					ImmSetCompositionWindow(hImc, &info);

					if (Setup::enableUnicode)
					{
						LOGFONTW	 font;

						font.lfHeight = objectProperties->fontSize;
						font.lfWidth = 0;
						font.lfEscapement = 0;
						font.lfOrientation = 0;
						font.lfWeight = objectProperties->fontWeight;
						font.lfItalic = false;
						font.lfUnderline = false;
						font.lfStrikeOut = false;
						font.lfOutPrecision = OUT_DEFAULT_PRECIS;
						font.lfClipPrecision = CLIP_DEFAULT_PRECIS;
						font.lfCharSet = DEFAULT_CHARSET;
						font.lfQuality = DEFAULT_QUALITY;
						font.lfPitchAndFamily = DEFAULT_PITCH | FF_ROMAN;
						wcscpy(font.lfFaceName, objectProperties->font);

						ImmSetCompositionFontW(hImc, &font);
					}
					else
					{
						LOGFONTA	 font;

						font.lfHeight = objectProperties->fontSize;
						font.lfWidth = 0;
						font.lfEscapement = 0;
						font.lfOrientation = 0;
						font.lfWeight = objectProperties->fontWeight;
						font.lfItalic = false;
						font.lfUnderline = false;
						font.lfStrikeOut = false;
						font.lfOutPrecision = OUT_DEFAULT_PRECIS;
						font.lfClipPrecision = CLIP_DEFAULT_PRECIS;
						font.lfCharSet = DEFAULT_CHARSET;
						font.lfQuality = DEFAULT_QUALITY;
						font.lfPitchAndFamily = DEFAULT_PITCH | FF_ROMAN;
						strcpy(font.lfFaceName, objectProperties->font);

						ImmSetCompositionFontA(hImc, &font);
					}

					ImmDestroyContext(hImc);
				}

				timer = new Timer();

				timer->onInterval.Connect(&EditBox::TimerProc, this);
				timer->Start(500);
			}

			break;
		case SM_MOUSEMOVE:
			if (!active) break;

			if (!objectProperties->checked && wnd->IsMouseOn(frame))
			{
				wnd->cursorset = True;

				LiSASetMouseCursor(LiSA_MOUSE_TEXTEDIT);

				objectProperties->checked = True;

				if (!objectProperties->clicked)
				{
					surface->Frame(frame, FRAME_UP);
				}
			}
			else if (objectProperties->checked && !wnd->IsMouseOn(frame))
			{
				wnd->cursorset = False;

				LiSASetMouseCursor(LiSA_MOUSE_ARROW);

				objectProperties->checked = False;

				if (!objectProperties->clicked)
				{
					surface->Frame(frame, FRAME_DOWN);
				}
			}

			break;
		case SM_MOUSELEAVE:
			if (!active) break;

			if (objectProperties->checked || objectProperties->clicked)
			{
				wnd->cursorset = False;

				LiSASetMouseCursor(LiSA_MOUSE_ARROW);

				objectProperties->checked = False;

				if (!objectProperties->clicked) surface->Frame(frame, FRAME_DOWN);
			}

			break;
#ifdef __WIN32__
		case WM_KILLFOCUS:
		case WM_ACTIVATEAPP:
			if (objectProperties->clicked) Process(SM_LBUTTONDOWN, 0, 0);

			break;
		case WM_KEYDOWN:
			if (!active) break;

			if (objectProperties->clicked)
			{
				switch (wParam)
				{
					case VK_LEFT:
						if (promptPos == 0) break;

						if (!Binary::IsFlagSet(subtype, EDB_ASTERISK))	p1.x = frame.left + 3 + GetTextSizeX(visText, linePromptPos - leftCut, objectProperties->font, objectProperties->fontSize, objectProperties->fontWeight);
						else						p1.x = frame.left + 3 + GetTextSizeX(String().FillN('*', linePromptPos - leftCut), linePromptPos - leftCut, objectProperties->font, objectProperties->fontSize, objectProperties->fontWeight);

						p1.y = frame.top + 2 + currLine * METRIC_EDITBOXLINEHEIGHT;
						p2.x = p1.x;
						p2.y = p1.y + METRIC_EDITBOXLINEHEIGHT;

						surface->Line(p1, p2, Setup::ClientColor);

						if (!Binary::IsFlagSet(subtype, EDB_MULTILINE))
						{
							promptPos--;
							linePromptPos--;
						}

						if (!Binary::IsFlagSet(subtype, EDB_ASTERISK))	p1.x = frame.left + 3 + GetTextSizeX(visText, linePromptPos - leftCut, objectProperties->font, objectProperties->fontSize, objectProperties->fontWeight);
						else						p1.x = frame.left + 3 + GetTextSizeX(String().FillN('*', linePromptPos - leftCut), linePromptPos - leftCut, objectProperties->font, objectProperties->fontSize, objectProperties->fontWeight);

						p2.x = p1.x;

						surface->Line(p1, p2, Setup::TextColor);

						{
							HIMC		 hImc = ImmGetContext(wnd->hwnd);
							COMPOSITIONFORM	 info;

							info.dwStyle = CFS_POINT;
							info.ptCurrentPos.x = p1.x - 3;
							info.ptCurrentPos.y = p1.y - 2;

							ImmSetCompositionWindow(hImc, &info);

							ImmDestroyContext(hImc);
						}

						promptVisible = True;

						break;
					case VK_RIGHT:
						if (promptPos >= nOfChars) break;

						if (!Binary::IsFlagSet(subtype, EDB_ASTERISK))	p1.x = frame.left + 3 + GetTextSizeX(visText, linePromptPos - leftCut, objectProperties->font, objectProperties->fontSize, objectProperties->fontWeight);
						else						p1.x = frame.left + 3 + GetTextSizeX(String().FillN('*', linePromptPos - leftCut), linePromptPos - leftCut, objectProperties->font, objectProperties->fontSize, objectProperties->fontWeight);

						p1.y = frame.top + 2 + currLine * METRIC_EDITBOXLINEHEIGHT;
						p2.x = p1.x;
						p2.y = p1.y + METRIC_EDITBOXLINEHEIGHT;

						surface->Line(p1, p2, Setup::ClientColor);

						if (!Binary::IsFlagSet(subtype, EDB_MULTILINE))
						{
							promptPos++;
							linePromptPos++;
						}

						if (!Binary::IsFlagSet(subtype, EDB_ASTERISK))	p1.x = frame.left + 3 + GetTextSizeX(visText, linePromptPos - leftCut, objectProperties->font, objectProperties->fontSize, objectProperties->fontWeight);
						else						p1.x = frame.left + 3 + GetTextSizeX(String().FillN('*', linePromptPos - leftCut), linePromptPos - leftCut, objectProperties->font, objectProperties->fontSize, objectProperties->fontWeight);

						p2.x = p1.x;

						surface->Line(p1, p2, Setup::TextColor);

						{
							HIMC		 hImc = ImmGetContext(wnd->hwnd);
							COMPOSITIONFORM	 info;

							info.dwStyle = CFS_POINT;
							info.ptCurrentPos.x = p1.x - 3;
							info.ptCurrentPos.y = p1.y - 2;

							ImmSetCompositionWindow(hImc, &info);

							ImmDestroyContext(hImc);
						}

						promptVisible = True;

						break;
					case VK_UP:
						if (currLine >= 1) currLine--;

						break;
					case VK_DOWN:
						if (Binary::IsFlagSet(subtype, EDB_MULTILINE) && (nOfLines > (currLine + 1))) currLine++;

						break;
					case VK_RETURN:
						if (Binary::IsFlagSet(subtype, EDB_MULTILINE))
						{
// insert multiline code here
						}
						else
						{
							if (objectProperties->clicked) Process(SM_LBUTTONDOWN, 0, 1);
						}

						break;
					case VK_BACK:
					case VK_DELETE:
						if (promptPos == 0 && wParam == VK_BACK) break;
						if (promptPos == nOfChars && wParam == VK_DELETE) break;

						if (!Binary::IsFlagSet(subtype, EDB_ASTERISK))	p1.x = frame.left + 3 + GetTextSizeX(visText, linePromptPos - leftCut, objectProperties->font, objectProperties->fontSize, objectProperties->fontWeight);
						else						p1.x = frame.left + 3 + GetTextSizeX(String().FillN('*', linePromptPos - leftCut), linePromptPos - leftCut, objectProperties->font, objectProperties->fontSize, objectProperties->fontWeight);

						p1.y = frame.top + 2 + currLine * METRIC_EDITBOXLINEHEIGHT;
						p2.x = p1.x;
						p2.y = p1.y + METRIC_EDITBOXLINEHEIGHT;

						surface->Line(p1, p2, Setup::ClientColor);

						if (wParam == VK_BACK)
						{
							for (i = 0; i < promptPos - 1; i++)
							{
								newtext[i] = objectProperties->text[i];
							}

							for (i = promptPos - 1; i < nOfChars; i++)
							{
								newtext[i] = objectProperties->text[i + 1];
							}
						}
						else
						{
							for (i = 0; i < promptPos; i++)
							{
								newtext[i] = objectProperties->text[i];
							}

							for (i = promptPos; i < nOfChars; i++)
							{
								newtext[i] = objectProperties->text[i + 1];
							}
						}

						if (wParam == VK_BACK)
						{
							promptPos--;
							linePromptPos--;
						}

						prevPromptPos = promptPos;
						prevLinePromptPos = linePromptPos;
						prevCurrLine = currLine;

						SetText(newtext);

						promptPos = prevPromptPos;
						linePromptPos = prevLinePromptPos;
						currLine = prevCurrLine;

						if (!Binary::IsFlagSet(subtype, EDB_ASTERISK))	p1.x = frame.left + 3 + GetTextSizeX(visText, linePromptPos - leftCut, objectProperties->font, objectProperties->fontSize, objectProperties->fontWeight);
						else						p1.x = frame.left + 3 + GetTextSizeX(String().FillN('*', linePromptPos - leftCut), linePromptPos - leftCut, objectProperties->font, objectProperties->fontSize, objectProperties->fontWeight);

						p1.y = frame.top + 2 + currLine * METRIC_EDITBOXLINEHEIGHT;
						p2.x = p1.x;
						p2.y = p1.y + METRIC_EDITBOXLINEHEIGHT;

						surface->Line(p1, p2, Setup::TextColor);

						{
							HIMC		 hImc = ImmGetContext(wnd->hwnd);
							COMPOSITIONFORM	 info;

							info.dwStyle = CFS_POINT;
							info.ptCurrentPos.x = p1.x - 3;
							info.ptCurrentPos.y = p1.y - 2;

							ImmSetCompositionWindow(hImc, &info);

							ImmDestroyContext(hImc);
						}

						promptVisible = True;

						break;
					default:
						break;
				}

				retVal = Break;
			}

			break;
		case WM_CHAR:
			if (!active) break;

			if (objectProperties->clicked)
			{
				if (nOfChars + leftCut == maxSize) break;

				if (wParam >= 32)
				{
					if (Binary::IsFlagSet(subtype, EDB_NUMERIC) && (wParam < '0' || wParam > '9') && wParam != 45 && wParam != '.') break;

					for (i = 0; i < promptPos; i++)
					{
						newtext[i] = objectProperties->text[i];
					}

					newtext[(int) promptPos] = wParam;

					for (i = promptPos; i <= nOfChars; i++)
					{
						newtext[i+1] = objectProperties->text[i];
					}

					if (!Binary::IsFlagSet(subtype, EDB_ASTERISK))	p1.x = frame.left + 3 + GetTextSizeX(visText, linePromptPos - leftCut, objectProperties->font, objectProperties->fontSize, objectProperties->fontWeight);
					else						p1.x = frame.left + 3 + GetTextSizeX(String().FillN('*', linePromptPos - leftCut), linePromptPos - leftCut, objectProperties->font, objectProperties->fontSize, objectProperties->fontWeight);

					p1.y = frame.top + 2 + currLine * METRIC_EDITBOXLINEHEIGHT;
					p2.x = p1.x;
					p2.y = p1.y + METRIC_EDITBOXLINEHEIGHT;

					surface->Line(p1, p2, Setup::ClientColor);

					promptPos++;
					linePromptPos++;

					prevPromptPos = promptPos;
					prevLinePromptPos = linePromptPos;
					prevCurrLine = currLine;

					SetText(newtext);

					promptPos = prevPromptPos;
					linePromptPos = prevLinePromptPos;
					currLine = prevCurrLine;

					if (!Binary::IsFlagSet(subtype, EDB_ASTERISK))	p1.x = frame.left + 3 + GetTextSizeX(visText, linePromptPos - leftCut, objectProperties->font, objectProperties->fontSize, objectProperties->fontWeight);
					else						p1.x = frame.left + 3 + GetTextSizeX(String().FillN('*', linePromptPos - leftCut), linePromptPos - leftCut, objectProperties->font, objectProperties->fontSize, objectProperties->fontWeight);

					p1.y = frame.top + 2 + currLine * METRIC_EDITBOXLINEHEIGHT;
					p2.x = p1.x;
					p2.y = p1.y + METRIC_EDITBOXLINEHEIGHT;

					surface->Line(p1, p2, Setup::TextColor);

					{
						HIMC		 hImc = ImmGetContext(wnd->hwnd);
						COMPOSITIONFORM	 info;

						info.dwStyle = CFS_POINT;
						info.ptCurrentPos.x = p1.x - 3;
						info.ptCurrentPos.y = p1.y - 2;

						ImmSetCompositionWindow(hImc, &info);

						ImmDestroyContext(hImc);
					}

					promptVisible = True;
				}
			}

			break;
#endif
	}

	return retVal;
}

S::Int S::GUI::EditBox::Deactivate()
{
	if (!active) return Success;

	if (!registered)
	{
		active = False;

		return Success;
	}

	if (objectProperties->clicked)
	{
		if (timer != NIL)
		{
			timer->Stop();

			delete timer;

			timer = NIL;
		}

		objectProperties->clicked = False;
	}

	return Widget::Deactivate();
}

S::Int S::GUI::EditBox::SetText(String txt)
{
	objectProperties->text = txt;

	promptPos		= 0;
	linePromptPos		= 0;
	currLine		= 0;
	nOfLines		= CountLines();

	visText = NIL;

	for (int k = 0; k < objectProperties->text.Length(); k++)
	{
		if (objectProperties->text[k] == 13 && objectProperties->text[k + 1] != 10)		break;
		else if (objectProperties->text[k] == 10 && objectProperties->text[k + 1] != 13)	break;

		visText[k] = objectProperties->text[k];
	}

	if (registered && visible)
	{
		Paint(SP_PAINT);

		onClick.Emit();
	}

	return Success;
}

S::Int S::GUI::EditBox::CountLines()
{
	Int	 retVal = 1;
	String	 currLine;
	Int	 currPos = 0;

	lines.DeleteAll();

	for (int i = 0; i < objectProperties->text.Length(); i++)
	{
		if (objectProperties->text[i] == 13)
		{
			lines.AddEntry(currLine, retVal - 1);

			currLine	= NIL;
			currPos		= 0;
			retVal++;

			if (objectProperties->text[i+1] == 10) i++;
			i++;
		}
		else if (objectProperties->text[i] == 10)
		{
			lines.AddEntry(currLine, retVal - 1);

			currLine	= NIL;
			currPos		= 0;
			retVal++;

			if (objectProperties->text[i+1] == 13) i++;
			i++;
		}

		if (objectProperties->text[i] == 13 || objectProperties->text[i] == 10) i--;
		else currLine[(int) currPos++] = objectProperties->text[i];
	}

	return retVal;
}

S::String S::GUI::EditBox::GetLine(Int number)
{
	if (number >= nOfLines) return NIL;

	if (!Binary::IsFlagSet(subtype, EDB_ASTERISK))
	{
		if (number == currLine) return visText;

		return lines.GetEntry(number);
	}
	else
	{
		if (number == currLine) return String().FillN('*', visText.Length());

		return String().FillN('*', lines.GetEntry(number).Length());
	}
}

S::Void S::GUI::EditBox::TimerProc()
{
	if (!registered)		return;
	if (!active || !visible)	return;

	Surface	*surface = myContainer->GetDrawSurface();
	Point	 realPos = GetRealPosition();
	Rect	 frame;
	Point	 lineStart;
	Point	 lineEnd;

	frame.left	= realPos.x;
	frame.top	= realPos.y;
	frame.right	= realPos.x + objectProperties->size.cx - 1;
	frame.bottom	= realPos.y + objectProperties->size.cy - 1;

	if (!Binary::IsFlagSet(subtype, EDB_ASTERISK))	lineStart.x = frame.left + 3 + GetTextSizeX(visText, linePromptPos - leftCut, objectProperties->font, objectProperties->fontSize, objectProperties->fontWeight);
	else						lineStart.x = frame.left + 3 + GetTextSizeX(String().FillN('*', linePromptPos - leftCut), linePromptPos - leftCut, objectProperties->font, objectProperties->fontSize, objectProperties->fontWeight);

	lineStart.y = frame.top + 2 + currLine * METRIC_EDITBOXLINEHEIGHT;
	lineEnd.x = lineStart.x;
	lineEnd.y = lineStart.y + METRIC_EDITBOXLINEHEIGHT;

	if (promptVisible)	surface->Line(lineStart, lineEnd, Setup::ClientColor);
	else			surface->Line(lineStart, lineEnd, Setup::TextColor);

	promptVisible = !promptVisible;
}