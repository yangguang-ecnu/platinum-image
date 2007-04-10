/////////////////////////////////////////////////////////////////////////////////
//
//  FLTK userIO parameters
//  
//  Widgets that present and allow setting values for userIO blocks
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

#ifndef __FLTKuserIOparameters__
#define __FLTKuserIOparameters__

#include <string>

#include <FL/Fl_Image.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Check_Button.H>
//#include <FL/Fl_Counter.H>
//#include <FL/Fl_Menu_Button.H>

#include "ptmath.h"
#include "histogram.h"

#include "global.h"

class subclass_type_error : public std::exception 
    {
    private:
        std::string _M_msg;

    public:
        explicit 
            subclass_type_error(const std::string& __arg)  : std::exception(), _M_msg(__arg) { }

        virtual 
            ~subclass_type_error() throw() { }

        virtual const char* 
            what() const throw()
            { return _M_msg.c_str(); }
    };

// *** layout constants ***

//default width for layout, widget will be resized as it is added to the block
#define INITPARWIDGETWIDTH 100
//height of one parameter, so those that require just one line get uniform sizes
#define STDPARWIDGETHEIGHT 35
//portion of this used for title
#define PARTITLEMARGIN 15
//inset of parameters in userIO box
#define PARBOXBORDER 6
//width of popup menu boxes
#define PARMENUWIDTH 75

typedef int volumeIDtype;

const int par_control_height=20;   //height for each parameter control

typedef void (userIO_callback)(int block_ID,int param_num);

// *** custom widgets ***

class FLTKvolume_choice : public Fl_Choice  //widget for choosing volumes, in FLTKuserIOpar_volume
                                            //and other widgets using specific volumes, like histograms or points
    {
    public:
        FLTKvolume_choice (int x, int y);

        void image_vector_has_changed ();   //update volume menu from datamanager

        volumeIDtype id_value ();                   //return selected volume ID
    };

class FLTK_histogram_base : public Fl_Widget        //base class for widget displaying a histogram
    {
    protected:
        Fl_RGB_Image * screen_image;
        unsigned char * screen_image_data;
        thresholdparvalue threshold;        //current threshold - borrowed by viewport highlighting
        
        std::vector<threshold_overlay *> thresholders;

        void wipe_image_data();             //clear image & image data

        void draw ();                       //FLTK redraw method
    public:
        histogram_base * histogram;     //histogram that is displayed
                                        //and interacted with

        thresholdparvalue get_threshold ();
        
        FLTK_histogram_base (int x, int y, int w, int h);
        ~FLTK_histogram_base();
        
        void resize(int x, int y, int w, int h);    //delete screen_image for later reallocation on resize
        
        virtual void refresh ();                    //make sure image is allocated, call histogram object
                                                    //to render
    };

class FLTK_histogram_2D : public FLTK_histogram_base
    {
    protected:
        float h_treshold_min,h_treshold_max,v_treshold_min,v_treshold_max;
        float drag_ref_x, drag_ref_y;               //starting point of drag, in fraction-of-box coordinates
        void draw();
        enum histogram2D_drag_mode {READY_RECT, SET_RECT,MOVE_RECT, READY_OVAL,SET_OVAL,MOVE_OVAL};   //selection mode
        histogram2D_drag_mode drag_mode;

    public:
        FLTK_histogram_2D (int x, int y, int w, int h);
        ~FLTK_histogram_2D();

        int handle(int);                           //handle FLTK events

        void set_selmode (int);                    //set rectangular/oval selection mode
        void set_volumes (int hor, int vert);      //set volume IDs and update
    };

// *** Parameter classes ***

//Base class for numerous parameter objects populating userIO blocks. Handles value retrieval, updates and creation/deletion
class FLTKuserIOparameter_base : public Fl_Group  {
protected:
    char widget_label[512];    //parameter name
                               //C string 'cos that's the way FLTK rolls
    userIO_callback * par_callback;
    
public:
    FLTKuserIOparameter_base();
    FLTKuserIOparameter_base(int w, int h,const std::string name);
    
    void set_callback(userIO_callback * p_callback);
    static void par_update_callback (Fl_Widget *callingwidget, void *);

    virtual void image_vector_has_changed () {}   //trigger updating of parameters depending
                                                          //on available volumes
    
    //here are all possible value types in any subclass
    //subclass implements only supported types
    //these cannot be made pure virtuals (= 0), or
    //all subclasses would have to implement all
    //(and any additional) par_value functions

    virtual void par_value (float & v)
        {throw subclass_type_error("requested float, actual type " + type_name() );}
    virtual void par_value (long & v)
        {throw subclass_type_error("requested long, actual type " + type_name() );}
    virtual void par_value (Vector3D & v)
        {throw subclass_type_error("requested Vector3D, actual type " + type_name() );}
    virtual void par_value (volumeIDtype & v)
        {throw subclass_type_error("requested volume ID, actual type " + type_name() );}
    virtual void par_value (bool & v)
        {throw subclass_type_error("requested bool, actual type " + type_name() );}
    virtual void par_value (thresholdparvalue & v)
        {throw subclass_type_error("requested thresholds, actual type " + type_name() );}

    virtual const std::string type_name ()
        {return "unknown (base)";}

    //hack-ish information for histograms
    virtual int histogram_volume_ID (int )
        {return NOT_FOUND_ID;}
    };

class FLTKuserIOpar_float : public FLTKuserIOparameter_base    //float value (using slider)
    {
    protected:
        Fl_Value_Slider * control;
    public:
        FLTKuserIOpar_float (const std::string name, float ma=1, float mi=0);
                                                   //order of parameters reverse of expected,
                                                   //because min is more typically left at default
                                                   //than max
        const std::string type_name ();
        void par_value (float & v);
    };

class FLTKuserIOpar_int : public FLTKuserIOparameter_base    //integer value (using slider)
    {
    protected:
        Fl_Value_Slider * control;
    public:
        FLTKuserIOpar_int (const std::string name, long ma=255, long mi=0);
                                                   //order of parameters reverse of expected,
                                                   //because min is more typically left at default
                                                   //than max
        const std::string type_name ();
        void par_value (long & v);
    };

class FLTKuserIOpar_int_box : public FLTKuserIOparameter_base    //integer value (using input box)
    {
    protected:
        Fl_Value_Input * control;
    public:
        FLTKuserIOpar_int_box (const std::string name, long ma=255, long mi=0);
                                                   //order of parameters reverse of expected,
                                                   //because min is more typically left at default
                                                   //than max
        const std::string type_name ();
        void par_value (long & v);
    };

class FLTKuserIOpar_bool : public FLTKuserIOparameter_base    //boolean value (using check box)
    {
    protected:
        Fl_Check_Button * control;
    public:
        FLTKuserIOpar_bool (const std::string name, bool init_status);
                                                  
        const std::string type_name ();
        void par_value (bool & v);
    };

class FLTKuserIOpar_volume : public FLTKuserIOparameter_base   //volume selection (using popup menu)
    {
    protected:
        FLTKvolume_choice * control;
    public:
        FLTKuserIOpar_volume(std::string name);

        void image_vector_has_changed ();

        void par_value(volumeIDtype & v);                     //volume ID

        const std::string type_name ();
    };

class FLTKuserIOpar_histogram2D : public FLTKuserIOparameter_base 
    {
    protected:
        FLTKvolume_choice * volume_h;
        FLTKvolume_choice * volume_v;

        FLTK_histogram_2D * histogram_widget;

        Fl_Button * oval_button;
        Fl_Button * rect_button;
    public:
        FLTKuserIOpar_histogram2D(std::string name);
        void resize(int x, int y, int w, int h);
        void par_value (thresholdparvalue & v);
        const std::string type_name ();

        static void vol_change_callback (Fl_Widget *callingwidget, void *);       //called when volumes are changed in popup menus
        static void selmode_change_callback (Fl_Widget *callingwidget, void *);   //called when oval/rect button is pushed
        void image_vector_has_changed ();
        int histogram_volume_ID (int n);                                          //volume used for axis n in histogram
        void FLTKuserIOpar_histogram2D::highlight_ROI (regionofinterest *);       //highlight region of interest
        void set_volumes (int hor, int vert);                                     //set volumes in histogram and refresh image
    };

class FLTKuserIOpar_message : public FLTKuserIOparameter_base                  //message text
    {
    protected:
        Fl_Output * message;
    public:
        FLTKuserIOpar_message(std::string name, std::string message_text);

        //no par_value

        const std::string type_name ();
    };

// *** Planned parameters ***

/*class FLTKuserIOpar_point : public FLTKuserIOparameter_base    //selection of a 3D location (global coords)
    //by click in a viewport
    {
    protected:
        Fl_Button * control;
    public:
        FLTKuserIOpar_point(std::string name);
        ~FLTKuserIOpar_point();

       void par_value (Vector3D & v); 
    };

class FLTKuserIOpar_action : public FLTKuserIOparameter_base    //push button for doing something,
    //like testing the integrity of parameters
    {
    protected:
        Fl_Button * control;
    public:
        FLTKuserIOpar_action(std::string name);
        ~FLTKuserIOpar_action();

        void par_value (bool & v);
    };*/

#endif