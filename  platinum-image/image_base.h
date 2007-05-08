//////////////////////////////////////////////////////////////////////////
//
//  Untemplated base class for image storage, including volumes
//  contains 3D position and orientation information
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

#ifndef __image_base__
#define __image_base__

#include "data_base.h"

#include "global.h"
#include "color.h"

enum imageDataType
    {
    VOLDATA_UNDEFINED,
    VOLDATA_DOUBLE      = -8,
    VOLDATA_LONG        = -4,
    VOLDATA_SHORT       = -2,
    VOLDATA_CHAR        = -1,
    VOLDATA_BOOL        =  0,
    VOLDATA_UCHAR       =  1,
    VOLDATA_USHORT      =  2,
    VOLDATA_ULONG       =  4
    };


//Take note: the purpose of the base class is to allow access to instances in a
//template-unaware fashion. Hence the base class cannot be templated.

class image_base : public data_base
    {
    public:
        image_base();
        virtual image_base * alike (imageDataType unit) = 0;
       
        // image_base(const image_base &k) { *this=k; ::image_base(); }

        friend std::istream &operator>>(std::istream &in, image_base &k)
            { in >> k.ID; return in; }
        friend std::ostream &operator<<(std::ostream &ut, const image_base &k)
            { ut << "[image_base.ID= " << k.ID << " ]"; return ut; }

        virtual void initialize_dataset(int w, int h, int d)
            = 0; // create empty dataset

        static void load(std::vector<std::string> files);  //load files in supported formats
                                                    //as selected in "files" vector

        //virtual unsigned char get_display_voxel(int x, int y, int z) = 0;      //get unsigned char scaled to data
                                                                        //type range, for display purposes
                    
        virtual void get_display_voxel(RGBvalue &val,int x, int y, int z=0) = 0;
        virtual float get_number_voxel(int x, int y, int z) //get value as float for onscreen display in numbers
            = 0;
                                                                    //when other kinds than 3D volumes are implemented,
                                                                    //one might want to make these dimensionality-independent 
                                                                    //like get_size_by_dim(int dim)
        
        virtual float get_max_float() = 0;    //return max/min values in type-independent form     
        virtual float get_min_float() = 0;
            
        virtual void testpattern() = 0 ;

        virtual void make_image_an_itk_reader() = 0;
        virtual void save_image_to_VTK_file (std::string) = 0;

        virtual unsigned short get_size_by_dim(int dim) = 0;
        virtual bool same_size (image_base *) = 0;

        int get_id()
            { return ID; }

        Vector3D get_unit_to_voxel(Vector3D pos);
        
        virtual Matrix3D get_voxel_resize () = 0;

        virtual void image_has_changed(bool min_max_refresh = false) {};

        Matrix3D unit_to_voxel;    //cached transform from unit space to voxels this volume

        //Volume parameters
        //TODO: obtain from functions instead of storing

        Vector3D unit_center;    //computation aid: vector representing offset from
                                 //bottomleft to centered unit coordinate system
    };

#endif
