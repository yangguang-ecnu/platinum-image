//////////////////////////////////////////////////////////////////////////
//
//  Image_binary
//
//  Image type with binary voxel/pixel values
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

#ifndef __image_binary__
#define __image_binary__

#include "image_general.h"

template<int IMAGEDIM = 3>
class image_binary : public image_general <bool, IMAGEDIM>
    {
    public:
        image_binary ():image_general<bool, IMAGEDIM>() {}
        
        template<class SOURCETYPE>
            image_binary(image_general<SOURCETYPE, IMAGEDIM> * old_volume, bool copyData = true): image_general<bool, IMAGEDIM>(old_volume, copyData)
        {} //copy constructor
        
        image_binary (std::vector<std::string> files, long width, long height, bool bigEndian = false, long headerSize = 0, Vector3D voxelSize = Vector3D (1,1,4), unsigned int startFile = 1,unsigned int increment = 1): image_general<bool, IMAGEDIM> (files, width, height, bigEndian, headerSize, voxelSize, startFile,increment) {}
        
        // *** processing ***
        
        void fill_holes_2D(int direction=2, bool object_value=true); // Loops over the dimension given by direction and performs a slice-wise hole filling
    };

//with C++ templates, declaration and definition go together
#include "image_binary.hxx"

#endif
