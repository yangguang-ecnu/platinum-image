//////////////////////////////////////////////////////////////////////////
//
//  Image_binary $Revision$
//
//  Image type with binary voxel/pixel values
//  Implemented like image_label, only that processing expects 0/1 only
//
//  $LastChangedBy$
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

#ifndef __image_binary__
#define __image_binary__

#include "image_integer.h"

template<int IMAGEDIM = 3>
class image_binary : public image_label <IMAGEDIM>
    {
    public:
        image_binary(int w, int h, int d, IMGBINARYTYPE *ptr = NULL);

    template<class SOURCETYPE>
        image_binary(image_general<SOURCETYPE, IMAGEDIM> * old_image, bool copyData = true): image_label<IMAGEDIM>(old_image, copyData)
        { } //!copy constructor

    image_binary (std::vector<std::string> files, long width, long height, bool bigEndian = false, long headerSize = 0, Vector3D voxelSize = Vector3D (1,1,4), unsigned int startFile = 1,unsigned int increment = 1): image_label<IMAGEDIM> (files, width, height, bigEndian, headerSize, voxelSize, startFile,increment) 
        {} //!raw file constructor

    image_binary<IMAGEDIM>(itk::SmartPointer< itk::Image<IMGBINARYTYPE, IMAGEDIM > > &i):
        image_label<IMAGEDIM>(i)
            {} //!ITK image constructor

    // *** operations ***

    image_binary<IMAGEDIM> * logical_or(image_binary<IMAGEDIM> *input, IMGBINARYTYPE object_value=TRUE); ///Perform a voxelwise logical A OR B operation
    image_binary<IMAGEDIM> * logical_and(image_binary<IMAGEDIM> *input, IMGBINARYTYPE object_value=TRUE); ///Perform a voxelwise logical A AND B operation
    image_binary<IMAGEDIM> * logical_xor(image_binary<IMAGEDIM> *input, IMGBINARYTYPE object_value=TRUE); ///Perform a voxelwise logical A XOR B operation
    image_binary<IMAGEDIM> * logical_or_not(image_binary<IMAGEDIM> *input, IMGBINARYTYPE object_value=TRUE); ///Perform a voxelwise logical A OR (NOT B) operation
    image_binary<IMAGEDIM> * logical_and_not(image_binary<IMAGEDIM> *input, IMGBINARYTYPE object_value=TRUE); ///Perform a voxelwise logical A AND (NOT B) operation
    image_binary<IMAGEDIM> * logical_or_prev_and_next(int direction=2, IMGBINARYTYPE object_value=TRUE); ///Perform a voxelwise logical A OR (PrevSlice AND NextSlice) operation
    image_binary<IMAGEDIM> * logical_and_prev_or_next(int direction=2, IMGBINARYTYPE object_value=TRUE); ///Perform a voxelwise logical A OR (PrevSlice AND NextSlice) operation
    void invert(); ///Perform a voxelwise inversion

    // *** applications ***

    // 2D operations in image_binaryprocess
    void fill_holes_2D(int direction=2, IMGBINARYTYPE object_value=TRUE); ///Fill holes in objects defined by object_value in 2D-planes orthogonal to the axis given by direction.
    void largest_object_2D(int direction=2, IMGBINARYTYPE object_value=TRUE); ///Keep the largest object (defined by object_value) for each plane in 2D-planes orthogonal to the axis given by direction.
    void threshold_size_2D(int min_size, int direction=2, IMGBINARYTYPE object_value=TRUE); ///Keep all objects (defined by object_value) > min_size voxels for each plane in 2D-planes orthogonal to the axis given by direction.
    void cog_inside_2D(image_binary<IMAGEDIM>* mask, int direction=2, IMGBINARYTYPE object_value=TRUE); ///Keep all objects (defined by object_value) having their cog within mask in 2D-planes orthogonal to the axis given by direction.
	void erode_2D(int thickness=3, int direction=2, IMGBINARYTYPE object_value=TRUE); ///Morphological erode up to distance value=thickness for each plane in 2D-planes orthogonal to the axis given by direction.
	void dilate_2D(int thickness=3, int direction=2, IMGBINARYTYPE object_value=TRUE); ///Morphological dilate up to distance value=thickness for each plane in 2D-planes orthogonal to the axis given by direction.
	void outline_2D(int thickness=3, int direction=2, IMGBINARYTYPE object_value=TRUE); ///Morphological outline up to distance value=thickness for each plane in 2D-planes orthogonal to the axis given by direction.
    void connect_outer_2D(int direction=2, IMGBINARYTYPE object_value=TRUE); ///Connect outer contour of objects (defined by object_value) for each plane in 2D-planes orthogonal to the axis given by direction.
    image_integer<short, IMAGEDIM> * distance_34_2D(bool edge_is_object=false, int direction=2, IMGBINARYTYPE object_value=TRUE); ///Compute 34 chamfer distance map for each plane in 2D-planes orthogonal to the axis given by direction. If edge_is_object=true then everything outside the image is regarded to be object voxels.

    // 3D operations in image_binaryprocess
    void fill_holes_3D(IMGBINARYTYPE object_value=TRUE); ///Fill holes in objects defined by object_value.
	void largest_object_3D(IMGBINARYTYPE object_value=TRUE); ///Keep the largest object (defined by object_value).
	void erode_3D(int thickness=3, IMGBINARYTYPE object_value=TRUE);///Morphological erode up to distance value=thickness.

	//JK - there is a bug in dilate3D... An in-slice line (with 2 segments...) is sometimes seen....
	void dilate_3D(int thickness=3, IMGBINARYTYPE object_value=TRUE); ///Morphological dilate up to distance value=thickness.
	void outline_3D(int thickness=3, IMGBINARYTYPE object_value=TRUE); ///Morphological outline up to distance value=thickness.
    image_integer<short, IMAGEDIM> * distance_345_3D(bool edge_is_object=false, IMGBINARYTYPE object_value=TRUE); ///Compute 345 chamfer distance map. If edge_is_object=true then everything outside the image is regarded to be object voxels.

	void dilate_3D_26Nbh(IMGBINARYTYPE object_value=TRUE); ///Morphological dilate (26 Neighbourhood)
	int find_voxel_index_percent_object_content(int dir, int object_content_percent, IMGBINARYTYPE object_value=TRUE); 
	void convex_hull_line_filling_3D(int dir, IMGBINARYTYPE object_value=TRUE); 

    };

template <int DIM>
image_binary<DIM>* binary_copycast (image_base* input); //! Converts might-be binary types into image_binary

#endif
