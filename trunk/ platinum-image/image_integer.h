//////////////////////////////////////////////////////////////////////////
//
//  Image_integer $Revision$
//
//  Image type with integer values, in itself and as base for more
//  task-specific subclasses
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

#ifndef __image_integer__
#define __image_integer__

#include "image_scalar.h"

template<class ELEMTYPE, int IMAGEDIM = 3>
class image_integer : public image_scalar <ELEMTYPE, IMAGEDIM>
    {
    public:
        image_integer ():image_scalar<ELEMTYPE, IMAGEDIM>()
            {}
        image_integer(int w, int h, int d, ELEMTYPE *ptr = NULL):image_scalar<ELEMTYPE, IMAGEDIM>(w, h, d, ptr) {};

    image_integer (itk::SmartPointer< itk::Image<ELEMTYPE, IMAGEDIM > > &i):image_scalar<ELEMTYPE, IMAGEDIM>(i) {}

    template<class SOURCETYPE>
    image_integer(image_general<SOURCETYPE, IMAGEDIM> * old_image, bool copyData = true): image_scalar<ELEMTYPE, IMAGEDIM>(old_image, copyData)
        {} //copy constructor

    image_integer (std::vector<std::string> files, long width, long height, bool bigEndian = false, long headerSize = 0, Vector3D voxelSize = Vector3D (1,1,4), unsigned int startFile = 1,unsigned int increment = 1): image_scalar<ELEMTYPE, IMAGEDIM> (files, width, height, bigEndian, headerSize, voxelSize, startFile,increment) {}
    };

//with C++ templates, declaration and definition go together
#include "image_integer.hxx"

#endif
