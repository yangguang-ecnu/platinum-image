//////////////////////////////////////////////////////////////////////////
//
//   Data_base $Revision$
///
///  Untemplated base class for all data objects that are loadable/saveable
///  and may show up in the data list widgets (and others!)
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

#ifndef __data_base__
#define __data_base__

//#include <iostream>

#include "datawidget.h"
#include "ptmath.h"

#include "global.h"
#include "itkGDCMImageIO.h"

class data_base // We MUST have a virtual base class
    {
    private:
        void set_parameters();
    protected:
        int ID;
        datawidget * widget;

        void name_from_path(std::string filepath);   //sets image name from name portion of path
    
        data_base ();
        data_base (data_base *);
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

		bool get_origin_from_dicom_file(std::string dcm_file);
		bool get_direction_from_dicom_file(std::string dcm_file);
		void rotate(float fi_z,float fi_y,float fi_x);

        //virtual data_base * alike () = 0;

        // *** Metadata ***
        virtual void name (const std::string n);          //set name
        virtual const std::string name ();          //get name
        bool from_file();
        void from_file(bool f); //set "from file" status
    };

#endif
