//////////////////////////////////////////////////////////////////////////
//
//  Viewporttool $Revision$
//
/// The base class for implementing mouse behaviors/actions in a viewport
///
//  $LastChangedBy$
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

#include "image_base.h"
#include "event.h"

class viewport;
class renderer_base;
class thresholdparvalue;

class viewporttool 
{
public:
    //typedef viewporttool * (*taste_fcn_pointer)(viewport_event &);
    typedef viewporttool * (*vpt_create_pointer)(viewport_event &);

private:
    static Fl_Pack * statusArea;
    //static std::map<std::string, taste_fcn_pointer> tools;
    static std::map<std::string, vpt_create_pointer> tools;
    
    static std::string selected;  //key for the currently selected tool
    static void cb_toolbutton (Fl_Widget *,void *);
    
    template <class TOOL>
        static void Register ();
    template <class TOOL>
        static viewporttool *CreateObject(viewport_event &event);
protected:
    image_base *    image; //do dynamic_cast to whatever class that is needed
    viewport *      myPort;
    renderer_base * myRenderer; 
public:
    //static void Register (std::string,vpt_ctor_pointer);
    
    //tool selection & controls
    const static int toolbox_w = 250;
    static Fl_Window * toolbox;
    
    viewporttool(viewport_event &);
    virtual ~viewporttool();
    
    static void init (Fl_Pack *);
    
    static void select (std::string);
    static viewporttool * taste(viewport_event &,viewport *,renderer_base *);  //if the current tool responds to the event, return instance (which will be getting the events from now on until another tool is selected)
    virtual void handle(viewport_event &) = 0;
    //bool handle(int event,enum {create, adjust} );
};

class nav_tool : public viewporttool
{
protected:
    static const float wheel_factor;
    static const float zoom_factor;

    int dragLast [2]; //pt_event store drag origin (most useful) but
                      //for panning we need the pos at last iteration
public:
    nav_tool (viewport_event &);
    virtual void handle(viewport_event &);
    static const std::string name ();
};

class dummy_tool : public viewporttool //test tool
{
public:
    dummy_tool (viewport_event &);
    virtual void handle(viewport_event &);
    static const std::string name ();
};

class cursor_tool : public nav_tool //subclass of nav_tool because it is useful to be able to
                                    //do some navigation while working with a selection
{
protected:
    int selection [2]; //fraction of screen coordinates during selection
public:
    cursor_tool (viewport_event &);
    static const std::string name ();
    virtual void handle(viewport_event &);    
};

class freeform_ROI_tool : public viewporttool
{

};

class histogram_tool : public nav_tool //tool for userIO click & drag (only in Histo2D at this time)
{
    //NOTE: this tool was ported from an earlier architecture and
    //a bad example of viewporttool implementation. For a good example, see cursor_tool
private:
    FLTK2Dregionofinterest * ROI;
    threshold_overlay * overlay;
    
public:
    histogram_tool (viewport_event &event,thresholdparvalue * v = NULL,viewport * vp = NULL, renderer_base * r = NULL);
    
    void attach (viewport * vp, renderer_base * r);
    threshold_overlay * get_overlay();
    virtual void handle(viewport_event &);
    static const std::string name ();
};