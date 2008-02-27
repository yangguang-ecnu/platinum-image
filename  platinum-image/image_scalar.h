//////////////////////////////////////////////////////////////////////////
//
//   Image_scalar $Revision$
///
///  Image type with scalar voxel/pixel values
///  Mostly the same as image_general
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

#ifndef __image_scalar__
#define __image_scalar__

#include <stack>
#include <queue>
#include "image_general.h"
#include "bias_field.h"
#include "filters.h"
#include "Utilities/vxl/contrib/mil3d_trilin_interp_3d.h"
//#include "Utilities/tricubic1.0.0/libtricubic/tricubic.h" (//http://www.lekien.com/~francois/software/tricubic/)
//#include "fcm.h"

#define TRUE 1
#define FALSE 0



template<class ELEMTYPE, int IMAGEDIM = 3>
class image_scalar : public image_general <ELEMTYPE, IMAGEDIM>
{
public:
    image_scalar (): image_general<ELEMTYPE, IMAGEDIM>()
        {};
    image_scalar(int w, int h, int d, ELEMTYPE *ptr = NULL):image_general<ELEMTYPE, IMAGEDIM>(w, h, d, ptr) {};
    image_scalar (itk::SmartPointer< itk::OrientedImage<ELEMTYPE, IMAGEDIM > > &i):image_general<ELEMTYPE, IMAGEDIM>(i) {}
    //copy constructor
    template<class SOURCETYPE>
        image_scalar(image_general<SOURCETYPE, IMAGEDIM> * old_image, bool copyData = true): image_general<ELEMTYPE, IMAGEDIM>(old_image, copyData)
            {};

	image_scalar(ELEMTYPE * inData, unsigned long inDataNumElems, long width, long height, Vector3D voxelSize) : image_general<ELEMTYPE, IMAGEDIM>(inData,inDataNumElems, width, height, voxelSize) {}
    //raw constructor
    image_scalar(std::vector<std::string> files, long width, long height, bool bigEndian, long headerSize, Vector3D voxelSize, unsigned int startFile = 1,unsigned int increment = 1) : image_general<ELEMTYPE, IMAGEDIM> (files, width, height, bigEndian, headerSize, voxelSize, startFile,increment){};
    image_scalar(const string filepath, const string name=""):image_general<ELEMTYPE, IMAGEDIM>(filepath, name){}


	//------------------------- Interpolations -------------------------
	//Implementation "inspired" by "ResampleImageFilter7" example in the ITK documentation
	void interpolate_spline_ITK_3D(image_scalar<ELEMTYPE, IMAGEDIM > *ref_im, int spline_order=3); 


//	void interpolate_bilinear_2D(float phys_x, float phys_y, int vox_z);
//	void interpolate_trilinear_3D_ITK(float phys_x, float phys_y, float phys_z); //no boundary checks in "itkLinearInterpolateImageFunction.h" 
	void interpolate_trilinear_3D_vxl(image_scalar<ELEMTYPE, IMAGEDIM > *src_im); //Implementation using the vxl package, alpha-tested

	void resample_with_spline_interpolation_3D(int newxsize, int newysize, int newzsize, int spline_order=3);

	// Tricubic interpolation using method described in:
	// F. Lekien, J.E. Marsden
	// Tricubic Interpolation in Three Dimensions
	// International Journal for Numerical Methods in Engineering, 63 (3), 455-471, 2005
	void interpolate_tricubic_3D(image_scalar<ELEMTYPE, IMAGEDIM > *src_im); //Implementation from "libtricubic" package

	//Speed the "interpolate_tricubic_3D" function up by precalculationg the numericla diffs for the whole image.
	image_scalar<double,3>* get_num_diff_image_1storder_central_diff_3D(int direction);	//voxel based (i.e. no real/physical dimensions included),  alpha-tested
	image_scalar<double,3>* get_num_diff_image_2ndorder_central_diff_3D(image_scalar<double,3>*df_dir1, int direction2);	//voxel based (i.e. no real/physical dimensions included) , alpha-tested

	void set_a_coeff2(double a[64], double f[8], double dfdx[8], double dfdy[8], double dfdz[8], double d2fdxdy[8], double d2fdxdz[8], double d2fdydz[8], double d3fdxdydz[8]);
//	void set_a_coeff(double a[64], double f[8], double dfdx[8], double dfdy[8], double dfdz[8], double d2fdxdy[8], double d2fdxdz[8], double d2fdydz[8], double d3fdxdydz[8]);
//	void set_a_coeff_stacked(double a[64], double x[64]);
	double tricubic_eval(double a[64], double x, double y, double z);



	//------------------------- Various Operations -------------------------

    image_binary<IMAGEDIM> * threshold(ELEMTYPE low, ELEMTYPE high=std::numeric_limits<ELEMTYPE>::max(), IMGBINARYTYPE true_inside_threshold=true); ///Return a image_binary where all voxels with values between low and high gets the value true_inside_threshold.
	void draw_line_2D(int x0, int y0, int x1, int y1, int z, ELEMTYPE value, int direction=2); ///Draw a line between (x0,y0) and (x1,y1) in plane z using color described by value. The coordinates are given on the plane orthogonal to the axis given by direction.
	bool row_sum_threshold(int* res, ELEMTYPE low_thr, ELEMTYPE high_thr, int row_direction=0, int z_direction=2, int first_slice=-1, int last_slice=-1); ///Compute optimal split level for each slice
    void mask_out(image_binary<IMAGEDIM> *mask, IMGBINARYTYPE object_value=TRUE, ELEMTYPE blank=0); ///All voxels in the current image where the corresponding mask voxels != object_value are set to blank.
    void mask_out(int low_x, int high_x, int low_y, int high_y, int low_z, int high_z, ELEMTYPE blank=0); ///All voxels within the given sub-volume are set to blank.
    std::vector<double> get_slice_sum(int direction=2);

	void flip_voxel_data_3D(int direction);

	ELEMTYPE get_intensity_at_lower_percentile(float percentile, bool ignore_zero_intensity);
    void save_histogram_to_txt_file(const std::string filename, const std::string separator=";");
    void save_histogram_to_txt_file(const std::string filename, bool reload_hist_from_image=true, gaussian *g=NULL, const std::string separator=";");
//    void save_histogram_to_txt_file2(const std::string filename, bool reload_hist_from_image=true, const std::string separator=";");

    image_scalar<ELEMTYPE, IMAGEDIM>* create2Dhistogram_3D(image_scalar<ELEMTYPE, IMAGEDIM> *second_image, bool remove_zero_intensity=false, int scale_a=-1, int scale_b=-1, image_binary<IMAGEDIM>* mask=NULL); 

	//the resulting histogram volume will have the intensities of first/second in the x/y directions.
	//The z direction will gives the different 2D-histograms in the specified direction "hist_slc_dir"
	image_scalar<ELEMTYPE, IMAGEDIM>* create_slicewise_2Dhistograms_3D(image_scalar<ELEMTYPE, IMAGEDIM> *second_image, int hist_slc_dir=2, bool remove_zero_intensity=false, int scale_a=-1, int scale_b=-1); 

	void smooth_ITK(Vector3D radius); 
	void smooth_3D(Vector3D g); 

	image_binary<IMAGEDIM>* region_grow_3D(Vector3D seed, ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max());
	image_binary<IMAGEDIM>* region_grow_3D(image_binary<IMAGEDIM> *seed_image, ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max());
	image_binary<IMAGEDIM>* region_grow_3D(queue<Vector3D> seed_queue, ELEMTYPE min_intensity, ELEMTYPE max_intensity);
	
	image_binary<IMAGEDIM>* region_grow_robust_3D(Vector3D seed, ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max(), int nr_accepted_neighbours=26, int radius=1);

	//------------------- Filter functions ----------------------
	image_scalar<ELEMTYPE, IMAGEDIM>* get_gradmagn_filter3D_image_3D(Vector3D from, Vector3D to, GRAD_MAG_TYPE type = XY);

//	void medianFilter2D();
//	void meanFilter();
//	void discreteGaussFilter(double gaussianVariance,int maxKernelWidth);
//	void gradientFilter();
//	void gradientFilter2D();
//	itk::MeanImageFilter<theImageType,theImageType>::Pointer filter = itk::MeanImageFilter<theImageType,theImageType>::New();

	//TODO: to implement different PROJ_MODES and to return a 2D Image...
	image_scalar<ELEMTYPE, 3>* create_projection_3D(int dir, PROJECTION_MODE PROJ=PROJ_MAX); //enum PROJECTION_MODE {PROJ_MEAN, PROJ_MAX}; 
	

	//Calculates TruePositive, FalsePositive values and "Udupa-index" for voxels larger than zero...
	void calculate_TP_FP_Udupa_3D(float &tp, float &fp, float &udupa, image_scalar<ELEMTYPE, IMAGEDIM>* ground_truth, ELEMTYPE gt_obj_val=1, ELEMTYPE this_obj_val=1 ); 

	// return the voxel position of POINT_TYPE (i.e. max gradient magnitude, max value, ...) using voxel position and voxel radius
	Vector3D get_pos_of_type_in_region_voxel ( Vector3D center, Vector3D radius, POINT_TYPE point_type );
	
	Vector3D get_pos_of_max_grad_mag_in_region_voxel ( Vector3D center, Vector3D radius, GRAD_MAG_TYPE type );

	void scale_slice_by_factor_3d(int dir, float factor, int slice);

	float get_mean_from_slice_3d(int dir, int slice, ELEMTYPE low_thres, ELEMTYPE high_thres);

	float calculate_entropy_2d();

	void filter_3D(filter_base* filter, int borderflag=0, image_binary<IMAGEDIM>* mask=NULL, int maskflag=0);

// -------------- bias field operations ---------------
	
	void fill_image_with_bias_field_data3D(bias_poly<3> b);


	// --------- image_scalarprocess.hxx ------- (file for application specific implementations) -----

	// See description in: Kullberg2006 - J Magn Reson Imaging. 2006 Aug;24(2):394-401.
	// Whole-body T1 mapping improves the definition of adipose tissue: consequences for automated image analysis.
	// Usage: large_flip->calc...(small_flip) 
	// Meta data required: TR, Flip_angle
	// Note that function can be moved to image_storage...
    image_scalar<ELEMTYPE, IMAGEDIM>* calculate_T1Map_from_two_flip_angle_MRvolumes_3D(image_scalar<ELEMTYPE, IMAGEDIM > *small_flip, float body_thres=0, float t1_min=0, float t1_max=2000); 
    image_scalar<ELEMTYPE, IMAGEDIM>* calculate_T1Map_3D(vector<image_scalar<ELEMTYPE, IMAGEDIM > *> v, float body_thres=0, float t1_min=0, float t1_max=2000); 

	// return the gradient magnitude using voxel position
	float grad_mag_voxel ( Vector3D point, GRAD_MAG_TYPE type );
	float grad_mag_voxel ( int x, int y, int z, GRAD_MAG_TYPE type );
	
	float weight_of_type( Vector3D center, Vector3D current, WEIGHT_TYPE type );

	//Simple functions for segmentation of bodies and lungs form whole-body MRI scans
	//Assumes the feet direction is in increasing voxel-y direction... (nose is in neg Z-direction) 
	image_binary<3>* appl_wb_segment_body_from_sum_image(int initial_thres);
	image_binary<3>* appl_wb_segment_lungs_from_sum_image(int initial_upper_thres, image_binary<3> *body_mask);
	void appl_wb_segment_find_crotch_pos_from_water_percent_image(int &pos_x, int &pos_y, int mip_thres=950);
	image_binary<3>* appl_wb_segment_VAT_mask_from_this_water_percent_abd_subvolume(image_binary<3> *bin_body);

	//JK move to private later....
	float get_mean_least_square_difference_to_template_3D(Vector3D pos, image_scalar<ELEMTYPE, IMAGEDIM> *small_template);
	image_scalar<float, IMAGEDIM>* get_mean_least_square_difference_image_3D(Vector3D from_pos, Vector3D to_pos, image_scalar<ELEMTYPE, IMAGEDIM> *small_template);

private:
	void filter_3d_border_voxel(filter_base* filter, image_scalar<float,3>* copy, int x, int y, int z, int borderflag, image_binary<IMAGEDIM>* mask, int maskflag);

};


#endif