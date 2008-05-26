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

FLTK_Event_viewport::FLTK_Event_viewport(int X,int Y,int W,int H) : Fl_Widget(X,Y,W,H)
{}

int FLTK_Event_viewport::handle(int event){
	FLTK_draw_viewport *fvp = (FLTK_draw_viewport*)this->parent();

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


void FLTK_Event_viewport::draw()
{}


VTK_FLTKviewport::VTK_FLTKviewport(int X,int Y,int W,int H, viewport *vp_parent) : Fl_Overlay_Window(X,Y,W,H)
{
	 viewport_parent = vp_parent;
//	event_widget = new FLTK_Event_viewport(0,0,W,H);
	
	 vtkFlRenderWindowInteractor *fl_vtk_window = NULL;

//	   create_window_with_rwi(fl_vtk_window, main_window,"Cone3.cxx Main Window");
//void create_window_with_rwi(vtkFlRenderWindowInteractor *&flrwi, ow *&flw, char *title)
  // set up main FLTK window
//   flw = new Fl_Window(300,330,title);
   //flw = new ow(0,0,300,330);
   
   // and instantiate vtkFlRenderWindowInteractor (here it acts like a FLTK window, i.e. you could also instantiate it as child of a
   // Fl_Group in a window)
   fl_vtk_window = new vtkFlRenderWindowInteractor(0,0,W,H,"");
    
   // this will be replaced if it's the main window 
//   flw->callback(non_main_window_callback, flrwi);

   // this will result in a little message under the rendering
//   Fl_Box* box = new Fl_Box(5,261,290,34, "3 = stereo, j = joystick, t = trackball, "
//                            "w = wireframe, s = surface, p = pick; "
//                            "you can also resize the window");
//   box->labelsize(10);
//   box->align(FL_ALIGN_WRAP);
   
   // we want a button with which the user can quit the application
//   Fl_Button* quit_button = new Fl_Button(100,300,100,25,"quit");
//   quit_button->callback(quit_cb,NULL);
   
   // we're done populating the flw
   //flw->end();
   this->end();

   // if the main window gets resized, the vtk window should resize with it
   this->resizable(fl_vtk_window);

//-------------------------------
//-------------------------------
  // create a rendering window and renderer
  vtkRenderer *ren = vtkRenderer::New();
  ren->SetBackground(0.1,0.1,0.1);
  
  vtkRenderWindow *renWindow = vtkRenderWindow::New();
  renWindow->AddRenderer(ren);

  // uncomment the statement below if things aren't rendering 100% on your
  // configuration; the debug output could give you clues as to why
  //renWindow->DebugOn();
   
  // NB: here we treat the vtkFlRenderWindowInteractor just like any other old vtkRenderWindowInteractor
  fl_vtk_window->SetRenderWindow(renWindow);

  // just like with any other vtkRenderWindowInteractor(), you HAVE to call
  // Initialize() before the interactor will function.  See the docs in vtkRenderWindowInteractor.h
  fl_vtk_window->Initialize();

  // create an actor and give it cone geometry
  vtkConeSource *cone = vtkConeSource::New();
  cone->SetResolution(8);
  vtkPolyDataMapper *coneMapper = vtkPolyDataMapper::New();
  coneMapper->SetInput(cone->GetOutput());
  vtkActor *coneActor = vtkActor::New();
  coneActor->SetMapper(coneMapper);
  coneActor->GetProperty()->SetColor(1.0, 0.0, 1.0);

  // assign our actor to the renderer
  ren->AddActor(coneActor);

  // We can now delete all our references to the VTK pipeline (except for
  // our reference to the vtkFlRenderWindowInteractor) as the objects
  // themselves will stick around until we dereference fl_vtk_window
  ren->Delete();
  renWindow->Delete();
  cone->Delete();
  coneMapper->Delete();
  coneActor->Delete();


}


//FLTK_draw_viewport::FLTK_draw_viewport(int X,int Y,int W,int H) : Fl_Window(X,Y,W,H)
FLTK_draw_viewport::FLTK_draw_viewport(int X,int Y,int W,int H, viewport *vp_parent) : Fl_Overlay_Window(X,Y,W,H)
//FLTK_draw_viewport::FLTK_draw_viewport(int X,int Y,int W,int H) : Fl_Widget(X,Y,W,H)
	{
	 viewport_parent = vp_parent;

//	cout<<"FLTK_draw_viewport::FLTK_draw_viewport "<<X<<" "<<Y<<" "<<W<<" "<<H<<endl;
//	event_widget = new Fl_Button(X,Y,W,H,"test");
//	event_widget = new Fl_Button(X+50,Y+50,100,40,"test");
//	event_widget = new Fl_Button(50,50,50,50,"hej");
	event_widget = new FLTK_Event_viewport(0,0,W,H);


//	event_widget->show();
//	this->box(FL_UP_BOX);
//	this->color(FL_BLUE);


	needs_rerendering();
	callback_action=CB_ACTION_NONE;

//	resizable(this);	//JK-
//	show();				//JK-
//	Fl::run();			//JK-
	}

	
void FLTK_draw_viewport::draw_overlay()
{
	this->viewport_parent->draw_overlay();
}


FLTK_draw_viewport::~FLTK_draw_viewport()
{}


void FLTK_draw_viewport::draw()
    {

    //The draw() virtual method is called when FLTK wants you to redraw your widget.
    //It will be called if and only if damage()  is non-zero, and damage() will be cleared to zero after it returns
//	cout<<"FLTK_draw_viewport::draw()..."<<endl;
    callback_event = viewport_event (pt_event::draw,this);
    //callback_event.FLTK_event::attach (this);

	//        viewport_widget->callback(viewport_callback, this); //viewport (_not_ FLTK_draw_viewport) handles the callbacks

    do_callback(CB_ACTION_DRAW);
    }

void FLTK_draw_viewport::draw(unsigned char *rgbimage)
    {
//	cout<<"FLTK_draw_viewport::draw(unsigned char *rgbimage)..."<<endl;
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


void FLTK_draw_viewport::resize  	(int new_in_x,int new_in_y, int new_in_w,int new_in_h) {
//	cout<<"FLTK_draw_viewport::resize..."<<endl;

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

void FLTK_draw_viewport::do_callback (callbackAction action)
    {
    callback_action=action;

    Fl_Widget::do_callback();
    
    callback_action=CB_ACTION_NONE;
    }


void FLTK_draw_viewport::needs_rerendering ()
{
    needsReRendering = true;
}