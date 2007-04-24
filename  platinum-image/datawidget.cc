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

#include <FL/Fl_Pack.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Group.H>

#include "datamanager.h"
#include "rendermanager.h"

using namespace std;

extern datamanager datamanagement;
extern rendermanager rendermanagement;
extern uchar *animage; //defined in datamanager.cc

const int datawidget::thumbnail_size = 128;

const int num_colormenu_items=2;
Fl_Menu_Item colorpopup[] = {
    {"Edit",	FL_ALT+'e'},
    {"Load",	FL_ALT+'l'},
    {0}
    };

const int num_featuremenu_items=3;
Fl_Menu_Item featurepopup[] = {
    {"Remove",	0},
    {"Save as VTK...",	0},
    {"Duplicate",	0},
    {0}
    };

enum {remove_mi_num=0,
    save_mi_num,
    dup_mi_num};

void datawidget::change_name_callback(Fl_Widget *callingwidget, void *thisdatawidget)
{
    datamanagement.set_volume_name(((datawidget *)thisdatawidget)->volume_id,string(((Fl_Input *)callingwidget)->value()));
}

//remove some confusing placeholders
#define USERTEST

datawidget::datawidget (int datatype,int vol_id,std::string n)
    {
    Fl_Widget *featuremenu;

    volume_id=vol_id;

#ifndef USERTEST
    Fl_RGB_Image *thisimage;
    Fl_Widget *priobutton,
        *maxbutton,
        *minbutton,
        *colormenu;
    packer = new Fl_Group(0,0,100,50,"");

    //// min/max...
    //
    priobutton = new Fl_Button(0,0,60,25);
    priobutton->color(FL_YELLOW);

    //// ..and priobutton on the SAME space (we will use only one of them at a time)
    //
    maxbutton = new Fl_Button(0,25,30,25, "max");
    minbutton = new Fl_Button(30,25,30,25, "min");

    //not used right now, deactivate
    maxbutton->deactivate();
    minbutton->deactivate();

#else
    packer = new Fl_Group(0,0,100,25,"");
#endif

    thumbnail = new unsigned char [thumbnail_size*thumbnail_size];

    //thumbnail image, deactivated for now
    packer->image( new Fl_RGB_Image(thumbnail, thumbnail_size, thumbnail_size, 1));
    packer->image( NULL);

    filenamebutton = new Fl_Input(0,0,75,25);
    ((Fl_Input *)filenamebutton)->labeltype(FL_NO_LABEL);
    ((Fl_Input *)filenamebutton)->box(FL_THIN_DOWN_BOX);
    ((Fl_Input *)filenamebutton)->color(FL_BACKGROUND_COLOR); 
    ((Fl_Input *)filenamebutton)->callback(change_name_callback, this);
    ((Fl_Input *)filenamebutton)->when(FL_WHEN_RELEASE);

    name(n);
    filenamebutton->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);

    featuremenu = new Fl_Menu_Button(75,0,25,25,"");
    ((Fl_Menu_Button *)featuremenu)->align(FL_ALIGN_CENTER|FL_ALIGN_CLIP);
    ((Fl_Menu_Button *)featuremenu)->box(FL_THIN_UP_BOX);

    featuremenu->user_data(this);    //reference to this object so callbacks can know what to delete etc.

    ((Fl_Menu_Button *)featuremenu)->copy(featurepopup);
    Fl_Menu_Item* local_featuremenu;

    local_featuremenu=(Fl_Menu_Item*)((Fl_Menu_Button *)featuremenu)->menu();
    local_featuremenu[remove_mi_num].callback(datamanager::removedata_callback,&datamanagement);
    local_featuremenu[save_mi_num].callback(datamanager::save_vtk_volume_callback,&datamanagement);

    //Duplicate is not implemented, deactivate
    local_featuremenu[dup_mi_num].deactivate();

#ifndef USERTEST
    colormenu = new Fl_Menu_Button(60,25,40,25,"Color");
    ((Fl_Menu_Button *)colormenu)->align(FL_ALIGN_BOTTOM|FL_ALIGN_INSIDE|FL_ALIGN_TEXT_OVER_IMAGE|FL_ALIGN_CLIP);
    ((Fl_Menu_Button *)colormenu)->box(FL_THIN_UP_BOX);
    for (int i=0;i < num_colormenu_items; i++)
        {colorpopup[i].deactivate();}
    ((Fl_Menu_Button *)colormenu)->copy(colorpopup);

    thisimage = new Fl_RGB_Image(animage, 40, 25, RGBApixmap_bytesperpixel);
    colormenu->image(thisimage);
#endif

    packer->end();

    packer->resizable(filenamebutton);

    datamanagement.add_datawidget(this);
    }

datawidget::~datawidget ()
    {
    //this might be executed after window and all is deleted, check for widget existence some way!

    if (datamanagement.FLTK_running())
        {
        packer->hide();   //packer must be hidden before removal (strangely enough)
        //or it will cause an exception
        delete packer->image();
        packer->image(NULL);
        }
    
    datamanagement.remove_datawidget(this);
    delete [] thumbnail;
    }

void datawidget::refresh_thumbnail ()
    {
    rendermanagement.render_thumbnail(thumbnail, thumbnail_size, thumbnail_size, volume_id);
    }

int datawidget::get_volume_id()
    {
    return volume_id;
    }

const string datawidget::name()
    {
    return _name;
    }

void datawidget::name(std::string n)
    {
    _name = n;

    ((Fl_Input *)filenamebutton)->value(NULL);
    ((Fl_Input *)filenamebutton)->value(n.c_str());

    //when interactively changed, redrawing widget is
    //done elsewhere (most notably in datamanagement.set_volume_name( ... )
    }

Fl_Group* datawidget::get_widget()
    {
    return packer;
    }