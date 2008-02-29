
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

//rotation examples based on the "basic" image processing coordinate system
//(x-->right, y-->down and z--> the view direction of the screen)
//+fi_z rotates the image volume: "Counterclockwise" of the z-direction
//+fi_y rotates the image volume: "Counterclockwise" of the y-direction
//+fi_x rotates the image volume: "Counterclockwise" of the x-direction

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











// sample mean
vnl_float_2 mean(const std::vector<vnl_float_2> & x)
{
	vnl_float_2 sum(0.0, 0.0);
	for ( std::vector<vnl_float_2>::const_iterator itr = x.begin(); itr != x.end(); itr++ )
		{ sum += *itr; }
	sum /= x.size();
	return sum;
}

// sample mean (centroid)
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

// Hotelling's two-sample t-square statistic
float tsquare(const std::vector<vnl_float_2> & x, const std::vector<vnl_float_2> & y)
{
	const float nx = x.size();	// use float to avoid int/int = int later
	const float ny = y.size();
	const vnl_float_2 xmean = mean(x);
	const vnl_float_2 ymean = mean(y);
	const vnl_float_2x2 s = cov(x, y);
	const vnl_float_2x2 s_inv = vnl_matrix_inverse<float>(s).inverse();
	
	// vnl_float_2 a_star = s_inv * (xmean - ymean);

	return ((nx * ny) / (nx + ny)) * dot_product(xmean - ymean, s_inv * (xmean - ymean));
}

// 
double invcdf(double p, double a, double b)
{ 
	// Return inverse cumulative distribution function. 
	if ( p <= 0.0 || p >= 1.0 )
		{ pt_error::error("invcdf(): value out of range", pt_error::serious); }
		
	double x = invbetai(p, 0.5 * a, 0.5 * b); 
	return b * x / (a * (1.0 - x));
}

double invbetai(double p, double a, double b)
{ 
	const double EPS = 1.e-8; 
	double pp,t,u,err,x,al,h,w,afac,a1=a-1.,b1=b-1.; 
	int j; 
	if (p <= 0.) return 0.; 
	else if (p >= 1.) return 1.; 
	else if (a >= 1. && b >= 1.)
	{ 
		pp = (p < 0.5)? p : 1. - p; 
		t = sqrt(-2.*log(pp)); 
		x = (2.30753+t*0.27061)/(1.+t*(0.99229+t*0.04481)) - t; 
		if (p < 0.5) x = -x; 
		al = (SQR(x)-3.)/6.;						// prova att ändra SQR(x) till x*x och ta bort #define i h-filen
		h = 2./(1./(2.*a-1.)+1./(2.*b-1.)); 
		w = (x*sqrt(al+h)/h)-(1./(2.*b-1)-1./(2.*a-1.))*(al+5./6.-2./(3.*h)); 
		x = a/(a+b*exp(2.*w)); 
	}
	else
	{ 
		double lna = log(a/(a+b)), lnb = log(b/(a+b)); 
		t = exp(a*lna)/a; 
		u = exp(b*lnb)/b; 
		w = t + u; 
		if (p < t/w) x = pow(a*w*p,1./a); 
		else x = 1. - pow(b*w*(1.-p),1./b); 
	} 
	afac = -gammln(a)-gammln(b)+gammln(a+b); 
	for (j=0;j<10;j++)
	{ 
		if (x == 0. || x == 1.) return x; 
		err = betai(a,b,x) - p;									// varför är inte anropet betai(x,a,b) ?? felskrivet?
		t = exp(a1*log(x)+b1*log(1.-x) + afac); 
		u = err/t;
		x -= (t = u/(1.-0.5*MIN(1.,u*(a1/x - b1/(1.-x)))));		// prova att ändra MIN() till min och ta bort #define i h-filen
		if (x <= 0.) x = 0.5*(x + t);
		if (x >= 1.) x = 0.5*(x + t + 1.); 
		if (fabs(t) < EPS*x && j > 0) break; 
	} 
	return x; 
} 

/*
double invbetaiAF(const double p, const double a, const double b)
{
	double x;
	double i = 0;
	double j = 1;
	double precision = 1.0e-8;		// converge until 8 decimals
	
	std::cout << "precision " << precision << std::endl;

	while ( (j - i) > precision )
	{
		x = (i + j) / 2;
		
		if ( betai(a, b, x) > p )					// varför är inte anropet betai(x,a,b) ?? felskrivet? prova!
			{ j = x; }
		else
			{ i = x; }
	}
	return x;
}
*/

double gammln(const double xx)
{ 
	int j;
	double x,tmp,y,ser; 
	static const double cof[14]={57.1562356658629235,-59.5979603554754912, 
	14.1360979747417471,-0.491913816097620199,.339946499848118887e-4, 
	.465236289270485756e-4,-.983744753048795646e-4,.158088703224912494e-3, 
	-.210264441724104883e-3,.217439618115212643e-3,-.164318106536763890e-3, 
	.844182239838527433e-4,-.261908384015814087e-4,.368991826595316234e-5}; 
	if (xx <= 0) throw("bad arg in gammln"); 
	y=x=xx; 
	tmp = x+5.24218750000000000; 
	tmp = (x+0.5)*log(tmp)-tmp; 
	ser = 0.999999999999997092; 
	for (j=0;j<14;j++) ser += cof[j]/++y; 
	return tmp+log(2.5066282746310005*ser/x); 
} 

/*
// Lanczos approximation is used to compute the Gamma function.
// The logarithm of the Gamma function is computed since the
// Gamma function will overflow at quite small values.
double gammalnAF(double x)
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
*/

// 
double betai(const double a, const double b, const double x)
{ 
	//return betaiAF();

	// Returns incomplete beta function Ix .a; b/ for positive a and b, and x between 0 and 1. 

	//	static const int SWITCH = 3000;

	double bt;
	
	if ( a <= 0.0 || b <= 0.0 )
		{ pt_error::error("betai(): value out of range", pt_error::serious); }
	if ( x < 0.0 || x > 1.0 ) 
		{ pt_error::error("betai(): value out of range", pt_error::serious); }
	if ( x == 0.0 || x == 1.0 )
		{ pt_error::error("betai(): value out of range", pt_error::serious); }
	
	//if (a > SWITCH && b > SWITCH) return betaiapprox(a,b,x);
	
	bt = exp(gammln(a + b) - gammln(a) - gammln(b) + a * log(x) + b * log(1.0 - x));
	
	if (x < (a + 1.0) / (a + b + 2.0))
		{ return bt * betacf(a, b, x) / a; }
	else
		{ return 1.0 - bt * betacf(b, a, 1.0 - x) / b; }
}

// Incomplete beta function
double betaiAF(const double a, const double b, const double x)
{
	// OBS! om jag ändrar ordningen på parametrarna så måste jag också se till att anropen sker därefter

	if ( x < 0.0 || x > 1.0 )
		{ pt_error::error("betai(): value out of range", pt_error::serious); }
	if ( a <= 0.0 || b <= 0.0 )
		{ pt_error::error("betai(): value out of range", pt_error::serious); }
		
	if ( x < 0.5 )
		{ return betacf(a, b, x); }
	else
		{ return 1.0 - betacf(b, a, 1.0 - x); }
}

// 
double betacf(const double a, const double b, const double x)
{ 
	// Evaluates continued fraction for incomplete beta function by modified Lentz’s method 
	// (§5.2). User should not call directly. 
	
	const double EPS = numeric_limits<double>::epsilon();
	const double FPMIN = numeric_limits<double>::min() / EPS;
	
	int m,m2; 
	double aa, c, d, del, h, qab, qam, qap;
	
	qab = a+b;						// These q’s will be used in factors that occur in the coefficients (6.4.6).
	qap = a + 1.0; 
	qam = a - 1.0; 
	c = 1.0;						// First step of Lentz’s method. 
	d = 1.0 - qab * x / qap;
	
	if ( fabs(d) < FPMIN )
		{ d = FPMIN; }
		
	d = 1.0 / d; 
	h = d;  
	for ( m = 1; m < 10000; m++ )
	{ 
		m2 = 2 * m; 
		aa = m * (b - m) * x / ((qam + m2) * (a + m2)); 
		d = 1.0 + aa * d;			// One step (the even one) of the recurrence.
		
		if ( fabs(d) < FPMIN )
			{ d = FPMIN; }
			
		c = 1.0 + aa / c; 
		
		if (fabs(c) < FPMIN)
			{ c = FPMIN; }
			
		d = 1.0 / d; 
		h *= d * c; 
		aa = -(a + m) * (qab + m) * x / ((a + m2) * (qap + m2)); 
		d = 1.0 + aa * d;			// Next step of the recurrence (the odd one).
		
		if ( fabs(d) < FPMIN) d=FPMIN; 
		
		c=1.0+aa/c; 
		
		if (fabs(c) < FPMIN) c=FPMIN; 
		d=1.0/d; 
		del=d*c; 
		h *= del; 
		if (fabs(del-1.0) <= EPS) break; // Are we done? 
	} 
	return h;
} 

/*
double betacf(const double a, const double b, const double x)
{
	// OBS! om jag ändrar ordningen på parametrarna så måste jag också se till att anropen sker därefter

}
*/
