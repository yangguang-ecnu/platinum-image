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
#include "voxel_tools.h"
#include "Utilities/vxl/contrib/mil3d_trilin_interp_3d.h"
//#include "Utilities/tricubic1.0.0/libtricubic/tricubic.h" (//http://www.lekien.com/~francois/software/tricubic/)
//#include "fcm.h"

#define TRUE 1
#define FALSE 0



template<class ELEMTYPE, int IMAGEDIM = 3>
class image_scalar : public image_general <ELEMTYPE, IMAGEDIM>
{
private:
	//void filter_3d_border_voxel(filter_base* filter, image_scalar<float,3>* copy, int x, int y, int z, int borderflag, image_binary<IMAGEDIM>* mask, int maskflag);
	void filter_3d_border_voxel(filter_base* filter, image_scalar<ELEMTYPE,IMAGEDIM>* copy, int x, int y, int z, int borderflag, image_binary<IMAGEDIM>* mask, int maskflag);

protected:
//    void set_parameters (image_scalar<ELEMTYPE, IMAGEDIM> * from_image);         //clone parameters from another image

public:
    image_scalar(): image_general<ELEMTYPE, IMAGEDIM>(){};
    image_scalar(int w, int h, int d, ELEMTYPE *ptr = NULL):image_general<ELEMTYPE, IMAGEDIM>(w, h, d, ptr) {};
    image_scalar(itk::SmartPointer< itk::OrientedImage<ELEMTYPE, IMAGEDIM > > &i):image_general<ELEMTYPE, IMAGEDIM>(i) {}
    //copy constructor
    template<class SOURCETYPE>
        image_scalar(image_general<SOURCETYPE, IMAGEDIM> * old_image, bool copyData = true): image_general<ELEMTYPE, IMAGEDIM>(old_image, copyData)
            {};

	image_scalar(ELEMTYPE * inData, unsigned long inDataNumElems, long width, long height, Vector3D voxelSize) : image_general<ELEMTYPE, IMAGEDIM>(inData,inDataNumElems, width, height, voxelSize) {}
    //raw constructor
    image_scalar(std::vector<std::string> files, long width, long height, bool bigEndian, long headerSize, Vector3D voxelSize, unsigned int startFile = 1,unsigned int increment = 1) : image_general<ELEMTYPE, IMAGEDIM> (files, width, height, bigEndian, headerSize, voxelSize, startFile,increment){};
    image_scalar(const string filepath, const string name=""):image_general<ELEMTYPE, IMAGEDIM>(filepath, name){}


    //ELEMTYPE get_number_voxel(itk::Vector<int,IMAGEDIM>);
    virtual float get_number_voxel(int x, int y, int z) const;  //the use of virtual makes for example "complex<>" class work...


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
	image_scalar<ELEMTYPE, IMAGEDIM>* get_subvolume_from_slice_3D(int slice, int dir=2);
	image_scalar<ELEMTYPE, IMAGEDIM>* get_subvolume_from_region_3D(Vector3Dint vox_pos, Vector3Dint vox_size);	
	image_scalar<ELEMTYPE, IMAGEDIM>* get_subvolume_from_region_3D(int x1, int y1, int z1, int x2, int y2, int z2);	
	image_scalar<ELEMTYPE, IMAGEDIM>* get_subvolume_from_thresholded_region_3D(ELEMTYPE from_val, ELEMTYPE to_val=std::numeric_limits<ELEMTYPE>::max());

	void crop_3D(image_binary<3> *mask);
	image_scalar<ELEMTYPE, IMAGEDIM>* crop_and_return_3D(image_binary<3> *mask);
	//... get_sub_region(...)

    image_binary<IMAGEDIM> * threshold(ELEMTYPE low, ELEMTYPE high=std::numeric_limits<ELEMTYPE>::max(), IMGBINARYTYPE true_inside_threshold=true); ///Return a image_binary where all voxels with values between low and high gets the value true_inside_threshold.
	void draw_line_2D(int x0, int y0, int x1, int y1, int z, ELEMTYPE value, int direction=2); ///Draw a line between (x0,y0) and (x1,y1) in plane z using color described by value. The coordinates are given on the plane orthogonal to the axis given by direction.
	bool row_sum_threshold(int* res, ELEMTYPE low_thr, ELEMTYPE high_thr, int row_direction=0, int z_direction=2, int first_slice=-1, int last_slice=-1); ///Compute optimal split level for each slice
    void mask_out(image_binary<IMAGEDIM> *mask, IMGBINARYTYPE object_value=TRUE, ELEMTYPE blank=0); ///All voxels in the current image where the corresponding mask voxels != object_value are set to blank.
    void mask_out(int low_x, int high_x, int low_y, int high_y, int low_z, int high_z, ELEMTYPE blank=0); ///All voxels within the given sub-volume are set to blank.
    void mask_out_from_planes_3D(vector<plane3D> planes, ELEMTYPE blank=0);
	void mask_out_from_planes_3D(plane3D plane1, plane3D plane2=plane3D(), plane3D plane3=plane3D(), plane3D plane4=plane3D(), plane3D plane5=plane3D(), ELEMTYPE blank=0);
	std::vector<double> get_slice_sum(int direction=2);

	void flip_voxel_data_3D(int direction);

	ELEMTYPE get_intensity_at_lower_percentile(float percentile, bool ignore_zero_intensity);
//    void save_histogram_to_txt_file(const std::string filename, const std::string separator=";");
    void save_histogram_to_txt_file(const std::string filename, gaussian *g=NULL, bool reload_hist_from_image=false, const std::string separator=";");

    image_scalar<unsigned short, 3>* create2Dhistogram_3D(image_scalar<ELEMTYPE, IMAGEDIM> *second_image, bool remove_zero_intensity=false, int scale_a=-1, int scale_b=-1, image_binary<IMAGEDIM>* mask=NULL); 

	//the resulting histogram volume will have the intensities of first/second in the x/y directions.
	//The z direction will gives the different 2D-histograms in the specified direction "hist_slc_dir"
	image_scalar<ELEMTYPE, IMAGEDIM>* create_slicewise_2Dhistograms_3D(image_scalar<ELEMTYPE, IMAGEDIM> *second_image, int hist_slc_dir=2, bool remove_zero_intensity=false, int scale_a=-1, int scale_b=-1); 

	void smooth_ITK(Vector3D radius); 
	void smooth_3D(Vector3D g); 

	image_binary<IMAGEDIM>* region_grow_corners_3D(ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max());
	image_binary<IMAGEDIM>* region_grow_3D(Vector3D seed, ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max());
	image_binary<IMAGEDIM>* region_grow_3D(image_binary<IMAGEDIM> *seed_image, ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max());
	image_binary<IMAGEDIM>* region_grow_3D(queue<Vector3D> seed_queue, ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max());
	image_binary<IMAGEDIM>* region_grow_3D_if_equal_or_lower_intensity(queue<Vector3D> seed_queue, ELEMTYPE min_intensity=1);
	image_binary<IMAGEDIM>* region_grow_3D_if_lower_intensity(image_binary<IMAGEDIM> *seed_image, ELEMTYPE min_intensity=1);
	image_binary<IMAGEDIM>* region_grow_3D_if_lower_intensity(queue<Vector3D> seed_queue, ELEMTYPE min_intensity=1);
	image_binary<3>* region_grow_3D_using_object_labeling(Vector3D seed, ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max());
	image_binary<3>* region_grow_3D_using_object_labeling(image_binary<IMAGEDIM> *seed_image, ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max());
	image_binary<3>* region_grow_3D_using_object_labeling(queue<Vector3D> seed_queue, ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max());

	//image_binary<IMAGEDIM>* region_grow_robust_3D(Vector3D seed, ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max(), int nr_accepted_neighbours=26, int radius=1);
	void region_grow_robust_in_slice_3D(image_binary<3>* result, Vector2Dint seed, ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max(), int nr_accepted_neighbours=8, int radius=1, int slice=0, int dir=2);
	void region_grow_robust_in_slice_3D(image_binary<3>* result, image_binary<3>* seed_image, ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max(), int nr_accepted_neighbours=8, int radius=1, int slice=0, int dir=2);
	void region_grow_robust_in_slice_3D(image_binary<3>* result, stack<Vector2Dint> seeds, ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max(), int nr_accepted_neighbours=8, int radius=1, int slice=0, int dir=2);
	image_binary<IMAGEDIM>* region_grow_robust_3D(Vector3Dint seed, ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max(), int nr_accepted_neighbours=26, int radius=1);

	Vector3D get_center_of_gravity(ELEMTYPE lower_int_limit, ELEMTYPE upper_int_limit=std::numeric_limits<ELEMTYPE>::max(),  SPACE_TYPE type = VOXEL_SPACE);
	Vector3D get_in_slice_center_of_gravity_in_dir(int dir, int slice, ELEMTYPE lower_int_limit, ELEMTYPE upper_int_limit=std::numeric_limits<ELEMTYPE>::max(),  SPACE_TYPE type = VOXEL_SPACE);
	vector<Vector3D> get_in_slice_center_of_gravities_in_dir(int dir, ELEMTYPE lower_int_limit, ELEMTYPE upper_int_limit=std::numeric_limits<ELEMTYPE>::max(), SPACE_TYPE type = VOXEL_SPACE);
	Vector3D get_pos_of_highest_value_in_region(int x1, int y1, int z1, int x2, int y2, int z2, ELEMTYPE upper_limit=std::numeric_limits<ELEMTYPE>::max());
	Vector3D get_pos_of_highest_value_in_region(Vector3D from_pos, Vector3D to_pos, ELEMTYPE upper_limit=std::numeric_limits<ELEMTYPE>::max());
	Vector3D get_pos_of_highest_value(ELEMTYPE upper_limit=std::numeric_limits<ELEMTYPE>::max());
	Vector3D get_pos_of_lowest_value_in_region(int x1, int y1, int z1, int x2, int y2, int z2, ELEMTYPE lower_limit=std::numeric_limits<ELEMTYPE>::min());
	Vector3D get_pos_of_lowest_value_in_region(Vector3D from_pos, Vector3D to_pos, ELEMTYPE lower_limit=std::numeric_limits<ELEMTYPE>::min());
	Vector3D get_pos_of_lowest_value(ELEMTYPE lower_limit=std::numeric_limits<ELEMTYPE>::min());

	image_scalar<ELEMTYPE, IMAGEDIM>* correct_inclined_object_slicewise_after_cg_line(int dir, line3D cg_line, SPACE_TYPE type = VOXEL_SPACE);


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

	float get_mean_from_slice_3d(int dir, int slice, image_binary<IMAGEDIM>* mask=NULL);

	void logarithm_3d(int zero_handling=0);
	void abs_3d();

	double calculate_entropy_2d();

	void filter_3D(filter_base* filter, int borderflag=0, image_binary<IMAGEDIM>* mask=NULL, int maskflag=0);

// -------------- bias field operations ---------------
	
	void fill_image_with_bias_field_data3D(bias_poly<3> b);


	// --------- image_scalarprocess.hxx ------- (file for application specific implementations) -----

	//------------------- Functions using voxel_set class ----------------------
	voxel_set<ELEMTYPE> get_voxel_set_from_image_data_3D();
	voxel_set<ELEMTYPE> get_voxel_set_from_image_data_3D(ELEMTYPE exclude_value);
//	voxel get_median_voxel_3D(ELEMTYPE *exclude_value=NULL);

	voxel_set<ELEMTYPE> set_val_to_voxel_that_has_no_neighbour_with_val_in_vox_radius(int radius, Vector3Dint pos, ELEMTYPE from_val, ELEMTYPE to_val, ELEMTYPE nb_val);
	voxel_set<ELEMTYPE> set_val_to_voxel_that_has_some_neighbour_with_val_in_vox_radius(int radius, Vector3Dint pos, ELEMTYPE from_val, ELEMTYPE to_val, ELEMTYPE nb_val);


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

	//-----------------------------------------------------------------
	//Functions for segmentation of bodies and lungs form whole-body MRI scans
	//Assumes the feet direction is in increasing voxel-y direction... (nose is in neg Z-direction) 

	//Function for localizing main axis of objects (via 2D center of gravities)... pixels are not weighted by their intensities...
	float appl_wb_correct_inclination(image_scalar<ELEMTYPE, IMAGEDIM>*fat, image_scalar<ELEMTYPE, IMAGEDIM>*water);
	image_binary<3>* appl_wb_segment_body_from_sum_image(int initial_thres=3000);
	image_binary<3>* appl_wb_segment_rough_lung_from_sum_image(image_binary<3> *mask, float lung_volume_in_litres=2.5);
	image_binary<3>* appl_wb_segment_right_lung_from_sum_image(image_binary<3> *right_thorax_body_mask, float lung_volume_in_litres=2.5, int low_threshold=50);
	image_binary<3>* appl_wb_segment_both_lungs_from_sum_image(image_binary<3> *body_mask, float lung_volume_in_litres=2.5, int low_threshold=50);
//	image_binary<3>* appl_wb_segment_lungs_from_sum_image(image_binary<3> *body_mask, float lung_volume_in_litres=5);
	void appl_wb_segment_find_crotch_pos_from_water_percent_image(int &pos_x, int &pos_y, int mip_thres=950);
	image_binary<3>* appl_wb_segment_VAT_mask_from_this_water_percent_abd_subvolume(image_binary<3> *bin_body, string base="");
	void appl_wb_normalize_features_slicewise_by_global_mean_on_this_float (image_scalar<float, 3>* second_feature, image_scalar<float, 3>* sum=NULL, image_binary<3>* body_lung_mask=NULL);
	void appl_wb_SIM_bias_correction_on_this_float(image_scalar<float, 3>* second_feature, int num_iterations=1, float iteration_strength=0.02, float map_x_smoothing_std_dev=60, float map_y_smoothing_std_dev=15, float map_z_smoothing_std_dev=60, float feat1_smoothing_std_dev=30, float feat2_smoothing_std_dev=30, image_binary<3>* body_lung_mask=NULL, int num_buckets_feat1=200, int num_buckets_feat2=200, bool save_corrected_images_each_iteration=false, bool save_histogram_each_iteration=false, bool save_field_each_iteration=false);

	void appl_1D_SIM_bias_correction(image_binary<3>* mask, int num_iterations=1, float iteration_strength=0.02, float map_x_smoothing_std_dev=60, float map_y_smoothing_std_dev=60, float map_z_smoothing_std_dev=6, float feat1_smoothing_std_dev=30, int num_buckets_feat1=200, bool save_corrected_images_each_iteration=false, bool save_histogram_each_iteration=false, bool save_field_each_iteration=false);


	//JK move to private later...
	float get_mean_least_square_difference_to_template_3D(Vector3D pos, image_scalar<ELEMTYPE, IMAGEDIM> *small_template);
	image_scalar<float, IMAGEDIM>* get_mean_least_square_difference_image_3D(Vector3D from_center_pos, Vector3D to_center_pos, image_scalar<ELEMTYPE, IMAGEDIM> *small_template); //least square fit small template to region (center voxel template coordinates...)

};

#endif
