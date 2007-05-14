//////////////////////////////////////////////////////////////////////////
//
//   UserIOmanager $Revision$
//
///  Maintains a list of userIO blocks, and relays all operations on
///  blocks and their parameters. The interface to this class is
///  independent of the FL toolkit.
//
//   $LastChangedBy$

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

#ifndef __userIOmanager__
#define __userIOmanager__

#include <list>

#include <FL/Fl_Scroll.H>

#include "ptmath.h"
#include "userIO.h"

class horizresizeablescroll : public Fl_Scroll  //Fl_Scroll with integrated packer that resizes horizontally
                                                //(and scrolls/packs vertically)
    {
    public:
        Fl_Pack * interior;                     //the actual list
        horizresizeablescroll(int x, int y, int w, int h, const char *label = 0);
        void resize(int x, int y, int w, int h);
        void begin ();
        void end ();
    };

#include "global.h"

class userIOmanager
    {
    private:
        horizresizeablescroll * widgets_scroll;

        std::list<userIO *> IOblocks;
        userIO * block_from_ID(int IOblockID);

        //void maintenance ();    //called on idle or other time, to cleanup deleted objects and such

    public:
        userIOmanager();
        ~userIOmanager();
        void setup();                               //create FLTK list widget and all
                                                    //puts associated widgets in Fl_Group::current()

        void image_vector_has_changed();    //triggers rebuild of

        //create userIO block, add to manager and return ID
        int add_userIO (std::string name, userIO_callback* cback = NULL,std::string ok_label = "");  
        void show_message (std::string name, std::string message);  //the simple way to pop up a box of information
        
        void finish_userIO(int userIO_ID);         //end group and redraw all widgets, ready to use

        void delete_userIO (int userIO_ID);

        //add parameter to userIO block, return parameter number
        int add_par_float(int userIO_ID, std::string new_param_name,float max=1,float min=0);
        int add_par_int(int userIO_ID, std::string new_param_name,long max=255,long min=0);
        int add_par_int_box(int userIO_ID, std::string new_param_name,long max=255,long min=0);
        int add_par_bool(int userIO_ID, std::string new_param_name,bool init_status = false);

        int add_par_image(int userIO_ID, std::string new_param_name);
        int add_par_histogram_2D(int userIO_ID, std::string new_param_name);
        int add_par_message(int userIO_ID, std::string new_param_name, std::string message);

        template <class ptype>
            ptype get_parameter(int IOblockID, int num);

        std::vector<FLTKuserIOpar_histogram2D *> get_histogram_for_image (int imageID);
    };

template <class ptype>
ptype userIOmanager::get_parameter(int uIO_ID, int par_num)
    {
    ptype parameter;

    parameter = block_from_ID(uIO_ID)->template get_par<ptype>(par_num);

    return parameter;
    }

#endif