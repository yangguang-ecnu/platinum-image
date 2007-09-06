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

	for (int k=0; k < this->datasize[2]; k++)
	{
		userIOmanagement.progress_update(k,"Trilinear interp. progress...",this->datasize[2]);
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

	userIOmanagement.progress_update(1,"Precalculating derivatives...",7);
	image_scalar<double,3> *fx = get_num_diff_image_1storder_central_diff_3D(0);
	userIOmanagement.progress_update(2,"Precalculating derivatives...",7);
	image_scalar<double,3> *fy = get_num_diff_image_1storder_central_diff_3D(1);
	userIOmanagement.progress_update(3,"Precalculating derivatives...",7);
	image_scalar<double,3> *fz = get_num_diff_image_1storder_central_diff_3D(2);
	userIOmanagement.progress_update(4,"Precalculating derivatives...",7);
	image_scalar<double,3> *fxy = get_num_diff_image_2ndorder_central_diff_3D(fx,1);
	userIOmanagement.progress_update(5,"Precalculating derivatives...",7);
	image_scalar<double,3> *fxz = get_num_diff_image_2ndorder_central_diff_3D(fx,2);
	userIOmanagement.progress_update(6,"Precalculating derivatives...",7);
	image_scalar<double,3> *fyz = get_num_diff_image_2ndorder_central_diff_3D(fy,2);
	userIOmanagement.progress_update(7,"Precalculating derivatives...",7);
	image_scalar<double,3> *fxyz = get_num_diff_image_2ndorder_central_diff_3D(fxy,2);

	for (int k=0; k < this->datasize[2]; k++)
	{
		userIOmanagement.progress_update(k,"interpolate_tricubic_3D - progress...",this->datasize[2]);
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

template <class ELEMTYPE, int IMAGEDIM>
image_binary<IMAGEDIM> * image_scalar<ELEMTYPE, IMAGEDIM>::threshold(ELEMTYPE low, ELEMTYPE high, IMGBINARYTYPE true_inside_threshold)
	{
    image_binary<IMAGEDIM> * output = new image_binary<IMAGEDIM> (this,false);
        
    typename image_storage<ELEMTYPE >::iterator i = this->begin();
    typename image_binary<IMAGEDIM>::iterator o = output->begin();
    
    while (i != this->end()) //images are same size and
                       //should necessarily end at the same time
        {
        if(*i>=low && *i<=high)
            {*o=true_inside_threshold;}
        else
            {*o=!true_inside_threshold;}

        ++i; ++o;
        }

	//output->image_has_changed();    
    return output;
	}

template <class ELEMTYPE, int IMAGEDIM>	
void image_scalar<ELEMTYPE, IMAGEDIM>::draw_line_2D(int x0, int y0, int x1, int y1, int z, ELEMTYPE value, int direction)
	{
	bool steep=abs(y1-y0)>abs(x1-x0);
	int temp;
	if(steep)
		{
		temp=x0;
		x0=y0;
		y0=temp;
		temp=x1;
		x1=y1;
		y1=temp;
		}
	if(x0>x1)
		{
		temp=x0;
		x0=x1;
		x1=temp;
		temp=y0;
		y0=y1;
		y1=temp;
		}
	int deltax=x1-x0;
	int deltay=abs(y1-y0);
	int error= -deltax/2;
	int ystep;
	int y=y0;
	if(y0<y1)
		ystep=1;
	else
		ystep=-1;
	int x;
	for(x=x0; x<=x1; x++)
		{
		if(steep)
			{
			this->set_voxel_by_dir(y,x,z,value,direction);
			}
		else
			{
			this->set_voxel_by_dir(x,y,z,value,direction);
			}
		error+=deltay;
		if(error>0)
			{
			y+=ystep;
			error-=deltax;
			}
		}
	//this->image_has_changed();
	}

template <class ELEMTYPE, int IMAGEDIM>
bool image_scalar<ELEMTYPE, IMAGEDIM>::row_sum_threshold(int* res, ELEMTYPE low_thr, ELEMTYPE high_thr, int row_direction, int z_direction, int first_slice, int last_slice)
	{
	double totdiff=0;
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim_and_dir(0,z_direction);
	max_v=this->get_size_by_dim_and_dir(1,z_direction);
	max_w=this->get_size_by_dim_and_dir(2,z_direction);
	
	memset(res,0,sizeof(int)*max_w);
	ELEMTYPE p;//pixel value
	double sum=0;
	int optthr;
	if(first_slice<0)
		first_slice=0;
	if(last_slice<0)
		last_slice=max_w-1;
 	if(row_direction==(z_direction+1)%3)
		{
		double* hist=new double[max_v];
		for(w=first_slice; w<=last_slice; w++)
			{
			for(v=0; v<max_v; v++)
				{
				int sum=0;
				for(u=0; u<max_u; u++)
					{
					p=this->get_voxel_by_dir(u,v,w,z_direction);
					if(p>=low_thr && p<=high_thr)
						{
						sum++;
						}
					}
				hist[v]=sum;
				}

			//Test all threshold values
			double sum1=0;
			double sum2=0;
			double n1=0;
			double n2=0;
			double cumSum=0;
			double cumSum_2=0;
			double cumN=0;

			int j;
			for(j=0; j<max_v; j++)
				{
				cumSum+=j*hist[j];
				cumSum_2+=j*j*hist[j];
				cumN+=hist[j];
				}
			double maxvar=0;
			optthr=0;
			double optdiff=0;
			for(j=0; j<max_v; j++)
				{
				//Compute mean and std
				sum1+=j*hist[j];
				n1+=hist[j];
				sum2=cumSum-sum1;
				n2=cumN-n1;		

				if(n1>0 && n2>0)
					{
					double mean1=sum1/n1;
					double mean2=sum2/n2;
					double betweenvar=n1*n2*(mean1-mean2)*(mean1-mean2)/(n1+n2);
					if(betweenvar>maxvar)
						{
						maxvar=betweenvar;
						optthr=j;
						optdiff=sum1-sum2;
						}
					}
				}
			totdiff+=optdiff;
			//double cumSumU=0;
			//double cumSumU_2=0;
			//double cumSumV=0;
			//double cumSumV_2=0;
			//cumN=0;
			//for(v=0; v<optthr; v++)
			//	{
			//	int sum=0;
			//	for(u=0; u<max_u; u++)
			//		{
			//		p=this->get_voxel_by_dir(u,v,w,z_direction);
			//		if(p>=low_thr && p<=high_thr)
			//			{
			//			cumSumV+=v;
			//			cumSumV_2+=v*v;
			//			cumSumU+=u;
			//			cumSumU_2+=u*u;
			//			cumN++;
			//			}
			//		}
			//	}
			//double stdU=sqrt((cumSumU_2-cumSumU*cumSumU/cumN)/(cumN-1));
			//double stdV=sqrt((cumSumV_2-cumSumV*cumSumV/cumN)/(cumN-1));
			//double widthU=(cumN*stdU/stdV);
			//double widthV=(cumN*stdV/stdU);
			//out << "Slice: " << w << " Thr: " << optthr << " WidthU: " << widthU << " WidthV: " << widthV << " StdU: " << stdU << " StdV: " << stdV << " Sum: " << cumN<< endl;
			res[w]=optthr;
			}
		delete[] hist;
        //std::cout << "Slice: " << w-1 << " Thr: " << optthr << std::endl;
		}
	else
		{
		//ofstream out("stduv.txt");
		double* hist=new double[max_u];
		for(w=first_slice; w<=last_slice; w++)
			{
			for(u=0; u<max_u; u++)
				{
				int sum=0;
				for(v=0; v<max_v; v++)
					{
					p=this->get_voxel_by_dir(u,v,w,z_direction);
					if(p>=low_thr && p<=high_thr)
						{
						sum++;
						}
					}
				hist[u]=sum;
				}

			//Test all threshold values
			double sum1=0;
			double sum2=0;
			double n1=0;
			double n2=0;
			double cumSum=0;
			double cumSum_2=0;
			double cumN=0;

			int j;
			for(j=0; j<max_u; j++)
				{
				cumSum+=j*hist[j];
				cumSum_2+=j*j*hist[j];
				cumN+=hist[j];
				}
			double maxvar=0;
			optthr=0;
			double optdiff=0;
			for(j=0; j<max_u; j++)
				{
				//Compute mean and std
				sum1+=j*hist[j];
				n1+=hist[j];
				sum2=cumSum-sum1;
				n2=cumN-n1;		

				if(n1>0 && n2>0)
					{
					double mean1=sum1/n1;
					double mean2=sum2/n2;
					double betweenvar=n1*n2*(mean1-mean2)*(mean1-mean2)/(n1+n2);
					if(betweenvar>maxvar)
						{
						maxvar=betweenvar;
						optthr=j;
						optdiff=sum1-sum2;
						}
					}
				}
			totdiff+=optdiff;
			//double cumSumU=0;
			//double cumSumU_2=0;
			//double cumSumV=0;
			//double cumSumV_2=0;
			//cumN=0;
			//for(u=0; u<optthr; u++)
			//	{
			//	int sum=0;
			//	for(v=0; v<max_v; v++)
			//		{
			//		p=this->get_voxel_by_dir(u,v,w,z_direction);
			//		if(p>=low_thr && p<=high_thr)
			//			{
			//			cumSumV+=v;
			//			cumSumV_2+=v*v;
			//			cumSumU+=u;
			//			cumSumU_2+=u*u;
			//			cumN++;
			//			}
			//		}
			//	}
			//double stdU=sqrt((cumSumU_2-cumSumU*cumSumU/cumN)/(cumN-1));
			//double stdV=sqrt((cumSumV_2-cumSumV*cumSumV/cumN)/(cumN-1));
			//double widthU=sqrt(cumN*stdU/stdV);
			//double widthV=sqrt(cumN*stdV/stdU);
			//cout << "Slice: " << w << " Thr: " << optthr << " WidthU: " << widthU << " WidthV: " << widthV << " StdU: " << stdU << " StdV: " << stdV << " Sum: " << cumN<< endl;
			res[w]=optthr;
			}
		//out.close();
		delete[] hist;
        //std::cout << "Slice: " << w-1 << " Thr: " << optthr << std::endl;
		}
	return totdiff>0;
	}


template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::mask_out(image_binary<IMAGEDIM> *mask, IMGBINARYTYPE object_value, ELEMTYPE blank)
    {
    typename image_storage<ELEMTYPE>::iterator i = this->begin();
    image_storage<IMGBINARYTYPE >::iterator m = mask->begin();
    while (i != this->end()) //images are same size and should necessarily end at the same time
        {
        if(*m != object_value)
            {*i=blank;}
        ++i; ++m;
        }
	//this->min_max_refresh();
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::mask_out(int low_x, int high_x, int low_y, int high_y, int low_z, int high_z, ELEMTYPE blank)
    {
	int x,y,z;
	int max_x, max_y, max_z;
	max_x=this->get_size_by_dim(0);
	max_y=this->get_size_by_dim(1);
	max_z=this->get_size_by_dim(2);
	if(low_x<0)
		low_x=0;
	if(low_y<0)
		low_y=0;
	if(low_z<0)
		low_z=0;
	if(high_x<0 || high_x>max_x)
		high_x=max_x;
	if(high_y<0 || high_y>max_y)
		high_y=max_y;
	if(high_z<0 || high_z>max_z)
		high_z=max_z;
	for(z=low_z; z<high_z; z++)
		for(y=low_y; y<high_y; y++)
			for(x=low_x; x<high_x; x++)
				this->set_voxel(x,y,z,blank);
    }
	
	template <class ELEMTYPE, int IMAGEDIM>
		std::vector<double> image_scalar<ELEMTYPE, IMAGEDIM>::get_slice_sum(int direction)
	{
		int u,v,w;
		int max_u, max_v, max_w;
		max_u=this->get_size_by_dim_and_dir(0,direction);
		max_v=this->get_size_by_dim_and_dir(1,direction);
		max_w=this->get_size_by_dim_and_dir(2,direction);
		ELEMTYPE p;//pixel value
		double sum=0;
		std::vector<double> res;
		for(w=0; w<max_w; w++)
		{
			double sum=0;
			for(v=0; v<max_v; v++)
			{
				for(u=0; u<max_u; u++)
				{
					p=this->get_voxel_by_dir(u,v,w,direction);
					sum+=p;
				}
			}
			res.push_back(sum);
		}

		return res;
	}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::copy(image_integer<ELEMTYPE, IMAGEDIM> *source, int low_x, int high_x, int low_y, int high_y, int low_z, int high_z, int direction)
	{
		int x,y,z;
		int max_x, max_y, max_z;
		max_x=this->get_size_by_dim_and_dir(0,direction);
		max_y=this->get_size_by_dim_and_dir(1,direction);
		max_z=this->get_size_by_dim_and_dir(2,direction);
		if(low_x<0)
			low_x=0;
		if(low_y<0)
			low_y=0;
		if(low_z<0)
			low_z=0;
		if(high_x<0 || high_x>max_x)
			high_x=max_x;
		if(high_y<0 || high_y>max_y)
			high_y=max_y;
		if(high_z<0 || high_z>max_z)
			high_z=max_z;
		for(z=low_z; z<high_z; z++)
			for(y=low_y; y<high_y; y++)
				for(x=low_x; x<high_x; x++)
					this->set_voxel_by_dir(x,y,z,source->get_voxel_by_dir(x,y,z,direction),direction);
	}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::flip_voxel_data_3D(int direction)
	{
		//loop over floor(half) of the volume and copy that half to the other...
		ELEMTYPE tmp;

		switch(direction){
		case 0:
			for(int z=0; z<datasize[2]; z++){		
				for(int y=0; y<datasize[1]; y++){
					for(int x=0; x<datasize[0]/2; x++){
						tmp = get_voxel(x,y,z);
						set_voxel(x,y,z,get_voxel(datasize[0]-x-1,y,z));
						set_voxel(datasize[0]-x-1,y,z,tmp);
					}
				}
			}
			break;
		case 1: 
			for(int z=0; z<datasize[2]; z++){		
				for(int y=0; y<datasize[1]/2; y++){
					for(int x=0; x<datasize[0]; x++){
						tmp = get_voxel(x,y,z);
						set_voxel(x,y,z,get_voxel(x,datasize[1]-y-1,z));
						set_voxel(x,datasize[1]-y-1,z,tmp);
					}
				}
			}
			break;
		case 2: 
			for(int z=0; z<datasize[2]/2; z++){
				for(int y=0; y<datasize[1]; y++){
					for(int x=0; x<datasize[0]; x++){
						tmp = get_voxel(x,y,z);
						set_voxel(x,y,z,get_voxel(x,y,datasize[2]-z-1));
						set_voxel(x,y,datasize[2]-z-1,tmp);
					}
				}
			}
			break;
		default:
			pt_error::error("image_scalar<>-flip_voxel_data_3D(), erroneous direction",pt_error::debug);
			break;
		}
	}

#include "image_scalarprocess.hxx"

#endif