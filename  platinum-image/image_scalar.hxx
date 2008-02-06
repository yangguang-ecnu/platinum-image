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

#include "userIOmanager.h"
#include "datamanager.h"

extern userIOmanager userIOmanagement;
extern datamanager datamanagement;



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
void image_scalar<ELEMTYPE, IMAGEDIM >::interpolate_spline_ITK_3D(image_scalar<ELEMTYPE, IMAGEDIM > *ref_im)
{
	cout<<"interpolate_spline_ITK_3D..."<<endl;
	typedef itk::ResampleImageFilter<theImageType, theImageType>  FilterType;
	typename FilterType::Pointer filter = FilterType::New();

//	typedef itk::AffineTransform< double, IMAGEDIM >  TransformType;
//	typedef itk::Rigid3DTransform< double > TransformType;
	typedef itk::MatrixOffsetTransformBase<double,3,3> TransformType;
	TransformType::Pointer transform = TransformType::New();

	TransformType::MatrixType m;

	typename itk::OrientedImage<ELEMTYPE, IMAGEDIM >::DirectionType d = ref_im->get_orientation_itk();
	cout<<"m="<<endl;
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			m[i][j] = d[i][j];
			cout<<m[i][j]<<" ";
		}
		cout<<endl;
	}
//	transform->SetRotationMatrix(m);
//	transform->SetMatrix(m);


	typedef itk::BSplineInterpolateImageFunction<theOrientedImageType, double >  InterpolatorType;
	typename InterpolatorType::Pointer interpolator = InterpolatorType::New(); //3-rd order is default.....

	filter->SetInput(this->get_image_as_itk_output());
	filter->SetTransform( transform );
	filter->SetInterpolator( interpolator );//3-rd order is default.....
	filter->SetDefaultPixelValue( 1 );
	filter->SetOutputOrigin( ref_im->get_origin_itk() );
	filter->SetOutputSpacing( ref_im->get_voxel_size_itk() );
	filter->SetSize( ref_im->get_size_itk() );
	filter->SetOutputDirection(m);


	try{
		cout<<"filter update..."<<endl;
		filter->Update();
	}catch( itk::ExceptionObject & excep ){
		std::cerr << "Exception catched !" << std::endl;
		std::cerr << excep << std::endl;
	}
	this->replicate_itk_to_image(filter->GetOutput());
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

	Vector3D s = this->get_physical_size();
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
				if(src_im->is_voxelpos_inside_image_border_3D(svox[0],svox[1],svox[2],1))
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
	userIOmanagement.progress_update(0,"interpolate_tricubic_3D - progress...Done",this->datasize[2]);

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
void image_scalar<ELEMTYPE, IMAGEDIM>::flip_voxel_data_3D(int direction)
	{
		//loop over floor(half) of the volume and copy that half to the other...
		ELEMTYPE tmp1;
		ELEMTYPE tmp2;
		int pos2;

		switch(direction){
		case 0:
			for(int z=0; z<this->datasize[2]; z++){		
				for(int y=0; y<this->datasize[1]; y++){
					for(int x=0; x<this->datasize[0]/2; x++){
						pos2 = this->datasize[0]-1-x;
						tmp1 = this->get_voxel(x,y,z);
						tmp2 = this->get_voxel(pos2,y,z);
						this->set_voxel(x,y,z,tmp2);
						this->set_voxel(pos2,y,z,tmp1);
					}
				}
			}
			break;
		case 1: 
			for(int z=0; z<this->datasize[2]; z++){		
				for(int y=0; y<this->datasize[1]/2; y++){
					for(int x=0; x<this->datasize[0]; x++){
						pos2 = this->datasize[1]-1-y;
						tmp1 = this->get_voxel(x,y,z);
						tmp2 = this->get_voxel(x,pos2,z);
						this->set_voxel(x,y,z,tmp2);
						this->set_voxel(x,pos2,z,tmp1);
					}
				}
			}
			break;
		case 2: 
			for(int z=0; z<this->datasize[2]/2; z++){
				for(int y=0; y<this->datasize[1]; y++){
					for(int x=0; x<this->datasize[0]; x++){
						pos2 = this->datasize[2]-1-z;
						tmp1 = this->get_voxel(x,y,z);
						tmp2 = this->get_voxel(x,y,pos2);
						this->set_voxel(x,y,z,tmp2);
						this->set_voxel(x,y,pos2,tmp1);
					}
				}
			}
			break;
		default:
			pt_error::error("image_scalar<>-flip_voxel_data_3D(), erroneous direction",pt_error::debug);
			break;
		}
	}

template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_scalar<ELEMTYPE, IMAGEDIM>::get_intensity_at_lower_percentile(float percentile, bool ignore_zero_intensity)
{
	return this->stats->get_intensity_at_histogram_lower_percentile(percentile, ignore_zero_intensity);
}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::save_histogram_to_txt_file(const std::string filename, const std::string separator)
	{
		cout<<"save_histogram_to_txt_file..."<<endl;
		cout<<this->stats<<endl;
		pt_error::error_if_null(this->stats,"image_scalar<ELEMTYPE, IMAGEDIM>::save_histogram_to_txt_file - stats==NULL",pt_error::debug);
		this->stats->save_histogram_to_txt_file(filename, separator);
	}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::save_histogram_to_txt_file(const std::string filename, bool reload_hist_from_image, gaussian *g, const std::string separator)
	{
		cout<<"save_histogram_to_txt_file..."<<endl;
		cout<<this->stats<<endl;
		pt_error::error_if_null(this->stats,"image_scalar<ELEMTYPE, IMAGEDIM>::save_histogram_to_txt_file - stats==NULL",pt_error::debug);
		this->stats->save_histogram_to_txt_file(filename, reload_hist_from_image, g, separator);
	}
/*
template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::save_histogram_to_txt_file2(const std::string filename, bool reload_hist_from_image, const std::string separator)
	{
		cout<<"save_histogram_to_txt_file..."<<endl;
		cout<<this->stats<<endl;
		pt_error::error_if_null(this->stats,"image_scalar<ELEMTYPE, IMAGEDIM>::save_histogram_to_txt_file2 - stats==NULL",pt_error::debug);
		this->stats->save_histogram_to_txt_file2(filename, reload_hist_from_image, separator);
	}
*/
template <class ELEMTYPE, int IMAGEDIM>
image_scalar<ELEMTYPE, IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::create2Dhistogram_3D(image_scalar<ELEMTYPE, IMAGEDIM> *second_image, bool remove_zero_intensity, int scale_a, int scale_b)
{
	if(scale_a<=0){	scale_a = this->get_max(); }
	if(scale_b<=0){	scale_b = second_image->get_max(); }

	image_scalar<ELEMTYPE, IMAGEDIM> *hist = new image_scalar<ELEMTYPE, IMAGEDIM>(scale_a,scale_b,1);
	hist->fill(0);

	if(!this->same_size(second_image)){
		pt_error::error("image_scalar<ELEMTYPE, IMAGEDIM>::create2Dhistogram - image size not equal",pt_error::warning);

	}else{
		int this_x;
		int this_y;
		for(int z=0; z<this->datasize[2]; z++){
			for(int y=0; y<this->datasize[1]; y++){
				for(int x=0; x<this->datasize[0]; x++){
					this_x = this->get_voxel(x,y,z)*float(scale_a)/this->get_max_float();
					this_y = second_image->get_voxel(x,y,z)*float(scale_b)/second_image->get_max_float();
					hist->set_voxel(this_x,this_y,0,hist->get_voxel(this_x,this_y,0)+1);
				}
			}
		}
	}
	if(remove_zero_intensity){
		hist->set_voxel(0,0,0,0);
	}
	hist->data_has_changed(true);
	return hist;
}

template <class ELEMTYPE, int IMAGEDIM>
image_scalar<ELEMTYPE, IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::create_slicewise_2Dhistograms_3D(image_scalar<ELEMTYPE, IMAGEDIM> *second_image, int hist_slc_dir, bool remove_zero_intensity, int scale_a, int scale_b){

	if(scale_a<=0){	scale_a = this->get_max(); }
	if(scale_b<=0){	scale_b = second_image->get_max(); }

	image_scalar<ELEMTYPE, IMAGEDIM> *hist;

	if(!this->same_size(second_image)){
		pt_error::error("image_scalar<ELEMTYPE, IMAGEDIM>::create2Dhistogram - image size not equal",pt_error::warning);
		return NULL;
	}else{
		int this_a;
		int this_b;

		if(hist_slc_dir==2){
			hist = new image_scalar<ELEMTYPE, IMAGEDIM>(scale_a,scale_b,this->datasize[2]);
			hist->fill(0);
			for(int z=0; z<this->datasize[2]; z++){
				for(int y=0; y<this->datasize[1]; y++){
					for(int x=0; x<this->datasize[0]; x++){
						this_a = this->get_voxel(x,y,z)*float(scale_a-1)/this->get_max_float();
						this_b = second_image->get_voxel(x,y,z)*float(scale_b-1)/second_image->get_max_float();
						hist->set_voxel(this_a,this_b,z, hist->get_voxel(this_a,this_b,z)+1);
					}
				}
			}
		}else if(hist_slc_dir==1){
			hist = new image_scalar<ELEMTYPE, IMAGEDIM>(scale_a,scale_b,this->datasize[1]);
			hist->fill(0);
			for(int z=0; z<this->datasize[2]; z++){
				for(int y=0; y<this->datasize[1]; y++){
					for(int x=0; x<this->datasize[0]; x++){
						this_a = this->get_voxel(x,y,z)*float(scale_a-1)/this->get_max_float();
						this_b = second_image->get_voxel(x,y,z)*float(scale_b-1)/second_image->get_max_float();
						hist->set_voxel(this_a,this_b,y, hist->get_voxel(this_a,this_b,y)+1);
					}
				}
			}
		}else if(hist_slc_dir==0){
			hist = new image_scalar<ELEMTYPE, IMAGEDIM>(scale_a,scale_b,this->datasize[0]);
			hist->fill(0);
			for(int z=0; z<this->datasize[2]; z++){
				for(int y=0; y<this->datasize[1]; y++){
					for(int x=0; x<this->datasize[0]; x++){
						this_a = this->get_voxel(x,y,z)*float(scale_a-1)/this->get_max_float();
						this_b = second_image->get_voxel(x,y,z)*float(scale_b-1)/second_image->get_max_float();
						hist->set_voxel(this_a,this_b,x, hist->get_voxel(this_a,this_b,x)+1);
					}
				}
			}

		}else{
			pt_error::error("image_scalar<ELEMTYPE, IMAGEDIM>::create2Dhistogram - image directions nor recognized",pt_error::warning);	
		}

		if(remove_zero_intensity){
			for(int z=0; z<hist->datasize[2]; z++){
				hist->set_voxel(0,0,z,0);
			}
		}

	hist->data_has_changed(true);
	return hist;
	}
}




// The "calculate_T1Map_3D" function is based on following publication... (Chen2006)
// Rapid High-Resolution T1 Mapping by Variable Flip Angles: Accurate and Precise Measurements in the Presence of Radiofrequency Field Inhomogeneity
// Hai-Ling Margaret Cheng and Graham A Wright, Magnetic Resonance in Medicine 55:566ñ574 (2006)

// The "FLASH" signal equation is use din linear form.
// least square based linear regression is used to determine the T1-values..
// Add all volumes to the vector v for simple implementation...

template <class ELEMTYPE, int IMAGEDIM>
image_scalar<ELEMTYPE, IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::calculate_T1Map_3D(vector<image_scalar<ELEMTYPE, IMAGEDIM > *> v, float body_thres, float t1_min, float t1_max)
{
	cout<<"calculate_T1Map_3D..."<<endl;

	int N = v.size();
//	int N = 3;
	if(N<2){
		cout<<"...to few volumes loaded!"<<endl;
		return NULL;
	}else{

	image_scalar<ELEMTYPE, IMAGEDIM>* t1map = new image_scalar<ELEMTYPE, IMAGEDIM>(this);
	t1map->fill(0);

	vnl_vector<float> alpha(N);		//flip angles...
	for( int i=0;i<N;i++){
		alpha(i) = v[i]->meta.get_flip()*PI/180.0;
	}
	cout<<"alpha="<<alpha<<endl;

//	vnl_vector<float> S(N);	 //MR signal... 3*1, printed as 1*3 echo times for the N echoes...
	vnl_vector<float> y(N);
	vnl_vector<float> x(N);
	float TR = v[0]->meta.get_data_float(DCM_TR);
	cout<<"TR="<<TR<<endl;
	float y_mean=0;
	float x_mean=0;
	float Sxx=0;
	float Sxy=0;
	float t1=0;

	for(int k=0; k<t1map->datasize[2]; k++){
		cout<<".";
		for(int j=0; j<t1map->datasize[1]; j++){
			for(int i=0; i<t1map->datasize[0]; i++){

				t1=0;
				if( v[0]->get_voxel(i,j,k)>body_thres ){		//intensity in first image
					for(int l=0; l<N; l++){
						//S(l) = v[l]->get_voxel(i,j,k);
						//y(l) = S(l)/sin(alpha(l));
						//x(l) = S(l)/tan(alpha(l));
						y(l) = v[l]->get_voxel(i,j,k)/sin(alpha(l));
						x(l) = v[l]->get_voxel(i,j,k)/tan(alpha(l));
					}

					y_mean = y.sum()/float(N);
					x_mean = x.sum()/float(N);
					Sxx=0;
					Sxy=0;

					for(int l=0; l<N; l++){
						Sxx += (x(l)-x_mean)*(x(l)-x_mean);
						Sxy += (x(l)-x_mean)*(y(l)-y_mean);
					}
					t1 = -TR/log(Sxy/Sxx);

				}
				if(t1<t1_min){t1=t1_min;}
				if(t1>t1_max){t1=t1_max;}

				t1map->set_voxel(i,j,k,t1);
			}
		}
	}

	cout<<"Done"<<endl;

	t1map->data_has_changed(true);	//true is needed since intensity min/max needs to be recalculated...
	return t1map;
	}
}





template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::smooth_ITK(Vector3D radius)
{
	typename itk::MeanImageFilter<theImageType,theImageType>::Pointer filter = itk::MeanImageFilter<theImageType,theImageType>::New();

	typename theSizeType r;
	//	r[0] = 1; // radius along x
	//	r[1] = 1; // radius along y
	//	r[2] = 1; // radius along z
	r[0] = radius[0];
	r[1] = radius[1];
	r[2] = radius[2];

	filter->SetRadius(r);
	filter->SetInput(this->get_image_as_itk_output());
	filter->Update();

	this->replicate_itk_to_image(filter->GetOutput());
	//JK - verify... ööö
}



template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::smooth_3D(Vector3D r)
{
	cout<<"smooth_3D..."<<endl;
	image_scalar<ELEMTYPE, IMAGEDIM> *res = new image_scalar<ELEMTYPE, IMAGEDIM>(this,0);

	res->fill(0); //not very time efficient... the outer borders would be enough...

	if(r[0]>=0 && r[1]>=0 && r[2]>=0)
	{
		float num_neighbours = (1+2*r[0])*(1+2*r[1])*(1+2*r[2]);
		float sum = 0;
		float mean1 = 0;
		float tmp = 0;
		float limit = 100;
		float num_counted = 0;

		cout<<"k=";
		for(int k=r[2]; k < this->datasize[2]-r[2]; k++){
			cout<<k<<",";
			for(int j=r[1]; j < this->datasize[1]-r[1]; j++){
				for(int i=r[0]; i < this->datasize[0]-r[0]; i++){

					sum = 0;
					for(int n=k-r[2]; n<=k+r[2]; n++){
						for(int m=j-r[1]; m<=j+r[1]; m++){
							for(int l=i-r[0]; l<=i+r[0]; l++){
								sum += float(this->get_voxel(l,m,n));
							}
						}
					}
					mean1 = sum/num_neighbours;
					res->set_voxel(i,j,k,mean1);
/*
					sum = 0;
					num_counted = 0;
					for(int n=k-r[2]; n<=k+r[2]; n++){
						for(int m=j-r[1]; m<=j+r[1]; m++){
							for(int l=i-r[0]; l<=i+r[0]; l++){
								tmp = float(this->get_voxel(l,m,n));
								if(abs(tmp-mean1)<limit)
								{
									sum += tmp;
									num_counted++;
								}
							}
						}
					}

					res->set_voxel(i,j,k,sum/num_counted);	//mean2...
*/
				}
			}
		}
	}

	copy_data(res,this);
	delete res;
}

	

template <class ELEMTYPE, int IMAGEDIM>
image_binary<IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::region_grow_3D(Vector3D seed, ELEMTYPE min_intensity, ELEMTYPE max_intensity)
{

	image_binary<IMAGEDIM> *res = new image_binary<IMAGEDIM>(this,0);
	res->fill(false);
	res->set_voxel(seed[0],seed[1],seed[2],true);

	int sx = this->datasize[0];
	int sy = this->datasize[1];
	int sz = this->datasize[2];
//	cout<<sx<<" "<<sy<<" "<<sz<<endl;

	queue<Vector3D> s;
	s.push(seed);
	Vector3D pos;
	Vector3D pos2;

	ELEMTYPE val;

	while(s.size()>0){
		pos = s.front();
		s.pop();

//		if(i%100000==0){
//			val = this->get_voxel(pos[0],pos[1],pos[2]);
//			cout<<"i="<<i<<" pos="<<pos[0]<<" "<<pos[1]<<" "<<pos[2]<<" val="<<val<<" s.size()="<<s.size()<<endl;
//			cin>>c;
//		}
//		i++;

		for(int x=std::max(0,int(pos[0]-1)); x<=std::min(int(pos[0]+1),sx-1); x++){
			for(int y=std::max(0,int(pos[1]-1)); y<=std::min(int(pos[1]+1),sy-1); y++){
				for(int z=std::max(0,int(pos[2]-1)); z<=std::min(int(pos[2]+1),sz-1); z++){
					val = this->get_voxel(x,y,z);

					if(val>=min_intensity && val<=max_intensity && res->get_voxel(x,y,z)==false){
						pos2[0]=x; pos2[1]=y; pos2[2]=z;
						s.push(pos2);
						res->set_voxel(x,y,z,true);
					}
				}
			}
		}
	}
	cout<<"region_grow_3D --> Done...(s.size()="<<s.size()<<")"<<endl;
//	res->save_to_VTK_file("c:\\Joel\\TMP\\region_grow.vtk");
//	cout<<"before..."<<endl;
//	copy_data(res,this);
	return res;
}

template <class ELEMTYPE, int IMAGEDIM>
image_binary<IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::region_grow_robust_3D(Vector3D seed, ELEMTYPE min_intensity, ELEMTYPE max_intensity, int nr_accepted_neighbours, int radius)
{
	image_binary<IMAGEDIM> *res = new image_binary<IMAGEDIM>(this,0);
	res->fill(false);
	res->set_voxel(seed[0],seed[1],seed[2],true);

	image_scalar<ELEMTYPE, IMAGEDIM> *neighb = new image_scalar<ELEMTYPE, IMAGEDIM>(this,0);
	neighb->fill(0);

	int sx = this->datasize[0];
	int sy = this->datasize[1];
	int sz = this->datasize[2];
	cout<<sx<<" "<<sy<<" "<<sz<<endl;
	int r = radius;

	int* seed2 = new int[3];
	int* pos = new int[3];
	int* pos2 = new int[3];
	seed2[0] = int(seed[0]);
	seed2[1] = int(seed[1]);
	seed2[2] = int(seed[2]);
	stack<int*> s;
	s.push(seed2);

//	stack<Vector3D> s;
//	s.push(seed);
//	Vector3D pos;
//	Vector3D pos2;

	ELEMTYPE val;
	ELEMTYPE val2;
	int nr_acc=0;

	while(s.size()>0){
		pos = s.top();
		s.pop();
		val = this->get_voxel(pos[0],pos[1],pos[2]);

		if(val>=min_intensity && val<=max_intensity){

			for(int x=std::max(0,int(pos[0]-1)); x<=std::min(int(pos[0]+1),sx-1); x++){
				for(int y=std::max(0,int(pos[1]-1)); y<=std::min(int(pos[1]+1),sy-1); y++){
					for(int z=std::max(0,int(pos[2]-1)); z<=std::min(int(pos[2]+1),sz-1); z++){
						val = this->get_voxel(x,y,z);
						if(val>=min_intensity && val<=max_intensity && res->get_voxel(x,y,z)==false){

							//check number of accepted neighbours...
							nr_acc=0;
							for(int a=std::max(0,int(x-r)); a<=std::min(int(x+r),sx-1); a++){
								for(int b=std::max(0,int(y-r)); b<=std::min(int(y+r),sy-1); b++){
									for(int c=std::max(0,int(z-r)); c<=std::min(int(z+r),sz-1); c++){
										val2 = this->get_voxel(a,b,c);
										if(val2>=min_intensity && val2<=max_intensity){
											nr_acc++;
										}
									}
								}
							}
							neighb->set_voxel(x,y,z,nr_acc);

							if(nr_acc>=nr_accepted_neighbours){
								pos2[0]=x; pos2[1]=y; pos2[2]=z;
								s.push(pos2);
								res->set_voxel(x,y,z,true);
							}

						}//if
					}//z
				}//y
			}//x

		}//if val...
	}//while

	cout<<"region_grow_robust_3D --> Done..."<<endl;
//	cout<<"s.size()="<<s.size()<<endl;
//	res->save_to_VTK_file("c:\\Joel\\TMP\\region_grow.vtk");


//	neighb->save_to_VTK_file("c:\\Joel\\TMP\\region_grow_nr_acc.vtk");
	delete neighb;

//	copy_data(res,this);
	//delete res;
	return res;
}


template <class ELEMTYPE, int IMAGEDIM>
image_scalar<ELEMTYPE, 3>* image_scalar<ELEMTYPE, IMAGEDIM>::create_projection_3D(int dir, PROJECTION_MODE PROJ)
{ //enum PROJECTION_MODE {PROJ_MEAN, PROJ_MAX}; 

	image_scalar<ELEMTYPE, 3>* res = new image_scalar<ELEMTYPE, 3>(); 
	res->set_parameters(this); //copy rotation and size infor to tmp image first...

	//JK change this to 2D later, when there is time for 3D-->2D verification...

	int dx = this->get_size_by_dim(0);
	int dy = this->get_size_by_dim(1);
	int dz = this->get_size_by_dim(2);

	float line_max=0;
	float line_sum=0;


	switch(dir){
	case 0:
		res->initialize_dataset(dz,dy,1);

		for(int z=0; z<this->datasize[2]; z++){		
			for(int y=0; y<this->datasize[1]; y++){
				if(PROJ==PROJ_MAX){
					line_max=0;
					for(int x=0; x<this->datasize[0]; x++){
						line_max = std::max(line_max,float(this->get_voxel(x,y,z)));
					}
					res->set_voxel(z,y,0,line_max);
				}
			}
		}
		break;
	case 1: 
		res->initialize_dataset(dx,dz,1);

		for(int z=0; z<this->datasize[2]; z++){		
			for(int x=0; x<this->datasize[0]; x++){
				if(PROJ==PROJ_MAX){
					line_max=0;
					for(int y=0; y<this->datasize[1]; y++){
						line_max = std::max(line_max,float(this->get_voxel(x,y,z)));
					}
					res->set_voxel(x,z,0,line_max);
				}
			}
		}
		break;
	case 2: 
		res->initialize_dataset(dx,dy,1);

		for(int y=0; y<this->datasize[1]; y++){		
			for(int x=0; x<this->datasize[0]; x++){
				if(PROJ==PROJ_MAX){
					line_max=0;
					for(int z=0; z<this->datasize[2]; z++){
						line_max = std::max(line_max,float(this->get_voxel(x,y,z)));
					}
					res->set_voxel(x,y,0,line_max);
				}
			}
		}
		break;
	default:
		pt_error::error("create_projection-->strange direction...",pt_error::debug);
		break;
	}
	return res;
}


template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::calculate_TP_FP_Udupa_3D(float &tp, float &fp, float &udupa, image_scalar<ELEMTYPE, IMAGEDIM>* ground_truth, ELEMTYPE gt_obj_val, ELEMTYPE this_obj_val)
{
	pt_error::error_if_false(this->same_size(ground_truth)," calculate_TP_FP_Udupa-->NOT same size...",pt_error::debug);

	float any_object=0;
	float hit=0;
	float over=0;
	float under=0;
	float nr_in_gt=0;

	bool val;
	bool gt;

	for(int z=0; z<this->datasize[2]; z++){
		for(int y=0; y<this->datasize[1]; y++){
			for(int x=0; x<this->datasize[0]; x++){
				val = this->get_voxel(x,y,z)>0?true:false;
				gt = ground_truth->get_voxel(x,y,z)>0?true:false;

				if(val&&gt){
					hit++;
					any_object++;
					nr_in_gt++;
				}else if(val){
					over++;
					any_object++;
				}else if(gt){
					under++;
					any_object++;
					nr_in_gt++;
				}
			}
		}
	}

	tp = hit/nr_in_gt;
	fp = over/nr_in_gt;
	udupa = hit/any_object;
}

template <class ELEMTYPE, int IMAGEDIM>
Vector3D image_scalar<ELEMTYPE, IMAGEDIM>::get_pos_of_max_grad_mag_in_region_voxel( Vector3D center, Vector3D radius, GRAD_MAG_TYPE type )
{
	if ( ! this->is_voxelpos_within_image_3D( center ) )
	{	// the point is not within the image
		pt_error::error("get_pos_of_type_in_region_voxel--> point is not within image",pt_error::debug);		
		return center; //is this useful?
	}

	image_scalar<ELEMTYPE, IMAGEDIM> * res = new image_scalar<ELEMTYPE, IMAGEDIM>( this, 0 );
	res->fill( 0 ); //full image size... for simplicity...

	float center_grad_mag = grad_mag_voxel( center, type );
	
	
	Vector3Dint start = center - radius;
	Vector3Dint end = center + radius;
	
	for ( int i = 0; i < 3; i++ )
	{	// set start and end to be within the image
		start[i] = start[i] < 0 ? 0 : start[i];
		end[i] = end[i] >= this->datasize[i] ? ( this->datasize[i] -  1 ) : end[i];
	}

	Vector3D pos = center;	// return center position if no other is found

	for ( int z = start[2]; z <= end[2]; z++ )
	{
		for ( int y = start[1]; y <= end[1]; y++ )
		{
			for ( int x = start[0]; x <= end[0]; x++ )
			{
				Vector3Dint current = create_Vector3Dint( x, y, z );
				Vector3Dint center_as_int = center;

				float weight = weight_of_type( center_as_int, current, CHESSBOARD );

				float current_grad_mag = grad_mag_voxel( x, y, z, type );
				
				float current_grad_mag_weighted = 0;
				
				if ( weight != 0 )
					{ current_grad_mag_weighted = current_grad_mag / weight; }
				else
					{ current_grad_mag_weighted = current_grad_mag; }	// weight == 0 means the center of the region
				
				if ( current_grad_mag_weighted > center_grad_mag )
				{ 
					pos = create_Vector3Dint( x, y, z );
				}
				
				res->set_voxel( x, y, z, floor( current_grad_mag_weighted ) );
			
			}
		}
	}
	
	//res->set_voxel( pos[0], pos[1], pos[2], 511 );
	datamanagement.add( res );

	// tag bort !!
	//pos[2] = center[2];

	return pos;
}


template <class ELEMTYPE, int IMAGEDIM>
Vector3D image_scalar<ELEMTYPE, IMAGEDIM>::get_pos_of_type_in_region_voxel( Vector3D center, Vector3D radius, POINT_TYPE type )
{	
	if ( ! this->is_voxelpos_within_image_3D( center ) )
	{	// the point is not within the image
		pt_error::error("get_pos_of_type_in_region_voxel--> point is not within image",pt_error::debug);
		return center;
	}

	switch ( type )
	{
		case MAX_GRAD_MAG_X:
			return get_pos_of_max_grad_mag_in_region_voxel( center, radius, X );
		case MAX_GRAD_MAG_Y:
			return get_pos_of_max_grad_mag_in_region_voxel( center, radius, Y );
		case MAX_GRAD_MAG_Z:
			return get_pos_of_max_grad_mag_in_region_voxel( center, radius, Z );
		case MAX_GRAD_MAG_XY:
			return get_pos_of_max_grad_mag_in_region_voxel( center, radius, XY );
		case MAX_GRAD_MAG_XZ:
			return get_pos_of_max_grad_mag_in_region_voxel( center, radius, XZ );
		case MAX_GRAD_MAG_YZ:
			return get_pos_of_max_grad_mag_in_region_voxel( center, radius, YZ );
		case MAX_GRAD_MAG_XYZ:
			return get_pos_of_max_grad_mag_in_region_voxel( center, radius, XYZ );
		default:
			return center;
	}
}


template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::scale_slice_by_factor_3d(int dir, float factor, int slice) 
{
	if (dir<0 || dir>2) {
		pt_error::error("Direction dir must be between 0 and 2 in scale_slice_by_factor_3d", pt_error::debug);
	}
	if(slice<0 || slice>=get_size_by_dim(dir)){
		pt_error::error("Slice out of bounds in scale_slice_by_factor_3d",pt_error::debug); 
	}
	if (dir==0)	{
		for(int j=0; j < this->get_size_by_dim(1); j++){
			for(int k=0; k < this->get_size_by_dim(2); k++){
				this->set_voxel(slice,j,k, int(float(this->get_voxel(slice,j,k))*factor));
			}
		}
	}
	else if (dir==1) {
		for(int i=0; i < this->get_size_by_dim(0); i++){
			for(int k=0; k < this->get_size_by_dim(2); k++){
				this->set_voxel(i,slice,k, int(float(this->get_voxel(i,slice,k))*factor));
			}
		}
	}
	else {
		for(int i=0; i < this->get_size_by_dim(0); i++){
			for(int j=0; j < this->get_size_by_dim(1); j++){
				this->set_voxel(i,j,slice, int(float(this->get_voxel(i,j,slice))*factor));
			}
		}
	}
}


template <class ELEMTYPE, int IMAGEDIM>
float image_scalar<ELEMTYPE, IMAGEDIM>::get_mean_from_slice_3d(int dir, int slice, int low_thres, int high_thres) 
{
	if (dir<0 || dir>2) {
		pt_error::error("Direction dir must be between 0 and 2 in get_mean_from_slice_3d", pt_error::debug);
	}
	if(slice<0 || slice>=get_size_by_dim(dir)){
		pt_error::error("Slice out of bounds in get_mean_from_slice_3d",pt_error::debug); 
	}
	int value=0;
	float mean=0;
	int no_voxels=0;

	if (dir==0)	{
		for (int j=0; j < this->get_size_by_dim(1); j++){
			for(int k=0; k < this->get_size_by_dim(2); k++){
				value=this->get_voxel(slice,j,k);
				if (value >= low_thres && value <= high_thres) {
					mean += value;
					no_voxels++;
				}
			}
		}
	}
	else if (dir==1)	{
		for (int i=0; i < this->get_size_by_dim(0); i++){
			for(int k=0; k < this->get_size_by_dim(2); k++){
				value=this->get_voxel(i,slice,k);
				if (value >= low_thres && value <= high_thres) {
					mean += value;
					no_voxels++;
				}
			}
		}
	}
	else {
		for (int i=0; i < this->get_size_by_dim(0); i++){
			for(int j=0; j < this->get_size_by_dim(1); j++){
				value=this->get_voxel(i,j,slice);
				if (value >= low_thres && value <= high_thres) {
					mean += value;
					no_voxels++;
				}
			}
		}
	}
	return mean=mean/no_voxels;
}

// old
//template <class ELEMTYPE, int IMAGEDIM>
//Vector3D image_scalar<ELEMTYPE, IMAGEDIM>::get_pos_of_type_in_region_voxel ( Vector3D center, Vector3D radius, POINT_TYPE point_type )
//{
//	image_scalar<ELEMTYPE, IMAGEDIM> * res = new image_scalar<ELEMTYPE, IMAGEDIM>( this, 0 );
//	res->fill( 0 );
//
//	Vector3D pos;
//	
//	if ( ! is_voxelpos_within_image_3D( center ) )
//	{	// the point is not within the image
//		// TODO: use pt_error
//		return pos;
//		// return center;
//	}
//
//	center[0] = floor( center[0] );
//	center[1] = floor( center[1] );
//	center[2] = floor( center[2] );
//
//
//	Vector3D start = center - radius;
//	Vector3D end = center + radius;
//
//	pos = center;	// the center position is returned if no other is found
//
//	for ( int z = start[2]; z <= end[2]; z++ )
//	{
//		for ( int y = start[1]; y <= end[1]; y++ )
//		{
//			for ( int x = start[0]; x <= end[0]; x++ )
//			{
//			
//				switch ( point_type )
//				{
//					case MAX_GRAD_MAG_X:
//					case MAX_GRAD_MAG_Y:
//					case MAX_GRAD_MAG_Z:
//					case MAX_GRAD_MAG:
//						// use chessboard distance as weight
//						int dist_x = abs ( x - center[0] );
//						int dist_y = abs ( y - center[1] );
//						int dist_z = abs ( z - center[2] );
//						float weight = max( max( dist_x, dist_y ), dist_z );
//						if ( weight != 0 )
//						{ // not the center voxel (i.e. weight > 0)
//							GRAD_MAG_TYPE grad_mag_type;
//							switch ( grad_mag_type )
//							{
//								case MAX_GRAD_MAG_X: grad_mag_type = X; break;
//								case MAX_GRAD_MAG_Y: grad_mag_type = Y; break;
//								case MAX_GRAD_MAG_Z: grad_mag_type = Z; break;
//								case MAX_GRAD_MAG: grad_mag_type = XYZ; break;
//							}
//							float current_grad_mag = grad_mag_voxel( x, y, z, grad_mag_type );
//
//							// This line should be outside the loop because the value does not change
//							// but it is more intuitive to have it here
//							float center_grad_mag = grad_mag_voxel( center, point_type );
//							
//							if ( ( current_grad_mag / weight ) > center_grad_mag )
//								{ pos = create_Vector3D( x, y, z ); }
//							
////							std::cout << "test " << current_grad_mag << std::endl;
////							std::cout << "test floor " << floor( current_grad_mag ) << std::endl;
//							res->set_voxel( x, y, z, floor( current_grad_mag / weight ) );
//								
//						}
//						break;
//						
//
//				}
//			}
//		}
//	}
//
//	res->set_voxel( pos[0], pos[1], pos[2], 511 );
//	datamanagement.add( res );
//
//	return pos;
//}

#include "image_scalarprocess.hxx"

#endif