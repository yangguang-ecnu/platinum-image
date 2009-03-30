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


template <class ELEMTYPE, int IMAGEDIM>
void image_complex<ELEMTYPE, IMAGEDIM>::set_complex_parameters()
    {
    tfunction = NULL;

//	transfer_function();
    }


/*
template <class ELEMTYPE, int IMAGEDIM>
void image_complex<ELEMTYPE, IMAGEDIM>::transfer_function(transfer_complex<ELEMTYPE > * const t)
{
    if (this->tfunction != NULL)
        {delete this->tfunction;}

    if (t == NULL)
//        this->tfunction = new transfer_complex<ELEMTYPE>(this);
    else
        this->tfunction = t;
}
*/


template <class ELEMTYPE, int IMAGEDIM>
void image_complex<ELEMTYPE, IMAGEDIM>::get_display_voxel(RGBvalue &val,int x, int y, int z) const
{
	val.r( (IMGELEMCOMPTYPE)(100) );
	val.g( (IMGELEMCOMPTYPE)(0) );
	val.b( (IMGELEMCOMPTYPE)(0) );

//	this->tfunction->get(this->get_voxel(x, y, z),val);

//		val.r( (IMGELEMCOMPTYPE)(image_vector[0]->get_voxel(x, y, z)) ); //JK4 - involve transfer functions later....
//    this->tfunction->get(get_voxel(x, y, z),val);
    //val.set_mono(255*(get_voxel (x, y, z)-minvalue)/(maxvalue-minvalue));
}

template <class ELEMTYPE, int IMAGEDIM>
void image_complex<ELEMTYPE, IMAGEDIM>::silly_test()
{
	cout<<"* This is a silly test , JK"<<endl;
}


template <class ELEMTYPE, int IMAGEDIM>
float image_complex<ELEMTYPE, IMAGEDIM>::get_number_voxel(int x, int y, int z) const
{
    return 0;
}

//JK - I have not managed to specialize this function for "complex<ELEMTYPE>" - I think the whole class needs to be rewritten for "complex<ELEMTYPE>"
template <class ELEMTYPE, int IMAGEDIM>
float image_complex<ELEMTYPE, IMAGEDIM>::get_max_float() const
{
	//calculate the complex max value "in some sense"... magnitude?
	float max=255;
	return max; //JK4
}

#endif