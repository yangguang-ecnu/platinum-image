//////////////////////////////////////////////////////////////////////////
//
//  Datawidget
//
//  The datawidget is the GUI representation of a dataset (image/point).
//  It provides controls for saving, deleting and shows the thumbnail.
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

#ifndef __datawidget__
#define __datawidget__

#include <string>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Input.H>

#define DATAHANDLER_VOLUME_3D 1
#define DATAHANDLER_VECTOR_3D 2
#define DATAHANDLER_PIXMAP 3

#include "global.h"

//Class handling a dataset's entry in the list

class datawidget 
    {
    private:
        Fl_Group *packer;    //the toolkit-dependent "outer" widget
                            //that this object represents

        int volume_id;      //its associated volume
        Fl_Widget * filenamebutton;

        const static int thumbnail_size;
        uchar * thumbnail;

    public:
        ~datawidget ();
        datawidget (int datatype,int vol_id,std::string name);
        Fl_Group * get_widget(); //use with care
        void refresh_thumbnail ();
        int get_volume_id();
        void set_name(std::string n);
        static void change_name_callback(Fl_Widget *callingwidget, void *thisdatawidget);
    };
#endif