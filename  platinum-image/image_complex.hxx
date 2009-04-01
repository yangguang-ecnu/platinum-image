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

#include "transfer.hxx"


template <class ELEMTYPE, int IMAGEDIM>
image_complex<ELEMTYPE, IMAGEDIM>::image_complex():image_general<complex<ELEMTYPE>, IMAGEDIM>()
{   
	this->set_complex_parameters();
}

template <class ELEMTYPE, int IMAGEDIM>
image_complex<ELEMTYPE, IMAGEDIM>::image_complex(int w, int h, int d, complex<ELEMTYPE> *ptr):image_general<complex<ELEMTYPE>, IMAGEDIM>(w,h,d,ptr)
{   
	this->set_complex_parameters();
}

template <class ELEMTYPE, int IMAGEDIM>
image_complex<ELEMTYPE, IMAGEDIM>::image_complex(string path_image_re, string path_image_im,float unsigned_offset, string name)
{
	image_scalar<ELEMTYPE,IMAGEDIM> *re = new image_scalar<ELEMTYPE,IMAGEDIM>(path_image_re);
	image_scalar<ELEMTYPE,IMAGEDIM> *im = new image_scalar<ELEMTYPE,IMAGEDIM>(path_image_im);
	this->initialize_dataset(re->nx(),re->ny(),re->nz(), NULL);
	complex<ELEMTYPE> c;
	for(int z=0; z<re->nz(); z++){				
		for(int y=0; y<re->ny(); y++){				
			for(int x=0; x<re->nx(); x++){				
				c = complex<ELEMTYPE>( re->get_voxel(x,y,z)-unsigned_offset, im->get_voxel(x,y,z)-unsigned_offset );
				this->set_voxel(x,y,z, c);
			}
		}
	}

	this->set_complex_parameters();
	this->set_parameters(re);
	if(name!=""){
		this->name(name);
	}
	this->stats_refresh(true);

	delete re;
	delete im;
	//datamanagement.add(re,"re");
	//datamanagement.add(im,"im");
}


template <class ELEMTYPE, int IMAGEDIM>
void image_complex<ELEMTYPE, IMAGEDIM>::set_complex_parameters()
    {
	image_scalar<ELEMTYPE,IMAGEDIM> *tmp = new image_scalar<ELEMTYPE,IMAGEDIM>(1,1,1);
	tmp->name("image_complex_tmp_image");//this->get_magnitude_image();
	stats = NULL;
	set_stats_histogram( new histogram_1D<ELEMTYPE>(tmp) );  //hist1D constructor calls resize()... and calculate()

    tfunction = NULL;
	transfer_function();
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_complex<ELEMTYPE, IMAGEDIM>::set_stats_histogram(histogram_1D<ELEMTYPE > * h)
    {
    if (stats != NULL)
        {
        delete stats;
        }

    stats = h;
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_complex<ELEMTYPE, IMAGEDIM>::stats_refresh(bool min_max_refresh)
    {
//	if(min_max_refresh){
//		this->min_max_refresh();
//	}
    stats->calculate_from_image_complex(this); //when called without argument (=0) the histogram "resolution" is kept..

    }


template <class ELEMTYPE, int IMAGEDIM>
void image_complex<ELEMTYPE, IMAGEDIM>::transfer_function(transfer_complex<ELEMTYPE,IMAGEDIM > * const t)
{
    if (this->tfunction != NULL)
        {delete this->tfunction;}

    if (t == NULL)
        this->tfunction = new transfer_complex<ELEMTYPE,IMAGEDIM>(this);
    else
        this->tfunction = t;
}



template <class ELEMTYPE, int IMAGEDIM>
void image_complex<ELEMTYPE, IMAGEDIM>::get_display_voxel(RGBvalue &val,int x, int y, int z) const
{
//	val.r( (IMGELEMCOMPTYPE)(100) );
//	val.g( (IMGELEMCOMPTYPE)(0) );
//	val.b( (IMGELEMCOMPTYPE)(0) );

	this->tfunction->get(this->get_voxel(x, y, z),val);

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
    return get_voxel_magn(x,y,z);
}

//JK - I have not managed to specialize this function for "complex<ELEMTYPE>" - I think the whole class needs to be rewritten for "complex<ELEMTYPE>"
template <class ELEMTYPE, int IMAGEDIM>
float image_complex<ELEMTYPE, IMAGEDIM>::get_max_float() const
{
	return abs(stats->max());
}

template <class ELEMTYPE, int IMAGEDIM>
float image_complex<ELEMTYPE, IMAGEDIM>::get_max_float_safe() const
{
	if(stats!=NULL){
		return abs(stats->max()); //JK4
	}
	return 1000;
}
template <class ELEMTYPE, int IMAGEDIM>
float image_complex<ELEMTYPE, IMAGEDIM>::get_min_float() const
{
	return abs(stats->min());
}

template <class ELEMTYPE, int IMAGEDIM>
float image_complex<ELEMTYPE, IMAGEDIM>::get_min_float_safe() const
{
	if(stats!=NULL){
		return abs(stats->min()); //JK4
	}
	return 0;
}


template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_complex<ELEMTYPE, IMAGEDIM>::get_voxel_re(int x, int y, int z) const
{
	return real(this->get_voxel(x,y,z));
}

template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_complex<ELEMTYPE, IMAGEDIM>::get_voxel_im(int x, int y, int z) const
{
	return imag(this->get_voxel(x,y,z));
}

template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_complex<ELEMTYPE, IMAGEDIM>::get_voxel_magn(int x, int y, int z) const
{
	return abs(this->get_voxel(x,y,z));
}

template <class ELEMTYPE, int IMAGEDIM>
float image_complex<ELEMTYPE, IMAGEDIM>::get_voxel_phase(int x, int y, int z) const
{
	return arg(this->get_voxel(x,y,z));
}

template <class ELEMTYPE, int IMAGEDIM>
image_scalar<ELEMTYPE, IMAGEDIM>* image_complex<ELEMTYPE, IMAGEDIM>::get_magnitude_image()
{
	image_scalar<ELEMTYPE,IMAGEDIM> *m = new image_scalar<ELEMTYPE,IMAGEDIM>(this->nx(),this->ny(),this->nz());
	for(int z=0; z<this->nz(); z++){				
		for(int y=0; y<this->ny(); y++){				
			for(int x=0; x<this->nx(); x++){				
				m->set_voxel(x,y,z, this->get_voxel_magn(x,y,z) );
			}
		}
	}

	m->set_parameters(this);
	return m;
}


#endif