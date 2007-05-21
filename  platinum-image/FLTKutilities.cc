// $Id: $

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

#include "global.h"

#include <cstdlib> //for NULL

#include "FLTKutilities.h"

void init_fl_menu_item(Fl_Menu_Item & m)
    {
    m.label("");
    m.shortcut(0);
    m.callback((Fl_Callback *)NULL,0);
    m.user_data(NULL);
    m.flags= 0;
    m.labeltype(FL_NORMAL_LABEL);
    m.labelfont(0);
    m.labelsize(FLTK_LABEL_SIZE);
    m.labelcolor(FL_BLACK);
    }

horizresizeablescroll::horizresizeablescroll (int x, int y, int w, int h, const char *label) : Fl_Scroll (x,y,w,h,label)
    {
    //children are deleted automatically with parents, so  
    interior=new Fl_Pack(x,y,w-FLTK_SCROLLBAR_SIZE,h);
    type(Fl_Scroll::VERTICAL_ALWAYS);
    interior->type(FL_VERTICAL);
    box(FL_FLAT_BOX);
    interior->box(FL_NO_BOX);
    }

void horizresizeablescroll::begin ()
    {
    interior->begin();
    }

void horizresizeablescroll::end ()
    {
    interior->end();
    Fl_Scroll::end();
    }

void horizresizeablescroll::resize (int newx, int newy, int neww, int newh)
    {
    int scroll_x=interior->x()-x();
    int scroll_y=interior->y()-y();

    Fl_Widget::resize(newx, newy, neww, newh);
    interior->resize(newx+scroll_x,newy+scroll_y,neww-FLTK_SCROLLBAR_SIZE,interior->h());
    }