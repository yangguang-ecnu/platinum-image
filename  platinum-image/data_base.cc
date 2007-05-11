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

#include "data_base.h"

#include <sstream>

#include "fileutils.h"

using namespace std;

data_base::~data_base()
    {
    //destructor may be called from a menu choice in widget,
    //it would crash if deleted with delete(...)
    //Fl::delete_widget is designed to avoid this

    if (widget != NULL)
        {Fl::delete_widget (widget); }
    }

void data_base::name (const string n)
    {
    if (widget != NULL)
        { widget->name(n); }
#ifdef _DEBUG
    else
        {
        cout << "Attempt to set name(const string) on a widget-less data object" << endl;
        }
#endif
    }

const string data_base::name()
    {
    if (widget != NULL)
        {
        return widget->name();
        }

    return ( "(untitled)" );
    }

data_base::data_base()
    {
    widget = NULL;
    from_file(false);

    origin.Fill(0);
    direction.SetIdentity();
    }

data_base::data_base (data_base * source)
    {
    widget = NULL;
    from_file(source->from_file());

    origin = source->origin;
    direction = source->direction;
    }

bool data_base::from_file()
    {
    if (widget !=NULL)
        { return (widget->from_file()); }
    else
        { return false; }
    }

void data_base::from_file(bool f)
    {
    if (widget !=NULL)
        { widget->from_file(f); }
#ifdef _DEBUG
    else
        {
        cout << "Attempt to set from_file(bool) on a widget-less data object" << endl;
        }
#endif
    }

void data_base::name_from_path(string file_path)
    {
    name(path_end (file_path));
    
    //named from file, comes from file. Set status accordingly:
    from_file(true);
    }
