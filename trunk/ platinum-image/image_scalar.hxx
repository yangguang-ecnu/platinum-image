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

#ifndef __image_scalar_hxx__
#define __image_scalar_hxx__

#include "image_scalar.h"
#include "image_general.hxx"

template <class ELEMTYPE, int IMAGEDIM>
image_scalar<double,3>* image_scalar<ELEMTYPE, IMAGEDIM>::get_num_diff_image_1storder_central_diff_3D(int direction)
{	
	image_scalar<double,3> *res = new image_scalar<double,3>(this);
	if(direction==0){
		for (int k=0; k < this->datasize[2]; k++){
			for (int j=0; j < this->datasize[1]; j++){
				res->set_voxel(0,j,k, double(this->get_voxel(1,j,k)-this->get_voxel(0,j,k)));				//forward diff
				for (int i=1; i < this->datasize[0]-1; i++){
					res->set_voxel(i,j,k,0.5* double(this->get_voxel(i+1,j,k)-this->get_voxel(i-1,j,k)));	//central diff
				}
				res->set_voxel(this->datasize[0]-1,j,k, double(get_voxel(this->datasize[0]-1,j,k)-get_voxel(this->datasize[0]-2,j,k)));//backward diff
			}
		}

	}else if(direction==1){
		for (int k=0; k < this->datasize[2]; k++){
			for (int i=0; i < this->datasize[0]; i++){
				res->set_voxel(i,0,k, double(this->get_voxel(i,1,k)-this->get_voxel(i,0,k)));				//forward diff
				for (int j=1; j < this->datasize[1]-1; j++){
					res->set_voxel(i,j,k,0.5* double(this->get_voxel(i,j+1,k)-this->get_voxel(i,j-1,k)));	//central diff
				}
				res->set_voxel(i,this->datasize[1]-1,k, double(get_voxel(i,this->datasize[1]-1,k)-get_voxel(i,this->datasize[1]-2,k)));//backward diff
			}
		}
	}else{  // direction==2
		for (int j=0; j < this->datasize[1]; j++){
			for (int i=0; i < this->datasize[0]; i++){
				res->set_voxel(i,j,0, double(this->get_voxel(i,j,1)-this->get_voxel(i,j,0)));				//forward diff
				for (int k=1; k < this->datasize[2]-1; k++){
					res->set_voxel(i,j,k,0.5* double(this->get_voxel(i,j,k+1)-this->get_voxel(i,j,k-1)));	//central diff
				}
				res->set_voxel(i,j,this->datasize[2]-1, double(get_voxel(i,j,this->datasize[2]-1)-this->get_voxel(i,j,this->datasize[2]-2)));//backward diff
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
	//cout<<"image_scalar<ELEMTYPE, IMAGEDIM >::interpolate_trilinear_3D_vxl_speed..."<<endl;
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

	Vector3D s = this->get_size();
	float dx = s[0];
	float dy = s[1];
	float dz = s[2];
	float norm_dxdydz = sqrt(dx*dx + dy*dy + dz*dz);
	//		double val=0;
	//cout<<"nx/ny/nz/ xstep/ystep/zstep"<<nx<<"/"<<ny<<"/"<<nz<<"/ "<<xstep<<"/"<<ystep<<"/"<<zstep<<endl;

	//pre calculations for speedup...
	rdir2 = this->orientation*this->get_voxel_resize();
	sdir2 = src_im->orientation*src_im->get_voxel_resize();
	sdir2 = sdir2.GetInverse();
	a = sdir2*(this->origin - src_im->origin);
	b = sdir2*rdir2;

//	char buffer[100];
//	char buffer2[100];
//	sprintf(buffer2, "%f", this->datasize[2] );              

	for (int k=0; k < this->datasize[2]; k++)
	{
//		sprintf(buffer, "%f", k );              
//		userIOmanagement.status_area->message( "Trilinear interpolation: " + string(buffer) + "/" + string(buffer2) );
		//cout<<".";
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
				this->set_voxel(i,j,k,mil3d_safe_trilin_interp_3d<ELEMTYPE>(svox[0], svox[1], svox[2], src_im->dataptr, nx, ny, nz, xstep, ystep, zstep,dx,dy,dz,norm_dxdydz));
			}
		}
	}
}



template <class ELEMTYPE, int IMAGEDIM >
void image_scalar<ELEMTYPE, IMAGEDIM >::set_a_coeff2(double a[64], double f[8], double dfdx[8], double dfdy[8], double dfdz[8], double d2fdxdy[8], double d2fdxdz[8], double d2fdydz[8], double d3fdxdydz[8]) 
{
	// Tricubic interpolation using method described in:
	// F. Lekien, J.E. Marsden
	// Tricubic Interpolation in Three Dimensions
	// International Journal for Numerical Methods in Engineering, 63 (3), 455-471, 2005
	// ...used in the function: image_scalar.interpolate_tricubic_3D...

	for (int i=0;i<64;i++) {
		a[i] = 0.0;
		for (int j=0;j<8;j++) {
			a[i] += A_tricubic[i][0+j]*f[j];
			a[i] += A_tricubic[i][8+j]*dfdx[j];
			a[i] += A_tricubic[i][16+j]*dfdy[j];
			a[i] += A_tricubic[i][24+j]*dfdz[j];
			a[i] += A_tricubic[i][32+j]*d2fdxdy[j];
			a[i] += A_tricubic[i][40+j]*d2fdxdz[j];
			a[i] += A_tricubic[i][48+j]*d2fdydz[j];
			a[i] += A_tricubic[i][56+j]*d3fdxdydz[j];
		}
	}
}


template <class ELEMTYPE, int IMAGEDIM >
double image_scalar<ELEMTYPE, IMAGEDIM >::tricubic_eval(double a[64], double x, double y, double z) 
{
	double ret = 0.0;
	for (int i=0;i<4;i++) {
		for (int j=0;j<4;j++) {
			for (int k=0;k<4;k++) {
				ret += a[i+4*j+16*k]*pow(x,i)*pow(y,j)*pow(z,k);
			}
		}
	}
	return(ret);
}


template <class ELEMTYPE, int IMAGEDIM >
void image_scalar<ELEMTYPE, IMAGEDIM >::interpolate_tricubic_3D(image_scalar<ELEMTYPE, IMAGEDIM > *src_im)
{	
	//cout<<"image_scalar<ELEMTYPE, IMAGEDIM >::interpolate_tricubic_3D..."<<endl;
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

	Vector3D s = this->get_voxel_size();
	double dx = double(s[0]);
	double dy = double(s[1]);
	double dz = double(s[2]);

	//pre calculations for speedup...
	rdir2 = this->orientation*this->get_voxel_resize();
	sdir2 = src_im->orientation*src_im->get_voxel_resize();
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

	image_scalar<double,3> *fx = get_num_diff_image_1storder_central_diff_3D(0);
	image_scalar<double,3> *fy = get_num_diff_image_1storder_central_diff_3D(1);
	image_scalar<double,3> *fz = get_num_diff_image_1storder_central_diff_3D(2);
	image_scalar<double,3> *fxy = get_num_diff_image_2ndorder_central_diff_3D(fx,1);
	image_scalar<double,3> *fxz = get_num_diff_image_2ndorder_central_diff_3D(fx,2);
	image_scalar<double,3> *fyz = get_num_diff_image_2ndorder_central_diff_3D(fy,2);
	image_scalar<double,3> *fxyz = get_num_diff_image_2ndorder_central_diff_3D(fxy,2);


	for (int k=0; k < this->datasize[2]; k++)
	{
		//cout<<".";
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
					//					set_a_coeff(coeff,fval,dfdxval,dfdyval,dfdzval,d2fdxdyval,d2fdxdzval,d2fdydzval,d3fdxdydzval);
					set_a_coeff2(coeff,fval,dfdxval,dfdyval,dfdzval,d2fdxdyval,d2fdxdzval,d2fdydzval,d3fdxdydzval);
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


#endif