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

bool data_base::get_origin_from_dicom_file(std::string dcm_file)
{
	bool succeded = false;
	itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();

	if (dicomIO->CanReadFile(dcm_file.c_str()))
	{
		dicomIO->SetFileName(dcm_file.c_str());
		dicomIO->ReadImageInformation();		//get basic DICOM header
		this->origin[0] = float(dicomIO->GetOrigin(0));
		this->origin[1] = float(dicomIO->GetOrigin(1));
		this->origin[2] = float(dicomIO->GetOrigin(2));
		succeded = true;
	}
	return succeded;
}

bool data_base::get_direction_from_dicom_file(std::string dcm_file)
{
	bool succeded = false;
	itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();

	if (dicomIO->CanReadFile(dcm_file.c_str()))
	{
		dicomIO->SetFileName(dcm_file.c_str());
		dicomIO->ReadImageInformation();		//get basic DICOM header
		std::vector<double> a = dicomIO->GetDirection(0);
		std::vector<double> b = dicomIO->GetDirection(1);
		std::vector<double> c = dicomIO->GetDirection(2);
		this->direction[0][0] = a[0];
		this->direction[0][1] = b[0];
		this->direction[0][2] = c[0];
		this->direction[1][0] = a[1];
		this->direction[1][1] = b[1];
		this->direction[1][2] = c[1];
		this->direction[2][0] = a[2];
		this->direction[2][1] = b[2];
		this->direction[2][2] = c[2];
		succeded = true;
	}
	return succeded;
}

void data_base::rotate(float fi_z,float fi_y,float fi_x)
	{
		matrix_generator mg;
		direction = mg.get_rot_matrix_3D(fi_z,fi_y,fi_x)*direction;
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
