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
#include "rendermanager.h"
extern rendermanager rendermanagement;

#include <FL/Fl_Window.h>
#include <FL/Fl_Button.h>

// *** base class ***

// static members
Fl_Window * viewporttool::toolbox = NULL;
Fl_Pack * viewporttool::statusArea = NULL;
std::string viewporttool::selected = "";
std::map<std::string, viewporttool::vpt_create_pointer> viewporttool::tools = std::map<std::string, viewporttool::vpt_create_pointer>  ();

template <class TOOL>
void viewporttool::Register (std::string k)
{
    tools[k]=&CreateObject<TOOL>;
}

template <class TOOL>
viewporttool * viewporttool::CreateObject(viewport_event &event)
{
    return new TOOL (event);
}

void viewporttool::init (Fl_Pack * s)
{
    statusArea = s;
    Fl_Group::current(statusArea);
    
    //register tool classes
    
    //viewporttool::Register("Navigation tool",&(nav_tool::nav_tool));
    viewporttool::Register<nav_tool>("Navigation tool");
    //viewporttool::Register("Dummy tool",&(dummy_tool::dummy_tool));
    viewporttool::Register<dummy_tool>("Dummy tool");

    selected = "Navigation tool";
    
    //create toolbox widget
        
    toolbox = new Fl_Window (0,statusArea->y(),0,statusArea->h()); //toolfactory.buttons will set correct width
    const bool horizontal = true;
    int buttonSize  = horizontal? toolbox->h():toolbox->w();
    int x = 0; //toolbox is a subwindow, so it starts at 0
    int y = 0;
    
    toolbox->begin();
    
    for (std::map<std::string, vpt_create_pointer>::iterator i = tools.begin();i != tools.end();i++)
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


/*void viewporttool::Register (std::string k,vpt_ctor_pointer f)
{
    tools[k]=f;
}*/

viewporttool * viewporttool::taste(viewport_event & event,viewport * vp,renderer_base * r)
{
    /*viewporttool * result = NULL;
    
    for (std::map<std::string, taste_fcn_pointer>::iterator i = tools.begin();i != tools.end();i++)
        {
        result = i->second(event);
        }
	return result;*/
    
    
    viewporttool * result = tools[selected](event);
    
    if (!event.handled())
        {
        //the selected tool class did not want the event
        delete result;
        result = NULL;
        }
    else
        {
        result->myPort = vp;
        result->myRenderer = r;
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
    if (event.type() == pt_event::hover || event.type() == pt_event::browse || event.type() == pt_event::adjust || event.type() == pt_event::scroll)
        {event.grab();}
    }

void nav_tool::handle(viewport_event &event)
{
    if ( event.state() == pt_event::begin)
        {
        event.grab();
        
        dragLast[0] = event.mouse_pos()[0];
        dragLast[1] = event.mouse_pos()[1];
        }
    
    if (event.state() == pt_event::iterate)
        {
        FLTKviewport * fvp = event.get_FLTK_viewport();
        switch (event.type())
            {
            case pt_event::browse:
                {
                    event.grab();
                    
                    const int * pms = myPort->pixmap_size();
                    
                    const float pan_factor=(float)1/(std::min(pms[0],pms[1]));
                    float pan_x=0;
                    float pan_y=0;
                    
                    const int * mouse = event.mouse_pos();
                    
                    pan_x-=(mouse[0]-dragLast[0])*pan_factor;
                    pan_y-=(mouse[1]-dragLast[1])*pan_factor;
                    
                    //fvp->ROI->resize (drag[0],drag[1],1,fvp);
                    myRenderer->move(pan_x,pan_y);
                    
                    fvp->needs_rerendering();
                    //fvp->redraw();
                    
                    dragLast[0] = mouse[0];
                    dragLast[1] = mouse[1];
                }
                break;
                
                /*case CB_ACTION_WHEEL_ZOOM:
                zoom*=1+f->wheel_y*wheel_factor;
                f->needs_rerendering();
                break;*/
            /*    
            case adjust:
                event.grab();

                f->ROI->resize (0,0,1+f->drag_dy*zoom_factor,f);
                rendermanagement.move(rendererIndex,0,0,0,1+f->drag_dy*zoom_factor);
                
                //zooming invalidates ROI
                FLTK2Dregionofinterest::current_ROI = NULL;
                
                f->needs_rerendering();
                break;
                
            case hover:
                {
                    event.grab();

                    update_fbstring(f);
                }
                break;
                
            case scroll:
                event.grab();

                rendermanagement.move(rendererIndex,0,0,f->wheel_y*wheel_factor);    //relative coordinates are designed so that
                                                                                     //1 = one z voxel step for z pan
                f->ROI->attach_histograms(rendererIndex);
                
                f->needs_rerendering();
                
                update_fbstring(f);
                break;*/
            }
        }
    
}

/*viewporttool * nav_tool::taste_ (viewport_event & event)
    {
    nav_tool * t = new nav_tool(event);
    if (event.type
        
        
    return t;
    }*/

// *** dummy tool ***

dummy_tool::dummy_tool (viewport_event &event):viewporttool(event)
    {

    }

void dummy_tool::handle(viewport_event &event)
    {}

/*viewporttool * dummy_tool::taste_ (viewport_event &event)
    {
    return NULL;
    }*/