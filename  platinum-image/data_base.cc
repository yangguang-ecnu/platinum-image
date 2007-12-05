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

//#include "data_base.h"

#include "fileutils.h"

#include <sstream>
#include "datawidget.h"

#include "datamanager.h"
extern datamanager datamanagement;

int data_base::data_next_ID = 1;

using namespace std;

data_base::~data_base()
    {
	//cout<<"~data_base()"<<endl;

    //destructor may be called from a menu choice in widget, it would crash if deleted with delete(...)
    //Fl::delete_widget is designed to avoid this

    if (widget != NULL)
        {
		Fl::delete_widget (widget); 
		}
    datamanagement.remove_data (this);
    }


void data_base::name (const string n)
    {
    if (widget != NULL)
        { widget->name(n); }
    else
        {
		pt_error::pt_error ("Attempt to set name(const string) on a widget-less data object",pt_error::warning);
        }
    }

const string data_base::name() const
    {
    if (widget != NULL)
        {
        return widget->name();
        }

    return ( "(untitled)" );
    }

data_base::data_base()
    {
    ID = data_next_ID++;
    widget = NULL;
    from_file(false);

	meta = metadata();
    }

data_base::data_base (data_base * const source)
    {
    pt_error::error_if_null(source,"Attempting to copyconstruct data object from NULL object");

    ID = data_next_ID++;
    widget = NULL;
    from_file(source->from_file());
	meta = source->meta;
    }

void data_base::activate()
{
    //widget->show();
    widget->activate();
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

    else
        {
//        pt_error::error("Attempt to set from_file(bool) on a widget-less data object",pt_error::warning);
        }
    }

void data_base::name_from_path(string file_path)
    {
    name(path_end (file_path));
    
    //named from file, comes from file. Set status accordingly:
    from_file(true);
    }

void data_base::save_histogram_to_txt_file(const std::string filename, const std::string separator)
    {
        pt_error::pt_error ("Attempt to save_histogram_to_txt_file on a data_base object",pt_error::warning);
    }