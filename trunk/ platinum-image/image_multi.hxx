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

#ifndef __image_multi_hxx__
#define __image_multi_hxx__

#include "image_multi.h"


template <class ELEMTYPE, int IMAGEDIM>
image_multi<ELEMTYPE, IMAGEDIM>::image_multi(std::vector<std::string> file_names)
{
	cout<<"image_multi...(load_vector of images)......"<<endl;

	for(int i=0;i<file_names.size();i++){
		cout<<file_names[i]<<endl;
		this->image_vector.push_back(new image_scalar<ELEMTYPE, IMAGEDIM>(file_names[i]));
	}

	this->datasize[0] = this->image_vector[0]->nx();
	this->datasize[1] = this->image_vector[0]->ny();
	this->datasize[2] = this->image_vector[0]->nz();
	this->set_parameters(this->image_vector[0]);
	this->print_geometry();
	cout<<"........"<<endl;
}


template <class ELEMTYPE, int IMAGEDIM>
void image_multi<ELEMTYPE, IMAGEDIM>::get_display_voxel(RGBvalue &val,int x, int y, int z) const
{
//		cout<<"image_multi...get_display_voxel"<<endl;
		val.r( IMGELEMCOMPTYPE(image_vector[0]->get_voxel(x, y, z)) ); //JK3 - involve transfer functions later....
//    this->tfunction->get(get_voxel(x, y, z),val);
    //val.set_mono(255*(get_voxel (x, y, z)-minvalue)/(maxvalue-minvalue));
}

template <class ELEMTYPE, int IMAGEDIM>
float image_multi<ELEMTYPE, IMAGEDIM>::get_number_voxel(int x, int y, int z) const
    {
    return this->image_vector[0]->get_number_voxel(x,y,z);
    }


#endif