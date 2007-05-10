//////////////////////////////////////////////////////////////////////////
//
//  Image_binary $Revision$
//
//  Image type with binary voxel/pixel values
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
class image_binary : public image_integer <bool, IMAGEDIM>
    {
    public:
        image_binary ();

        image_binary(int w, int h, int d, bool *ptr = NULL);

    template<class SOURCETYPE>
        image_binary(image_general<SOURCETYPE, IMAGEDIM> * old_volume, bool copyData = true): image_integer<bool, IMAGEDIM>(old_volume, copyData)
        {} //copy constructor

    image_binary (std::vector<std::string> files, long width, long height, bool bigEndian = false, long headerSize = 0, Vector3D voxelSize = Vector3D (1,1,4), unsigned int startFile = 1,unsigned int increment = 1): image_integer<bool, IMAGEDIM> (files, width, height, bigEndian, headerSize, voxelSize, startFile,increment) {}

    virtual void transfer_function(transfer_base<bool > * t);

    // *** operations ***

    image_binary<IMAGEDIM> * logical_or(image_binary<IMAGEDIM> *input, bool object_value=true);
    image_binary<IMAGEDIM> * logical_and(image_binary<IMAGEDIM> *input, bool object_value=true);
    image_binary<IMAGEDIM> * logical_xor(image_binary<IMAGEDIM> *input, bool object_value=true);

    // *** applications ***

    // 2D operations in image_binaryprocess
    void fill_holes_2D(int direction=2, bool object_value=true);
    void largest_object_2D(int direction=2, bool object_value=true);
    void threshold_size_2D(int min_size, int direction=2, bool object_value=true);
    void cog_inside_2D(image_binary<IMAGEDIM>* mask, int direction=2, bool object_value=true);
	void erode_2D(int thickness=3, int direction=2, bool object_value=true);
	void dilate_2D(int thickness=3, int direction=2, bool object_value=true);
	void outline_2D(int thickness=3, int direction=2, bool object_value=true);
    image_integer<short, IMAGEDIM> * distance_34_2D(bool edge_is_object=false, int direction=2, bool object_value=true);

    // 3D operations in image_binaryprocess
    void fill_holes_3D(bool object_value=true);
	void largest_object_3D(bool object_value=true);
	void erode_3D(int thickness=3, bool object_value=true);
	void dilate_3D(int thickness=3, bool object_value=true);
	void outline_3D(int thickness=3, bool object_value=true);
    image_integer<short, IMAGEDIM> * distance_345_3D(bool edge_is_object=false, bool object_value=true);
    };

//with C++ templates, declaration and definition go together
#include "image_binary.hxx"
#include "image_binaryprocess.hxx"

#endif
