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


template <class ELEMTYPE, int IMAGEDIM>
image_scalar<double,3>* image_scalar<ELEMTYPE, IMAGEDIM>::get_num_diff_image_1storder_central_diff_3D(int direction)
{	
	image_scalar<double,3> *res = new image_scalar<double,3>(this);
	if(direction==0){
		for (int k=0; k < this->datasize[2]; k++){
			for (int j=0; j < this->datasize[1]; j++){
				res->set_voxel(0,j,k, double(get_voxel(1,j,k)-get_voxel(0,j,k)));				//forward diff
				for (int i=1; i < this->datasize[0]-1; i++){
					res->set_voxel(i,j,k,0.5* double(get_voxel(i+1,j,k)-get_voxel(i-1,j,k)));	//central diff
				}
				res->set_voxel(this->datasize[0]-1,j,k, double(get_voxel(this->datasize[0]-1,j,k)-get_voxel(this->datasize[0]-2,j,k)));//backward diff
			}
		}

	}else if(direction==1){
		for (int k=0; k < this->datasize[2]; k++){
			for (int i=0; i < this->datasize[0]; i++){
				res->set_voxel(i,0,k, double(get_voxel(i,1,k)-get_voxel(i,0,k)));				//forward diff
				for (int j=1; j < this->datasize[1]-1; j++){
					res->set_voxel(i,j,k,0.5* double(get_voxel(i,j+1,k)-get_voxel(i,j-1,k)));	//central diff
				}
				res->set_voxel(i,this->datasize[1]-1,k, double(get_voxel(i,this->datasize[1]-1,k)-get_voxel(i,this->datasize[1]-2,k)));//backward diff
			}
		}
	}else{  // direction==2
		for (int j=0; j < this->datasize[1]; j++){
			for (int i=0; i < this->datasize[0]; i++){
				res->set_voxel(i,j,0, double(get_voxel(i,j,1)-get_voxel(i,j,0)));				//forward diff
				for (int k=1; k < this->datasize[2]-1; k++){
					res->set_voxel(i,j,k,0.5* double(get_voxel(i,j,k+1)-get_voxel(i,j,k-1)));	//central diff
				}
				res->set_voxel(i,j,this->datasize[2]-1, double(get_voxel(i,j,this->datasize[2]-1)-get_voxel(i,j,this->datasize[2]-2)));//backward diff
			}
		}
	}

	return res;
}


template <class ELEMTYPE, int IMAGEDIM>
image_scalar<double,3>* image_scalar<ELEMTYPE, IMAGEDIM>::get_num_diff_image_2ndorder_central_diff_3D(image_scalar<double,3>*df_dir1, int direction2)
{	
	image_scalar<double,3> *res = new image_scalar<double,3>(this);
	if(direction2==0){
		for (int k=0; k < this->datasize[2]; k++){
			for (int j=0; j < this->datasize[1]; j++){
				res->set_voxel(0,j,k, double(df_dir1->get_voxel(1,j,k)-df_dir1->get_voxel(0,j,k)));				//forward diff
				for (int i=1; i < this->datasize[0]-1; i++){
					res->set_voxel(i,j,k,0.5* double(df_dir1->get_voxel(i+1,j,k)-df_dir1->get_voxel(i-1,j,k)));	//central diff
				}
				res->set_voxel(this->datasize[0]-1,j,k, double(df_dir1->get_voxel(this->datasize[0]-1,j,k)-df_dir1->get_voxel(this->datasize[0]-2,j,k)));//backward diff
			}
		}
	}else if(direction2==1){
		for (int k=0; k < this->datasize[2]; k++){
			for (int i=0; i < this->datasize[0]; i++){
				res->set_voxel(i,0,k, double(df_dir1->get_voxel(i,1,k)-df_dir1->get_voxel(i,0,k)));				//forward diff
				for (int j=1; j < this->datasize[1]-1; j++){
					res->set_voxel(i,j,k,0.5* double(df_dir1->get_voxel(i,j+1,k)-df_dir1->get_voxel(i,j-1,k)));	//central diff
				}
				res->set_voxel(i,this->datasize[1]-1,k, double(df_dir1->get_voxel(i,this->datasize[1]-1,k)-df_dir1->get_voxel(i,this->datasize[1]-2,k)));//backward diff
			}
		}
	}else{  // direction==2
		for (int j=0; j < this->datasize[1]; j++){
			for (int i=0; i < this->datasize[0]; i++){
				res->set_voxel(i,j,0, double(df_dir1->get_voxel(i,j,1)-df_dir1->get_voxel(i,j,0)));				//forward diff
				for (int k=1; k < this->datasize[2]-1; k++){
					res->set_voxel(i,j,k,0.5* double(df_dir1->get_voxel(i,j,k+1)-df_dir1->get_voxel(i,j,k-1)));	//central diff
				}
				res->set_voxel(i,j,this->datasize[2]-1, double(df_dir1->get_voxel(i,j,this->datasize[2]-1)-df_dir1->get_voxel(i,j,this->datasize[2]-2)));//backward diff
			}
		}
	}

	return res;
}


template <class ELEMTYPE, int IMAGEDIM >
void image_scalar<ELEMTYPE, IMAGEDIM >::interpolate_trilinear_3D_vxl(image_scalar<ELEMTYPE, IMAGEDIM > *src_im)
	{	
		cout<<"image_scalar<ELEMTYPE, IMAGEDIM >::interpolate_trilinear_3D_vxl_speed..."<<endl;
		Vector3D rvoxpos;	//integer voxel location in reference image
		Matrix3D rdir2;		//precalculated reference direction data
//		Vector3D phys;		//physical location in target image
		Matrix3D sdir2;
		Vector3D a;
		Matrix3D b;
		Vector3D svox;		//non-truncated voxel location in source image
		int nx = src_im->datasize[0];
		int ny = src_im->datasize[1];
		int nz = src_im->datasize[2];
		int xstep = 1;
		int ystep = nx;
		int zstep = nx*ny;
		float dx = this->voxel_resize[0][0];
		float dy = this->voxel_resize[1][1];
		float dz = this->voxel_resize[2][2];
		float norm_dxdydz = sqrt(dx*dx + dy*dy + dz*dz);
//		double val=0;
		cout<<"nx/ny/nz/ xstep/ystep/zstep"<<nx<<"/"<<ny<<"/"<<nz<<"/ "<<xstep<<"/"<<ystep<<"/"<<zstep<<endl;

		//pre calculations for speedup...
		rdir2 = this->direction*this->voxel_resize;
		sdir2 = src_im->direction*src_im->voxel_resize;
		sdir2 = sdir2.GetInverse();
		a = sdir2*(this->origin - src_im->origin);
		b = sdir2*rdir2;


		for (int k=0; k < this->datasize[2]; k++)
		{
			cout<<".";
			for (int j=0; j < this->datasize[1]; j++)
			{
				for (int i=0; i < this->datasize[0]; i++)
				{
					//--------------------------------
					//phys coordinates need to be transfomed (origin/rotation/scaling) into "local" physical source coordinates
					//A version not optimized for speed would look something like:
					//phys = this->get_physical_pos_for_voxel(i,j,k);
					//vox = src_im->get_voxelpos_from_physical_pos_3D(phys);
					//--------------------------------

					//phys = this->origin + rdir2*rvoxpos;
					//svox = sdir2*(phys - src_im->origin);
					//svox = sdir2*(this->origin + rdir2*rvoxpos - src_im->origin);
					//svox = sdir2*(this->origin - src_im->origin) + sdir2*rdir2*rvoxpos 
					//svox =                 a                     +           b*rvoxpos

					rvoxpos[0]=i;	rvoxpos[1]=j;	rvoxpos[2]=k;
					svox = a + b*rvoxpos;
//JK2 //TODO				this->set_voxel(i,j,k,mil3d_safe_trilin_interp_3d<ELEMTYPE>(svox[0], svox[1], svox[2], src_im->dataptr, nx, ny, nz, xstep, ystep, zstep,dx,dy,dz,norm_dxdydz));
				}
			}
		}
	}

/*

	template <class ELEMTYPE, int IMAGEDIM >
		void image_scalar<ELEMTYPE, IMAGEDIM >::interpolate_tricubic_3D_libtricubic(image_scalar<ELEMTYPE, IMAGEDIM > *src_im)
	{	
		cout<<"image_scalar<ELEMTYPE, IMAGEDIM >::interpolate_tricubic_3D_libtricubic..."<<endl;
		Vector3D rvoxpos;	//integer voxel location in reference image
		Matrix3D rdir2;		//precalculated reference direction data
		Matrix3D sdir2;
		Vector3D a;
		Matrix3D b;
		Vector3D svox;		//non-truncated voxel location in source image

		ELEMTYPE val;


//		int nx = src_im->datasize[0];
//		int ny = src_im->datasize[1];
//		int nz = src_im->datasize[2];
//		int xstep = 1;
//		int ystep = nx;
//		int zstep = nx*ny;
		
		double dx = double(this->voxel_resize[0][0]);
		double dy = double(this->voxel_resize[1][1]);
		double dz = double(this->voxel_resize[2][2]);

		//pre calculations for speedup...
		rdir2 = this->direction*this->voxel_resize;
		sdir2 = src_im->direction*src_im->voxel_resize;
		sdir2 = sdir2.GetInverse();
		a = sdir2*(this->origin - src_im->origin);
		b = sdir2*rdir2;

		double fval[8];//={1.2, 2.3, 3.4, 4.5, 5.6, 6.7, 7.8, 8.9};
		double dfdxval[8];//={1.2, 2.3, 3.4, 4.5, 5.6, 6.7, 7.8, 8.9};
		double dfdyval[8];//={1.2, 2.3, 3.4, 4.5, 5.6, 6.7, 7.8, 8.9};
		double dfdzval[8];//={1.2, 2.3, 3.4, 4.5, 5.6, 6.7, 7.8, 8.9};
		double d2fdxdyval[8];//={1.2, 2.3, 3.4, 4.5, 5.6, 6.7, 7.8, 8.9};
		double d2fdxdzval[8];//={1.2, 2.3, 3.4, 4.5, 5.6, 6.7, 7.8, 8.9};
		double d2fdydzval[8];//={1.2, 2.3, 3.4, 4.5, 5.6, 6.7, 7.8, 8.9};
		double d3fdxdydzval[8];//={1.2, 2.3, 3.4, 4.5, 5.6, 6.7, 7.8, 8.9};

		double coeff[64];

		int id=0;
		int x =0;
		int y =0;
		int z =0;
		int xpos=0;
		int ypos=0;
		int zpos=0;

		///First we compute the 64 coefficients for the cube.
		///The first step is to scale the derivatives that have been computed
		///  in a rectangular box instead of the cube of side 1.
		///Notice that this step can be avoided by computing the numerical
		///  derivatives without dividing by the length of the boxes.
//		for (i=0;i<8;i++) {
//		fval[i]*=1.0;
//		dfdxval[i]*=dx;
//		dfdyval[i]*=dy;
//		dfdzval[i]*=dz;
//		d2fdxdyval[i]*=dx*dy;
//		d2fdxdzval[i]*=dx*dz;
//		d2fdydzval[i]*=dy*dz;
//		d3fdxdydzval[i]*=dx*dy*dz;
//		} 
		///Next we get the set of coefficient for this cube
//		tricubic_get_coeff(coeff,fval,dfdxval,dfdyval,dfdzval,d2fdxdyval,d2fdxdzval,d2fdydzval,d3fdxdydzval);
		///To get the value in the middle of the cube, we always use (.5,.5,.5)
		///  i.e. relative coordinates
//		f1=tricubic_eval(coeff,.5,.5,.5);
		///To get the value at a point x,y,z (with referrence to corner ID 0
		///  we devide by each length
//		f2=tricubic_eval(coeff,x/dx,y/dy,z/dz);
		///Derivatives can be computed similarly but need to be scaled by dx,dy,dz
//		dfdx=tricubic_eval(coeff,x/dx,y/dy,z/dz,1,0,0)/dx;
//		d2fdxdy=tricubic_eval(coeff,x/dx,y/dy,z/dz,1,1,0)/(dx*dy);

		image_scalar<double,3> *fx = get_num_diff_image_1storder_central_diff_3D(0);
		image_scalar<double,3> *fy = get_num_diff_image_1storder_central_diff_3D(1);
		image_scalar<double,3> *fz = get_num_diff_image_1storder_central_diff_3D(2);
		image_scalar<double,3> *fxy = get_num_diff_image_2ndorder_central_diff_3D(fx,1);
		image_scalar<double,3> *fxz = get_num_diff_image_2ndorder_central_diff_3D(fx,2);
		image_scalar<double,3> *fyz = get_num_diff_image_2ndorder_central_diff_3D(fy,2);
		image_scalar<double,3> *fxyz = get_num_diff_image_2ndorder_central_diff_3D(fxy,2);


		for (int k=0; k < this->datasize[2]; k++)
		{
			cout<<".";
			for (int j=0; j < this->datasize[1]; j++)
			{
				for (int i=0; i < this->datasize[0]; i++)
				{
					rvoxpos[0]=i;	rvoxpos[1]=j;	rvoxpos[2]=k;
					svox = a + b*rvoxpos;
					val = 0;
					if(src_im->is_voxelpos_inside_image_border_3D(svox,1))
					{
						for (id=0;id<8;id++) {
							switch(id){
								case 0:	x=0; y=0; z=0; break;
								case 1: x=1; y=0; z=0; break;
								case 2: x=0; y=1; z=0; break;
								case 3: x=1; y=1; z=0; break;
								case 4: x=0; y=0; z=1; break;
								case 5: x=1; y=0; z=1; break;
								case 6: x=0; y=1; z=1; break;
								case 7: x=1; y=1; z=1; break;
								default:x=0; y=0; z=0;
							}
							xpos = svox[0]+x;
							ypos = svox[1]+y;
							zpos = svox[2]+z;

							//fval[id]		= src_im->get_voxel(							xpos,ypos,zpos);
							//dfdxval[id]		= src_im->get_num_diff_1storder_central_diff_3D(xpos,ypos,zpos, 0);
							//dfdyval[id]		= src_im->get_num_diff_1storder_central_diff_3D(xpos,ypos,zpos, 1);
							//dfdzval[id]		= src_im->get_num_diff_1storder_central_diff_3D(xpos,ypos,zpos, 2);
							//d2fdxdyval[id]	= src_im->get_num_diff_2ndorder_central_diff_3D(xpos,ypos,zpos, 0, 1);
							//d2fdxdzval[id]	= src_im->get_num_diff_2ndorder_central_diff_3D(xpos,ypos,zpos, 0, 2);
							//d2fdydzval[id]	= src_im->get_num_diff_2ndorder_central_diff_3D(xpos,ypos,zpos, 1, 2);
							//d3fdxdydzval[id]= src_im->get_num_diff_3rdorder_central_diff_3D(xpos,ypos,zpos, 1, 2, 3);

							fval[id]		= src_im->get_voxel(xpos,ypos,zpos);
							dfdxval[id]		= fx->get_voxel(	xpos,ypos,zpos);
							dfdyval[id]		= fy->get_voxel(	xpos,ypos,zpos);
							dfdzval[id]		= fz->get_voxel(	xpos,ypos,zpos);
							d2fdxdyval[id]	= fxy->get_voxel(	xpos,ypos,zpos);
							d2fdxdzval[id]	= fxz->get_voxel(	xpos,ypos,zpos);
							d2fdydzval[id]	= fyz->get_voxel(	xpos,ypos,zpos);
							d3fdxdydzval[id]= fxyz->get_voxel(	xpos,ypos,zpos);
						}

						//Set the a-coefficients....				
						tricubic_get_coeff(coeff,fval,dfdxval,dfdyval,dfdzval,d2fdxdyval,d2fdxdzval,d2fdydzval,d3fdxdydzval);
						val = tricubic_eval(coeff,double(svox[0]-int(svox[0]))/dx,double(svox[1]-int(svox[1]))/dy,double(svox[2]-int(svox[2]))/dz);
					}
					this->set_voxel(i,j,k,val);
				}
			}
		}
		delete fx;
		delete fy;
		delete fz;
		delete fxy;
		delete fxz;
		delete fyz;
		delete fxyz;
	}
*/