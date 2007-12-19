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

#include "itkVTKImageIO.h"
//#include "bruker.h"

#include "itkMetaDataDictionary.h"	//used in dicomloader::
#include "itkMetaDataObject.h"		//used in dicomloader::
#include "itkGDCMImageIO.h"			//used in dicomloader::
#include "gdcmFileHelper.h"			//used in dicomloader::

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
		//Note: Vector3D voxel_size is defined in image_general...



    public:
        virtual image_base * alike (imageDataType unit) = 0;
       
        virtual void initialize_dataset(int w, int h, int d) = 0; // create empty dataset

        template <class LOADERTYPE>
            static void try_loader (std::vector<std::string> * f); //! helper for image_base::load
        static void load(const std::vector<std::string> files);  //load files in supported formats
                                                    //as selected in "files" vector

        virtual void transfer_function(std::string functionName) = 0; //! replace transfer function using string identifier
                    
        virtual void get_display_voxel(RGBvalue &val,int x, int y, int z=0) const = 0;
        virtual float get_number_voxel(int x, int y, int z) const = 0;//get value as float for onscreen display in numbers
                                                                    //when other kinds than 3D images are implemented,
                                                                    //one might want to make these dimensionality-independent 
                                                                    //like get_size_by_dim(int dim)
        
        virtual float get_max_float() const = 0;    //return max/min values in type-independent form     
//        virtual float get_min_float() const = 0;
            
        virtual void testpattern() = 0 ;

        virtual void make_image_an_itk_reader() = 0;

        virtual unsigned short get_size_by_dim(int dim) const = 0;  //return voxel dimensions
        virtual Vector3D get_physical_size () const = 0; //return size in world coordinates
        virtual bool same_size (image_base *) = 0;
        
        bool read_origin_from_dicom_file(std::string dcm_file);
		bool read_orientation_from_dicom_file(std::string dcm_file);
		void rotate(float,float,float);        

        Vector3D world_to_voxel( const Vector3D & wpos ) const;
		Vector3D voxel_to_world( const Vector3D & vpos ) const;

        
        virtual void data_has_changed(bool stats_refresh = true) = 0;
        // *** access methods ***

        virtual Matrix3D get_voxel_resize () const = 0;
        virtual const Vector3D get_voxel_size () const = 0;
        virtual void set_voxel_size(const Vector3D v) = 0;
        
        Matrix3D get_orientation () const;
		string get_orientation_as_dcm_string(); //print first two columns (cosines for x and y direction)
		void set_orientation(const Matrix3D m);
        Vector3D get_origin() const;
        void set_origin(const Vector3D v);

		void save_histogram_to_txt_file(const std::string filename, const std::string separator=";");

    };


class imageloader
{
protected:
    std::vector<std::string> * files;
public:
    imageloader(std::vector<std::string> * f);
    ~imageloader();
};


class dicomloader: public imageloader
{
private:
	DICOM_LOADER_TYPE this_load_type;
    itk::GDCMImageIO::Pointer dicomIO;
	std::vector<std::string> loaded_series; //! UIDs of the DICOM series loaded during this call
                         //! to prevent multiple selected frames from loading the same series multiple times
public:
    dicomloader(std::vector<std::string> *f);
    dicomloader(std::vector<std::string> *f, DICOM_LOADER_TYPE type);
    image_base * read();



// Remember that modifying the header entries of a DICOM file involves
// very serious risks for patients and therefore must be done with extreme caution.

//JK-Warning... These functions changes the series id. (0020,000e)...
//It is also problematic to pass on the orientation (when saving a file) using the gdcm package...

//	static void anonymize_single_dcm_file(string load_path, string save_path); 
//	static void anonymize_all_dcm_files_in_folder(string load_folder_path, string save_folder_path);
//	static void anonymize_all_dcm_files_in_folder2(string load_folder_path, string save_folder_path);
};


class vtkloader: public imageloader
{
private:
    itk::VTKImageIO::Pointer vtkIO;
    
public:
    vtkloader (std::vector<std::string> *);
    image_base * read ();
};

class analyze_objloader: public imageloader 
{
public:
    analyze_objloader (std::vector<std::string> *);
    image_base * read ();
};

class analyze_hdrloader: public imageloader 
{
private:
	int buf2int(unsigned char* buf);
	short buf2short(unsigned char* buf);
public:
    analyze_hdrloader (std::vector<std::string> *);
    image_base * read ();
};


#endif