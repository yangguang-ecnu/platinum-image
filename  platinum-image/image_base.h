//////////////////////////////////////////////////////////////////////////
//
//  Image_base $Revision $
//
//  Untemplated base class for image storage, including images
//  contains 3D position and orientation information
//
//  $LastChangedBy $
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
    VOLDATA_DOUBLE      = -16,
    VOLDATA_FLOAT       = -8,
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
    private:
        void set_parameters ();
    protected:
        image_base();
        image_base(image_base* const s);

        void redraw();

        // *** cached data ***

        Matrix3D unit_to_voxel_;    //cached transform from unit space to voxels space
									
        //image parameters
        //TODO: obtain from functions instead of storing

        Vector3D unit_center_;    //computation aid: vector representing offset from
        //bottomleft to centered unit coordinate system

    public:
        virtual image_base * alike (imageDataType unit) = 0;
       
        // image_base(const image_base &k) { *this=k; ::image_base(); }

        virtual void initialize_dataset(int w, int h, int d) = 0; // create empty dataset

        template <class LOADERTYPE>
            static void try_loader (std::vector<std::string> * f); //! helper for image_base::load

        static void load(const std::vector<std::string> files);  //load files in supported formats
                                                    //as selected in "files" vector

        virtual void transfer_function(std::string functionName) = 0; //! replace transfer function using string identifier

        //virtual unsigned char get_display_voxel(int x, int y, int z) = 0;      //get unsigned char scaled to data
                                                                        //type range, for display purposes
                    
        virtual void get_display_voxel(RGBvalue &val,int x, int y, int z=0) const = 0;
        virtual float get_number_voxel(int x, int y, int z) const //get value as float for onscreen display in numbers
            = 0;
                                                                    //when other kinds than 3D images are implemented,
                                                                    //one might want to make these dimensionality-independent 
                                                                    //like get_size_by_dim(int dim)
        
        virtual float get_max_float() const = 0;    //return max/min values in type-independent form     
        virtual float get_min_float() const = 0;
            
        virtual void testpattern() = 0 ;

        virtual void make_image_an_itk_reader() = 0;

        virtual unsigned short get_size_by_dim(int dim) const = 0; 
        virtual bool same_size (image_base *) = 0;

        Vector3D transform_unit_to_voxel(Vector3D pos); //convert float unit coords
                                                        //to voxel coords
                                                        //(-1,-1,-1) signifies
                                                        //out of bounds

        
         virtual void image_has_changed(bool stats_refresh = true) = 0;
        // *** access methods ***

        virtual Matrix3D get_voxel_resize () = 0;

        Matrix3D unit_to_voxel()
            {  return unit_to_voxel_; }
        Vector3D unit_center()
            { return unit_center_; }
    };

#endif
