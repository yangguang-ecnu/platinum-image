//////////////////////////////////////////////////////////////////////////
//
//  Binary process
//
//  Task-specific processing of binary images
//
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

// Loops over the dimension given by direction and performs a slice-wise hole filling
template <int IMAGEDIM>
void image_binary<IMAGEDIM>::fill_holes_2D(int direction, IMGBINARYTYPE object_value)
	{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim_and_dir(0,direction);
	max_v=this->get_size_by_dim_and_dir(1,direction);
	max_w=this->get_size_by_dim_and_dir(2,direction);
		
	IMGBINARYTYPE p;//pixel value
	int label,up,left;//neighbour labels 
	//image_general<int, IMAGEDIM> label_image(max_u, max_v, max_w);
    image_integer<int, IMAGEDIM> * label_image = new image_integer<int, IMAGEDIM> (max_u, max_v, max_w);
	int new_label;
	//init labels
	for(w=0; w<max_w; w++)
		{
		int number_of_objects=1;
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				p=this->get_voxel_by_dir(u,v,w,direction);
				if(p!=object_value)//Note: we want to label bkg-objects in order to remove holes
					{
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
					new_label=number_of_objects;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					if(new_label==number_of_objects)
						number_of_objects++;
					label_image->set_voxel(u,v,w,new_label);
					}
				else
					label_image->set_voxel(u,v,w,0);
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
				label=label_image->get_voxel(u,v,w);
				if(label>0)
					{
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
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
		memset(lut, 0, sizeof(int)*number_of_objects);
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
				label=label_image->get_voxel(u,v,w);
				if(lut[label]==1)//First label is background
					this->set_voxel_by_dir(u,v,w,!object_value,direction);
				else //all other labels are object
					this->set_voxel_by_dir(u,v,w,object_value,direction);
				}
			}
		delete[] changes;
		delete[] lut;
		}	

	//this->image_has_changed();
	delete label_image;
	}
	
template <int IMAGEDIM>
image_label<3>* image_binary<IMAGEDIM>::label_connected_objects_2D(int direction, IMGBINARYTYPE object_value)
{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim_and_dir(0, direction);
	max_v=this->get_size_by_dim_and_dir(1, direction);
	max_w=this->get_size_by_dim_and_dir(2, direction);
		
	IMGBINARYTYPE p;//pixel value
	int label,up,left;//neighbour labels 
    image_integer<int,IMAGEDIM> * label_image = new image_integer<int,IMAGEDIM> (this);
	int new_label;
	//init labels
	int number_of_objects=1;
	vector<int> changes;
	changes.push_back(0);
	for(w=0; w<max_w; w++) {
		for(v=0; v<max_v; v++) {
			for(u=0; u<max_u; u++) {
				p=this->get_voxel_by_dir(u,v,w,direction);
				if(p==object_value)	{
					up=(v>0)? label_image->get_voxel_by_dir(u,v-1,w,direction) : 0;
					left=(u>0)? label_image->get_voxel_by_dir(u-1,v,w,direction) : 0;
					while(up!=changes[up])
						up=changes[up];
					while(left!=changes[left])
						left=changes[left];
					new_label=number_of_objects;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					if(up>new_label)
						changes[up]=new_label;
					if(left>new_label)
						changes[left]=new_label;
					if(new_label==number_of_objects) {
						changes.push_back(number_of_objects);
						number_of_objects++;	
					}
					label_image->set_voxel_by_dir(u,v,w,new_label,direction);
				}
				else
					label_image->set_voxel_by_dir(u,v,w,0,direction);
			}
		}
	}
	
	// calculate look-up-table
	int* lut=new int[number_of_objects];
	memset(lut, 0, sizeof(int)*number_of_objects);
	new_label=1;
	for(int i=1; i<number_of_objects; i++) {
		label=i;
		while(label!=changes[label])
			label=changes[label];
		if(label==i) {
			lut[i]=new_label;
			new_label++;
		}
		else
			lut[i]=lut[label];
	}
	
    // set new labels from look-up table
	image_label<IMAGEDIM> * label_image_label = new image_label<IMAGEDIM> (label_image);
	int max_label=0;
	int new_max_label;
	for(w=0; w<max_w; w++) {
		new_max_label=max_label;
		for(v=0; v<max_v; v++) {
			for(u=0; u<max_u; u++) {
				label=label_image->get_voxel_by_dir(u,v,w,direction);
				if (lut[label]>new_max_label) 
					new_max_label=lut[label];
				if (label>0)
					label_image_label->set_voxel_by_dir(u,v,w,lut[label]-max_label,direction);
			}
		}
		max_label=new_max_label;
	}
	delete[] lut;
	delete label_image;
	return label_image_label;
}

template <int IMAGEDIM>
image_integer<unsigned long, 3>* image_binary<IMAGEDIM>::label_connected_objects_with_area_2D(int direction, IMGBINARYTYPE object_value)
{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim_and_dir(0,direction);
	max_v=this->get_size_by_dim_and_dir(1,direction);
	max_w=this->get_size_by_dim_and_dir(2,direction);
		
	IMGBINARYTYPE p;//pixel value
	int label,up,left;//neighbour labels 
    image_integer<unsigned long,IMAGEDIM> * label_image = new image_integer<unsigned long,IMAGEDIM> (this);
	int new_label;
	//init labels
	int number_of_objects=1;
	vector<int> changes;
	changes.push_back(0);
	vector<int> sizes;
	sizes.push_back(0);
	for(w=0; w<max_w; w++) {
		for(v=0; v<max_v; v++) {
			for(u=0; u<max_u; u++) {
				p=this->get_voxel_by_dir(u,v,w,direction);
				if(p==object_value)	{
					up=(v>0)? label_image->get_voxel_by_dir(u,v-1,w,direction) : 0;
					left=(u>0)? label_image->get_voxel_by_dir(u-1,v,w,direction) : 0;
					while(up!=changes[up])
						up=changes[up];
					while(left!=changes[left])
						left=changes[left];
					new_label=number_of_objects;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					if(up>new_label)
						changes[up]=new_label;
					if(left>new_label)
						changes[left]=new_label;
					if(new_label==number_of_objects) {
						changes.push_back(number_of_objects);
						sizes.push_back(0);
						number_of_objects++;	
					}
					label_image->set_voxel_by_dir(u,v,w,new_label,direction);
					sizes[new_label]++;
				}
				else
					label_image->set_voxel_by_dir(u,v,w,0,direction);
			}
		}
	}
	
	// calculate look-up-table and sizes
	int* lut=new int[number_of_objects];
	memset(lut, 0, sizeof(int)*number_of_objects);
	vector<int> tot_sizes;
	tot_sizes.push_back(0);
	new_label=1;
	for(int i=1; i<number_of_objects; i++) {
		label=i;
		while(label!=changes[label])
			label=changes[label];
		if(label==i) {
			lut[i]=new_label;
			new_label++;
			tot_sizes.push_back(0);
		}
		else
			lut[i]=lut[label];
		tot_sizes[lut[i]]+=sizes[i];
	}
	
    // set new labels from look-up table and sizes
	for(w=0; w<max_w; w++) {
		for(v=0; v<max_v; v++) {
			for(u=0; u<max_u; u++) {
				label=label_image->get_voxel_by_dir(u,v,w,direction);
				if (label>0)
					label_image->set_voxel_by_dir(u,v,w,tot_sizes[lut[label]],direction);
			}
		}
	}
	delete[] lut;
	return label_image;
}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::largest_object_2D(int direction, IMGBINARYTYPE object_value)
	{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim_and_dir(0,direction);
	max_v=this->get_size_by_dim_and_dir(1,direction);
	max_w=this->get_size_by_dim_and_dir(2,direction);
		
	IMGBINARYTYPE p;//pixel value
	int label,up,left;//neighbour labels 
	//image_general<int, IMAGEDIM> label_image(max_u, max_v, max_w);
    image_integer<int,IMAGEDIM> * label_image = new image_integer<int, IMAGEDIM> (max_u, max_v, max_w);
	int new_label;
	//init labels
	for(w=0; w<max_w; w++)
		{
		int number_of_objects=1;
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				p=this->get_voxel_by_dir(u,v,w,direction);
				if(p==object_value)
					{
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
					new_label=number_of_objects;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					if(new_label==number_of_objects)
						number_of_objects++;
					label_image->set_voxel(u,v,w,new_label);
					}
				else
					label_image->set_voxel(u,v,w,0);
				}
			}
		int* changes=new int[number_of_objects];
		int i;
		for(i=0; i<number_of_objects; i++)
			changes[i]=i;
		int* sizes=new int[number_of_objects];
		memset(sizes, 0, sizeof(int)*number_of_objects);
		//merge labels
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				label=label_image->get_voxel(u,v,w);
				if(label>0)
					{
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
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
					sizes[new_label]++;						
					}
				}
			}
		int* lut=new int[number_of_objects];
		memset(lut, 0, sizeof(int)*number_of_objects);
		int* tot_sizes=new int[number_of_objects];
		memset(tot_sizes, 0, sizeof(int)*number_of_objects);
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
			tot_sizes[lut[i]]+=sizes[i];
			}
		int max_size=0;
		int max_label=1;
		for(i=1; i<new_label; i++)
			{
			if(tot_sizes[i]>max_size)
				{
				max_size=tot_sizes[i];
				max_label=i;
				}
			}
		//set all labels !=1 -> obj
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				label=label_image->get_voxel(u,v,w);
				if(lut[label]==max_label)//First label is background
					this->set_voxel_by_dir(u,v,w,object_value,direction);
				else //all other labels are object
					this->set_voxel_by_dir(u,v,w,!object_value,direction);
				}
			}
		delete[] changes;
		delete[] sizes;
		delete[] lut;
		delete[] tot_sizes;
		}

	//this->image_has_changed();
	delete label_image;				
	}
	
template <int IMAGEDIM>
void image_binary<IMAGEDIM>::threshold_size_2D(int min_size, int direction, IMGBINARYTYPE object_value)
	{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim_and_dir(0,direction);
	max_v=this->get_size_by_dim_and_dir(1,direction);
	max_w=this->get_size_by_dim_and_dir(2,direction);
		
	IMGBINARYTYPE p;//pixel value
	int label,up,left;//neighbour labels 
	//image_general<int, IMAGEDIM> label_image(max_u, max_v, max_w);
    image_integer<int,IMAGEDIM> * label_image = new image_integer<int, IMAGEDIM> (max_u, max_v, max_w);
	int new_label;
	//init labels
	for(w=0; w<max_w; w++)
		{
		int number_of_objects=1;
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				p=this->get_voxel_by_dir(u,v,w,direction);
				if(p==object_value)
					{
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
					new_label=number_of_objects;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					if(new_label==number_of_objects)
						number_of_objects++;
					label_image->set_voxel(u,v,w,new_label);
					}
				else
					label_image->set_voxel(u,v,w,0);
				}
			}
		int* changes=new int[number_of_objects];
		int i;
		for(i=0; i<number_of_objects; i++)
			changes[i]=i;
		int* sizes=new int[number_of_objects];
		memset(sizes, 0, sizeof(int)*number_of_objects);
		//merge labels
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				label=label_image->get_voxel(u,v,w);
				if(label>0)
					{
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
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
					sizes[new_label]++;						
					}
				}
			}
		int* lut=new int[number_of_objects];
		memset(lut, 0, sizeof(int)*number_of_objects);
		int* tot_sizes=new int[number_of_objects];
		memset(tot_sizes, 0, sizeof(int)*number_of_objects);
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
			tot_sizes[lut[i]]+=sizes[i];
			}
		//set all labels !=1 -> obj
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				label=label_image->get_voxel(u,v,w);
				if(tot_sizes[lut[label]]>=min_size)
					this->set_voxel_by_dir(u,v,w,object_value,direction);
				else
					this->set_voxel_by_dir(u,v,w,!object_value,direction);
				}
			}
		delete[] changes;
		delete[] sizes;
		delete[] lut;
		delete[] tot_sizes;
		}
	
	//this->image_has_changed();
	delete label_image;				
	}
	
template <int IMAGEDIM>
void image_binary<IMAGEDIM>::cog_inside_2D(image_binary<IMAGEDIM>* mask, int direction, IMGBINARYTYPE object_value)
	{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim_and_dir(0,direction);
	max_v=this->get_size_by_dim_and_dir(1,direction);
	max_w=this->get_size_by_dim_and_dir(2,direction);
		
	IMGBINARYTYPE p,m;//pixel value
	int label,up,left;//neighbour labels 
	//image_general<int, IMAGEDIM> label_image(max_u, max_v, max_w);
    image_integer<int,IMAGEDIM> * label_image = new image_integer<int, IMAGEDIM> (max_u, max_v, max_w);
	int new_label;
	//init labels
	for(w=0; w<max_w; w++)
		{
		int number_of_objects=1;
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				p=this->get_voxel_by_dir(u,v,w,direction);
				if(p==object_value)//Note: we want to label bkg-objects in order to remove holes
					{
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
					new_label=number_of_objects;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					if(new_label==number_of_objects)
						number_of_objects++;
					label_image->set_voxel(u,v,w,new_label);
					}
				else
					label_image->set_voxel(u,v,w,0);
				}
			}
		int* changes=new int[number_of_objects];
		int i;
		for(i=0; i<number_of_objects; i++)
			changes[i]=i;
		int* sizes=new int[number_of_objects];
		memset(sizes, 0, sizeof(int)*number_of_objects);
		//merge labels
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				label=label_image->get_voxel(u,v,w);
				if(label>0)
					{
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
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
					sizes[new_label]++;						
					}
				}
			}
		int* lut=new int[number_of_objects];
		memset(lut, 0, sizeof(int)*number_of_objects);
		int* tot_sizes=new int[number_of_objects];
		memset(tot_sizes, 0, sizeof(int)*number_of_objects);
		double *cog_u=new double[number_of_objects];
		memset(cog_u, 0, sizeof(double)*number_of_objects);
		double *cog_v=new double[number_of_objects];
		memset(cog_v, 0, sizeof(double)*number_of_objects);
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
			tot_sizes[lut[i]]+=sizes[i];
			}
		if(new_label>1)
			{
			for(v=0; v<max_v; v++)
				{
				for(u=0; u<max_u; u++)
					{
					label=label_image->get_voxel(u,v,w);
					if(label>0)
						{
						cog_u[lut[label]]+=u;
						cog_v[lut[label]]+=v;
						}
					}
				}
			}
		int u0=max_u;
		int u1=0;
		int v0=max_v;
		int v1=0;
		
		//Find bounding rectangle to mask
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				m=mask->get_voxel_by_dir(u,v,w,direction);			
				if(m==object_value)
					{
					if(u<u0)
						u0=u;
					if(u>u1)
						u1=u;
					if(v<v0)
						v0=v;
					if(v>v1)
						v1=v;
					}
				}
			}
		for(i=1; i<new_label; i++)
			{
			if(tot_sizes[i]>0)
				{
				cog_u[i]/=tot_sizes[i];
				cog_v[i]/=tot_sizes[i];
				u=(int)(cog_u[i]+0.5);
				v=(int)(cog_v[i]+0.5);
				m=mask->get_voxel_by_dir(u,v,w,direction);				
				if(m!=object_value)
					{					
					int nHits=0;
					int du;
					int dv;
					bool hit;
					
					//Right
					hit=false;
					for(du=u; du<=u1 &&!hit; du++)
						{
						m=mask->get_voxel_by_dir(du,v,w,direction);				
						if(m==object_value)
							{
							hit=true;
							nHits++;
							}
						}
					//Left
					hit=false;
					for(du=u; du>=u0 &&!hit; du--)
						{
						m=mask->get_voxel_by_dir(du,v,w,direction);				
						if(m==object_value)
							{
							hit=true;
							nHits++;
							}
						}

					//Up
					hit=false;						
					for(dv=v; dv>=v0 &&!hit; dv--)
						{
						m=mask->get_voxel_by_dir(u,dv,w,direction);				
						if(m==object_value)
							{
							hit=true;
							nHits++;
							}
						}
					
					//Down	
					hit=false;					
					for(dv=v; dv<=v1 &&!hit; dv++)
						{
						m=mask->get_voxel_by_dir(u,dv,w,direction);				
						if(m==object_value)
							{
							hit=true;
							nHits++;
							}
						}
					
					if(nHits==3)
						{
						//Diagonals
						double step_u;
						double step_v;
						double u2;
						double v2;
						int r;

						for(r=-3; r<=3 && nHits==3; r++)
							{
							if(r==0)
								r=1;
							nHits=0;
							//Upper left
							hit=false;
							step_u=-1.0;
							step_v=-r/3.0;
							u2=u;
							v2=v;
							while(u2>=u0 && v2>=v0 && u2<=u1 && v2<=v1 &&!hit)
								{
								m=mask->get_voxel_by_dir(u,dv,w,direction);				
								if(m==object_value)
									{
									hit=true;
									nHits++;
									}
								else
									{
									u2+=step_u;
									v2+=step_v;
									}
								}
							//Upper right
							hit=false;
							step_u=r/3.0;
							step_v=-1.0;
							u2=u;
							v2=v;
							while(u2>=u0 && v2>=v0 && u2<=u1 && v2<=v1 &&!hit)
								{
								m=mask->get_voxel_by_dir(u,dv,w,direction);				
								if(m==object_value)
									{
									hit=true;
									nHits++;
									}
								else
									{
									u2+=step_u;
									v2+=step_v;
									}
								}
							//Lower left
							hit=false;
							step_u=-r/3.0;
							step_v=1.0;
							u2=u;
							v2=v;
							while(u2>=u0 && v2>=v0 && u2<=u1 && v2<=v1 &&!hit)
								{
								m=mask->get_voxel_by_dir(u,dv,w,direction);				
								if(m==object_value)
									{
									hit=true;
									nHits++;
									}
								else
									{
									u2+=step_u;
									v2+=step_v;
									}
								}
							//Lower right
							hit=false;
							step_u=1.0;
							step_v=r/3.0;
							u2=u;
							v2=v;
							while(u2>=u0 && v2>=v0 && u2<=u1 && v2<=v1 &&!hit)
								{
								m=mask->get_voxel_by_dir(u,dv,w,direction);				
								if(m==object_value)
									{
									hit=true;
									nHits++;
									}
								else
									{
									u2+=step_u;
									v2+=step_v;
									}
								}
							}	
						if(nHits<4)
							tot_sizes[i]=0;		
						}
					else if(nHits<3)
						tot_sizes[i]=0;
					}
				}
			}
		//set all labels !=1 -> obj
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				label=label_image->get_voxel(u,v,w);
				if(tot_sizes[lut[label]]>0)
					this->set_voxel_by_dir(u,v,w,object_value,direction);
				else
					this->set_voxel_by_dir(u,v,w,!object_value,direction);
				}
			}
		delete[] changes;
		delete[] sizes;
		delete[] lut;
		delete[] tot_sizes;
		delete[] cog_u;
		delete[] cog_v;
		}
	
	//this->image_has_changed();
	delete label_image;				
	}
		
template <int IMAGEDIM>
void image_binary<IMAGEDIM>::connect_outer_2D(int direction, IMGBINARYTYPE object_value)
	{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim_and_dir(0,direction);
	max_v=this->get_size_by_dim_and_dir(1,direction);
	max_w=this->get_size_by_dim_and_dir(2,direction);
		
	IMGBINARYTYPE p;//pixel value

	for(w=0; w<max_w; w++)
		{
		int number_of_pixels=0;
		int u_at_min_u=max_u;
		int u_at_max_u=0;
		int v_at_min_u=max_v;
		int v_at_max_u=0;
		int u_at_min_v=max_u;
		int u_at_max_v=0;
		int v_at_min_v=max_v;
		int v_at_max_v=0;
		bool found=false;
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				p=this->get_voxel_by_dir(u,v,w,direction);
				if(p==object_value)
					{
					found=true;
					if(u<u_at_min_u) {u_at_min_u=u; v_at_min_u=v;}
					if(u>u_at_max_u) {u_at_max_u=u; v_at_max_u=v;}
					if(v<v_at_min_v) {v_at_min_v=v; u_at_min_v=u;}
					if(v>v_at_max_v) {v_at_max_v=v; u_at_max_v=u;}
					}
				}
			}
		if(found)
			{
			bool hit;

			//Start from upper left
			int prev_u=u_at_min_v;
			int prev_v=v_at_min_v;
			for(v=v_at_min_v+1; v<v_at_min_u; v++)
				{
				hit=false;
				for(u=u_at_min_u; u<prev_u && !hit; )
					{
					p=this->get_voxel_by_dir(u,v,w,direction);
					if(p==object_value)
						hit=true;
					else
						u++;
					}
				if(hit)
					{
					this->draw_line_2D(u,v,prev_u,prev_v,w,object_value,direction);
					prev_u=u;
					prev_v=v;
					}
				}
			this->draw_line_2D(u_at_min_u,v_at_min_u,prev_u,prev_v,w,object_value,direction);

			//Start from lower left
			prev_u=u_at_max_v;
			prev_v=v_at_max_v;
			for(v=v_at_max_v-1; v>v_at_min_u; v--)
				{
				hit=false;
				for(u=u_at_min_u; u<prev_u && !hit; )
					{
					p=this->get_voxel_by_dir(u,v,w,direction);
					if(p==object_value)
						hit=true;
					else
						u++;
					}
				if(hit)
					{
					this->draw_line_2D(u,v,prev_u,prev_v,w,object_value,direction);
					prev_u=u;
					prev_v=v;
					}
				}
			this->draw_line_2D(u_at_min_u,v_at_min_u,prev_u,prev_v,w,object_value,direction);

			//Start from upper right
			prev_u=u_at_min_v;
			prev_v=v_at_min_v;
			for(v=v_at_min_v; v<v_at_max_u; v++)
				{
				hit=false;
				for(u=u_at_max_u; u>prev_u && !hit; )
					{
					p=this->get_voxel_by_dir(u,v,w,direction);
					if(p==object_value)
						hit=true;
					else
						u--;
					}
				if(hit)
					{
					this->draw_line_2D(u,v,prev_u,prev_v,w,object_value,direction);
					prev_u=u;
					prev_v=v;
					}
				}
			this->draw_line_2D(u_at_max_u,v_at_max_u,prev_u,prev_v,w,object_value,direction);

			//Start from lower right
			prev_u=u_at_max_v;
			prev_v=v_at_max_v;
			for(v=v_at_max_v; v>v_at_max_u; v--)
				{
				hit=false;
				for(u=u_at_max_u; u>prev_u && !hit; )
					{
					p=this->get_voxel_by_dir(u,v,w,direction);
					if(p==object_value)
						hit=true;
					else
						u--;
					}
				if(hit)
					{
					this->draw_line_2D(u,v,prev_u,prev_v,w,object_value,direction);
					prev_u=u;
					prev_v=v;
					}
				}
			this->draw_line_2D(u_at_max_u,v_at_max_u,prev_u,prev_v,w,object_value,direction);
			}
		}
	//this->image_has_changed();				
	}

template <int IMAGEDIM>
image_integer<short, IMAGEDIM> *  image_binary<IMAGEDIM>::distance_34_2D(bool edge_is_object, int direction, IMGBINARYTYPE object_value)
	{
	image_integer<short, IMAGEDIM>* output = new image_integer<short,IMAGEDIM> (this,false);
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim_and_dir(0,direction);
	max_v=this->get_size_by_dim_and_dir(1,direction);
	max_w=this->get_size_by_dim_and_dir(2,direction);
	int veryhigh = std::max(max_u,max_v)*4;//std::numeric_limits<int>::max()-9;
	int initvalue=(edge_is_object)?veryhigh:0;
	IMGBINARYTYPE p;//pixel value
	int d,ul,um,ur,ml,mr,ll,lm,lr;//neighbour labels 
	for(w=0; w<max_w; w++)
		{
		//Forward pass
		for(v=0; v<max_v; v++)
			{
			u=0;
			ml=initvalue;
			ul=initvalue;
			um=(v>0)? output->get_voxel_by_dir(u,v-1,w,direction) : initvalue;
			for(u=0; u<max_u; u++)
				{
				p=this->get_voxel_by_dir(u,v,w,direction);
				ur=(v>0 && u<max_u-1)? output->get_voxel_by_dir(u+1,v-1,w,direction) : initvalue;
				d=(p==object_value)?std::min(std::min(4+ul,3+um),std::min(4+ur,3+ml)):0;
				output->set_voxel_by_dir(u,v,w,d,direction);
				ml=d;
				ul=um;
				um=ur;
				}
			}
		//Backward pass
		for(v=max_v-1; v>=0; v--)
			{
			u=max_u-1;
			mr=initvalue;
			lr=initvalue;
			lm=(v<max_v-1)? output->get_voxel_by_dir(u,v+1,w,direction) : initvalue;
			for(u=max_u-1; u>=0; u--)
				{
				d=output->get_voxel_by_dir(u,v,w,direction);
				ll=(v<max_v-1 && u>0)? output->get_voxel_by_dir(u-1,v+1,w,direction) : initvalue;
				d=std::min(d,std::min(std::min(4+ll,3+lm),std::min(4+lr,3+mr)));
				output->set_voxel_by_dir(u,v,w,d,direction);
				mr=d;
				lr=lm;
				lm=ll;
				}
			}
		}
	//output->min_max_refresh();
	return output;
	}

//3D functions	
	
template <int IMAGEDIM>
void image_binary<IMAGEDIM>::fill_holes_3D(IMGBINARYTYPE object_value)
	{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim(0);
	max_v=this->get_size_by_dim(1);
	max_w=this->get_size_by_dim(2);
		
	IMGBINARYTYPE p;//pixel value
	int label,above,up,left;//neighbour labels 
    image_integer<int,IMAGEDIM> * label_image = new image_integer<int, IMAGEDIM> (max_u, max_v, max_w);
	int new_label;
	//init labels
	int number_of_objects=1;
	for(w=0; w<max_w; w++)
		{
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				p=this->get_voxel(u,v,w);
				if(p!=object_value)//Note: we want to label bkg-objects in order to remove holes
					{
					above=(w>0)? label_image->get_voxel(u,v,w-1) : 0;
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
					new_label=number_of_objects;
					if(above>0 && above<new_label)
						new_label=above;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					if(new_label==number_of_objects)
						number_of_objects++;
					label_image->set_voxel(u,v,w,new_label);
					}
				else
					label_image->set_voxel(u,v,w,0);
				}
			}
		}
	int* changes=new int[number_of_objects];
	int i;
	for(i=0; i<number_of_objects; i++)
		changes[i]=i;
	//merge labels
	for(w=0; w<max_w; w++)
		{
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				label=label_image->get_voxel(u,v,w);
				if(label>0)
					{
					above=(w>0)? label_image->get_voxel(u,v,w-1) : 0;
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
					while(label!=changes[label])
						label=changes[label];
					while(above!=changes[above])
						above=changes[above];
					while(up!=changes[up])
						up=changes[up];
					while(left!=changes[left])
						left=changes[left];
					new_label=label;
					if(above>0 && above<new_label)
						new_label=above;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					changes[label]=new_label;
					if(above>0)
						changes[above]=new_label;
					if(up>0)
						changes[up]=new_label;
					if(left>0)
						changes[left]=new_label;						
					}
				}
			}
		}
	int* lut=new int[number_of_objects];
	memset(lut, 0, sizeof(int)*number_of_objects);
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
	for(w=0; w<max_w; w++)
		{
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				label=label_image->get_voxel(u,v,w);
				if(lut[label]==1)//First label is background
					this->set_voxel(u,v,w,!object_value);
				else //all other labels are object
					this->set_voxel(u,v,w,object_value);
				}
			}
		}
	
	//this->image_has_changed();
	delete[] changes;
	delete[] lut;
	delete label_image;				
	}	
	

	template <int IMAGEDIM>
image_integer<short, IMAGEDIM> *  image_binary<IMAGEDIM>::distance_345_3D(bool edge_is_object, IMGBINARYTYPE object_value)
	{
	image_integer<short, IMAGEDIM>* output = new image_integer<short,IMAGEDIM> (this,false);
	int u,v,w;
	int max_u=this->get_size_by_dim(0);
	int max_v=this->get_size_by_dim(1);
	int max_w=this->get_size_by_dim(2);
	int veryhigh = (std::max(max_w,std::max(max_u,max_v))*5); //5 times the largest voxel direction... //std::numeric_limits<int>::max()-11;
	int initvalue = (edge_is_object)?veryhigh:0;
	IMGBINARYTYPE p;//pixel value
	int d,ul,um,ur,ml,mr,ll,lm,lr,aul,aum,aur,aml,amm,amr,all,alm,alr;//neighbour labels 
	//Forward pass

	for(w=0; w<max_w; w++){
		for(v=0; v<max_v; v++){
			u=0;
			ml=initvalue;
			ul=initvalue;
			aml=initvalue;
			aul=initvalue;
			all=initvalue;
			um=(v>0)? output->get_voxel(u,v-1,w) : initvalue;
			aum=(v>0 && w>0)? output->get_voxel(u,v-1,w-1) : initvalue;
			amm=(w>0)? output->get_voxel(u,v,w-1) : initvalue;
			alm=(v<max_v && w>0)? output->get_voxel(u,v+1,w-1) : initvalue;
			for(u=0; u<max_u; u++){
				p=this->get_voxel(u,v,w);
				ur=(u<max_u-1 && v>0)? output->get_voxel(u+1,v-1,w) : initvalue;
				aur=(u<max_u-1 && v>0 && w>0)? output->get_voxel(u+1,v-1,w-1) : initvalue;
				amr=(u<max_u-1 && w>0)? output->get_voxel(u+1,v,w-1) : initvalue;
				alr=(u<max_u-1 && v<max_v-1 && w>0)? output->get_voxel(u+1,v+1,w-1) : initvalue;
				d=(p==object_value)?std::min(
											std::min(
												std::min( std::min(5+aul,4+aum),std::min(5+aur,4+aml) ),
                                                std::min( std::min(3+amm,4+amr),std::min(5+all,4+alm) )
												),
											std::min( std::min( std::min(5+alr,4+ul),std::min(3+um,4+ur) ), 3+ml )
											):0;
				output->set_voxel(u,v,w,d);
				ml=d; //??? d is calculated...			//traverse the information minimie "get_voxel" calls...
				ul=um;
				um=ur;
				aul=aum;
				aum=aur;
				aml=amm;
				amm=amr;
				all=alm;
				alm=alr;
				}
			}
		}

	//Backward pass
	for(w=max_w-1; w>=0; w--){
		for(v=max_v-1; v>=0; v--){
			u=max_u-1;
			mr=initvalue;
//			ur=initvalue; //bug --> ur-->lr		//another bug was fixed by setting all "w-1" --> "w+1"
			lr=initvalue;
			amr=initvalue;
			aur=initvalue;
			alr=initvalue;
			lm=(v<max_v-1)? output->get_voxel(u,v+1,w) : initvalue;
			aum=(v>0 && w<max_w-1)? output->get_voxel(u,v-1,w+1) : initvalue;
			amm=(w<max_w-1)? output->get_voxel(u,v,w+1) : initvalue;
			alm=(v<max_v && w<max_w-1)? output->get_voxel(u,v+1,w+1) : initvalue;

			for(u=max_u-1; u>=0; u--){
				p=this->get_voxel(u,v,w);
				d=output->get_voxel(u,v,w);
				ll=(u>0 && v<max_v-1)? output->get_voxel(u-1,v+1,w) : initvalue;
				aul=(u>0 && v>0 && w<max_w-1)? output->get_voxel(u-1,v-1,w+1) : initvalue;
				aml=(u>0 && w<max_w-1)? output->get_voxel(u-1,v,w+1) : initvalue;
				all=(u>0 && v<max_v-1 && w<max_w-1)? output->get_voxel(u-1,v+1,w+1) : initvalue;
				d=(p==object_value)?std::min(
                                            std::min(
												std::min( std::min(5+aul,4+aum),std::min(5+aur,4+aml) ),
                                                std::min( std::min(3+amm,4+amr),std::min(5+all,4+alm) )
												),
											std::min( 
												std::min(std::min(5+alr,4+ll),std::min(3+lm,4+lr) ),
                                                std::min(d,3+mr)
												)
											):0;
				output->set_voxel(u,v,w,d);
				mr=d;
				lr=lm;
				lm=ll;
				aur=aum;
				aum=aul;
				amr=amm;
				amm=aml;
				alr=alm;
				alm=all;
				}
			}
		}

	//output->min_max_refresh();
	return output;
	}

template <int IMAGEDIM>
image_label<3>* image_binary<IMAGEDIM>::label_connected_objects_3D(IMGBINARYTYPE object_value)
{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim(0);
	max_v=this->get_size_by_dim(1);
	max_w=this->get_size_by_dim(2);
		
	IMGBINARYTYPE p;//pixel value
	int label,above,up,left;//neighbour labels 
    image_integer<int,IMAGEDIM> * label_image = new image_integer<int,IMAGEDIM> (this);
	int new_label;
	//init labels
	int number_of_objects=1;
	vector<int> changes;
	changes.push_back(0);
	for(w=0; w<max_w; w++) {
		for(v=0; v<max_v; v++) {
			for(u=0; u<max_u; u++) {
				p=this->get_voxel(u,v,w);
				if(p==object_value)	{
					above=(w>0)? label_image->get_voxel(u,v,w-1) : 0;
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
					while(above!=changes[above])
						above=changes[above];
					while(up!=changes[up])
						up=changes[up];
					while(left!=changes[left])
						left=changes[left];
					new_label=number_of_objects;
					if(above>0 && above<new_label)
						new_label=above;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					if(above>new_label)
						changes[above]=new_label;
					if(up>new_label)
						changes[up]=new_label;
					if(left>new_label)
						changes[left]=new_label;
					if(new_label==number_of_objects) {
						changes.push_back(number_of_objects);
						number_of_objects++;	
					}
					label_image->set_voxel(u,v,w,new_label);
				}
				else
					label_image->set_voxel(u,v,w,0);
			}
		}
	}
	
	// calculate look-up-table
	int* lut=new int[number_of_objects];
	memset(lut, 0, sizeof(int)*number_of_objects);
	new_label=1;
	for(int i=1; i<number_of_objects; i++) {
		label=i;
		while(label!=changes[label])
			label=changes[label];
		if(label==i) {
			lut[i]=new_label;
			new_label++;
		}
		else
			lut[i]=lut[label];
	}
	
    // set new labels from look-up table
	image_label<IMAGEDIM> * label_image_label = new image_label<IMAGEDIM> (label_image);
	for(w=0; w<max_w; w++) {
		for(v=0; v<max_v; v++) {
			for(u=0; u<max_u; u++) {
				label=label_image->get_voxel(u,v,w);
				if (label>0)
					label_image_label->set_voxel(u,v,w,lut[label]);
			}
		}
	}
	delete[] lut;
	delete label_image;
	return label_image_label;
}

template <int IMAGEDIM>
image_integer<unsigned long,3>* image_binary<IMAGEDIM>::label_connected_objects_with_volume_3D(IMGBINARYTYPE object_value)
{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim(0);
	max_v=this->get_size_by_dim(1);
	max_w=this->get_size_by_dim(2);
		
	IMGBINARYTYPE p;//pixel value
	int label,above,up,left;//neighbour labels 
    image_integer<unsigned long,IMAGEDIM> * label_image = new image_integer<unsigned long,IMAGEDIM> (this);
	int new_label;
	//init labels
	int number_of_objects=1;
	vector<int> changes;
	changes.push_back(0);
	vector<int> sizes;
	sizes.push_back(0);
	for(w=0; w<max_w; w++) {
		for(v=0; v<max_v; v++) {
			for(u=0; u<max_u; u++) {
				p=this->get_voxel(u,v,w);
				if(p==object_value)	{
					above=(w>0)? label_image->get_voxel(u,v,w-1) : 0;
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
					while(above!=changes[above])
						above=changes[above];
					while(up!=changes[up])
						up=changes[up];
					while(left!=changes[left])
						left=changes[left];
					new_label=number_of_objects;
					if(above>0 && above<new_label)
						new_label=above;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					if(above>new_label)
						changes[above]=new_label;
					if(up>new_label)
						changes[up]=new_label;
					if(left>new_label)
						changes[left]=new_label;
					if(new_label==number_of_objects) {
						changes.push_back(number_of_objects);
						sizes.push_back(0);
						number_of_objects++;	
					}
					label_image->set_voxel(u,v,w,new_label);
					sizes[new_label]++;
				}
				else
					label_image->set_voxel(u,v,w,0);
			}
		}
	}
	
	// calculate look-up-table and sizes
	int* lut=new int[number_of_objects];
	memset(lut, 0, sizeof(int)*number_of_objects);
	vector<int> tot_sizes;
	tot_sizes.push_back(0);
	new_label=1;
	for(int i=1; i<number_of_objects; i++) {
		label=i;
		while(label!=changes[label])
			label=changes[label];
		if(label==i) {
			lut[i]=new_label;
			new_label++;
			tot_sizes.push_back(0);
		}
		else
			lut[i]=lut[label];
		tot_sizes[lut[i]]+=sizes[i];
	}
	
    // set new labels from look-up table and sizes
	for(w=0; w<max_w; w++) {
		for(v=0; v<max_v; v++) {
			for(u=0; u<max_u; u++) {
				label=label_image->get_voxel(u,v,w);
				if (label>0) {
					label_image->set_voxel(u,v,w,tot_sizes[lut[label]]);
				}
			}
		}
	}
	delete[] lut;
	return label_image;
}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::largest_object_3D(IMGBINARYTYPE object_value)
	{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim(0);
	max_v=this->get_size_by_dim(1);
	max_w=this->get_size_by_dim(2);
		
	IMGBINARYTYPE p;//pixel value
	int label,above,up,left;//neighbour labels 
    image_integer<int,IMAGEDIM> * label_image = new image_integer<int, IMAGEDIM> (max_u, max_v, max_w);
	int new_label;
	//init labels
	int number_of_objects=1;
	for(w=0; w<max_w; w++)
		{
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				p=this->get_voxel(u,v,w);
				if(p==object_value)
					{
					above=(w>0)? label_image->get_voxel(u,v,w-1) : 0;
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
					new_label=number_of_objects;
					if(above>0 && above<new_label)
						new_label=above;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					if(new_label==number_of_objects)
						number_of_objects++;
					label_image->set_voxel(u,v,w,new_label);
					}
				else
					label_image->set_voxel(u,v,w,0);
				}
			}
		}
	int* changes=new int[number_of_objects];
	int i;
	int* sizes=new int[number_of_objects];
	memset(sizes, 0, sizeof(int)*number_of_objects);
	for(i=0; i<number_of_objects; i++)
		changes[i]=i;
	//merge labels
	for(w=0; w<max_w; w++)
		{
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				label=label_image->get_voxel(u,v,w);
				if(label>0)
					{
					above=(w>0)? label_image->get_voxel(u,v,w-1) : 0;
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
					while(label!=changes[label])
						label=changes[label];
					while(above!=changes[above])
						above=changes[above];
					while(up!=changes[up])
						up=changes[up];
					while(left!=changes[left])
						left=changes[left];
					new_label=label;
					if(above>0 && above<new_label)
						new_label=above;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					changes[label]=new_label;
					if(above>0)
						changes[above]=new_label;
					if(up>0)
						changes[up]=new_label;
					if(left>0)
						changes[left]=new_label;
					sizes[new_label]++;									
					}
				}
			}
		}
	int* lut=new int[number_of_objects];
	memset(lut, 0, sizeof(int)*number_of_objects);
	int* tot_sizes=new int[number_of_objects];
	memset(tot_sizes, 0, sizeof(int)*number_of_objects);
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
		tot_sizes[lut[i]]+=sizes[i];
		}
	int max_size=0;
	int max_label=1;
	for(i=1; i<new_label; i++)
		{
		if(tot_sizes[i]>max_size)
			{
			max_size=tot_sizes[i];
			max_label=i;
			}
		}
	//set all labels !=1 -> obj
	for(w=0; w<max_w; w++)
		{
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				label=label_image->get_voxel(u,v,w);
				if(lut[label]==max_label)//First label is background
					this->set_voxel(u,v,w,object_value);
				else //all other labels are object
					this->set_voxel(u,v,w,!object_value);
				}
			}
		}
	
	//this->image_has_changed();
	delete[] changes;
	delete[] sizes;
	delete[] lut;
	delete[] tot_sizes;
	delete label_image;
	}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::largest_objects_3D(int num_objects, IMGBINARYTYPE object_value)
{
	image_binary<3> *res = new image_binary<3>(this,0);
	res->fill(!object_value);

	image_binary<3> *tmp;
	for(int i=0;i<num_objects;i++){
		cout<<"i="<<i<<endl;
		tmp = new image_binary<3>(this);
		tmp->mask_out(res,!object_value);	//remove already selected objects...
		tmp->largest_object_3D();
		res->combine(tmp,COMB_MAX);
		cout<<"res_vol="<<res->get_number_of_voxels_with_value(object_value)<<endl;
		delete tmp;
	}

	copy_data(res,this);
}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::erode_2D(int thickness, int direction, IMGBINARYTYPE object_value)
	{		
	bool edge_is_object=false;
    image_integer<short, IMAGEDIM> * distance_image = distance_34_2D(edge_is_object, direction, object_value);
	image_binary <IMAGEDIM> * threshold_image = distance_image->threshold(thickness+1, std::numeric_limits<short>::max(), object_value);
	delete distance_image; 
	copy_data(threshold_image,this);
	delete threshold_image;
	}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::dilate_2D(int thickness, int direction, IMGBINARYTYPE object_value)
	{		
	bool edge_is_object=true;
    image_integer<short, IMAGEDIM> * distance_image = distance_34_2D(edge_is_object, direction, !object_value);
	image_binary <IMAGEDIM> * threshold_image = distance_image->threshold(0, thickness, object_value);
	delete distance_image;
	copy_data(threshold_image,this);
	delete threshold_image;
	}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::outline_2D(int thickness, int direction, IMGBINARYTYPE object_value)
	{		
	bool edge_is_object=false;
    image_integer<short, IMAGEDIM> * distance_image = distance_34_2D(edge_is_object, direction, object_value);
	image_binary <IMAGEDIM> * threshold_image = distance_image->threshold(3, thickness, object_value);
	delete distance_image;
	copy_data(threshold_image,this);
	delete threshold_image;
	}
	
template <int IMAGEDIM>
void image_binary<IMAGEDIM>::erode_3D(int thickness, IMGBINARYTYPE object_value)
	{		
	bool edge_is_object=false;
    image_integer<short, IMAGEDIM> * distance_image = distance_345_3D(edge_is_object, object_value);
	image_binary <IMAGEDIM> * threshold_image = distance_image->threshold(thickness+1, std::numeric_limits<short>::max(), object_value);
	delete distance_image;
	copy_data(threshold_image,this);
	delete threshold_image;
	}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::dilate_3D(int thickness, IMGBINARYTYPE object_value)
	{		
	bool edge_is_object=true;
    image_integer<short, IMAGEDIM> * distance_image = distance_345_3D(edge_is_object, !object_value);
	image_binary <IMAGEDIM> * threshold_image = distance_image->threshold(0, thickness, object_value);
	delete distance_image;
	copy_data(threshold_image,this);
	delete threshold_image;
	}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::outline_3D(int thickness, IMGBINARYTYPE object_value)
	{		
	bool edge_is_object=false;
    image_integer<short, IMAGEDIM> * distance_image = distance_345_3D(edge_is_object, object_value);
	image_binary <IMAGEDIM> * threshold_image = distance_image->threshold(3, thickness, object_value);
	delete distance_image;
	copy_data(threshold_image,this);
	delete threshold_image;
	}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::erode_3D_26Nbh(IMGBINARYTYPE object_value)
{
	dilate_3D_26Nbh(!object_value);
}


template <int IMAGEDIM>
void image_binary<IMAGEDIM>::dilate_3D_26Nbh(IMGBINARYTYPE object_value)
{
	int x,y,z,r,s,t;
    image_binary<IMAGEDIM> * res = new image_binary<IMAGEDIM>(this,0); //resulting image...
	res->set_parameters(this); //copy rotation and size infor to tmp image first...

//	cout<<"object_value="<<object_value<<endl;
//	cout<<"!object_value="<<!object_value<<endl;
	res->fill(!object_value);

	int max_x=this->get_size_by_dim(0);
	int max_y=this->get_size_by_dim(1);
	int max_z=this->get_size_by_dim(2);
		
	int radius = 1;

	//x,y,z
	for(z=0; z<max_z; z++){
		for(y=0; y<max_y; y++){
			for(x=0; x<max_x; x++){
				//if you find an object voxel... 
				//fill the whole neighbourhood in the resulting image...
				
				if(this->get_voxel(x,y,z)==object_value){
					//r,s,t
					for(t=std::max(0,z-radius); t<=std::min(max_z-1,z+radius); t++){
						for(s=std::max(0,y-radius); s<=std::min(max_y-1,y+radius); s++){
							for(r=std::max(0,x-radius); r<=std::min(max_x-1,x+radius); r++){
								res->set_voxel(r,s,t,object_value);
							}
						}
					}
				}

			}
		}
	}

	copy_data(res,this);
	delete res;
}


template <int IMAGEDIM>
int image_binary<IMAGEDIM>::find_voxel_index_percent_object_content(int dir, int object_content_percent, IMGBINARYTYPE object_value)
{
	float total = this->get_number_of_voxels_with_value(object_value);
	int p = object_content_percent;
	if(p<0 || p>100){
		pt_error::error("find_voxel_index_percent_object_content - Strange percent value...",pt_error::debug);
	}

	float limit = float(p)/100.0*total;
	float current_number=0;
	int backup_return_value=0;

	switch(dir){
	case 0:
		for(int x=0; x<this->datasize[0]; x++){
			for(int z=0; z<this->datasize[2]; z++){		
				for(int y=0; y<this->datasize[1]; y++){
					if(this->get_voxel(x,y,z) == object_value){
						current_number++;
					}
				}
			}
			if(current_number>=limit){
				return x;
			}
			backup_return_value=x;
		}
		break;
	case 1: 
		for(int y=0; y<this->datasize[1]; y++){
			for(int z=0; z<this->datasize[2]; z++){		
				for(int x=0; x<this->datasize[0]; x++){
					if(this->get_voxel(x,y,z) == object_value){
						current_number++;
					}
				}
			}
			if(current_number>=limit){
				return y;
			}
			backup_return_value=y;
		}
		break;
	case 2: 
		for(int z=0; z<this->datasize[2]; z++){		
			for(int y=0; y<this->datasize[1]; y++){
				for(int x=0; x<this->datasize[0]; x++){
					if(this->get_voxel(x,y,z) == object_value){
						current_number++;
					}
				}
			}
			if(current_number>=limit){
				return z;
			}
			backup_return_value=z;
		}
		break;
	default:
		pt_error::error("find_voxel_index_percent_object_content, erroneous direction...",pt_error::debug);
		break;
	}

	return backup_return_value;
}


template <int IMAGEDIM>
void image_binary<IMAGEDIM>::convex_hull_line_filling_3D(int dir, IMGBINARYTYPE object_value)
{
	int first;
	int last;
	int sx = this->datasize[0];
	int sy = this->datasize[1];
	int sz = this->datasize[2];

	switch(dir){
	case 0:
		for(int z=0; z<sz; z++){		
			for(int y=0; y<sy; y++){
				first=-1;
				last=-1;
				for(int x=0; x<sx; x++){
					if(this->get_voxel(x,y,z) == object_value){
						if(first==-1){
							first = x;
						}else{
							last = x;
						}
					}
				}
				if(last>-1){
					this->draw_line_2D(first,y,last,y,z,object_value);
				}
			}
		}
		break;
	case 1:
		for(int z=0; z<sz; z++){		
			for(int x=0; x<sx; x++){
				first=-1;
				last=-1;
				for(int y=0; y<sy; y++){
					if(this->get_voxel(x,y,z) == object_value){
						if(first==-1){
							first = y;
						}else{
							last = y;
						}
					}
				}
				if(last>-1){
					this->draw_line_2D(x,first,x,last,z,object_value);
				}
			}
		}
		break;
	case 2:
		for(int y=0; y<sy; y++){
			for(int x=0; x<sx; x++){
				first=-1;
				last=-1;
				for(int z=0; z<sz; z++){		
					if(this->get_voxel(x,y,z) == object_value){
						if(first==-1){
							first = z;
						}else{
							last = z;
						}
					}
				}
				if(last>-1){
					for(int z=first; z<=last; z++){	
						this->set_voxel(x,y,z,object_value);
					}
				}
			}
		}
		break;
	default:
		pt_error::error("convex_hull_line_filling_3D, erroneous direction...",pt_error::debug);
		break;
	}

}


template <int IMAGEDIM>
void image_binary<IMAGEDIM>::get_num_neighbours_distribution_3D_26Nbh(vector<int> &num_nb, vector<int> &num_vox, IMGBINARYTYPE object_value)
{
	int nx = this->nx();
	int ny = this->ny();
	int nz = this->nz();
	int tmp;
	
	num_nb.clear();
	num_vox.clear();
	for(int i=0; i<=26; i++){
		num_nb.push_back(i);
		num_vox.push_back(0);
	}

	for(int z=0; z<nz; z++){		
		for(int y=0; y<ny; y++){
			for(int x=0; x<nx; x++){
				if(this->get_voxel(x,y,z)==object_value){
					tmp=0;

					for(int t=std::max(0,z-1); t<=std::min(z+1,nz-1); t++){
						for(int s=std::max(0,y-1); s<=std::min(y+1,ny-1); s++){
							for(int r=std::max(0,x-1); r<=std::min(x+1,nx-1); r++){
								if(this->get_voxel(r,s,t)==object_value){
									tmp++;
								}
							}
						}
					}

					num_vox[tmp]++;

				}

			}
		}
	}

	int inside_limit = 17;
	float border=0;
	float sum=0;

	for(int i=0; i<=26; i++){
		sum += num_vox[i];
		if(i<=inside_limit){
			border += num_vox[i];
		}
	}

	cout<<"num_nb\tnum_vox\percent"<<endl;
	for(int i=0; i<=26; i++){
		cout<<num_nb[i]<<"\t"<<num_vox[i]<<"\t"<<float(num_vox[i])/sum<<endl;
	}

	cout<<"border_percent="<<border/sum<<endl;
}


template <int IMAGEDIM>
float image_binary<IMAGEDIM>::get_border_volume_ratio_3D_26Nbh(int num_nb_inside_limit, IMGBINARYTYPE object_value=TRUE)
{
	vector<int> num_nb;
	vector<int> num_vox;
	this->get_num_neighbours_distribution_3D_26Nbh(num_nb, num_vox, object_value);

	float border=0;
	float sum=0;

	for(int i=0; i<=26; i++){
		sum += num_vox[i];
		if(i<=num_nb_inside_limit){
			border += num_vox[i];
		}
	}
	return border/sum;
}
