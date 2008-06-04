//////////////////////////////////////////////////////////////////////////
//
//   Image_multi $Revision$
///
///  Abstract image class for data types with multiple values per voxel, such as vector/complex/RGB/timeseries images
///  Class-Structure Plan
///  *image_multi_scalar
///    Stores a vector of image_scalars (all with the same spatial properties...)
///    Implements functions as FCM, multi-dim-histograms etc.
///    *image_multi_time_series (for example a time series over a contrast injection...)
///    *image_complex_re_im (uses two image_scalars)
///    *image_complex_mag_pha
///      *image_complex_inout_phase
///    *image_multi_complex (handles a vector of image_complex..)
///  *image_multi_integer (if this type of class is needed...)
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

//#include "image_general.h"
#include "image_scalar.h"

template<class ELEMTYPE, int IMAGEDIM = 3>
class image_multi : public image_general<ELEMTYPE, IMAGEDIM>
    {
	private:
		vector<image_scalar<ELEMTYPE, IMAGEDIM>*> image_vector;

    //redundant declaration of constructor, since those cannot be inherited
    public:
		image_multi(std::vector<std::string> file_names);

		virtual void get_display_voxel(RGBvalue &val,int x, int y, int z=0) const;
        virtual float get_number_voxel(int x, int y, int z) const;  //the use of virtual makes for example "complex<>" class work...

    };

#endif
