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

#ifndef __image_complex_hxx__
#define __image_complex_hxx__

#include "image_complex.h"
//#include "image_multi.hxx"



template<class ELEMTYPE, int IMAGEDIM>
void image_complex<ELEMTYPE,IMAGEDIM>::silly_test()
    {
	cout<<"* This is a silly test , JK"<<endl;
	}


template<class ELEMTYPE, int IMAGEDIM>
float image_complex<ELEMTYPE,IMAGEDIM>::get_number_voxel(int x, int y, int z) const
    {
    return 0;
    }




#endif