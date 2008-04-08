//////////////////////////////////////////////////////////////////////////
//
//   Viewport $Revision$
///
///  Abstraction of widget containing controls and image pane
///  (implemented in FLTKviewport) for each viewpoint of data
///
//   $LastChangedBy$


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

#ifndef __viewport__
#define __viewport__

#include "FLTKviewport.h"
#include "viewporttool.h"

#include "global.h"

#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Menu_Button.H>

class threshold_overlay;
class thresholdparvalue;
//extern datamanager datamanagement;

#define NO_RENDERER_ID 0

class viewport // friend with renderer_base
{
private:
    friend class FLTKviewport;
    friend class viewporttool;

    #pragma mark *** custom data ***

    int ID;  //viewport ID
    static int maxviewportID;
    
    static bool renderermenu_built;

	int rendererID; // this guy will render for us (each renderer instance contains an unique ID)
	int rendererIndex; // direct look up to vector array
                       //DEPRECATED: use either rendererID or - for fast access - pointer to renderer

	void draw_cursor(bool filledcenter);    // it's better that viewport draws the cursor
                                            //than cursor itself (drawing is ugly, FLTK-dependent; keep #renderingclasses down
	void draw_mousemode(int currentmode);
	void draw_coordinates(int x, int y);
	void draw_scolling_borders();

	//void  reslice();                         // will call renderer_base::reslice(...) and
                                             //supply our bitmap pointer to the 2D- or 3D-renderer which
                                             //in turn will do the actual reslicing
    viewporttool * busyTool;

    #pragma mark *** FLTK-related data ***

    uchar *rgbpixmap;

    int pixMapSize[2];

    void clear_rgbpixmap();             //fill for viewport without renderer

    Fl_Group *containerwidget;          //the containerwidget is the full viewport area: image + controls
    FLTKviewport *viewport_widget;      //the frame ("viewport") displaying a rendered image
    Fl_Gl_Window * GL_widget;
    
    Fl_Menu_Button * imagemenu_button;   
    Fl_Menu_Button * directionmenu_button;
    Fl_Menu_Button * renderermenu_button;
    Fl_Menu_Button * blendmenu_button;
    Fl_Pack *viewport_buttons;          //group containing per-viewport widgets such as the image menu

	// functions called by the main callback
	// will often call somehing in images (or, maybe it should be layer's, or cursor's) front end
	// void  move_cursor_relative(position3D offset);
	// void  set_cursor(position3D offset);
    
    
    void update_objects_menu();   //set rendering status for images
                                //from rendercombination for this viewport's renderer
    void rebuild_renderer_menu ();//update checkmark for current renderer type
    static void cb_renderer_select (Fl_Widget * o, void * v);
    void rebuild_blendmode_menu ();//update checkmark for current blend mode
	
	Matrix3D get_direction();
	void set_direction( const Matrix3D & dir );
	void set_direction( preset_direction direction ); 
	
	
public:
    viewport();
    virtual ~viewport();
	int x();	
	int y();
	int w();
	int h();

	void viewport_callback(Fl_Widget *callingwidget);                               //callback that handles events
                                                                                    //always redraws
    static void viewport_callback(Fl_Widget *callingwidget, void *thisviewport);    //FLTK callback wrapper

    static void toggle_image_callback(Fl_Widget *callingwidget, void * params );
    static void set_direction_callback(Fl_Widget *callingwidget, void * params );
    static void set_blendmode_callback(Fl_Widget *callingwidget, void * params );
	
    bool render_if_needed (FLTKviewport * f);
        
    // *** refresh methods ***
    //called when any update of the visual parts of viewport is affected, i.e.
    //image and/or menu of images

    void viewport::refresh_after_toolswitch();  //!refresh if viewport has a busy tool (selection etc.).
                                                //!The tool will be deleted
    void refresh_from_geometry (int g);         //!refresh if it uses the geometry specified by argument
    void refresh_from_combination (int c);      //!refresh if it uses the geometry specified by argument
    void refresh ();                            //!re-builds menu and makes viewport re-render and redraw eventually
    void refresh_overlay();                     //re-draws overlays
	void refresh_overlay_from_geometry(int g);
    
    threshold_overlay * get_threshold_overlay (thresholdparvalue *);

    void update_viewsize(int width, int height);

    void update_fbstring (FLTKviewport* f);   //refresh values in number-at-pointer string

    #pragma mark *** operators ***
	// virtual const viewport &operator=(const viewport &k) { return k; }
	bool virtual operator<<(const viewport &k) { return ID==k.ID; }
	bool virtual operator==(const viewport &k) { return ID==k.ID; }
	bool virtual operator==(const int &k) { return ID==k; }
	bool virtual operator!=(const viewport &k) { return ID!=k.ID; }
	bool virtual operator<(const viewport &k) { return ID<k.ID; }
	bool virtual operator>(const viewport &k) { return ID>k.ID; }
	friend std::istream &operator>>(std::istream &in, viewport &k) { in >> k.ID; return in; }
	friend std::ostream &operator<<(std::ostream &ut, const viewport &k) { ut << "[viewport. ID= " << k.ID << " rendererID: " << k.rendererID << " rendererIndex:  " << k.rendererIndex << "] "; return ut; }

	void initialize_viewport(int xpos, int ypos, int width, int height); 
    
    void initialize_GL ();
    void hide_GL ();
    
    int get_id() const ;
	int get_renderer_id() const ;
    
    const int * pixmap_size ()const;

	void connect_renderer(int rID);

	void  set_timer_delay(int delay = 0); // if zero, remove timer and always render directly
	
	void enable_and_set_direction( preset_direction direction );

};

#endif
