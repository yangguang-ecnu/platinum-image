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

using namespace std;

extern datamanager datamanagement;
extern rendermanager rendermanagement;
extern uchar *animage; //defined in datamanager.cc

const int datawidget::thumbnail_size = 128;

// *** begin FLUID ***

void datawidget::cb_filenamebutton_i(Fl_Input*, void*) {
    datamanagement.set_volume_name(volume_id,string(filenamebutton->value()));
    }
void datawidget::cb_filenamebutton(Fl_Input* o, void* v) {
    ((datawidget*)(o->parent()->parent()->parent()))->cb_filenamebutton_i(o,v);
    }

Fl_Menu_Item datawidget::menu_featuremenu[] = {
 {"Remove", 0,  (Fl_Callback*)datamanager::removedata_callback, (void*)(&datamanagement), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Save as VTK...", 0,  (Fl_Callback*)datamanager::save_vtk_volume_callback, (void*)(&datamanagement), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Duplicate", 0,  0, 0, 1, FL_NORMAL_LABEL, 0, 14, 0},
 {"Transfer function", 0,  (Fl_Callback*)toggle_tfunction, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0}
};
Fl_Menu_Item* datawidget::remove_mi = datawidget::menu_featuremenu + 0;
Fl_Menu_Item* datawidget::save_vtk_mi = datawidget::menu_featuremenu + 1;
Fl_Menu_Item* datawidget::duplicate_mi = datawidget::menu_featuremenu + 2;
Fl_Menu_Item* datawidget::transferfunction_mi = datawidget::menu_featuremenu + 3;

datawidget::datawidget(int datatype,int id, std::string n): Fl_Group(0,0,270,130,NULL) {
    std::cout << "datawidget::datawidget " << n << endl; 
    volume_id = id;
    thumbnail_image = new unsigned char [thumbnail_size*thumbnail_size];

  datawidget *o = this;
o->box(FL_FLAT_BOX);
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
    o->menu(menu_featuremenu);
  }
  resizable(filenamebutton);
  o->end();
}
{ Fl_Box* o = thumbnail = new Fl_Box(0, 25, 270, 65);
  o->hide();
  Fl_Group::current()->resizable(o);
  image( new Fl_RGB_Image(thumbnail_image, thumbnail_size, thumbnail_size, 1));
  image( NULL);
}
{ Fl_Pack* o = extras = new Fl_Pack(0, 90, 270, 40);
  { Fl_Group* o = tfunction_ = new Fl_Group(0, 90, 270, 40);
    o->box(FL_THIN_DOWN_BOX);
    o->labelsize(11);
    o->align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    o->end();
    Fl_Group::current()->resizable(o);
  }
  resizable(this);
  o->end();
}
type(0);
datamanagement.add_datawidget(this);
name(n);
end();
}

// *** end FLUID ***

Fl_Group * datawidget::reset_tf_controls()
    {
    tfunction_->clear();
    return tfunction_;
    }

void datawidget::toggle_tfunction(Fl_Widget* callingwidget, void*)
    {
    datawidget * the_datawidget=(datawidget *)(callingwidget->user_data());

    if (the_datawidget->tfunction_->parent() != NULL)
        {
        the_datawidget->extras->remove(the_datawidget->tfunction_); 
        //the_datawidget->extras->size(the_datawidget->extras->w(),0);
        }
    else
        { 
        the_datawidget->extras->add(the_datawidget->tfunction_);
        //the_datawidget->extras->size(the_datawidget->extras->w(),the_datawidget->tfunction_->h());
        }
    }

datawidget::~datawidget ()
    {
    //this might be executed after window and all is deleted, check for widget existence some way!

    if (datamanagement.FLTK_running())
        {
        hide();   //packer must be hidden before removal (strangely enough)
        //or it will cause an exception
        delete image();
        image(NULL);
        }
    
    datamanagement.remove_datawidget(this);
    delete [] thumbnail;
    }

void datawidget::refresh_thumbnail ()
    {
    rendermanagement.render_thumbnail(thumbnail_image, thumbnail_size, thumbnail_size, volume_id);
    }

int datawidget::get_volume_id()
    {
    return volume_id;
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
    //done elsewhere (most notably in datamanagement.set_volume_name( ... )
    }

//Fl_Group* datawidget::get_widget()
//    {
//    return packer;
//    }