// $Id$

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

#ifndef __image_label_hxx__
#define __image_label_hxx__

#include "image_label.h"
#include "image_integer.hxx"


template <int IMAGEDIM>
image_label<IMAGEDIM>::image_label(int w, int h, int d, IMGBINARYTYPE *ptr):image_integer<IMGLABELTYPE, IMAGEDIM>(w, h, d, ptr)
{
transfer_function();
}

template <int IMAGEDIM>
image_label<IMAGEDIM>::image_label(itk::SmartPointer< itk::OrientedImage<IMGLABELTYPE, IMAGEDIM > > &i):image_integer<IMGLABELTYPE, IMAGEDIM>(i) 
{
transfer_function();
}
/*
template <int IMAGEDIM>
template<class SOURCETYPE>
image_label<IMAGEDIM>::image_label(image_general<SOURCETYPE, IMAGEDIM> * old_image, bool copyData): image_integer<IMGLABELTYPE, IMAGEDIM>(old_image, copyData)
{
transfer_function();
} //copy constructor
*/

template <int IMAGEDIM>
image_label<IMAGEDIM>::image_label(IMGLABELTYPE * inData, unsigned long inDataNumElems, long width, long height, Vector3D voxelSize) : image_integer<IMGLABELTYPE, IMAGEDIM>(inData,inDataNumElems, width, height, voxelSize)
{
transfer_function();
}

template <int IMAGEDIM>
image_label<IMAGEDIM>::image_label (std::vector<std::string> files, long width, long height, bool bigEndian, long headerSize, Vector3D voxelSize, unsigned int startFile,unsigned int increment): image_integer<IMGLABELTYPE, IMAGEDIM> (files, width, height, bigEndian, headerSize, voxelSize, startFile,increment) 
{
transfer_function();
}

template <int IMAGEDIM>
image_label<IMAGEDIM>::image_label(const string filepath, const string name):image_integer<IMGLABELTYPE, IMAGEDIM>(filepath, name) 
{
transfer_function();
}

template <int IMAGEDIM>
image_label<IMAGEDIM>::image_label(vector< image_binary<IMAGEDIM>* > images, const string name):image_integer<IMGLABELTYPE, IMAGEDIM>(images[0]->nx(),images[0]->ny(),images[0]->nz())
{
	this->fill(0);
	this->set_parameters(images[0]);
	for(int i=0;i<images.size();i++){
		this->fill_region_of_mask_3D(images[i],i+1);
	}
	this->data_has_changed();
	transfer_function();
}
/*
template <int IMAGEDIM>
image_label<IMAGEDIM>::image_label(image_binary<IMAGEDIM>* image1, image_binary<IMAGEDIM>* image2, image_binary<IMAGEDIM>* image3, image_binary<IMAGEDIM>* image4, image_binary<IMAGEDIM>* image5, const string name):image_integer<IMGLABELTYPE, IMAGEDIM>(image1->nx(),image1->ny(),image1->nz())
{
	int num_images=5;
	if (image2==NULL) {num_images=1;}
	else if (image3==NULL) {num_images=2;}
	else if (image4==NULL) {num_images=3;}
	else if (image5==NULL) {num_images=4;}
	
	vector< image_binary<IMAGEDIM>* > images(num_images);
    images[0]=image1;
	if (image2!=NULL) {images[1]=image2;}
	if (image3!=NULL) {images[2]=image3;}
	if (image4!=NULL) {images[3]=image4;}
	if (image5!=NULL) {images[4]=image5;}
	
	this->fill(0);
	this->set_parameters(images[0]);
	for(int i=0;i<num_images;i++){
		this->fill_region_of_mask_3D(images[i],i+1);
	}
	this->data_has_changed();
	transfer_function();
}
*/

template<int IMAGEDIM >
void image_label<IMAGEDIM >:: transfer_function(transfer_base<IMGLABELTYPE > * const t)
    {
    if (this->tfunction != NULL)
        {delete this->tfunction;}

    if (t == NULL)
        this->tfunction = new transfer_mapcolor<IMGLABELTYPE >(this);
    else
        this->tfunction = t;
    }



template <int IMAGEDIM>
image_label<IMAGEDIM>* image_label<IMAGEDIM>::get_subvolume_from_region_3D(Vector3Dint vox_pos, Vector3Dint vox_size)
{
	return get_subvolume_from_region_3D(vox_pos[0],vox_pos[1],vox_pos[2],vox_pos[0]+vox_size[0],vox_pos[1]+vox_size[1],vox_pos[2]+vox_size[2]);
}

template <int IMAGEDIM>
image_label<IMAGEDIM>* image_label<IMAGEDIM>::get_subvolume_from_region_3D(int x1, int y1, int z1, int x2, int y2, int z2)
{
	cout<<"image_label-get_subvolume_from_region_3D..."<<endl;

	int nx=this->nx();
	int ny=this->ny();
	int nz=this->nz();
	x1 = max(x1,0);	x2 = min(x2,nx);
	y1 = max(y1,0);	y2 = min(y2,ny);
	z1 = max(z1,0);	z2 = min(z2,nz);

	image_label<IMAGEDIM>* res = new image_label<IMAGEDIM>(x2-x1+1, y2-y1+1, z2-z1+1);
	res->set_parameters(this);
	res->set_origin(this->get_physical_pos_for_voxel(x1,y1,z1));

	for (int z=z1, res_z=0; z<=z2; z++, res_z++){
		for (int y=y1, res_y=0; y<=y2; y++,res_y++){
			for (int x=x1, res_x=0; x<=x2; x++,res_x++){
				res->set_voxel(res_x,res_y,res_z, this->get_voxel(x,y,z));
			}
		}
	}
	return res;
}





template <int IMAGEDIM>
image_label<IMAGEDIM>* label_copycast (image_base* const input) //! Converts IMGLABELTYPE (uchar) into image_label
    {
    image_label<IMAGEDIM > * output = NULL;

    image_integer <IMGLABELTYPE,IMAGEDIM>* input_general =
        dynamic_cast<image_integer <IMGLABELTYPE, IMAGEDIM> *> (input) ; 

    if (input_general != NULL) //! If cast was successful, input had the tried type and input_general can be used in a call to new class' copy constructor
        {
        output = new image_label<IMAGEDIM> (input_general,true);
        }

    return output;
    }


#endif