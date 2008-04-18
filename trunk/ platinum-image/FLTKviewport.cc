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

//extern viewmanager viewmanagement;
//extern rendermanager rendermanagement;

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

FLTKviewport2::FLTKviewport2(int X,int Y,int W,int H) : Fl_Widget(X,Y,W,H)
{}

int FLTKviewport2::handle(int event){
	FLTKviewport *fvp = (FLTKviewport*)this->parent();

	fvp->callback_event = viewport_event(event,fvp);

    switch (event)
        {
        case FL_ENTER:
            Fl::focus(this);
            //allows keyboard events to be received once the mouse is inside
        case FL_FOCUS:
        case FL_UNFOCUS:
            //usually, one wants to show that a widget has focus
            //which is shown/hidden with these events
            //with mouse-over focus however, this can be annoying
            
            fvp->callback_event.grab();
            break;
        }
    
   fvp->do_callback ();

/*
    if(callback_event.handled()){
		return 1; 
	}else{
		return 0; 
	}
    
    callback_event = viewport_event (pt_event::no_type,NULL);
    return 1;
*/

return 1;//	return Fl_Widget::handle(event);
}


void FLTKviewport2::draw()
{}



//FLTKviewport::FLTKviewport(int X,int Y,int W,int H) : Fl_Window(X,Y,W,H)
FLTKviewport::FLTKviewport(int X,int Y,int W,int H, viewport *vp_parent) : Fl_Overlay_Window(X,Y,W,H)
//FLTKviewport::FLTKviewport(int X,int Y,int W,int H) : Fl_Widget(X,Y,W,H)
	{
	 viewport_parent = vp_parent;

//	cout<<"FLTKviewport::FLTKviewport "<<X<<" "<<Y<<" "<<W<<" "<<H<<endl;
//	drawing_widget = new Fl_Button(X,Y,W,H,"test");
//	drawing_widget = new Fl_Button(X+50,Y+50,100,40,"test");
//	drawing_widget = new Fl_Button(50,50,50,50,"hej");
	drawing_widget = new FLTKviewport2(0,0,W,H);


//	drawing_widget->show();
//	this->box(FL_UP_BOX);
//	this->color(FL_BLUE);


	needs_rerendering();
	callback_action=CB_ACTION_NONE;

//	resizable(this);	//JK-
//	show();				//JK-
//	Fl::run();			//JK-
	}

	
void FLTKviewport::draw_overlay()
{
	this->viewport_parent->draw_overlay();
}


FLTKviewport::~FLTKviewport()
{}


void FLTKviewport::draw()
    {

    //The draw() virtual method is called when FLTK wants you to redraw your widget.
    //It will be called if and only if damage()  is non-zero, and damage() will be cleared to zero after it returns
//	cout<<"FLTKviewport::draw()..."<<endl;
    callback_event = viewport_event (pt_event::draw,this);
    //callback_event.FLTK_event::attach (this);

	//        viewport_widget->callback(viewport_callback, this); //viewport (_not_ FLTKviewport) handles the callbacks

    do_callback(CB_ACTION_DRAW);
    }

void FLTKviewport::draw(unsigned char *rgbimage)
    {
//	cout<<"FLTKviewport::draw(unsigned char *rgbimage)..."<<endl;
//	cout<<"("<<x()<<" "<<y()<<" "<<w()<<" "<<h()<<")"<<endl;
    const int D=RGBpixmap_bytesperpixel;

    //damage (FL_DAMAGE_ALL);

    if (w() > 0 && h() > 0)
    {
        // do not redraw zero-sized viewport, fl_draw_image will break down
		
		fl_draw_image(rgbimage,0,0,w(),h());

/*		This code is probably not needed anymore:

        #if defined(__APPLE__)
            const int LD=w(); //size of one pixmap line
			// fl_draw_image(rgbimage,x(),y(),w(),h(), D,LD) ;
            fl_draw_image(rgbimage,0,0,w(),h(), D,LD); //JK- Drawing is done relative to the window...
        #else
			// fl_draw_image(rgbimage,x(),y(),w(),h(), D) ;
            fl_draw_image(rgbimage,0,0,w(),h(), D); //JK- Drawing is done relative to the window...
        #endif
*/
    }

    //draw_feedback();
    }


void FLTKviewport::resize  	(int new_in_x,int new_in_y, int new_in_w,int new_in_h) {
//	cout<<"FLTKviewport::resize..."<<endl;

    //store new size so CB_ACTION_RESIZE will know about it (via callback)
    resize_w=new_in_w; resize_h=new_in_h;
    callback_event = viewport_event (pt_event::resize, this);
    callback_event.set_resize(resize_w,resize_h);
    
    do_callback();
    
    //do the actual resize - redraw will follow, eventually
	Fl_Overlay_Window::resize(new_in_x,new_in_y,new_in_w,new_in_h);
    
    //Update "new" size so that pixmap reevaluation won't be triggered until next resize
    resize_w=w(); resize_h=h();
}

void FLTKviewport::do_callback (callbackAction action)
    {
    callback_action=action;

    Fl_Widget::do_callback();
    
    callback_action=CB_ACTION_NONE;
    }


void FLTKviewport::needs_rerendering ()
{
    needsReRendering = true;
}