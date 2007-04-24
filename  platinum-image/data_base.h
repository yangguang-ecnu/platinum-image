//////////////////////////////////////////////////////////////////////////
//
//  Untemplated base class for all data objects that are loadable/saveable
//  and may show up in the data list widgets (and others!)
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

#ifndef __data_base__
#define __data_base__

//#include <iostream>

#include "datawidget.h"
#include "ptmath.h"

#include "global.h"

class data_base // We MUST have a virtual base class
    {
    protected:
        int ID;
        datawidget * widget;
        bool fromFile; //indicates whether the data was created inside
        //the program and perhaps needs to be saved;
        //should be indicated by some kind of widget

        void name_from_path(std::string filepath);   //sets image name from name portion of path

    public:       
        virtual ~data_base();

        //virtual const data_base &operator=(const data_base &k) { return k; }
        bool virtual operator<<(const data_base &k)
            { return ID==k.ID; }
        bool virtual operator==(const data_base &k)
            { return ID==k.ID; }
        bool virtual operator==(const int &k)
            { return ID==k; }
        bool virtual operator!=(const data_base &k)
            { return ID!=k.ID; }
        bool virtual operator<(const data_base &k)
            { return ID<k.ID; }
        bool virtual operator>(const data_base &k)
            { return ID>k.ID; }

        Vector3D origin; //make protected, use access methods instead
        Matrix3D direction;
        //virtual data_base * alike () = 0;

        // *** Metadata ***
        virtual void name (const std::string n);          //set name
        virtual const std::string name ();          //get name
        void from_file(bool f); //set "from file" status
    };

#endif
