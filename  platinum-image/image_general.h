//////////////////////////////////////////////////////////////////////////
//
//   Image_general $Revision$
///
///  Abstract base class for the various image classes, templated over
///  dimension and voxel data type
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

#ifndef __image_general__
#define __image_general__

#define PI 3.1415926536
#include "image_storage.h"

//forward declarations, needed with GCC for unknown reasons
template<class ELEMTYPE, int IMAGEDIM>
    class image_integer;
template<class ELEMTYPE, int IMAGEDIM>
    class image_scalar; 
template<int IMAGEDIM>
    class image_binary;
template<int IMAGEDIM>
    class image_label;
template<class ELEMTYPE, int IMAGEDIM>
    class image_multi; 

#include <string>
#include <vector>
#include <iterator>

#include "itkImage.h"
#include "itkImportImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "global.h"
#include "color.h"

template<class ELEMTYPE, int IMAGEDIM = 3>
class image_general : public image_storage <ELEMTYPE >
    {
    protected:
        image_general<ELEMTYPE, IMAGEDIM>(int w, int h, int d, ELEMTYPE *ptr = NULL);

        unsigned short datasize[IMAGEDIM]; //image size
        
        Vector3D voxel_size;	//3D voxel size in mm (hot tip: use z = 0 for 2D)
        
        typename itk::ImportImageFilter<ELEMTYPE, IMAGEDIM>::Pointer   ITKimportfilter;
        typename itk::Image<ELEMTYPE, IMAGEDIM >::Pointer                ITKimportimage;
        
        //typename itk::ImageFileWriter<theImageType>::Pointer       ITKwriterfilter; //get the writer to write in this here array
        
        // *** Constructors & factories ***
        image_general<ELEMTYPE, IMAGEDIM>();
        image_general<ELEMTYPE, IMAGEDIM>(itk::SmartPointer< itk::Image<ELEMTYPE, IMAGEDIM > > &i);
        template<class SOURCETYPE> 
            image_general(image_general<SOURCETYPE, IMAGEDIM> * old_image, bool copyData = true);

        void set_parameters ();                                                     //reset & calculate parameters
        void set_parameters (itk::SmartPointer< itk::Image<ELEMTYPE, IMAGEDIM > > &i);   //set parameters from ITK metadata
        template <class sourceType>
            void set_parameters (image_general<sourceType, IMAGEDIM> * from_image);         //clone parameters from another image
        
        void calc_transforms (); //used by set_parameters(...) functions
                                 //to recalculate cached transform(s)

    public:                                                    
        image_base * alike (imageDataType);

        image_general(ELEMTYPE * inData, unsigned long inDataNumElems, long width, long height, Vector3D voxelSize);
        //create image from pre-loaded raw data

        image_general (std::vector<std::string>, long width, long height, bool bigEndian = false, long headerSize = 0, Vector3D voxelSize = Vector3D (1,1,4), unsigned int startFile = 1,unsigned int increment = 1);
        //create image from file set. Notes:
        //A. uses POSIX paths ( / for directories) on all platforms - inlcuding Windows
        //B. Three modes of operation:
        //   1. single file - get multiple slices as specified in arguments
        //   2. multiple files in vector - each file contains one slice
        //   3. multiple files in directory, specified by path in first (only)
        //      element of vector. Reads all files (directories ignored) in argument dir. 

        void initialize_dataset(int w, int h, int d);                           //overloading from image_base
        void initialize_dataset(int w, int h, int d, ELEMTYPE *ptr);          //load might happen outside class

        //initialize image from ITK image
        void replicate_itk_to_image();     //use object's own ITK image pointer
        void replicate_itk_to_image(itk::SmartPointer< itk::Image<ELEMTYPE, IMAGEDIM > > &i);

        virtual void data_has_changed(bool stats_refresh = true);          //called when image data has been changed

        /*static image_base * type_from_DICOM_file (std::string file_path);
        static image_base * type_from_VTK_file (std::string file_path);*/

		void set_voxel_size(float dx, float dy, float dz=0);			//physical voxel size
		bool read_voxel_size_from_dicom_file(std::string dcm_file);	//physical voxel size	

		bool is_voxelpos_within_image_3D(int vp_x, int vp_y, int vp_z);  
		bool is_voxelpos_within_image_3D(Vector3D vp);  
		bool is_voxelpos_inside_image_border_3D(int vp_x, int vp_y, int vp_z, int dist=1);
		bool is_voxelpos_inside_image_border_3D(Vector3D vp, int dist=1);  
		bool is_physical_pos_within_image_3D(Vector3D phys_pos);
		Vector3D get_voxelpos_from_physical_pos_3D(Vector3D phys_pos);	//no truncation is performed in this function.
		Vector3D get_voxelpos_integers_from_physical_pos_3D(Vector3D phys_pos);	//truncation is performed in this function.

        // *** element access methods ***
        ELEMTYPE get_voxel(int x, int y, int z=0) const;
        ELEMTYPE get_voxel_in_physical_pos(Vector3D phys_pos);  
        ELEMTYPE get_voxel_in_physical_pos_mean_3D_interp26(Vector3D phys_pos);  
		ELEMTYPE get_voxel_in_physical_pos_26NB_weighted(Vector3D phys_pos, float w1, float w2, float w3, float w4);
        //ELEMTYPE get_voxel(unsigned long offset); //deprecated: use iterator!

		Vector3D get_physical_pos_for_voxel(int x, int y, int z);

        RGBvalue get_display_voxel(itk::Vector<int,IMAGEDIM>) const;
        virtual void get_display_voxel(RGBvalue &val,int x, int y, int z=0) const;

        ELEMTYPE get_voxel_by_dir(int u, int v, int w, int direction=2);

        //ELEMTYPE get_number_voxel(itk::Vector<int,IMAGEDIM>);
        float get_number_voxel(int x, int y, int z) const;

        void set_voxel(int x, int y, int z, ELEMTYPE voxelvalue);
		void set_voxel_by_dir(int u, int v, int w, ELEMTYPE value, int direction=2);
		void fill_region_3D(int x, int y, int z, int dx, int dy, int dz, ELEMTYPE value);

        void give_parametersXYplane(int renderstartX, int renderstartY, int renderwidth, int renderheight, int &startoffset, int &patchXoffset );
        void testpattern();
        
        // *** size functions ***
        unsigned short get_size_by_dim(int dim) const;
        unsigned short get_size_by_dim_and_dir(int dim, int direction); //! get size in direction orthogonal to direction arg
        
        bool same_size (image_base * other);				//test whether other image has same voxel dimensions
        bool same_size (image_base * other, int direction); //test whether other image has same voxel dimensions
        
        Vector3D get_size () const;
        
        const Vector3D get_voxel_size () const;       //return voxel size
        Matrix3D get_voxel_resize () const;           //return voxel size as matrix


		//****** Sub volume operations ********
		image_general<ELEMTYPE, IMAGEDIM>* get_subvolume_from_slices_3D(int start_slice, int every_no_slice, int slice_dir=2);	
		void copy_slice_from_3D(image_general<ELEMTYPE, IMAGEDIM> *src, int from_slice_no, int to_slice_no, int slice_dir=2);
	
		void add_slice_from_3D(image_general<ELEMTYPE, IMAGEDIM> *src, int from_slice_no, int slice_dir=2);
		//adds image slice (currently only of correct size...) in positive x/y/z directions... 
		//A temporary image is needed, therefor, this cannot be implemented in "image_general"



        void make_image_an_itk_reader();               //initialize ITKimportfilter

        //return ITKimportfilter
        typename itk::ImportImageFilter< ELEMTYPE, IMAGEDIM >::Pointer   itk_import_filter();

        //return ITK image from image     
        typename itk::Image<ELEMTYPE, IMAGEDIM >::Pointer                itk_image();

        void load_dataset_from_VTK_file(std::string file_path);
        void load_dataset_from_DICOM_files(std::string dir_path,std::string seriesIdentifier);

        void save_to_VTK_file(const std::string file_path);

		void set_geometry(float ox,float oy,float oz,float dx,float dy,float dz,float fi_x,float fi_y,float fi_z);
        bool read_geometry_from_dicom_file(std::string dcm_file);
        void print_geometry();
        void print_physical_corner_coords();
        
        // *** processing ***
        template<class TARGETTYPE>	//Nearest neighbour image re-sampling-test
            void resample_into_this_image_NN(image_general<TARGETTYPE, 3> * new_image);
        double get_num_diff_1storder_central_diff_3D(int x, int y, int z, int direction);	//voxel based (i.e. no real dimensions included)
		double get_num_diff_2ndorder_central_diff_3D(int x, int y, int z, int direction1, int direction2);	//voxel based (i.e. no real dimensions included)
		double get_num_diff_3rdorder_central_diff_3D(int x, int y, int z, int direction1, int direction2, int direction3); //voxel based (i.e. no real dimensions included)
        void filter_image_slw_mean_4NB_3D();
};

template <template <class,int=3 > class IMGCLASS>
image_base* allocate_image (bool floatType, bool signedType, unsigned int voxel_type, std::vector<std::string> files, long width, long height, bool bigEndian, long headerSize, Vector3D voxelSize);

template <template <class, int> class requestedClass, class ELEM, int DIM>
requestedClass<ELEM, DIM>* scalar_copycast (image_base* input);

#endif
