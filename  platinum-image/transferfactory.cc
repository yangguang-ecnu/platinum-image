//////////////////////////////////////////////////////////////////////////
//
//   Transferfactory $Revision$
///
///  Object factory that makes transfer function objects and the
///  transfer function menu
///
//   $LastChangedBy$
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

#include "datawidget.h"

#include "FLTKutilities.h"
#include "transfer.h"

/*transferswitcher::transferswitcher(int X, int Y, int W, int H, const char *L)
: Fl_Group(X, Y, W, H, L) {
    transferswitcher *o = this;
        { Fl_Menu_Button* o = switchbtn = new Fl_Menu_Button(120, 0, 100, 15, "Default");
        o->labelsize(10);
        }
    end();
    }

void transferswitcher::clear() {
    remove(switchbtn);
    Fl_Group::clear();
    add(switchbtn);
    }

void transferswitcher::resize(int x,int y,int w,int h) {
    Fl_Group::resize(x,y,w,h);
    //switchbtn new size & pos
    }*/

//adding a new transfer function:

//1. add name to tfunction_names
//2. add a corresponding line to Create (...)

const std::string transferfactory::tfunction_names[] =
    {"Default",
    "Brightness/contrast",
    "Labels",
    "Linear",
    "Spline",
    "" };

transferfactory transfer_manufactured::factory = transferfactory();

transferfactory::transferfactory ()
    {
    num_items = 0;

    while  (tfunction_names[num_items] != "")
        { num_items++; }
    }

transferfactory::~transferfactory()
    { }

template <class ELEMTYPE >
transfer_base<ELEMTYPE > * transferfactory::Create(factoryIdType unique_id,image_storage<ELEMTYPE > * s)
    {
    int n = 0;

    //transfer function template constructors, same order as in tfunction_names[]:

    if (unique_id == tfunction_names [n++] )
        {return transfer_default<ELEMTYPE>(s);}

    if (unique_id == tfunction_names [n++] )
        {return transfer_brightnesscontrast<ELEMTYPE>(s);}

    if (unique_id == tfunction_names [n++] )
        {return transfer_mapcolor<ELEMTYPE>(s);}

    if (unique_id == tfunction_names [n++] )
        {return transfer_linear<ELEMTYPE>(s);}

    if (unique_id == tfunction_names [n++] )
        {return transfer_spline<ELEMTYPE>(s);}
    }

Fl_Menu_Item * transferfactory::function_menu (Fl_Callback * cb) //! get menu 
    {
    Fl_Menu_Item * fmenu;

    fmenu = new Fl_Menu_Item [num_items+1];

    for (int m=0; m < num_items; m++)
        {
        init_fl_menu_item(fmenu[m]);

        fmenu[m].label(tfunction_names[m].c_str());
        fmenu[m].callback(cb);
        fmenu[m].argument(m);
        fmenu[m].flags = FL_MENU_RADIO;
        }

    fmenu[num_items].label(NULL);

    return fmenu;
	}