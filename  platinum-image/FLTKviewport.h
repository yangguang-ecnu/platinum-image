//////////////////////////////////////////////////////////////////////////
//
//  FLTKviewport
//
//  FLTK implementation of the central pane of each viewport that
//  displays the rendered image and can be clicked/dragged in.
//
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

#ifndef __FLTKviewport__
#define __FLTKviewport__

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Image.H>
#include <FL/fl_draw.H>

#include <iostream>
#include <vector>

#include <sstream>
#include <string>

#include "global.h"

#include "threshold.h"
#include "FLTK2Dregionofinterest.h"

//Callback action identifiers.

enum callbackAction {
    CB_ACTION_NONE,
    CB_ACTION_DRAW,
    CB_ACTION_RESIZE,
    CB_ACTION_HOVER,
    CB_ACTION_CLICK_PASS,   //pass click event through to renderer or specific application
    CB_ACTION_DRAG_PASS,    //pass drag event through to renderer or specific application
    CB_ACTION_DRAG_PAN,     //pan rendered image
    CB_ACTION_DRAG_ZOOM,    //zoom rendered image
    CB_ACTION_DRAG_FLIP,    //flip through rendered image sequence
    CB_ACTION_WHEEL_ZOOM,	//mouse wheel zoom
    CB_ACTION_WHEEL_FLIP	//mouse wheel flip through rendered image sequence
    };

//update viewport resizing immediately, the other UI widgets will lag behind however
//no effect on double buffered systems like OSX
#define SNAPPY_RESIZE

class FLTKviewport : public Fl_Widget
{
	    friend class viewport;
        friend class threshold_overlay;
    public:
	    FLTKviewport(int X,int Y,int W,int H);  //constructor
        ~FLTKviewport();
	    void draw(unsigned char *rgbimage); //joel
                                            //our "active" draw method - will redraw directly whenever it is called
                                            //this method draws the argument rgbimage AND
                                            //feedback (coordinates, cursor)
	    void resize (int new_x,int new_y,int new_w,int new_h);
	    int handle(int event);
    private:
        void draw();                //FLTK draw call - called when FLTK wants the viewport updated
	    void draw_feedback();       //draws the cursor
	    bool needs_re_rendering;	//set to true when we need to update the data drawn on screen
	   
        std::string feedback_string;      //info (coordinates and such)
        FLTK2Dregionofinterest * ROI;     //overlay for selecting and displaying a
                                          //region of interest
        threshold_overlay * thresholder;  //overlay displaying threshold from a histogram
        
        void do_callback (callbackAction action);  //do callback with specified action

	    //Variables used by callback function to process events
        int mouse_pos[2];
	    int drag_dx;
	    int drag_dy;
	    int wheel_y;            //mouse wheel rotation
	    int callback_action;    //which action to perform during click or drag processed by callback
	    int resize_w;	        //if FLTKviewport is resized --> needs_re_rendering = true;
	    int resize_h;
        };
#endif