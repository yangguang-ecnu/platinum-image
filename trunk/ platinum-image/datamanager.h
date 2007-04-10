//////////////////////////////////////////////////////////////////////////
//
//  Datamanager
//
//  The datamanager maintains a list of work data loaded or created at
//  runtime. It has functions for loading and saving data (that invoke
//  functions in the relevant classes)
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

#ifndef __datamanager__
#define __datamanager__

#include <iostream>
#include <fstream>
#include <list>
#include <vector>

#include <FL/Fl_Pack.H>
#include <FL/Fl_File_Chooser.H>

#include "point_base.h"
#include "datawidget.h"
#include "userIOmanager.h"  //for horizresizeablescroll class

#include "global.h"

#include "image_base.h"

enum volumeFileType {UNKNOWN_TYPE=-1, VTK_TYPE, DICOM_TYPE};

class datamanager
    {
    public:
        static const unsigned int MAXVOLUMES = 50;

    protected:
        std::vector<image_base*> volumes;
        std::vector<point_base*> vectors;

        bool closing;                           //whether destructor has been called, meaning
                                                //among other things that all FLTK widgets are
                                                //already dead
        friend class rendercombination;

        horizresizeablescroll *data_widget_box;  //widget that lists the datawidgets.
                                                 //Has to be available for adding and removing entries

        Fl_Menu_Item raw_volume_menu[MAXVOLUMES];   //menu items for loaded volumes,
                                                    //kept updated but without checkmarks

        void rebuild_volume_menu();
        void image_vector_has_changed();

    public:
        datamanager();
        ~datamanager();

        void add_datawidget(datawidget * the_widget);
        void remove_datawidget(datawidget * the_widget);
        void datawidgets_setup();
        void refresh_datawidgets();                 //trigger redraw of widgets when
                                                    //list information has been updated
        void loadvolume_setup();
        void listvolumes();
        bool FLTK_running();        //when datamanager destructor is called,
                                    //the window is closed and all FLTK widgets (including
                                    //the data_widget_box) have been deleted.
                                    //Functions called at this precious moment
                                    //need a way of knowing whether their widgets
                                    //still exist.
                                    //If widget-associated objects (like the datawidget)
                                    //are FLTK subclasses, they are deleted automatically
                                    //and this won't be a problem.


        int first_volume();         //for iterating through volumes: returns ID of first volume in vector
        int next_volume(int index); //for iterating through volumes: returns next ID in volumes[],
                                    //0 if argument is last ID and -1 if not found

        std::string get_volume_name(int ID);
        void set_volume_name(int ID,std::string n);
        static void loadvolume_callback(Fl_Widget *callingwidget, void *thisdatamanager);
        void loadvolumes();
        static void removedata_callback(Fl_Widget *, void *);
        static void save_vtk_volume_callback(Fl_Widget *, void *);

        void add(image_base * v);           //add volume to vector, notify other managers
        void image_has_changed (int volume_ID, bool recalibrate = false);    //signal that contents of a volume has changed,
                                                    //to update thumbnails, display
        //recalibrate determines whether max values etc. are refreshed
        // - doing this may distort the display of changes to the image
        void remove_volume (int id);                //remove volume from vector, notify other managers

        void loadvector_callback();
        int create_empty_volume(int x, int y, int z, int unit);     //creates empty volume of given size
                                                                    //and fills it with testpattern()
                                                                    //returns ID

        int create_empty_volume(image_base *, imageDataType unit);    //create empty volume with same
                                                                    //dimensions as argument
        int find_image_index(int uniqueID);
        image_base * get_image (int ID);

        void parse_and_load_file(char filename[]);
        void parse_identifier(std::ifstream &in, int &identifierindex);

        const Fl_Menu_Item * FLTK_volume_menu_items();
    };

#endif