//////////////////////////////////////////////////////////////////////////
//
//  Datawidget $Revision$
//
//  The datawidget is the GUI representation of a dataset (image/point).
//  It provides controls for saving, deleting and shows the thumbnail.
//
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

#ifndef __datawidget__
#define __datawidget__

#include <string>

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Box.H>


#define DATAHANDLER_VOLUME_3D 1
#define DATAHANDLER_VECTOR_3D 2
#define DATAHANDLER_PIXMAP 3

#include "global.h"

#define MAXDATANAME 512

class datawidget : public Fl_Pack {
public:
    datawidget(int X, int Y, int W, int H, const char *L = 0);
private:
    int image_id;
    const static int thumbnail_size;
    uchar * thumbnail_image;
    std::string _name;
    enum {remove_mi_num=0,save_mi_num, dup_mi_num};
    Fl_Pack *hpacker;
    Fl_Input *filenamebutton;
    void cb_filenamebutton_i(Fl_Input*, void*);
    static void cb_filenamebutton(Fl_Input*, void*);
    Fl_Menu_Button *featuremenu;
    static Fl_Menu_Item menu_featuremenu[];
public:
    static Fl_Menu_Item *remove_mi;
    static Fl_Menu_Item *save_vtk_mi;
    static Fl_Menu_Item *duplicate_mi;
    static Fl_Menu_Item *transferfunction_mi;
private:
    Fl_Box *thumbnail;
public:
    Fl_Pack *extras;
private:
    Fl_Group *tfunction_;
public:
    datawidget(int datatype,int id, std::string n);
    void tfunction(Fl_Group * t);;
    static void toggle_tfunction(Fl_Widget* callingwidget, void*);
    Fl_Group * reset_tf_controls();

    // *** end of Fluid ***

protected:
    bool fromFile; ///indicates whether the data was created inside the program and perhaps needs to be saved;
public:
    ~datawidget ();
    void refresh_thumbnail ();

    static void change_name_callback(Fl_Widget *callingwidget, void *thisdatawidget);

    // *** access methods ***
    int get_image_id();
    void name(std::string n);
    const std::string name();
    void from_file(bool);
    bool from_file();
    };

#endif