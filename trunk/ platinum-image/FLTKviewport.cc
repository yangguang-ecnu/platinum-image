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

#include "FLTKviewport.h"

#include <FL/Enumerations.H>

using namespace std;

string eventnames[] =
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

FLTKviewport::FLTKviewport(int X,int Y,int W,int H) : Fl_Widget(X,Y,W,H)
    {
    needs_rerendering();
    
    callback_action=CB_ACTION_NONE;
    }

FLTKviewport::~FLTKviewport()
{}

void FLTKviewport::draw()
    {
    //The draw() virtual method is called when FLTK wants you to redraw your widget.
    //It will be called if and only if damage()  is non-zero, and damage() will be cleared to zero after it returns
    
    callback_event = viewport_event (pt_event::draw,this);
    //callback_event.FLTK_event::attach (this);
    
    do_callback(CB_ACTION_DRAW);
    }

void FLTKviewport::draw(unsigned char *rgbimage)
    {
    const int D=RGBpixmap_bytesperpixel;

    //damage (FL_DAMAGE_ALL);

    if (w() > 0 && h() > 0)
        {
        //do not redraw zero-sized viewport, fl_draw_image
        //will break down

        #if defined(__APPLE__)
            const int LD=w(); //size of one pixmap line
            fl_draw_image(rgbimage,x(),y(),w(),h(), D,LD) ;
        #else
            fl_draw_image(rgbimage,x(),y(),w(),h(), D) ;
        #endif
        }

    //draw_feedback();
    }

void FLTKviewport::resize  	(int new_in_x,int new_in_y, int new_in_w,int new_in_h) {
    //store new size so CB_ACTION_RESIZE will know about it (via callback)
    resize_w=new_in_w; resize_h=new_in_h;
    callback_event = viewport_event (pt_event::resize, this);
    callback_event.set_resize(resize_w,resize_h);
    
    do_callback();
    
    //do the actual resize - redraw will follow, eventually
    Fl_Widget::resize (new_in_x,new_in_y,new_in_w,new_in_h);
    
    //Update "new" size so that pixmap reevaluation won't be triggered until next resize
    resize_w=w(); resize_h=h();
}

void FLTKviewport::do_callback (callbackAction action)
    {
    callback_action=action;

    Fl_Widget::do_callback();
    
    callback_action=CB_ACTION_NONE;
    }

int FLTKviewport::handle(int event){
    
    callback_event = viewport_event (event,this);

    switch (event)
        {
        case FL_ENTER:
            Fl::focus(this);
            //allows keyboard events to be received
            //once the mouse is inside
        case FL_FOCUS:
        case FL_UNFOCUS:
            //usually, one wants to show that a widget has focus
            //which is shown/hidden with these events
            //with mouse-over focus however, this can be annoying
            
            callback_event.grab();
            
            break;
        }
    
    //callback_event = viewport_event (event,this);
    
    do_callback ();
    
    if (callback_event.handled())
        { return 1; }
    else
        {return 0; }
    
    callback_event = viewport_event (pt_event::no_type,NULL);
    
    return 1;
}

void FLTKviewport::needs_rerendering ()
{
    needsReRendering = true;
}