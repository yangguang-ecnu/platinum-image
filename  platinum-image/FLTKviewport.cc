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
    cout << "FLTKviewport: " << X << " " << Y << " " << W << " " << H << endl;

    needs_re_rendering=true;

    callback_action=CB_ACTION_NONE;
    
    //the regionofinterest acts as an overlay for each viewport/FLTKviewport, so it's
    //created and deleted together with it
    ROI=new FLTK2Dregionofinterest(this);
    
    //the thresholding overlay will be created when needed
    thresholder=NULL;
    }

FLTKviewport::~FLTKviewport()
{
    delete ROI;
    if (thresholder !=NULL)
        {delete thresholder;}
}

void FLTKviewport::draw()
    {
    //The draw() virtual method is called when FLTK wants you to redraw your widget.
    //It will be called if and only if damage()  is non-zero, and damage() will be cleared to zero after it returns

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

    draw_feedback();
    }

void FLTKviewport::resize  	(int new_in_x,int new_in_y, int new_in_w,int new_in_h) {
    //store new size so CB_ACTION_RESIZE will know about it (via callback)
    resize_w=new_in_w; resize_h=new_in_h;
    do_callback(CB_ACTION_RESIZE);

    //do the actual resize - redraw will follow, eventually
    Fl_Widget::resize (new_in_x,new_in_y,new_in_w,new_in_h);
    
    //Update "new" size so that pixmap reevaluation won't be triggered until next resize
    resize_w=w(); resize_h=h();
    
#if defined(SNAPPY_RESIZE)
    //update viewport resizing immediately, 
    //redraw will happen anyway, the rest of the UI will lag behind however
    do_callback(CB_ACTION_DRAW);
#endif
    
    //thresholding overlay has exact size - delete, it will be recreated
    //at new size if needed
    if (thresholder !=NULL)
        {
        delete thresholder;
        thresholder=NULL;
        }
}

void FLTKviewport::do_callback (callbackAction action)
    {
    callback_action=action;

    Fl_Widget::do_callback();
    callback_action=CB_ACTION_NONE;
    }

int FLTKviewport::handle(int event){

    switch (event){
        case FL_PUSH:
            do_callback(CB_ACTION_CLICK_PASS);
            break;
            
        case FL_RELEASE:
            switch (Fl::event_button())
                {
                case FL_LEFT_MOUSE:
                    if (ROI != NULL)
                        {
                        ROI->drag_end();
                        }
                    break;
                }
            break;
        case FL_MOVE:
            do_callback(CB_ACTION_HOVER);
            
            //redraw the rgbimage without rerendering
            break;
        case FL_ENTER:
            window()->make_current(); //When entering window - make current!
            break;
            
        case FL_LEAVE:
            //redraw without cursor
            //since the feedback_string may be applicable only to the current
            //viewport, it should be cleared somewhere around here so that
            //it is only displayed following a suitable event

            do_callback(CB_ACTION_DRAW);
            break;
            
        case FL_DRAG:
            drag_dx = Fl::event_x() - mouse_pos[0];
            drag_dy = Fl::event_y() - mouse_pos[1];
            
            if (drag_dx !=0 || drag_dy !=0)
                {
                if(Fl::event_button() == FL_LEFT_MOUSE){
                    do_callback(CB_ACTION_DRAG_PASS);
                    }
                if(Fl::event_button() == FL_MIDDLE_MOUSE){
                    do_callback(CB_ACTION_DRAG_PAN);
                }
                if(Fl::event_button() == FL_RIGHT_MOUSE){
                    do_callback(CB_ACTION_DRAG_ZOOM);
                }            
            }
                break;
        case FL_MOUSEWHEEL:
            wheel_y=Fl::event_dy();
            
            do_callback(CB_ACTION_WHEEL_FLIP);
            
            wheel_y=0;
            
            break;
        default:
            callback_action=0;
            return 0;
            
    }
    mouse_pos[0]=Fl::event_x();
    mouse_pos[1]=Fl::event_y();

    callback_action=0; //better safe than sorry
    return 1;
}

void FLTKviewport::draw_feedback() // draws the cursor
{
    int ex = Fl::event_x();
    int ey = Fl::event_y();
    
    fl_push_clip(x(),y(),w(),h());
    
    fl_color(fl_rgb_color(0, 255, 255));

    if (thresholder != NULL)
    {
        thresholder->FLTK_draw();
    }
    
    if (ROI != NULL)
    {
        ROI->draw(this);
    }
    
    if (!(ex < x() || ey < y() || ex > x()+w() || ey > y() + h()) )
    {
        //only draw cursor when mouse is inside viewport
        //TODO: don't draw any extra fanciness during ROI interaction
        
        fl_font(FL_HELVETICA | FL_BOLD,14);
        fl_draw(feedback_string.c_str(), x()+w()-20, y()+3,15,35, FL_ALIGN_RIGHT);
        
        fl_color(FL_DARK3);
        
        fl_begin_line();
        
        fl_xyline(x(),ey,x()+w(),ey);		//joel - horizontal line (x y x1 y1)
        fl_xyline(ex,y(),ex,y()+h());		//joel - vertical line (x y x1 y1)
        
        fl_end_line();
    }
    fl_pop_clip();
}