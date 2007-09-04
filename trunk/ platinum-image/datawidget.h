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

/*typedef enum {
    image,
    points
    } datawidget_type;*/

#include "global.h"

#define MAXDATANAME 512

#include "image_base.h"
#include "point_collection.h"
/*class data_base;
class image_base;
class point_collection;*/

class datawidget_base : public Fl_Pack {
protected:
    int data_id;  
    bool fromFile; ///indicates whether the data was created inside the program and perhaps needs to be saved;
    std::string _name;
    
    Fl_Pack *hpacker;
    Fl_Input *datanamebutton;
    void cb_filenamebutton_i(Fl_Input*, void*);
    static void cb_filenamebutton(Fl_Input*, void*);
    Fl_Menu_Button *featuremenu;

    // *** thumbnail
    const static int thumbnail_size;
    uchar * thumbnail_image;
    Fl_Box *thumbnail;
    
    // *** menus       
    enum {remove_mi_num=0,save_mi_num, dup_mi_num};
    const static Fl_Menu_Item menu_featuremenu_base[];
    const static Fl_Menu_Item *remove_mi;
    const static Fl_Menu_Item *save_vtk_mi;
    const static Fl_Menu_Item *duplicate_mi;
    Fl_Pack *extras;
    
    datawidget_base(data_base * d, std::string n);
public:
    static void change_name_callback(Fl_Widget *callingwidget, void *thisdatawidget);;
    //void make_window();

    virtual ~datawidget_base ();
    void refresh_thumbnail ();

    // *** access methods ***
    int get_data_id() const;
    
    void name(std::string n);
    const std::string name() const;
    
    void from_file(bool);
    bool from_file() const;
    };

template <class DATATYPE>
class datawidget:public datawidget_base
{};

template <>
class datawidget<point_collection>:public datawidget_base
{
public:
    datawidget(point_collection* p, std::string n);
};

template <>
class datawidget<image_base>:public datawidget_base
{   
    const static Fl_Menu_Item tfunctionmenu;			//the extra "transfer_funtion row"
    const static Fl_Menu_Item transferfunction_mi; 
    static void cb_transferswitch(Fl_Widget* o, void* v);
    static void toggle_tfunction(Fl_Widget* callingwidget, void*);
    
    Fl_Group *tfunction_;
    Fl_Menu_Item * menu_featuremenu_plustf;		//The "new" version of the "main menu"

    Fl_Menu_Item * tfunction_submenu;			//The transfer function submenu. Transferfactory handles all versions of transfer functions

    void setup_transfer_menu(Fl_Menu_Item* submenuitem, image_base * im);
public:
        datawidget(image_base* im, std::string n);
    virtual ~datawidget();
    
    void tfunction(Fl_Group * t);
    Fl_Group * reset_tf_controls();
    
};

#endif