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

        //image parameters
        //TODO: obtain from functions instead of storing
        
        Vector3D origin;
        Matrix3D orientation;

    public:
        virtual image_base * alike (imageDataType unit) = 0;
       
        virtual void initialize_dataset(int w, int h, int d) = 0; // create empty dataset

        template <class LOADERTYPE>
            static void try_loader (std::vector<std::string> * f); //! helper for image_base::load

        static void load(const std::vector<std::string> files);  //load files in supported formats
                                                    //as selected in "files" vector

        virtual void transfer_function(std::string functionName) = 0; //! replace transfer function using string identifier
                    
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

        virtual unsigned short get_size_by_dim(int dim) const = 0;  //return voxel dimensions
        virtual Vector3D get_size () const = 0; //return size in world coordinates
        virtual bool same_size (image_base *) = 0;
        
        bool read_origin_from_dicom_file(std::string dcm_file);
		bool read_orientation_from_dicom_file(std::string dcm_file);
		void rotate(float,float,float);        

        Vector3D world_to_voxel(const Vector3D) const;

        
         virtual void data_has_changed(bool stats_refresh = true) = 0;
        // *** access methods ***

        virtual Matrix3D get_voxel_resize () const = 0;
        virtual const Vector3D get_voxel_size () const = 0;

        
        Matrix3D get_orientation () const;
        Vector3D get_origin () const;
    };


class imageloader
{
protected:
    std::vector<std::string> * files;
public:
    imageloader(std::vector<std::string> * f)
        {
        files = f;
        }
    ~imageloader()
        { }
};

//The dicomloader loads a 
class dicomloader: public imageloader
{
private:
    itk::GDCMImageIO::Pointer dicomIO;
    
	std::vector<std::string> loaded_series; //! UIDs of the DICOM series loaded during this call
                                  //! to prevent multiple selected frames
                                  //! from loading the same series multiple times
public:
    dicomloader (std::vector<std::string> *);
    image_base * read ();
};


#endif