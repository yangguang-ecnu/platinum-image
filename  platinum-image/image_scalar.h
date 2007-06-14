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
        //raw constructor
        image_scalar(std::vector<std::string> files, long width, long height, bool bigEndian, long headerSize, Vector3D voxelSize, unsigned int startFile = 1,unsigned int increment = 1) : image_general<ELEMTYPE, IMAGEDIM> (files, width, height, bigEndian, headerSize, voxelSize, startFile,increment)
            {};

        // *** operations ***

        //	void interpolate_bilinear_2D(float phys_x, float phys_y, int vox_z);
        //	void interpolate_trilinear_3D_ITK(float phys_x, float phys_y, float phys_z); //no boundary checks in "itkLinearInterpolateImageFunction.h" 
        void interpolate_trilinear_3D_vxl(image_scalar<ELEMTYPE, IMAGEDIM > *src_im); //Implementation from vxl package
    };

// templates in standard C++ requires entire class definition in header

#include "image_scalar.hxx"


#endif
