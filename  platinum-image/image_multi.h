//////////////////////////////////////////////////////////////////////////
//
//   Image_multi $Revision$
///
///  Abstract image class for data types with multiple values per voxel,
///  such as vector, complex and RGB
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

#ifndef __image_multi__
#define __image_multi__

#include "image_general.h"

//#include <complex>

template<class ELEMTYPE, int IMAGEDIM = 3>
class image_multi : public image_general <ELEMTYPE, IMAGEDIM>
    {
    //redundant declaration of constructor, since those cannot be inherited
    public:
        image_multi ():image_general<ELEMTYPE, IMAGEDIM>() {}
        
        template<class SOURCETYPE>
        image_multi(image_general<SOURCETYPE, IMAGEDIM> * old_image, bool copyData = true): image_general<ELEMTYPE, IMAGEDIM>(old_image, copyData)
        {} //copy constructor
        
        image_multi (std::vector<std::string> files, long width, long height, bool bigEndian = false, long headerSize = 0, Vector3D voxelSize = Vector3D (1,1,4), unsigned int startFile = 1,unsigned int increment = 1): image_general<ELEMTYPE, IMAGEDIM> (files, width, height, bigEndian, headerSize, voxelSize, startFile,increment) {}
    };

// templates in standard C++ requires entire class definition in header

#include "image_multi.hxx"

#endif
