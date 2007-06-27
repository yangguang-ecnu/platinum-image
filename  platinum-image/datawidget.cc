// $Id$

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

#include "datawidget.h"

#include "datamanager.h"
#include "rendermanager.h"
#include "transfer.h"

using namespace std;

extern datamanager datamanagement;
extern rendermanager rendermanagement;
extern uchar *animage; //defined in datamanager.cc

const int datawidget::thumbnail_size = 128;

void datawidget::cb_transferswitch(Fl_Widget* o, void* v) {
    transferfactory::tf_menu_params * par = reinterpret_cast<transferfactory::tf_menu_params *>(v);

    //Each item has space for a callback function and an argument for that function. Due to back compatability, the Fl_Menu_Item itself is not passed to the callback, instead you have to get it by calling  ((Fl_Menu_*)w)->mvalue()  where w is the widget argument.

    /*const Fl_Menu_Item * item = reinterpret_cast<Fl_Menu_*>(o)->mvalue();
    
    transferfactory::tf_menu_params * par = reinterpret_cast<transferfactory::tf_menu_params *>(item->user_data());*/

    par->switch_tf();
    }

transferfactory::tf_menu_params::tf_menu_params (const std::string t,image_base * i)
    {
    image = i;
    type = t;
    }

/*void datawidget::tf_menu_params::image(image_base * i)
    {
    image = i;
    }
void datawidget::tf_menu_params::type (std::string t)
    {
    type = t;
    }*/
void transferfactory::tf_menu_params::switch_tf()
    {
    pt_error::error_if_null(image,"Trying to make tfunction menu item with image = NULL");
    image->transfer_function(type);
    }

// *** begin FLUID ***

void datawidget::cb_filenamebutton_i(Fl_Input*, void*) {
  datamanagement.set_image_name(image_id,string(filenamebutton->value()));
}
void datawidget::cb_filenamebutton(Fl_Input* o, void* v) {
  ((datawidget*)(o->parent()->parent()))->cb_filenamebutton_i(o,v);
}

Fl_Menu_Item datawidget::menu_featuremenu[] = {
 {"Remove", 0,  (Fl_Callback*)datamanager::removedata_callback, (void*)(&datamanagement), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Save as VTK...", 0,  (Fl_Callback*)datamanager::save_vtk_image_callback, (void*)(&datamanagement), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Duplicate", 0,  0, 0, 1, FL_NORMAL_LABEL, 0, 14, 0},
 {"Transfer function", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {"Show/hide", 0,  (Fl_Callback*)toggle_tfunction, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0}
};
Fl_Menu_Item* datawidget::remove_mi = datawidget::menu_featuremenu + 0;
Fl_Menu_Item* datawidget::save_vtk_mi = datawidget::menu_featuremenu + 1;
Fl_Menu_Item* datawidget::duplicate_mi = datawidget::menu_featuremenu + 2;
Fl_Menu_Item* datawidget::tfunctionmenu = datawidget::menu_featuremenu + 3;
Fl_Menu_Item* datawidget::transferfunction_mi = datawidget::menu_featuremenu + 4;

// *** custom constructor declared in FLUID: ***

datawidget::datawidget(image_base * im, std::string n):Fl_Pack(0,0,270,130,NULL) {
    image_id = im->get_id();
    thumbnail_image = new unsigned char [thumbnail_size*thumbnail_size];

    // *** resume FLUID inits ***

    datawidget *o = this;
    o->box(FL_THIN_DOWN_FRAME);
    o->color(FL_BACKGROUND_COLOR);
    o->selection_color(FL_BACKGROUND_COLOR);
    o->labeltype(FL_NO_LABEL);
    o->labelfont(0);
    o->labelsize(14);
    o->labelcolor(FL_FOREGROUND_COLOR);
    o->align(FL_ALIGN_TOP);
    o->when(FL_WHEN_RELEASE);
        { Fl_Pack* o = hpacker = new Fl_Pack(0, 0, 270, 25);
        o->type(1);
            { Fl_Input* o = filenamebutton = new Fl_Input(0, 0, 240, 25);
            o->color(FL_LIGHT1);
            o->callback((Fl_Callback*)cb_filenamebutton, (void*)(this));
            o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
            Fl_Group::current()->resizable(o);
            }
            { Fl_Menu_Button* o = featuremenu = new Fl_Menu_Button(240, 0, 30, 25);
            o->box(FL_THIN_UP_BOX);
            o->user_data((void*)(this));
                { Fl_Menu_Item* o = &menu_featuremenu[3];
                setup_transfer_menu (o,im);
                }
            o->menu(menu_featuremenu);
            }
        resizable(filenamebutton);
        o->end();
        }
        { Fl_Box* o = thumbnail = new Fl_Box(0, 25, 270, 65);
        o->box(FL_EMBOSSED_BOX);
        o->hide();
        image( new Fl_RGB_Image(thumbnail_image, thumbnail_size, thumbnail_size, 1));
        image( NULL);
        }
        { Fl_Pack* o = extras = new Fl_Pack(0, 90, 270, 40);
        { Fl_Group* o = tfunction_ = new Fl_Group(0, 90, 270, 40);
        o->box(FL_EMBOSSED_FRAME);
        o->labelsize(11);
        o->align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
        o->hide();
        o->end();
        }
    o->end();
        }
    type(VERTICAL);
    resizable(extras);
    name(n);
    datamanagement.add_datawidget(this);
    end();
    }

// *** end FLUID ***

void datawidget::setup_transfer_menu(Fl_Menu_Item* submenuitem, image_base * im) {
    Fl_Menu_Item showhide_item;
    const int TFSUBNUMHEADITEMS = 1;

    showhide_item.label("Show/hide");
    showhide_item.shortcut (0);
    showhide_item.callback(static_cast<Fl_Callback*>(toggle_tfunction));
    showhide_item.user_data(NULL);
    showhide_item.flags = 128;
    showhide_item.labeltype(FL_NORMAL_LABEL);
    showhide_item.labelfont(0);
    showhide_item.labelsize(14);
    showhide_item.labelcolor(0);

    Fl_Menu_Item * tfunctions = transfer_manufactured::factory.function_menu(static_cast<Fl_Callback*>(cb_transferswitch));

    int subMenuSize = TFSUBNUMHEADITEMS+fl_menu_size (tfunctions);

    tfunction_submenu = new Fl_Menu_Item [subMenuSize+1];

    tfunction_submenu[0] = showhide_item;

    for (int i = TFSUBNUMHEADITEMS; i < subMenuSize ; i++)
        {
        int fmenuindex = i-TFSUBNUMHEADITEMS;

        tfunction_submenu [i] = tfunctions[fmenuindex];

        tfunction_submenu [i].user_data(new transferfactory::tf_menu_params (transferfactory::tf_name(fmenuindex), im ));
        }
    tfunction_submenu[subMenuSize].label(NULL);
    tfunction_submenu[TFSUBNUMHEADITEMS].set();

    submenuitem->flags = FL_SUBMENU_POINTER;
    submenuitem->user_data (tfunction_submenu);

    delete [] tfunctions;
    }

Fl_Group * datawidget::reset_tf_controls()
    {
    tfunction_->clear();
    tfunction_->begin();
    return tfunction_;
    }

void datawidget::toggle_tfunction(Fl_Widget* callingwidget, void*)
    {
    datawidget * the_datawidget=(datawidget *)(callingwidget->user_data());

    if (the_datawidget->tfunction_->visible())
        {
        the_datawidget->tfunction_->hide();
        }
    else
        { 
        the_datawidget->extras->size(the_datawidget->extras->w(),the_datawidget->extras->h()+the_datawidget->tfunction_->h());
        the_datawidget->tfunction_->show();
        }

    the_datawidget->parent()->parent()->redraw();
    }

datawidget::~datawidget ()
    {
    datamanagement.remove_datawidget(this);

    delete image();
    image(NULL);

    fl_menu_userdata_delete(tfunction_submenu);
    delete [] tfunction_submenu;

    delete [] thumbnail_image;
    }

void datawidget::refresh_thumbnail ()
    {
    rendermanagement.render_thumbnail(thumbnail_image, thumbnail_size, thumbnail_size, image_id);
    }

int datawidget::get_image_id()
    {
    return image_id;
    }

const string datawidget::name()
    {
    return std::string(_name);
    }

void datawidget::name(std::string n)
    {
    _name.assign(n);

    //filenamebutton->value(NULL);
    //((Fl_Input *)filenamebutton)->value(n.c_str());
    filenamebutton->value(_name.c_str());

    //when interactively changed, redrawing widget is
    //done elsewhere (most notably in datamanagement.set_image_name( ... )
    }

void datawidget::from_file(bool f)
    {
    fromFile = f;
    }

bool datawidget::from_file()
    {
    return fromFile;
    }