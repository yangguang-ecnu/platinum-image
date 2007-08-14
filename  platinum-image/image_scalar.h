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

#include "image_general.h"

#include "Utilities/vxl/contrib/mil3d_trilin_interp_3d.h"
//#include "Utilities/tricubic1.0.0/libtricubic/tricubic.h" (//http://www.lekien.com/~francois/software/tricubic/)

#define TRUE 1
#define FALSE 0

template<class ELEMTYPE, int IMAGEDIM = 3>
class image_scalar : public image_general <ELEMTYPE, IMAGEDIM>
{
public:
    image_scalar (): image_general<ELEMTYPE, IMAGEDIM>()
        {};
    image_scalar(int w, int h, int d, ELEMTYPE *ptr = NULL):image_general<ELEMTYPE, IMAGEDIM>(w, h, d, ptr) {};
    image_scalar (itk::SmartPointer< itk::Image<ELEMTYPE, IMAGEDIM > > &i):image_general<ELEMTYPE, IMAGEDIM>(i) {}
    //copy constructor
    template<class SOURCETYPE>
        image_scalar(image_general<SOURCETYPE, IMAGEDIM> * old_image, bool copyData = true): image_general<ELEMTYPE, IMAGEDIM>(old_image, copyData)
            {};

        image_scalar(ELEMTYPE * inData, unsigned long inDataNumElems, long width, long height, Vector3D voxelSize) : image_general<ELEMTYPE, IMAGEDIM>(inData,inDataNumElems, width, height, voxelSize) {}
        //raw constructor
        image_scalar(std::vector<std::string> files, long width, long height, bool bigEndian, long headerSize, Vector3D voxelSize, unsigned int startFile = 1,unsigned int increment = 1) : image_general<ELEMTYPE, IMAGEDIM> (files, width, height, bigEndian, headerSize, voxelSize, startFile,increment)
            {};


//	void interpolate_bilinear_2D(float phys_x, float phys_y, int vox_z);

//	void interpolate_trilinear_3D_ITK(float phys_x, float phys_y, float phys_z); //no boundary checks in "itkLinearInterpolateImageFunction.h" 
	void interpolate_trilinear_3D_vxl(image_scalar<ELEMTYPE, IMAGEDIM > *src_im); //Implementation using the vxl package, alpha-tested


	// Tricubic interpolation using method described in:
	// F. Lekien, J.E. Marsden
	// Tricubic Interpolation in Three Dimensions
	// International Journal for Numerical Methods in Engineering, 63 (3), 455-471, 2005

	//Speed the "interpolate_tricubic_3D" function up by precalculationg the numericla diffs for the whole image.
	image_scalar<double,3>* get_num_diff_image_1storder_central_diff_3D(int direction);	//voxel based (i.e. no real/physical dimensions included),  alpha-tested
	image_scalar<double,3>* get_num_diff_image_2ndorder_central_diff_3D(image_scalar<double,3>*df_dir1, int direction2);	//voxel based (i.e. no real/physical dimensions included) , alpha-tested

	void set_a_coeff2(double a[64], double f[8], double dfdx[8], double dfdy[8], double dfdz[8], double d2fdxdy[8], double d2fdxdz[8], double d2fdydz[8], double d3fdxdydz[8]);
//	void set_a_coeff(double a[64], double f[8], double dfdx[8], double dfdy[8], double dfdz[8], double d2fdxdy[8], double d2fdxdz[8], double d2fdydz[8], double d3fdxdydz[8]);
//	void set_a_coeff_stacked(double a[64], double x[64]);
	double tricubic_eval(double a[64], double x, double y, double z);

	void interpolate_tricubic_3D(image_scalar<ELEMTYPE, IMAGEDIM > *src_im); //Implementation from "libtricubic" package

    image_binary<IMAGEDIM> * threshold(ELEMTYPE low, ELEMTYPE high, IMGBINARYTYPE true_inside_threshold=true); ///Return a image_binary where all voxels with values between low and high gets the value true_inside_threshold.
	void draw_line_2D(int x0, int y0, int x1, int y1, int z, ELEMTYPE value, int direction=2); ///Draw a line between (x0,y0) and (x1,y1) in plane z using color described by value. The coordinates are given on the plane orthogonal to the axis given by direction.
	bool row_sum_threshold(int* res, ELEMTYPE low_thr, ELEMTYPE high_thr, int row_direction=0, int z_direction=2, int first_slice=-1, int last_slice=-1); ///Compute optimal split level for each slice
    void mask_out(image_binary<IMAGEDIM> *mask, IMGBINARYTYPE object_value=TRUE, ELEMTYPE blank=0); ///All voxels in the current image where the corresponding mask voxels != object_value are set to blank.
    void mask_out(int low_x, int high_x, int low_y, int high_y, int low_z, int high_z, ELEMTYPE blank=0); ///All voxels within the given sub-volume are set to blank.
    std::vector<double> get_slice_sum(int direction=2);
    void copy(image_integer<ELEMTYPE, IMAGEDIM> *source, int low_x, int high_x, int low_y, int high_y, int low_z, int high_z, int direction=2); ///Copy subvolume
};

#endif
