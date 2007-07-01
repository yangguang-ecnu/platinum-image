// $Id: $

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

#include "event.h"
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


pt_event::pt_event()
    {
    mousePos[0] = 0;
    mousePos[1] = 0;
    value = 0;                    
    type = no_type;
    state = no_state;
    }

void FLTK_event::set_type ()
    {
    switch (Fl::event_button())
        {
        case FL_LEFT_MOUSE:
            { type = adjust;}
        break;
        case FL_MIDDLE_MOUSE:
            { type = create; }
        break;
        case FL_RIGHT_MOUSE:
            { type = browse; }
        break;
        }
    }

FLTK_event::FLTK_event (int FL_event):pt_event()
    {
    switch (FL_event){
        case FL_PUSH:
            set_type();
            state = begin;
            break;

        case FL_RELEASE:
            set_type();
            state = end;
            break;

        case FL_MOVE:
            type = hover;
            state = iterate;
            break;

        case FL_ENTER:
            type = hover;
            state = begin;
            break;

        case FL_LEAVE:
            type = hover;
            state = end;
            break;

        case FL_DRAG:
            set_type();
            state = iterate;
            break;
        case FL_MOUSEWHEEL:
            type = scroll;
            state = iterate;

            break;
        default:
            {} //default will be both type and state = none
        }

    mousePos[0]=Fl::event_x();
    mousePos[1]=Fl::event_y();
    }

viewport_event::viewport_event (int FL_event):FLTK_event(FL_event)
    {
    //at this point the parent classes have digested the event from FLTK down to a Platinum description
    }