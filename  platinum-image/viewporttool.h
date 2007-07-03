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

class viewporttool 
{
public:
    typedef viewporttool * (*taste_fcn_pointer)(viewport_event &);
private:
    static Fl_Pack * statusArea;
    static std::map<std::string, taste_fcn_pointer> tools;
    static std::string selected;  //key for the currently selected tool
    static void cb_toolbutton (Fl_Widget *,void *);

protected:
    image_base * image; //do dynamic_cast to whatever class that is needed
public:
    static void Register (std::string,taste_fcn_pointer);
    
    //tool selection & controls
    const static int toolbox_w = 250;
    static Fl_Window * toolbox;
    
    viewporttool(viewport_event &);
    virtual ~viewporttool();
    
    static void init (Fl_Pack *);
    
    //virtual attach(image_base *) = 0;

    //static void grab (pt_event &event);
    static viewporttool * taste(viewport_event &);  //if the current tool responds to the event, return instance (which will be getting the events from now on until another tool is selected)
    virtual void handle(viewport_event &) = 0;
    //bool handle(int event,enum {create, adjust} );
};

class nav_tool : public viewporttool
{
public:
    nav_tool (viewport_event &);
    static viewporttool * taste_(viewport_event &);
    virtual void handle(viewport_event &);
};

class dummy_tool : public viewporttool
{
public:
    dummy_tool (viewport_event &);
    static viewporttool * taste_(viewport_event &);
    virtual void handle(viewport_event &);
};