//////////////////////////////////////////////////////////////////////////
//
//  Image_label $Revision$
//
//  Image type with integer values signifying distinct classes or "labels"
//  (i.e. not continuous values). Can be described as multiple binary
//  images rolled into one. 0 (zero) carries special meaning as
//  empty/background.
//
//  $LastChangedBy$

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

#ifndef __image_label__
#define __image_label__

#define IMGLABELTYPE unsigned char

#include "image_integer.h"

template<int IMAGEDIM = 3>
class image_label : public image_integer <IMGLABELTYPE, IMAGEDIM>
    {
    public:
        image_label ();

    image_label (itk::SmartPointer< itk::Image<IMGLABELTYPE, IMAGEDIM > > &i):image_integer<IMGLABELTYPE, IMAGEDIM>(i) {}
        
        template<class SOURCETYPE>
            image_label(image_general<SOURCETYPE, IMAGEDIM> * old_image, bool copyData = true): image_integer<IMGLABELTYPE, IMAGEDIM>(old_image, copyData)
        {} //copy constructor
        
        image_label (std::vector<std::string> files, long width, long height, bool bigEndian = false, long headerSize = 0, Vector3D voxelSize = Vector3D (1,1,4), unsigned int startFile = 1,unsigned int increment = 1): image_integer<IMGLABELTYPE, IMAGEDIM> (files, width, height, bigEndian, headerSize, voxelSize, startFile,increment) {}

    virtual void transfer_function(transfer_base<IMGLABELTYPE> * t = NULL);
    };

//with C++ templates, declaration and definition go together
#include "image_label.hxx"

#endif
