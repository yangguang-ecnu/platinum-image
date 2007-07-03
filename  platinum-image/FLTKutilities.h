//////////////////////////////////////////////////////////////////////////
//
//  FLTKutilities $Revision$
//
//  Helper functions and classes for FLTK usage
//
//  $LastChangedBy$
//

// This file is part of the Platinum library.
// Copyright (c) 2007 Uppsala University.
//
//    The Platinum library is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    The Platinum library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with the Platinum library; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef __FLTKutilities__
#define __FLTKutilities__

#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Pack.H>

void init_fl_menu_item(Fl_Menu_Item & m);

int fl_menu_size (Fl_Menu_Item *);

void fl_menu_userdata_delete (Fl_Menu_Item * m);

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

#endif
