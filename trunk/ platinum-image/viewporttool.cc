// $Id$

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

//#include "viewporttool.h"

#include "viewmanager.h"
extern viewmanager viewmanagement;

#include <FL/Fl_Window.h>
#include <FL/Fl_Button.h>

// *** base class ***

// static members
Fl_Window * viewporttool::toolbox = NULL;
Fl_Pack * viewporttool::statusArea = NULL;
std::string viewporttool::selected = "";
std::map<std::string, viewporttool::taste_fcn_pointer> viewporttool::tools = std::map<std::string, viewporttool::taste_fcn_pointer>  ();

void viewporttool::init (Fl_Pack * s)
{
    statusArea = s;
    Fl_Group::current(statusArea);
    
    //register tool classes
    
    viewporttool::Register("Navigation tool",&(nav_tool::taste_));
    viewporttool::Register("Dummy tool",&(dummy_tool::taste_));
    
    selected = "Navigation tool";
    
    //create toolbox widget
        
    toolbox = new Fl_Window (0,statusArea->y(),0,statusArea->h()); //toolfactory.buttons will set correct width
    const bool horizontal = true;
    int buttonSize  = horizontal? toolbox->h():toolbox->w();
    int x = 0; //toolbox is a subwindow, so it starts at 0
    int y = 0;
    
    toolbox->begin();
    
    for (std::map<std::string, taste_fcn_pointer>::iterator i = tools.begin();i != tools.end();i++)
        {
        std::string name = i->first;
        Fl_Button * button = new Fl_Button (x,y,buttonSize,buttonSize);
        button->label(strdup(name.substr(0,1).c_str()));
        button->tooltip(strdup(name.c_str()));
        
        button->box(FL_UP_BOX);
        button->down_box(FL_DOWN_BOX);
        button->type(FL_RADIO_BUTTON);
        button->callback(cb_toolbutton,(void*)&(i->first));

        if (name == selected)
            { button->set(); }
        
        if (horizontal)
            { x += buttonSize; }
        else
            { y += buttonSize; }
        }
    
    toolbox->size(x,y);
    toolbox->end();
}

viewporttool::viewporttool(viewport_event &)
{}

viewporttool::~viewporttool()
{}


void viewporttool::Register (std::string k,taste_fcn_pointer f)
{
    tools[k]=f;
}

viewporttool * viewporttool::taste(viewport_event & event)
{
    viewporttool * result = NULL;
    
    for (std::map<std::string, taste_fcn_pointer>::iterator i = tools.begin();i != tools.end();i++)
        {
        result = i->second(event);
        }
	return result;
}

void viewporttool::cb_toolbutton (Fl_Widget * button,void * key_ptr)
{
    std::string * key = reinterpret_cast<std::string *>(key_ptr);
    
    selected = *key;

    viewmanagement.refresh_viewports_after_toolswitch();
}

// *** tool classes ***
//registered in viewporttool::init

// *** navigation tool ***

nav_tool::nav_tool (viewport_event & event):viewporttool(event)
    {
    }

void nav_tool::handle(viewport_event &event)
    {}

viewporttool * nav_tool::taste_ (viewport_event & event)
    {
    return NULL;
    }

// *** dummy tool ***

dummy_tool::dummy_tool (viewport_event &event):viewporttool(event)
    {

    }

void dummy_tool::handle(viewport_event &event)
    {}

viewporttool * dummy_tool::taste_ (viewport_event &event)
    {
    return NULL;
    }