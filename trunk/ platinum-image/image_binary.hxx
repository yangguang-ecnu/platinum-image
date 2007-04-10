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

#ifndef __image_binary__hxx__
#define __image_binary__hxx__

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::fill_holes_2D(int direction, bool object_value)
	{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim_and_dir(0,direction);
	max_v=this->get_size_by_dim_and_dir(1,direction);
	max_w=this->get_size_by_dim_and_dir(2,direction);
		
	bool p;//pixel value
	int label,up,left;//neighbour labels 
	image_general<int, IMAGEDIM> label_image(max_u, max_v, max_w);
	int new_label;
	//init labels
	for(w=0; w<max_w; w++)
		{
		int number_of_objects=1;
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				p=image_general<bool,IMAGEDIM>::get_voxel_by_dir(u,v,w,direction);
				if(p!=object_value)//Note: we want to label bkg-objects in order to remove holes
					{
					up=(v>0)? label_image.get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image.get_voxel(u-1,v,w) : 0;
					new_label=number_of_objects;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					if(new_label==number_of_objects)
						number_of_objects++;
					label_image.set_voxel(u,v,w,new_label);
					}
				else
					label_image.set_voxel(u,v,w,0);
				}
			}
		int* changes=new int[number_of_objects];
		int i;
		for(i=0; i<number_of_objects; i++)
			changes[i]=i;
		//merge labels
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				label=label_image.get_voxel(u,v,w);
				if(label>0)
					{
					up=(v>0)? label_image.get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image.get_voxel(u-1,v,w) : 0;
					while(label!=changes[label])
						label=changes[label];
					while(up!=changes[up])
						up=changes[up];
					while(left!=changes[left])
						left=changes[left];
					new_label=label;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					changes[label]=new_label;
					if(up>0)
						changes[up]=new_label;
					if(left>0)
						changes[left]=new_label;						
					}
				}
			}
		int* lut=new int[number_of_objects];
		new_label=1;
		for(i=1; i<number_of_objects; i++)
			{
			label=i;
			while(label!=changes[label])
				label=changes[label];
			if(label==i)
				{
				lut[i]=new_label;
				new_label++;
				}
			else
				lut[i]=lut[label];
			}
		//set all labels !=1 -> obj
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				label=label_image.get_voxel(u,v,w);
				if(lut[label]==1)//First label is background
					this->set_voxel_by_dir(u,v,w,!object_value,direction);
				else //all other labels are object
					this->set_voxel_by_dir(u,v,w,object_value,direction);
				}
			}
		}				
	}

//template <int IMAGEDIM>
//    template <class SOURCETYPE>
//image_binary<IMAGEDIM>::threshold(image_general<SOURCETYPE, IMAGEDIM>* input, SOURCETYPE low, SOURCETYPE high, bool object_value=true)
//    {
//    this = input->alike<bool>();
//    int i;
//    int n_voxels=1;
//    for(i=0; i<IMAGEDIM; i++)
//        n_voxels*=get_size_by_dim(i);
//    SOURCETYPE p; //boolean
//    for(i=0; i<n_voxels; i++)
//        {
//        p=input->get_voxel(i);
//        if(p>=low && p<=high)
//            set_voxel(i,object_value);
//        else
//            set_voxel(i,!object_value);
//        }
//    }

#endif
