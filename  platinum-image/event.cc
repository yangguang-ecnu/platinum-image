// $Id$

// This file is part of the Platinum library.
// Copyright (c) 2007 Uppsala University.
//
//    The Platinum library is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    The Platinum library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with the Platinum library; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

//#include "event.h"

#include "FLTKviewport.h"
#include <FL/Fl.H>

std::string FLTK_event::eventnames[] =
    {
    //array allows event names to be printed to strings and whatnot
    "FL_NO_EVENT",		//0
    "FL_PUSH",			//1
    "FL_RELEASE",		//2
    "FL_ENTER",		    //3
    "FL_LEAVE",		    //4
    "FL_DRAG",			//5
    "FL_FOCUS",		    //6
    "FL_UNFOCUS",		//7
    "FL_KEYDOWN",		//8
    "FL_KEYUP",	    	//9
    "FL_CLOSE",	    	//10
    "FL_MOVE",			//11
    "FL_SHORTCUT",		//12
    "FL_DEACTIVATE",	//13
    "FL_ACTIVATE",		//14
    "FL_HIDE",			//15
    "FL_SHOW",			//16
    "FL_PASTE",		    //17
    "FL_SELECTIONCLEAR",//18
    "FL_MOUSEWHEEL",	//19
    "FL_DND_ENTER", 	//20
    "FL_DND_DRAG",		//21
    "FL_DND_LEAVE", 	//22
    "FL_DND_RELEASE",	//23
    };

// *** pt_event ***

pt_event::pt_event()
    {
    mousePos[0] = 0;
    mousePos[1] = 0;
    mouseStart[0] = 0;
    mouseStart[1] = 0;
    value = 0;                    
    type_ = no_type;
    state_ = no_state;
    handled_ = false;
    }

void pt_event::grab ()
{
    handled_ = true;
}

void pt_event::ungrab ()
{
    handled_ = false;
}

void pt_event::set_resize (int w, int h)
{
    resizeDim[0] = w;
    resizeDim[1] = h;
    
    type_ = resize;
}

bool pt_event::handled ()
{
    return handled_;
}

const int * pt_event::mouse_pos_global()
{
    return mousePos;
}

const int pt_event::scroll_delta()
{
    return wheelDelta;   
}

const int * pt_event::drag_start()
{
    return mouseStart;
}

const int * pt_event::get_resize()
{
    return resizeDim;
}

const pt_event::pt_event_type pt_event::type()
{
    return type_;
}    

const pt_event::pt_event_state pt_event::state()
{
    return state_;
}

bool pt_event::has_modifier(const int mods)
{
    return (modifier & mods )!= 0;
}

bool pt_event::key_combo (const int k)
{
    return (k == value + modifier);
}


// *** FLTK_event ***

void FLTK_event::set_type ()
{    
    ulong state = 0x0;
    state = Fl::event_state();
    
    if (Fl::event_state(FL_BUTTON1))
        { type_ = adjust;}
    
    //middle MB or left + shift key
    if(state & FL_BUTTON2 || (state & FL_BUTTON1) && (state & FL_SHIFT) )
        { type_ = browse; }
    
    //right MB or left + alt key
    if(state & FL_BUTTON3 || (state & FL_BUTTON1) && (state & FL_ALT) )
        //if (Fl::event_state(FL_BUTTON3) ||  (Fl::event_state() & ~(FL_BUTTON1 | FL_ALT)))
        { type_ = create; }
		
	// left MB double click
	if ( (state & FL_BUTTON1) && Fl::event_clicks() > 0 )
	{	//  Fl::event_clicks returns non-zero if the most recent FL_PUSH or FL_KEYBOARD was a "double click"
		type_ = focus_viewports;
		Fl::event_clicks(0);	// set it to zero so that later code does not think an item was double-clicked
	}
	
	// left MB + ctrl key
	if ( (state & FL_BUTTON1) && (state & FL_CTRL) )
		{ type_ = rotate; }
}

FLTK_event::FLTK_event (FLTKviewport * fvp) : pt_event ()
{ 
    attach (fvp);
}

FLTK_event::FLTK_event (int FL_event,FLTKviewport * fvp):pt_event()
    {
    attach (fvp);
    
    modifier = (Fl::event_state(FL_SHIFT)  ? shift_key : 0 ) + (Fl::event_state(FL_ALT)  ? alt_key : 0 )+ (Fl::event_state(FL_META)  ? meta_key : 0 ) + (Fl::event_state(FL_CTRL)  ? ctrl_key : 0 );

    switch (FL_event){
        case FL_PUSH:
            set_type();
            state_ = begin;
            
            mouseStart[0] = Fl::event_x();
            mouseStart[1] = Fl::event_y();
            break;

        case FL_RELEASE:
            set_type();
            state_ = end;
            break;

        case FL_MOVE:
            type_ = hover;
            state_ = iterate;
            
            mouseStart[0] = Fl::event_x();
            mouseStart[1] = Fl::event_y();
            break;

        case FL_ENTER:
            type_ = hover;
            state_ = begin;
            
            break;

        case FL_LEAVE:
            type_ = hover;
            state_ = end;
            break;

        case FL_DRAG:
            set_type();
            state_ = iterate;
            break;
            
        case FL_MOUSEWHEEL:
            type_ = scroll;
            state_ = iterate;
            break;
            
        case FL_KEYDOWN:
            value = Fl::event_key(); //pt_event enumerations have the same values
                                     //as FLTK event, see event.h
            
                                     //IMPORTANT: to receive keyboard events,
                                     //the widget has to either get focus (with tab)
                                     //or set itself as focus, AND additionally,
                                     //accept the FL_FOCUS event
            type_ = key;
            
            state_ = begin;            
        case FL_KEYUP:            
            state_ = end;
            break;
            
        default:
            {} //default will be both type and state = none
        }

    mousePos[0]=Fl::event_x();
    mousePos[1]=Fl::event_y();
    wheelDelta = Fl::event_dy();
    }

void FLTK_event::attach (Fl_Widget * w)
{
    myWidget = w;
}

std::vector<int> FLTK_event::mouse_pos_local()
{
    std::vector<int> result;
    
    result.push_back(mousePos[0]-myWidget->x());
    result.push_back(mousePos[1]-myWidget->y());
    
    return result;
}

// *** viewport_event ***

viewport_event::viewport_event (int FL_event, FLTKviewport * fvp):FLTK_event(FL_event, fvp)
{
    //at this point the parent classes have digested the event from FLTK down to a Platinum description
}

viewport_event::viewport_event (pt_event_type t, FLTKviewport * fvp):FLTK_event(fvp)
{
    type_ = t;
    state_ = idle;
}

FLTKviewport * viewport_event::get_FLTK_viewport()
{
    return dynamic_cast<FLTKviewport *> (myWidget);
}

void viewport_event::resize_point (int &x,int &y)
{
    int oldSize [2];
    FLTKviewport * fvp = get_FLTK_viewport();
    
    oldSize[0] = fvp->w(); oldSize[1] = fvp->h();
    
    float scaling = std::max(resizeDim[0],resizeDim[1])/std::max(oldSize[0],oldSize[1]);
    
    x = (float)(x - oldSize[0]/2) * scaling + resizeDim[0]/2;
    y = (float)(y - oldSize[1]/2) * scaling + resizeDim[1]/2;
}
