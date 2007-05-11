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
void image_binary<IMAGEDIM>::fill_holes_2D(int direction, bool object_value)
	{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim_and_dir(0,direction);
	max_v=this->get_size_by_dim_and_dir(1,direction);
	max_w=this->get_size_by_dim_and_dir(2,direction);
		
	bool p;//pixel value
	int label,up,left;//neighbour labels 
	//image_general<int, IMAGEDIM> label_image(max_u, max_v, max_w);
    image_integer<short, IMAGEDIM> * label_image = new image_integer<short, IMAGEDIM> (max_u, max_v, max_w);
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
		}				
	}
	
template <int IMAGEDIM>
void image_binary<IMAGEDIM>::largest_object_2D(int direction, bool object_value)
	{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim_and_dir(0,direction);
	max_v=this->get_size_by_dim_and_dir(1,direction);
	max_w=this->get_size_by_dim_and_dir(2,direction);
		
	bool p;//pixel value
	int label,up,left;//neighbour labels 
	//image_general<int, IMAGEDIM> label_image(max_u, max_v, max_w);
    image_integer<short,IMAGEDIM> * label_image = new image_integer<short, IMAGEDIM> (max_u, max_v, max_w);
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
		}				
	}
	
template <int IMAGEDIM>
void image_binary<IMAGEDIM>::threshold_size_2D(int min_size, int direction, bool object_value)
	{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim_and_dir(0,direction);
	max_v=this->get_size_by_dim_and_dir(1,direction);
	max_w=this->get_size_by_dim_and_dir(2,direction);
		
	bool p;//pixel value
	int label,up,left;//neighbour labels 
	//image_general<int, IMAGEDIM> label_image(max_u, max_v, max_w);
    image_integer<short,IMAGEDIM> * label_image = new image_integer<short, IMAGEDIM> (max_u, max_v, max_w);
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
		}				
	}
	
template <int IMAGEDIM>
void image_binary<IMAGEDIM>::cog_inside_2D(image_binary<IMAGEDIM>* mask, int direction, bool object_value)
	{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim_and_dir(0,direction);
	max_v=this->get_size_by_dim_and_dir(1,direction);
	max_w=this->get_size_by_dim_and_dir(2,direction);
		
	bool p,m;//pixel value
	int label,up,left;//neighbour labels 
	//image_general<int, IMAGEDIM> label_image(max_u, max_v, max_w);
    image_integer<short,IMAGEDIM> * label_image = new image_integer<short, IMAGEDIM> (max_u, max_v, max_w);
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
		}				
	}
	
template <int IMAGEDIM>
image_integer<short, IMAGEDIM> *  image_binary<IMAGEDIM>::distance_34_2D(bool edge_is_object, int direction, bool object_value)
	{
	image_integer<short, IMAGEDIM>* output = new image_integer<short,IMAGEDIM> (this,false);
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim_and_dir(0,direction);
	max_v=this->get_size_by_dim_and_dir(1,direction);
	max_w=this->get_size_by_dim_and_dir(2,direction);
	int veryhigh = std::max(max_u,max_v)*2;//std::numeric_limits<int>::max()-9;
	int initvalue=(edge_is_object)?veryhigh:0;
	bool p;//pixel value
	int d,ul,um,ur,ml,mr,ll,lm,lr;//neighbour labels 
	for(w=0; w<max_w; w++)
		{
		//Forward pass
		for(v=0; v<max_v; v++)
			{
			u=0;
			ml=initvalue;
			ul=initvalue;
			um=(v>0)? output->get_voxel(u,v-1,w) : initvalue;
			for(u=0; u<max_u; u++)
				{
				p=this->get_voxel_by_dir(u,v,w,direction);
				ur=(v>0 && u<max_u-1)? output->get_voxel(u+1,v-1,w) : initvalue;
				d=(p==object_value)?std::min(std::min(4+ul,3+um),std::min(4+ur,3+ml)):0;
				output->set_voxel(u,v,w,d);
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
			lm=(v<max_v-1)? output->get_voxel(u,v+1,w) : initvalue;
			for(u=max_u-1; u>=0; u--)
				{
				d=output->get_voxel(u,v,w);
				ll=(v<max_v-1 && u>0)? output->get_voxel(u-1,v+1,w) : initvalue;
				d=std::min(d,std::min(std::min(4+ll,3+lm),std::min(4+lr,3+mr)));
				output->set_voxel(u,v,w,d);
				mr=d;
				lr=lm;
				lm=ll;
				}
			}
		}
	return output;
	}

//3D functions	
	
template <int IMAGEDIM>
void image_binary<IMAGEDIM>::fill_holes_3D(bool object_value)
	{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim(0);
	max_v=this->get_size_by_dim(1);
	max_w=this->get_size_by_dim(2);
		
	bool p;//pixel value
	int label,above,up,left;//neighbour labels 
    image_integer<short,IMAGEDIM> * label_image = new image_integer<short, IMAGEDIM> (max_u, max_v, max_w);
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
	}	
		
template <int IMAGEDIM>
image_integer<short, IMAGEDIM> *  image_binary<IMAGEDIM>::distance_345_3D(bool edge_is_object, bool object_value)
	{
	image_integer<short, IMAGEDIM>* output = new image_integer<short,IMAGEDIM> (this,false);
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim(0);
	max_v=this->get_size_by_dim(1);
	max_w=this->get_size_by_dim(2);
	int veryhigh = (std::max(max_w,std::max(max_u,max_v))*5)/2;//std::numeric_limits<int>::max()-11;
	int initvalue=(edge_is_object)?veryhigh:0;
	bool p;//pixel value
	int d,ul,um,ur,ml,mr,ll,lm,lr,aul,aum,aur,aml,amm,amr,all,alm,alr;//neighbour labels 
	for(w=0; w<max_w; w++)
		{
		//Forward pass
		for(v=0; v<max_v; v++)
			{
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
			for(u=0; u<max_u; u++)
				{
				p=this->get_voxel(u,v,w);
				ur=(u<max_u-1 && v>0)? output->get_voxel(u+1,v-1,w) : initvalue;
				aur=(u<max_u-1 && v>0 && w>0)? output->get_voxel(u+1,v-1,w-1) : initvalue;
				amr=(u<max_u-1 && w>0)? output->get_voxel(u+1,v,w-1) : initvalue;
				alr=(u<max_u-1 && v<max_v-1 && w>0)? output->get_voxel(u+1,v+1,w-1) : initvalue;
				d=(p==object_value)?std::min(
                                             std::min(
                                                           std::min(
                                                                    std::min(5+aul,4+aum),
                                                                    std::min(5+aur,4+aml)
												),
                                                      std::min(
                                                               std::min(3+amm,4+amr),
                                                               std::min(5+all,4+alm)
												)
											),
                                             std::min(
                                                      std::min(
                                                               std::min(5+alr,4+ul),
                                                               std::min(3+um,4+ur)
												),
											3+ml
											)
										):0;
				output->set_voxel(u,v,w,d);
				ml=d;
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
	for(w=max_w-1; w>=0; w--)
		{
		for(v=max_v-1; v>=0; v--)
			{
			u=max_u-1;
			mr=initvalue;
			ur=initvalue;
			amr=initvalue;
			aur=initvalue;
			alr=initvalue;
			lm=(v<max_v-1)? output->get_voxel(u,v+1,w) : initvalue;
			aum=(v>0 && w<max_w-1)? output->get_voxel(u,v-1,w+1) : initvalue;
			amm=(w<max_w-1)? output->get_voxel(u,v,w+1) : initvalue;
			alm=(v<max_v && w<max_w-1)? output->get_voxel(u,v+1,w+1) : initvalue;
			for(u=max_u-1; u>=0; u--)
				{
				p=this->get_voxel(u,v,w);
				ll=(u>0 && v<max_v-1)? output->get_voxel(u-1,v+1,w) : initvalue;
				aul=(u>0 && v>0 && w<max_w-1)? output->get_voxel(u+1,v-1,w-1) : initvalue;
				aml=(u>0 && w<max_w-1)? output->get_voxel(u+1,v,w-1) : initvalue;
				all=(u>0 && v<max_v-1 && w<max_w-1)? output->get_voxel(u+1,v+1,w-1) : initvalue;
				d=(p==object_value)?std::min(
                                             std::min(
											std::min(
                                                     std::min(5+aul,4+aum),
                                                     std::min(5+aur,4+aml)
												),
                                                      std::min(
                                                               std::min(3+amm,4+amr),
                                                               std::min(5+all,4+alm)
												)
											),
                                             std::min(
                                                      std::min(
                                                               std::min(5+alr,4+ll),
                                                               std::min(3+lm,4+lr)
												),
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
	return output;
	}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::largest_object_3D(bool object_value)
	{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=get_size_by_dim(0);
	max_v=get_size_by_dim(1);
	max_w=get_size_by_dim(2);
		
	bool p;//pixel value
	int label,above,up,left;//neighbour labels 
    image_integer<short,IMAGEDIM> * label_image = new image_integer<short, IMAGEDIM> (max_u, max_v, max_w);
	int new_label;
	//init labels
	int number_of_objects=1;
	for(w=0; w<max_w; w++)
		{
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				p=get_voxel(u,v,w);
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
	int* sizes=new int[number_of_objects];
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
					set_voxel(u,v,w,object_value);
				else //all other labels are object
					set_voxel(u,v,w,!object_value);
				}
			}
		}
	}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::erode_2D(int thickness, int direction, bool object_value)
	{		
	bool edge_is_object=false;
    image_integer<short, IMAGEDIM> * distance_image = distance_34_2D(edge_is_object, direction, object_value);
	this->copy_image(distance_image->threshold(thickness+1, std::numeric_limits<short>::max(), object_value));
	}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::dilate_2D(int thickness, int direction, bool object_value)
	{		
	bool edge_is_object=true;
    image_integer<short, IMAGEDIM> * distance_image = distance_34_2D(edge_is_object, direction, !object_value);
	this->copy_image(distance_image->threshold(0, thickness, object_value));
	}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::outline_2D(int thickness, int direction, bool object_value)
	{		
	bool edge_is_object=false;
    image_integer<short, IMAGEDIM> * distance_image = distance_34_2D(edge_is_object, direction, object_value);
	this->copy_image(distance_image->threshold(3, thickness, object_value));
	}
	
template <int IMAGEDIM>
void image_binary<IMAGEDIM>::erode_3D(int thickness, bool object_value)
	{		
	bool edge_is_object=false;
    image_integer<short, IMAGEDIM> * distance_image = distance_345_3D(edge_is_object, object_value);
	this->copy_image(distance_image->threshold(thickness+1, std::numeric_limits<short>::max(), object_value));
	}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::dilate_3D(int thickness, bool object_value)
	{		
	bool edge_is_object=true;
    image_integer<short, IMAGEDIM> * distance_image = distance_345_3D(edge_is_object, !object_value);
	this->copy_image(distance_image->threshold(0, thickness, object_value));
	}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::outline_3D(int thickness, bool object_value)
	{		
	bool edge_is_object=false;
    image_integer<short, IMAGEDIM> * distance_image = distance_345_3D(edge_is_object, object_value);
	this->copy_image(distance_image->threshold(3, thickness, object_value));
	}