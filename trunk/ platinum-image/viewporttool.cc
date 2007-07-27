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
#include "viewmanager.h"
#include "rendermanager.h"
#include "userIOmanager.h"

extern viewmanager viewmanagement;
extern rendermanager rendermanagement;
extern viewmanager viewmanagement;
extern userIOmanager userIOmanagement;

#include "statusarea.h"

#include <FL/Fl_Window.h>
#include <FL/Fl_Button.h>

#pragma mark *** base class ***

// static members
Fl_Pack * viewporttool::toolbox = NULL;
statusarea * viewporttool::statusArea = NULL;
std::string viewporttool::selected = "";
std::map<std::string, viewporttool::vpt_create_pointer> viewporttool::tools = std::map<std::string, viewporttool::vpt_create_pointer>  ();

template <class TOOL>
void viewporttool::Register ()
{
    tools[TOOL::name()]=&CreateObject<TOOL>;
    TOOL::init();
}

template <class TOOL>
viewporttool * viewporttool::CreateObject(viewport_event &event)
{
    return new TOOL (event);
}

void viewporttool::init (int posX, int posY,statusarea * s)
{
    statusArea = s;
    
    //register tool classes
    
    viewporttool::Register<nav_tool>();
    //viewporttool::Register<dummy_tool>();
    viewporttool::Register<cursor_tool>();
    viewporttool::Register<histo2D_tool>();
    
    selected = "Navigation";
    
    //create toolbox widget
    const bool horizontal = true;

    toolbox = new Fl_Pack (posX,posY,1,statusArea->h());
    
    if (horizontal)
        { toolbox->type(FL_HORIZONTAL);}
    else
        {toolbox->type(FL_VERTICAL);};
        
    int buttonSize  = horizontal? toolbox->h():toolbox->w();
    int x = toolbox->x();
    int y = toolbox->y();
    
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
            { 
            x += buttonSize; 
            }
        else
            { 
            y += buttonSize;
            }
        }
    
    if (horizontal)
        { 
        y = buttonSize;
        }
    else
        { 
        x = buttonSize;
        }
    
    toolbox->resize(posX,posY,x,y);
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

void viewporttool::select (const std::string key)
{
    selected = key;

    int nc = toolbox->children();
    for (int c= 0; c < nc;c++)
        {
        Fl_Button * b = dynamic_cast< Fl_Button * >(toolbox->child(c));

        if (*(reinterpret_cast<const std::string *>(b->user_data())) == key)
            {
            b->setonly();
            }
        }  
}

viewporttool * viewporttool::taste(viewport_event & event,viewport * vp,renderer_base * r)
{
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
    
    userIOmanagement.select_tool (*key);
}

// *** tool classes ***
//registered in viewporttool::init

#pragma mark *** navigation tool ***

const float nav_tool::wheel_factor=0.02;
const float nav_tool::zoom_factor=0.01;

nav_tool::nav_tool (viewport_event & event):viewporttool(event)
    {
    //a tool constructor has to respond to the type of events it accepts by grabbing them,
    //or it won't be created
    if (event.type() == pt_event::hover || event.type() == pt_event::browse || event.type() == pt_event::adjust || event.type() == pt_event::scroll)
        {event.grab();}
    }

const std::string nav_tool::name()
{
    return "Navigation";
}

void nav_tool::init()
{}

void nav_tool::handle(viewport_event &event)
{
    if ( event.state() == pt_event::begin)
        {
        event.grab();
        
        //get pointer to renderer
        renderer = rendermanagement.get_renderer( myPort->get_renderer_id());
        
        dragLast[0] = event.mouse_pos_global()[0];
        dragLast[1] = event.mouse_pos_global()[1];
        }
    
    if (!event.handled())
        {
        const int * pms = myPort->pixmap_size();
        const float pan_factor=(float)1/(std::min(pms[0],pms[1]));
        const int * mouse = event.mouse_pos_global();
        
        FLTKviewport * fvp = event.get_FLTK_viewport();
        
        switch (event.type())
            {
            case pt_event::browse:
                if ( event.state() == pt_event::iterate)
                    {
                    event.grab();
                    
                    myRenderer->move(-(mouse[0]-dragLast[0])*pan_factor,-(mouse[1]-dragLast[1])*pan_factor);
                    
                    fvp->needs_rerendering();
                    }
                break;
                
            case pt_event::adjust:
                if ( event.state() == pt_event::iterate)
                    {
                    event.grab();
                    
                    myRenderer->move(0,0,0,1+(mouse[1]-dragLast[1])*zoom_factor);
                    
                    fvp->needs_rerendering();
                    }
                break;
                
            case pt_event::scroll:
                if ( event.state() == pt_event::iterate)
                    {
                    event.grab();
                    
                    myRenderer->move(0,0,event.scroll_delta()*wheel_factor);
                    
                    fvp->needs_rerendering();
                    }
                break;
                
            case pt_event::hover:
                //display values
                switch (event.state() )
                    {
                    case pt_event::begin:
                    case pt_event::iterate:
                        {
                            event.grab();
                            
                            numbers.str("");
                            
                            //get values and update statusfield
                            
                            const std::map<std::string, float>values=myRenderer->get_values_screen(mouse[0],mouse[1],fvp->w(),fvp->h());
                            
                            if (values.empty())
                                {
                                userIOmanagement.interactive_message();
                                }
                            else
                                {
                                for (std::map<std::string,float>::const_iterator itr = values.begin(); itr != values.end();itr++)
                                    {
                                    if (itr != values.begin())
                                        { numbers << "; ";}
                                    numbers << itr->first << ": " << itr->second;
                                    }
                                
                                userIOmanagement.interactive_message(numbers.str());
                                }
                        }
                        break;
                    case pt_event::end:
                        event.grab();
                        userIOmanagement.interactive_message();
                        break;
                        
                    }
                break;                
            }
        
        if (event.state() == pt_event::end)
            {
            renderer = NULL;
            }
        
        dragLast[0] = mouse[0];
        dragLast[1] = mouse[1];
        }
}

/*viewporttool * nav_tool::taste_ (viewport_event & event)
    {
    nav_tool * t = new nav_tool(event);
    if (event.type
        
        
    return t;
    }*/

#pragma mark *** dummy tool ***

dummy_tool::dummy_tool (viewport_event &event):viewporttool(event)
    {

    }

const std::string dummy_tool::name()
{
    return "Dummy";
}

void dummy_tool::handle(viewport_event &event)
{}

threshold_overlay * histo2D_tool::get_overlay ()
{
    return overlay;
}

#pragma mark *** cursor_tool ***

Fl_Output * cursor_tool::coord_display = NULL;
Fl_Button * cursor_tool::make_button = NULL;

cursor_tool::cursor_tool(viewport_event &event):nav_tool(event)
{
    if (event.type() == pt_event::create || event.type() == pt_event::hover)
        {
        event.grab();
        }
    
    selection[0] = -1;
}

const std::string cursor_tool::name()
{
    return "Cursor";
}

void cursor_tool::init()
{
    const int controls_w = 90;
    
    Fl_Group * controls = userIOmanagement.status_area->add_pane<Fl_Group>(name());
    controls->size(controls_w+5,controls->h());
    //controls->type(FL_HORIZONTAL);
    //coord_display = new Fl_Output (controls->x(),controls->y(),60,controls->h(),"Location");
    make_button = new Fl_Button (controls->x(),controls->y(),controls_w,controls->h(),"Make point");
    controls->box(FL_FLAT_BOX);
    controls->color(FL_RED);
    
    controls->end();
}

void cursor_tool::handle(viewport_event &event)
{
    std::vector<int> mouse = event.mouse_pos_local();
    
    FLTKviewport * fvp = event.get_FLTK_viewport();
    
    if (event.type() == pt_event::create)
        {
        event.grab();
        switch (event.state())
            {
            case pt_event::begin:
            case pt_event::iterate:
                
                selection[0] = mouse[0];
                selection[1] = mouse[1];
                
                fvp->damage(FL_DAMAGE_ALL);
                
                break;
            }
        }
    if (event.type() == pt_event::draw)
        {
        if (selection[0] > 0)
            {
            event.grab();
            
            const int chsize = 6;
            const int chmarg = chsize + 2;
            const int chlen = 8;
            int drawC [2];
            drawC [0] = selection[0]+fvp->x();
            drawC [1] = selection[1]+fvp->y();

            fl_push_clip(fvp->x(),fvp->y(),fvp->w(),fvp->h());
            
            fl_color(FL_GRAY);
            fl_line(drawC[0], drawC[1]-chmarg, drawC[0], drawC[1]-chmarg-chlen); //above 
            fl_line(drawC[0]+chmarg,drawC[1],drawC[0]+chmarg+chlen,drawC[1]); //right 
            fl_line(drawC[0], drawC[1]+chmarg, drawC[0], drawC[1]+chmarg+chlen); //below 
            fl_line(drawC[0]-chmarg,drawC[1],drawC[0]-chmarg-chlen,drawC[1]); //left
            
            fl_color(FL_WHITE);
            fl_circle(drawC[0],drawC[1],chsize/2);
            
            fl_pop_clip();
            }
        }    
    
    if (event.type() == pt_event::resize)
        {
        event.grab();
    
        event.resize_point(selection[0],selection[1]);
        }
    
    if (event.type() == pt_event::hover )
        {
        event.grab();

        switch (event.state())
            {
            case pt_event::begin:
                //get pointer to renderer
                //renderer = rendermanagement.get_renderer( myPort->get_renderer_id());
            case pt_event::iterate:
                {
                    numbers.str("");
                    //get coords and update statusfield
                    Vector3D pos;
                    pos = myRenderer->view_to_voxel(mouse[0], mouse[1],fvp->w(),fvp->h());
                    if (pos[0]<0) //negative coordinates signify outside of
                                  //(positive and negative) bounds
                        {
                        userIOmanagement.interactive_message();
                        }
                    else
                        {
                        numbers << pos;
                        userIOmanagement.interactive_message(numbers.str());
                        }
                }
                break;
            case pt_event::end:
                //renderer = NULL;
                userIOmanagement.interactive_message();
                break;
            }
        }
    
    nav_tool::handle(event);
}
