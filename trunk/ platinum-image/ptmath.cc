
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


#include "ptmath.h"
#include "error.h"
#include <vcl_iostream.h>
#include <vnl/algo/vnl_matrix_inverse.h>

line3D::line3D()
{
	point = create_Vector3D(0,0,0);
	direction = create_Vector3D(0,0,0);
}

line3D least_square_fit_line_to_points_in_3D(vector<Vector3D> v)
{
	line3D line = line3D();

	for(int p=0;p<v.size();p++){
		cout<<"p="<<p<<" "<<v[p]<<endl;
		line.point += v[p];
	}
	line.point /= v.size();
	cout<<"center="<<endl<<line.point<<endl;

	Vector3D R = create_Vector3D(0,0,0);
	for(int p=0;p<v.size();p++){
		R += (v[p]-line.point)*(v[p]-line.point);
	}
	return line;
}


void pt_spline1D(float x[],float y[],int n,float yp1,float ypn,float y2[])
{
	int i,k;
	float p,qn,sig,un,*u;

	u = new float[n+2];

	if (yp1 > 0.99e30)			//The lower boundary condition is set either to be "natural" y2=0...
		y2[1] = u[1] = 0.0;		
	else {						//...or else to have a specific first derivative.
		y2[1] = -0.5;			
		u[1] = (3.0/(x[2]-x[1]))*((y[2]-y[1])/(x[2]-x[1])-yp1);
	}

	for(i=2;i<=n;i++) {
		sig = (x[i]-x[i-1])/(x[i+1]-x[i-1]);
		p = sig*y2[i-1]+2.0;
		y2[i] = (sig-1.0)/p;
		u[i] = (y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]);
		u[i] = (6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
	}
	if (ypn > 0.99e30)
		qn = un = 0.0;
	else {
		qn=0.5;
		un=(3.0/(x[n]-x[n-1]))*(ypn-(y[n]-y[n-1])/(x[n]-x[n-1]));
	}

	y2[n] = (un-qn*u[n-1])/(qn*y2[n-1]+1.0);
	for (k=n-1;k>=1;k--)
		y2[k]=y2[k]*y2[k+1]+u[k];

	delete u;
}


float pt_splint1D(float xa[],float ya[],float y2a[],int n,float x)
{
	int klo,khi,k;
	float h,b,a;

	klo=1;
	khi=n;
	while (khi-klo > 1) {
		k=(khi+klo) >> 1;
		if (xa[k] > x) khi=k;
		else klo=k;
	}
	h=xa[khi]-xa[klo];
	if ( h == 0.0) 
		std::cout<<"Bad xa input to routine splint"<<std::endl;
	a = (xa[khi]-x)/h;
	b = (x-xa[klo])/h;

	return a*ya[klo]+b*ya[khi]+((a*a*a-a)*y2a[klo]+(b*b*b-b)*y2a[khi])*(h*h)/6.0;
}

/*
Matrix3D matrix_generator::get_rot_x_matrix_3D(float fi)			//fi in radians
{
    Matrix3D r;
    r[0][0] = 1;	r[1][0] = 0; 		r[2][0] = 0;
    r[0][1] = 0;	r[1][1] = cos(fi); 	r[2][1] = -sin(fi);
    r[0][2] = 0;	r[1][2] = sin(fi); 	r[2][2] = cos(fi);
    return r;
}

Matrix3D matrix_generator::get_rot_y_matrix_3D(float fi)			//fi in radians
{
    Matrix3D r;
    r[0][0] = cos(fi);	r[1][0] = 0; 		r[2][0] = sin(fi);
    r[0][1] = 0;		r[1][1] = 1;		r[2][1] = 0;
    r[0][2] = -sin(fi);	r[1][2] = 0;		r[2][2] = cos(fi);
    return r;
}

Matrix3D matrix_generator::get_rot_z_matrix_3D(float fi)			//fi in radians
{
    Matrix3D r;
    r[0][0] = cos(fi);	r[1][0] = -sin(fi);	r[2][0] = 0;
    r[0][1] = sin(fi);	r[1][1] = cos(fi);	r[2][1] = 0;
    r[0][2] = 0;		r[1][2] = 0;		r[2][2] = 1;
    return r;
}

// Note that order of the angles are z, y and x!
Matrix3D matrix_generator::get_rot_matrix_3D(int fi_z, int fi_y, int fi_x)	//fi_z/y/x in radians
{
    return get_rot_z_matrix_3D(float(fi_z)*PI/180.0)*get_rot_y_matrix_3D(float(fi_y)*PI/180.0)*get_rot_x_matrix_3D(float(fi_x)*PI/180.0);
}

// Note that order of the angles are z, y and x!
Matrix3D matrix_generator::get_rot_matrix_3D(float fi_z, float fi_y, float fi_x)	//fi_z/y/x in radians
{
    return get_rot_z_matrix_3D(fi_z)*get_rot_y_matrix_3D(fi_y)*get_rot_x_matrix_3D(fi_x);
}
*/

Matrix3D create_rot_x_matrix_3D(float fi_rad)
{
    Matrix3D r;
    r[0][0] = 1;	r[1][0] = 0; 			r[2][0] = 0;
    r[0][1] = 0;	r[1][1] = cos(fi_rad); 	r[2][1] = -sin(fi_rad);
    r[0][2] = 0;	r[1][2] = sin(fi_rad); 	r[2][2] = cos(fi_rad);
    return r;
}

Matrix3D create_rot_y_matrix_3D(float fi_rad)
{
    Matrix3D r;
    r[0][0] = cos(fi_rad);	r[1][0] = 0; 		r[2][0] = sin(fi_rad);
    r[0][1] = 0;			r[1][1] = 1;		r[2][1] = 0;
    r[0][2] = -sin(fi_rad);	r[1][2] = 0;		r[2][2] = cos(fi_rad);
    return r;
}

Matrix3D create_rot_z_matrix_3D(float fi_rad)
{
    Matrix3D r;
	r[0][0] = cos(fi_rad);	r[1][0] = -sin(fi_rad);	r[2][0] = 0;
    r[0][1] = sin(fi_rad);	r[1][1] = cos(fi_rad);	r[2][1] = 0;
    r[0][2] = 0;			r[1][2] = 0;			r[2][2] = 1;
     return r;
}

Matrix3D create_rot_matrix_3D(float fi_x_rad, float fi_y_rad, float fi_z_rad)	//fi_z/y/x in radians
{
	//matrixes shall be mlutiplied as m_z*m_y*m_x... // After many mistakes... The arguments are given as x/y/z - rotation
    return create_rot_z_matrix_3D(fi_z_rad)*create_rot_y_matrix_3D(fi_y_rad)*create_rot_x_matrix_3D(fi_x_rad);
}


// Tricubic interpolation using method described in:
// F. Lekien, J.E. Marsden
// Tricubic Interpolation in Three Dimensions
// International Journal for Numerical Methods in Engineering, 63 (3), 455-471, 2005
// ...used in the function: image_scalar.interpolate_tricubic_3D...

const int A_tricubic[64][64] = {
{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{-3, 3, 0, 0, 0, 0, 0, 0,-2,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 2,-2, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{-3, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0,-3, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 9,-9,-9, 9, 0, 0, 0, 0, 6, 3,-6,-3, 0, 0, 0, 0, 6,-6, 3,-3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{-6, 6, 6,-6, 0, 0, 0, 0,-3,-3, 3, 3, 0, 0, 0, 0,-4, 4,-2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2,-2,-1,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 2, 0,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 2, 0,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{-6, 6, 6,-6, 0, 0, 0, 0,-4,-2, 4, 2, 0, 0, 0, 0,-3, 3,-3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2,-1,-2,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 4,-4,-4, 4, 0, 0, 0, 0, 2, 2,-2,-2, 0, 0, 0, 0, 2,-2, 2,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-3, 3, 0, 0, 0, 0, 0, 0,-2,-1, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,-2, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-3, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-3, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2, 0,-1, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9,-9,-9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 3,-6,-3, 0, 0, 0, 0, 6,-6, 3,-3, 0, 0, 0, 0, 4, 2, 2, 1, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-6, 6, 6,-6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-3,-3, 3, 3, 0, 0, 0, 0,-4, 4,-2, 2, 0, 0, 0, 0,-2,-2,-1,-1, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-6, 6, 6,-6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-4,-2, 4, 2, 0, 0, 0, 0,-3, 3,-3, 3, 0, 0, 0, 0,-2,-1,-2,-1, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,-4,-4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2,-2,-2, 0, 0, 0, 0, 2,-2, 2,-2, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
{-3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2, 0, 0, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0,-3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2, 0, 0, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 9,-9, 0, 0,-9, 9, 0, 0, 6, 3, 0, 0,-6,-3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,-6, 0, 0, 3,-3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2, 0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{-6, 6, 0, 0, 6,-6, 0, 0,-3,-3, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-4, 4, 0, 0,-2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2,-2, 0, 0,-1,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2, 0, 0, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2, 0, 0, 0,-1, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9,-9, 0, 0,-9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 3, 0, 0,-6,-3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,-6, 0, 0, 3,-3, 0, 0, 4, 2, 0, 0, 2, 1, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-6, 6, 0, 0, 6,-6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-3,-3, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-4, 4, 0, 0,-2, 2, 0, 0,-2,-2, 0, 0,-1,-1, 0, 0},
{ 9, 0,-9, 0,-9, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 3, 0,-6, 0,-3, 0, 6, 0,-6, 0, 3, 0,-3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 2, 0, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 9, 0,-9, 0,-9, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 3, 0,-6, 0,-3, 0, 6, 0,-6, 0, 3, 0,-3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 2, 0, 2, 0, 1, 0},
{-27,27,27,-27,27,-27,-27,27,-18,-9,18, 9,18, 9,-18,-9,-18,18,-9, 9,18,-18, 9,-9,-18,18,18,-18,-9, 9, 9,-9,-12,-6,-6,-3,12, 6, 6, 3,-12,-6,12, 6,-6,-3, 6, 3,-12,12,-6, 6,-6, 6,-3, 3,-8,-4,-4,-2,-4,-2,-2,-1},
{18,-18,-18,18,-18,18,18,-18, 9, 9,-9,-9,-9,-9, 9, 9,12,-12, 6,-6,-12,12,-6, 6,12,-12,-12,12, 6,-6,-6, 6, 6, 6, 3, 3,-6,-6,-3,-3, 6, 6,-6,-6, 3, 3,-3,-3, 8,-8, 4,-4, 4,-4, 2,-2, 4, 4, 2, 2, 2, 2, 1, 1},
{-6, 0, 6, 0, 6, 0,-6, 0, 0, 0, 0, 0, 0, 0, 0, 0,-3, 0,-3, 0, 3, 0, 3, 0,-4, 0, 4, 0,-2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2, 0,-2, 0,-1, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0,-6, 0, 6, 0, 6, 0,-6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-3, 0,-3, 0, 3, 0, 3, 0,-4, 0, 4, 0,-2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2, 0,-2, 0,-1, 0,-1, 0},
{18,-18,-18,18,-18,18,18,-18,12, 6,-12,-6,-12,-6,12, 6, 9,-9, 9,-9,-9, 9,-9, 9,12,-12,-12,12, 6,-6,-6, 6, 6, 3, 6, 3,-6,-3,-6,-3, 8, 4,-8,-4, 4, 2,-4,-2, 6,-6, 6,-6, 3,-3, 3,-3, 4, 2, 4, 2, 2, 1, 2, 1},
{-12,12,12,-12,12,-12,-12,12,-6,-6, 6, 6, 6, 6,-6,-6,-6, 6,-6, 6, 6,-6, 6,-6,-8, 8, 8,-8,-4, 4, 4,-4,-3,-3,-3,-3, 3, 3, 3, 3,-4,-4, 4, 4,-2,-2, 2, 2,-4, 4,-4, 4,-2, 2,-2, 2,-2,-2,-2,-2,-1,-1,-1,-1},
{ 2, 0, 0, 0,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{-6, 6, 0, 0, 6,-6, 0, 0,-4,-2, 0, 0, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-3, 3, 0, 0,-3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2,-1, 0, 0,-2,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 4,-4, 0, 0,-4, 4, 0, 0, 2, 2, 0, 0,-2,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,-2, 0, 0, 2,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-6, 6, 0, 0, 6,-6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-4,-2, 0, 0, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-3, 3, 0, 0,-3, 3, 0, 0,-2,-1, 0, 0,-2,-1, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,-4, 0, 0,-4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0,-2,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,-2, 0, 0, 2,-2, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0},
{-6, 0, 6, 0, 6, 0,-6, 0, 0, 0, 0, 0, 0, 0, 0, 0,-4, 0,-2, 0, 4, 0, 2, 0,-3, 0, 3, 0,-3, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2, 0,-1, 0,-2, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0,-6, 0, 6, 0, 6, 0,-6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-4, 0,-2, 0, 4, 0, 2, 0,-3, 0, 3, 0,-3, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2, 0,-1, 0,-2, 0,-1, 0},
{18,-18,-18,18,-18,18,18,-18,12, 6,-12,-6,-12,-6,12, 6,12,-12, 6,-6,-12,12,-6, 6, 9,-9,-9, 9, 9,-9,-9, 9, 8, 4, 4, 2,-8,-4,-4,-2, 6, 3,-6,-3, 6, 3,-6,-3, 6,-6, 3,-3, 6,-6, 3,-3, 4, 2, 2, 1, 4, 2, 2, 1},
{-12,12,12,-12,12,-12,-12,12,-6,-6, 6, 6, 6, 6,-6,-6,-8, 8,-4, 4, 8,-8, 4,-4,-6, 6, 6,-6,-6, 6, 6,-6,-4,-4,-2,-2, 4, 4, 2, 2,-3,-3, 3, 3,-3,-3, 3, 3,-4, 4,-2, 2,-4, 4,-2, 2,-2,-2,-1,-1,-2,-2,-1,-1},
{ 4, 0,-4, 0,-4, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0,-2, 0,-2, 0, 2, 0,-2, 0, 2, 0,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 4, 0,-4, 0,-4, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0,-2, 0,-2, 0, 2, 0,-2, 0, 2, 0,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0},
{-12,12,12,-12,12,-12,-12,12,-8,-4, 8, 4, 8, 4,-8,-4,-6, 6,-6, 6, 6,-6, 6,-6,-6, 6, 6,-6,-6, 6, 6,-6,-4,-2,-4,-2, 4, 2, 4, 2,-4,-2, 4, 2,-4,-2, 4, 2,-3, 3,-3, 3,-3, 3,-3, 3,-2,-1,-2,-1,-2,-1,-2,-1},
{ 8,-8,-8, 8,-8, 8, 8,-8, 4, 4,-4,-4,-4,-4, 4, 4, 4,-4, 4,-4,-4, 4,-4, 4, 4,-4,-4, 4, 4,-4,-4, 4, 2, 2, 2, 2,-2,-2,-2,-2, 2, 2,-2,-2, 2, 2,-2,-2, 2,-2, 2,-2, 2,-2, 2,-2, 1, 1, 1, 1, 1, 1, 1, 1}};


void print_datatype_numerical_limits()
{
	std::cout<<"Data Type: \t min \t\t max \t\t digits"<<std::endl;
	std::cout<<"---------------------------------------"<<std::endl;
	std::cout<<"double:   \t";		print_type_limits<double>();
//	std::cout<<"long double: \t";	print_type_limits<long double>();
	std::cout<<"float: \t";			print_type_limits<float>();
	std::cout<<"short: \t";			print_type_limits<short>();
	std::cout<<"unsigned short: \t";print_type_limits<unsigned short>();
	std::cout<<"int: \t";			print_type_limits<int>();
	std::cout<<"unsigned int: \t";	print_type_limits<unsigned int>();
	std::cout<<"long: \t";			print_type_limits<long>();
	std::cout<<"unsigned long: \t";	print_type_limits<unsigned long>();
	std::cout<<"char: \t";			print_type_limits<char>();
	std::cout<<"unsigned char: \t";	print_type_limits<unsigned char>();
	std::cout<<"signed char: \t";	print_type_limits<signed char>();
	std::cout<<"bool: \t";			print_type_limits<bool>();
}

double get_random_number_in_span(double min, double max)
{
	return min + (max-min)*(double(rand())/double(RAND_MAX));
}

Vector3D create_Vector3D(float x, float y, float z)
{
	Vector3D v;
	v[0]=x;
	v[1]=y;
	v[2]=z;
	return v;
}

Vector3Dint create_Vector3Dint( int x, int y, int z )
{
	Vector3Dint v;
	v[0] = x;
	v[1] = y;
	v[2] = z;
	return v;
}

Matrix3D outer_product(const Vector3D a, const Vector3D b)
{
	Matrix3D m;
	
	for ( int i  = 0; i < 3; i++ )
	{
		for ( int j = 0; j < 3; j++ )
			{ m[i][j] = a[i] * b[j]; }
	}
	return m;
}

unsigned int get_factorial(unsigned int i)
{
	if(i==1){
		return 1;
	}
	else{
		return i*get_factorial(i-1);
	}
}

unsigned int get_permutations(unsigned int n, unsigned int r)
{
	return get_factorial(n)/(get_factorial(r)*get_factorial(n-r));
}

unsigned int get_smallest_power_above(unsigned int this_val, unsigned int power_base)
{
	unsigned int res=0;
	for(unsigned int i=0;i<std::numeric_limits<unsigned int>::max();i++){
		res = pow(float(power_base),float(i));
		if(res>=this_val){
			return res;
		}
	}
	return 0;
}

vnl_float_2 mean(const std::vector<vnl_float_2> & x)
{
	vnl_float_2 sum(0.0, 0.0);
	for ( std::vector<vnl_float_2>::const_iterator itr = x.begin(); itr != x.end(); itr++ )
		{ sum += *itr; }
	sum /= x.size();
	return sum;
}

vnl_float_3 mean(const std::vector<vnl_float_3> & x)
{
	vnl_float_3 sum(0.0, 0.0, 0.0);
	for ( std::vector<vnl_float_3>::const_iterator itr = x.begin(); itr != x.end(); itr++ )
		{ sum += *itr; }
	sum /= x.size();
	return sum;
}

vnl_float_2x2 cov(const std::vector<vnl_float_2> & x)
{
	const vnl_float_2 xmean = mean(x);

	vnl_float_2x2 sx;
	sx.fill(0.0);
	for ( std::vector<vnl_float_2>::const_iterator itr = x.begin(); itr != x.end(); itr++ )
		{ sx += outer_product(*itr - xmean, *itr - xmean); }

	sx /= (x.size() - 1);
		
	return sx;
}

vnl_float_2x2 cov(const std::vector<vnl_float_2> & x, const std::vector<vnl_float_2> & y)
{
	vnl_float_2x2 sx = cov(x);
	sx *= (x.size() - 1);

	vnl_float_2x2 sy = cov(y);
	sy *= (y.size() - 1);
	
	vnl_float_2x2 s = sx + sy;
	s /= (x.size() + y.size() - 2);
	
	return s;
}

vnl_float_3x3 cov(const std::vector<vnl_float_3> & x)
{
	const vnl_float_3 xmean = mean(x);

	vnl_float_3x3 sx;
	sx.fill(0.0);
	for ( std::vector<vnl_float_3>::const_iterator itr = x.begin(); itr != x.end(); itr++ )
		{ sx += outer_product(*itr - xmean, *itr - xmean); }

	sx /= (x.size() - 1);
		
	return sx;
}

vnl_float_3x3 cov(const std::vector<vnl_float_3> & x, const std::vector<vnl_float_3> & y)
{
	vnl_float_3x3 sx = cov(x);
	sx *= (x.size() - 1);

	vnl_float_3x3 sy = cov(y);
	sy *= (y.size() - 1);
	
	vnl_float_3x3 s = sx + sy;
	s /= (x.size() + y.size() - 2);
	
	return s;
}

float tsquare2d(const std::vector<vnl_float_2> & x, const std::vector<vnl_float_2> & y)
{
	const float nx = x.size();				// use float to avoid "int/int = int" later
	const float ny = y.size();
	const vnl_float_2 xmean = mean(x);
	const vnl_float_2 ymean = mean(y);
	const vnl_float_2x2 s = cov(x, y);
	const vnl_float_2x2 s_inv = vnl_matrix_inverse<float>(s).inverse();

	return ((nx * ny) / (nx + ny)) * dot_product(xmean - ymean, s_inv * (xmean - ymean));
}

float tsquare3d(const std::vector<vnl_float_3> & x, const std::vector<vnl_float_3> & y)
{
	const float nx = x.size();				// use float to avoid "int/int = int" later
	const float ny = y.size();
	const vnl_float_3 xmean = mean(x);
	const vnl_float_3 ymean = mean(y);
	const vnl_float_3x3 s = cov(x, y);
	const vnl_float_3x3 s_inv = vnl_matrix_inverse<float>(s).inverse();

	return ((nx * ny) / (nx + ny)) * dot_product(xmean - ymean, s_inv * (xmean - ymean));
}

double invF(const double p, const double a, const double b)
{ 
	// Return inverse cumulative distribution function. 
	if ( p <= 0.0 || p >= 1.0 )
		{ pt_error::error("invcdf(): argument out of range", pt_error::serious); }
		
	double x = invIncompleteBeta(p, 0.5 * a, 0.5 * b); 
	return b * x / (a * (1.0 - x));
}

double invIncompleteBeta(const double p, const double a, const double b)
{
	double x;
	double i = 0;
	double j = 1;
	double precision = 1.0e-10;		// converge until 10 decimals

	while ( (j - i) > precision )
	{
		x = (i + j) / 2;
		
		if ( incompleteBeta(a, b, x) > p )
			{ j = x; }
		else
			{ i = x; }
	}
	return x;
}

// Lanczos approximation is used to compute the Gamma function.
// The logarithm of the Gamma function is computed since the
// Gamma function will overflow at quite small values.
double logGamma(double x)
{
	if ( x <= 0 )
		{ pt_error::error("gammaln(): value out of range", pt_error::serious); }

	const double c[7] = { 1.000000000190015, 76.18009172947146, -86.50532032941677,
		24.01409824083091, -1.231739572450155, 1.208650973866179e-3, -5.395239384953e-6 };

	double sum = 0.0;
	for ( int i = 1; i <= 6; i++ )
		{ sum += c[i] / (x + i); }

	return log((sqrt(2 * PI) / x) * (c[0] + sum)) + (x + 0.5) * log(x + 5.5) - (x + 5.5);
}


double incompleteBeta(const double a, const double b, const double x)
{ 
	if ( a <= 0.0 || b <= 0.0 )
		{ pt_error::error("betai(): argument out of range", pt_error::serious); }
	if ( x < 0.0 || x > 1.0 ) 
		{ pt_error::error("betai(): argument out of range", pt_error::serious); }

	if ( x == 0.0 || x == 1.0 )
		{ return x; }

	double result = std::exp(logGamma(a + b) - logGamma(a) - logGamma(b) + a * std::log(x) + b * std::log(1.0 - x));
	
	if ( x < ((a + 1.0) / (a + b + 2.0)) )
		{ return result * incompleteBetaCF(a, b, x) / a; }
	else
		{ return 1.0 - result * incompleteBetaCF(b, a, 1.0 - x) / b; }
}

double incompleteBetaCF(const double a, const double b, const double x)
{
	const double accuracy = std::numeric_limits<double>::epsilon();
	const double qlepsilon = std::numeric_limits<double>::min() / accuracy;

	double aa, del;
	double qab = a + b;
	double qap = a + 1.0;
	double qam = a - 1.0;
	double c = 1.0;
	double d = 1.0 - qab * x / qap;
	
	if ( std::fabs(d) < qlepsilon )
		{ d = qlepsilon; }

	d = 1.0 / d;
	double result = d;

	int j;
	for ( int i = 1; i <= 10000; i++ )
	{
		j = 2 * i;
		aa = i * (b - i) * x / ((qam + j) * (a + j));
		d = 1.0 + aa * d;
		
		if ( std::fabs(d) < qlepsilon )
			{ d = qlepsilon; }
			
		c	= 1.0 + aa / c;
		
		if ( std::fabs(c) < qlepsilon )
			{ c = qlepsilon; }
			
		d = 1.0 / d;
		result *= d * c;
		aa = -(a + i) * (qab + i) * x / ((a + j) * (qap + j));
		d = 1.0 + aa * d;
		
		if ( std::fabs(d) < qlepsilon )
			{ d = qlepsilon; }
			
		c = 1.0 + aa / c;
		
		if ( std::fabs(c) < qlepsilon )
			{ c = qlepsilon; }
			
		d = 1.0 / d;
		del = d * c;
		result *= del;
		
		if ( std::fabs(del - 1.0) < accuracy )
			{ break; }
	}
	return result;
}
