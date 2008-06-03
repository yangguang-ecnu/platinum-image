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
#include <FL/Fl_Overlay_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Pack.H>
//#include <FL/Fl_Image.H>
#include <FL/fl_draw.H>

#include <iostream>
#include <vector>
#include <sstream>
#include <string>

#include "viewport.h" //JK circular #include?

#include "global.h"
#include "threshold.h"
#include "histo2D_tool.h"
#include "event.h"


//------------------------------------
//Test - vtkFlRenderWindowInteractor -  here we have all the usual VTK stuff that we need for our pipeline

#include "Utilities/vtkFl/vtkFlRenderWindowInteractor.h"
//#include "Utilities/vtkFl/vtkFlRenderWindowInteractor.cxx"

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkConeSource.h>
#include <vtkSuperquadricSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkActor.h>
//------------------------------------


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





class FLTK_VTK_viewport : public Fl_Overlay_Window
{
//		vtkFlRenderWindowInteractor *fl_vtk_window; //JK-ööö simple test...
	public:
	    FLTK_VTK_viewport(int X,int Y,int W,int H, viewport *vp_parent);  //constructor
		void needs_rerendering(); //passes this on to the viewport...
	private:
		viewport *viewport_parent;
		void draw_overlay(){};
};





class FLTK_Event_viewport : public Fl_Widget
{
public:
    FLTK_Event_viewport(int X,int Y,int W,int H);  //constructor
    int handle(int event);
    void draw();                //FLTK draw call - called when FLTK wants the viewport updated
};






class FLTKpane : public Fl_Overlay_Window
{
	    friend class viewport;
        friend class FLTKviewport;
	    friend class FLTK_Event_viewport; //allows acces to e.g. callback_event
		friend class threshold_overlay;
		//friend class viewporttool;
        friend class histo2D_tool;

public:
	    FLTKpane(int X,int Y,int W,int H, viewport *vp_parent);  //constructor
        ~FLTKpane();
		void draw_overlay();
	    void draw(unsigned char *rgbimage); //joel
                                            //our "active" draw method - will redraw directly whenever it is called
                                            //this method draws the argument rgbimage AND
                                            //feedback (coordinates, cursor)
	    void resize (int new_x,int new_y,int new_w,int new_h);
//	    int handle(int event);
        void needs_rerendering();

        
private:
		viewport *viewport_parent;
		FLTK_Event_viewport *event_widget; //used to catch events in the viewport...

        void draw();                //FLTK draw call - called when FLTK wants the viewport updated
	    void draw_feedback();       //draws the cursor
	   
        std::string feedback_string;      //info (coordinates and such)
		void do_callback (callbackAction action = CB_ACTION_NONE);  //do callback with specified action
	    //Variables used by callback function to process events
        int mouse_pos[2];
	    int drag_dx;
	    int drag_dy;
	    int wheel_y;            //mouse wheel rotation
	    int callback_action;    //which action to perform during click or drag processed by callback
        viewport_event callback_event;
	    int resize_w;
	    int resize_h;
        };



class FLTKviewport : public Fl_Window   //handles the FLTK part of the viewport class
{
private:
	viewport *viewport_parent;

    Fl_Pack			*viewport_buttons;		//group containing per-viewport widgets such as the image menu
    Fl_Menu_Button	*datamenu_button;   
    Fl_Menu_Button	*directionmenu_button;
    Fl_Menu_Button	*renderermenu_button;
    Fl_Menu_Button	*blendmenu_button;

    void update_data_menu();   //set rendering status for images from rendercombination for this viewport's renderer
    void rebuild_renderer_menu();//update checkmark for current renderer type
    static void cb_renderer_select(Fl_Widget * o, void * v);
    void rebuild_blendmode_menu();//update checkmark for current blend mode
	
	
public:
    FLTKviewport(int xpos,int ypos,int width,int height, viewport *vp_parent, int buttonheight=20, int buttonwidth=70);
    virtual ~FLTKviewport();

    FLTKpane *viewport_widget;      //the frame ("viewport") displaying a rendered image //JK ööö TMP
//    FLTK_VTK_viewport *viewport_widget;    
	int h_pane();

	void viewport_callback(Fl_Widget *callingwidget);                               //Callback that handles events always redraws (pt_event::draw/resize)
    static void viewport_callback(Fl_Widget *callingwidget, void *thisviewport);    //FLTK callback wrapper

    static void toggle_image_callback(Fl_Widget *callingwidget, void *params);
    static void set_direction_callback(Fl_Widget *callingwidget, void *params);
    static void set_blendmode_callback(Fl_Widget *callingwidget, void *params);
	void set_direction_button_label(preset_direction direction);
	
    bool render_if_needed(FLTKpane *fp);
        
    // *** refresh methods ***
    //called when any update of the visual parts of viewport is affected, i.e. image and/or menu of images
    void refresh_menus();
    void refresh_overlay();                     //calls the redraw_overlay on the original FLTK_Overlay class... which schedules the redrawing... 
    
//    void update_fbstring(FLTKpane *fp);   //refresh values in number-at-pointer string
};

#endif
