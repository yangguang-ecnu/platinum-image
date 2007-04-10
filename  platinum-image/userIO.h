//////////////////////////////////////////////////////////////////////////
//
//  UserIO
//
//  Class defining an operation on data presented to the user in a list
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

#ifndef __userIO__
#define __userIO__

#include <vector>
#include <exception>

#include <FL/Fl_Button.H>
#include <FL/Fl_Pack.H>

#include "ptmath.h"

#include "FLTKuserIOparameters.h"

#include "global.h"

const unsigned int block_header_height=30;

enum {USERIO_CB_CLOSE=-2, USERIO_CB_OK};

class userIO : public Fl_Pack
    {
    private:
        static const unsigned int main_button_width;
        static int new_uIO_ID;
        std::vector<FLTKuserIOparameter_base *> parameters;

        Fl_Button * close_button,
            * OK_button;
        char name [1024];
        int ID;
        userIO_callback * action_callback;
    public:
        // *** constructors ***
        userIO(int x_init, int y_init, int w_init, int h_init, std::string n,userIO_callback* the_callback, std::string ok_label);   

        // *** overloaded FLTK function ***
        void resize (int newx, int newy, int neww, int newh);

        // *** operators ***
        bool operator==(const userIO &k)
            { return ID==k.ID; }
        bool operator==(const int &k)
            { return ID==k; }
        bool operator!=(const userIO &k)
            { return ID!=k.ID; }

        template <class ptype>
            ptype get_par (int par_num);

        int add_par (FLTKuserIOparameter_base * par);   //add parameter object, return number

        int get_par_num(FLTKuserIOparameter_base * par);

        int id();

        static void FLTK_callback (Fl_Widget *callingwidget, void * foo);

        void image_vector_has_changed();

        std::vector<FLTKuserIOpar_histogram2D *> get_histogram_for_volume(int volumeID); //histogram special
    };

template <class ptype>
ptype userIO::get_par (int par_num)
    {
    ptype value;

    parameters[par_num]->par_value(value);

    return value;
    }

#endif