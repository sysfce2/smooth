 /* The smooth Class Library
  * Copyright (C) 1998-2003 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the "Artistic License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/main.h>
#include "designer.h"
#include "edit_component.h"
#include "tools.h"

Int smooth::Main()
{
	Designer	*app = new Designer();

	Loop();

	delete app;

	return 0;
}

Designer::Designer()
{
	dlgcounter = 1;

	POINT		 pos;
	SIZE		 size;

	pos.x = 2;
	pos.y = 2;
	size.cx = 389;
	size.cy = 169;

	wnd		= new Window(String("smooth Designer v").Append(SMOOTH_VERSION));
	title		= new Titlebar(true, false, true);
	menubar		= new Menubar();
	iconbar		= new Menubar();
	statusbar	= new Statusbar("Ready");

	timer1		= new Timer();
	timer1->onInterval.Connect(&Designer::TimerProc, this);

	menu_file	= new Menu();
	menu_dialog	= new Menu();

	menu_widgets		= new Menu();
	menu_widgets_add	= new Menu();
	menu_widgets_add_smooth	= new Menu();

	menubar->AddEntry("&File", NIL, menu_file);
	menubar->AddEntry("&Dialog", NIL, menu_dialog);
	menubar->AddEntry("&Widgets", NIL, menu_widgets);

	menu_file->AddEntry("E&xit")->onClick.Connect(&Designer::Close, this);

	menu_dialog->AddEntry("&New")->onClick.Connect(&Designer::NewDialog, this);

	menu_widgets->AddEntry("&Add", NIL, menu_widgets_add);

	menu_widgets_add->AddEntry("&smooth controls", NIL, menu_widgets_add_smooth);

	menu_widgets_add_smooth->AddEntry("&Button")->onClick.Connect(&Designer::AddButton, this);
	menu_widgets_add_smooth->AddEntry("&Layer")->onClick.Connect(&Designer::AddLayer, this);
	menu_widgets_add_smooth->AddEntry("&Menubar")->onClick.Connect(&Designer::AddMenubar, this);

	RegisterObject(wnd);

	wnd->RegisterObject(title);
	wnd->RegisterObject(menubar);
	wnd->RegisterObject(iconbar);
	wnd->RegisterObject(statusbar);

	wnd->SetMetrics(Point(50, 50), Size(600, 85));
	wnd->SetIcon(SI_DEFAULT);

	wnd->doQuit.Connect(&Designer::ExitProc, this);

	wnd->Show();

	timer1->Start(100);
}

Designer::~Designer()
{
	timer1->Stop();

	wnd->UnregisterObject(title);
	wnd->UnregisterObject(menubar);
	wnd->UnregisterObject(iconbar);
	wnd->UnregisterObject(statusbar);

	UnregisterObject(wnd);

	delete title;
	delete wnd;
	delete menubar;
	delete iconbar;
	delete statusbar;
	delete timer1;
	delete menu_file;
	delete menu_dialog;
	delete menu_widgets;
	delete menu_widgets_add;
	delete menu_widgets_add_smooth;
}

void Designer::Close()
{
	wnd->Close();
}

Bool Designer::ExitProc()
{
	for (int i = 0; i < dlgs.GetNOfEntries(); i++)
	{
		delete dlgs.GetNthEntry(i);
	}

	dlgs.DeleteAll();

	return True;
}

void Designer::NewDialog()
{
	Designer_EditComponent	*dlg = new Designer_EditComponent(this, String("Dialog").Append(String::IntToString(dlgcounter++)));

	dlgs.AddEntry(dlg);

	dlg->ShowDialog();
}

void Designer::ReportStatus(Designer_Status stat)
{
	switch (stat.event)
	{
		case STATUS_EVENT_REPORT_NAME:
			if (active_dlg != stat.dlg) wnd->SetText(String("smooth Designer v").Append(SMOOTH_VERSION).Append(" - ").Append(stat.currdlgname));
			active_dlg = stat.dlg;
			break;
		case STATUS_EVENT_REPORT_MOUSEPOSITION:
			statusbar->SetText(String("Mouse: ").Append(String::IntToString(stat.mousex)).Append(", ").Append(String::IntToString(stat.mousey)));
			break;
		case STATUS_EVENT_REPORT_WINDOWPOSITION:
			statusbar->SetText(String("Position: ").Append(String::IntToString(stat.wndposx)).Append(", ").Append(String::IntToString(stat.wndposy)));
			break;
		case STATUS_EVENT_REPORT_WINDOWSIZE:
			statusbar->SetText(String("Size: ").Append(String::IntToString(stat.wndsizex)).Append(", ").Append(String::IntToString(stat.wndsizey)));
			break;
		case STATUS_EVENT_REPORT_QUIT:
			wnd->SetText(String("SMOOTH Designer v").Append(SMOOTH_VERSION));
			break;
	}

	status = stat;
}

void Designer::TimerProc()
{
	Designer_EditComponent	*operat;
	bool			 mouseonwindow = false;

	for (int i = 0; i < dlgs.GetNOfEntries(); i++)
	{
		operat = dlgs.GetNthEntry(i);

		if (MouseX(operat->GetWindow()->hwnd, WINDOW) > 0 && MouseY(operat->GetWindow()->hwnd, WINDOW) > 0 && MouseX(operat->GetWindow()->hwnd, WINDOW) < operat->GetWindow()->GetObjectProperties()->size.cx && MouseY(operat->GetWindow()->hwnd, WINDOW) < operat->GetWindow()->GetObjectProperties()->size.cy)
		{
			mouseonwindow = true;

			break;
		}
	}

	if (!mouseonwindow && (statusbar->GetText() != "Ready")) statusbar->SetText("Ready");
}

void Designer::AddButton()
{
	active_dlg->AddObject(SMOOTH_BUTTON);
}

void Designer::AddLayer()
{
	active_dlg->AddObject(SMOOTH_LAYER);
}

void Designer::AddMenubar()
{
	active_dlg->AddObject(SMOOTH_MENUBAR);
}