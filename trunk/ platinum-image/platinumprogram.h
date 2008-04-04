//////////////////////////////////////////////////////////////////////////
//
//  Helpers and intializations for programs using Platinum $Revision$
//
//  $LastChangedBy$
//

// This file is part of the Platinum library.
// Copyright (c) 2007 Uppsala University.
//
//    The Platinum library is free software; you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    The Platinum library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with the Platinum library; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA


#include <FL/Fl_Overlay_Window.H>

#include "viewmanager.h"
#include "datamanager.h"
#include "rendermanager.h"
#include "userIOmanager.h"

extern datamanager datamanagement;
extern rendermanager rendermanagement;
extern viewmanager viewmanagement;
extern userIOmanager userIOmanagement;


class myFl_Overlay_Window : public Fl_Overlay_Window{
	public:
		myFl_Overlay_Window(int w, int h):Fl_Overlay_Window(w,h){}
		void draw_overlay();
};


void platinum_init ();
//void platinum_setup (myFl_Overlay_Window & window, int num_viewports_h = 2, int num_viewports_v = 2,int tool_area_w = 250);
void platinum_setup (Fl_Window & window, int num_viewports_h = 2, int num_viewports_v = 2,int tool_area_w = 250);
static void close_program_callback(Fl_Widget *callingwidget, void *data);