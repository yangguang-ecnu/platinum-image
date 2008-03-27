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

line2D::line2D()
{
	point = create_Vector2D(0,0);
	direction = create_Vector2D(1,1);
}

line2D::line2D(Vector2D pnt1, Vector2D pnt2)
{
	pt_error::error_if_false((pnt2[0]-pnt1[0])!=0 || (pnt2[1]-pnt1[1])!=0, "Direction must be non-zero vector", pt_error::debug);
	point = pnt1;
	direction = pnt2-pnt1;
}

line2D::line2D(float x1, float y1, float x2, float y2)
{
	pt_error::error_if_false((x2-x1)!=0 || (y2-y1)!=0, "Direction must be non-zero vector", pt_error::debug);
	point = create_Vector2D(x1,y1);
	direction = create_Vector2D(x2-x1, y2-y1);
}

Vector2D line2D::get_point()
{
	return point;
}

Vector2D line2D::get_direction()
{
	return direction;
}

void line2D::set_point(Vector2D pnt)
{
	point=pnt;
}

void line2D::set_point(float point_x, float point_y)
{
	point = create_Vector2D(point_x,point_y);
}

void line2D::set_direction(Vector2D dir)
{
	pt_error::error_if_false(dir[0]!=0 || dir[1]!=0, "Direction must be non-zero vector", pt_error::debug);
	direction=dir;
}

void line2D::set_direction(float dir_x, float dir_y)
{
	pt_error::error_if_false(dir_x!=0 || dir_y!=0, "Direction must be non-zero vector", pt_error::debug);
	direction = create_Vector2D(dir_x,dir_y);
}

bool line2D::is_point_left_of_line(Vector2D pnt)
{
	return (pnt[1]-point[1])*direction[0]<(pnt[0]-point[0])*direction[1];
}

bool line2D::is_point_right_of_line(Vector2D pnt)
{
	return (pnt[1]-point[1])*direction[0]>(pnt[0]-point[0])*direction[1];
}

bool line2D::is_point_left_of_line(float x, float y)
{
	return is_point_left_of_line(create_Vector2D(x,y));
}

bool line2D::is_point_right_of_line(float x, float y)
{
	return is_point_right_of_line(create_Vector2D(x,y));
}

line3D::line3D()
{
	point = create_Vector3D(0,0,0);
	direction = create_Vector3D(1,1,1);
}

line3D::line3D(Vector3D pnt, Vector3D dir)
{
	pt_error::error_if_false(dir[0]!=0 || dir[1]!=0 || dir[2]!=0, "Direction must be non-zero vector", pt_error::debug);
	point = pnt;
	direction = dir;
}

line3D::line3D(float point_x, float point_y, float point_z, float dir_x, float dir_y, float dir_z)
{
	pt_error::error_if_false(dir_x!=0 || dir_y!=0 || dir_z!=0, "Direction must be non-zero vector", pt_error::debug);
	point = create_Vector3D(point_x,point_y,point_z);
	direction = create_Vector3D(dir_x, dir_y, dir_z);
}

Vector3D line3D::get_point()
{
	return point;
}

Vector3D line3D::get_direction()
{
	return direction;
}

void line3D::set_point(Vector3D pnt)
{
	point=pnt;
}

void line3D::set_point(float point_x, float point_y, float point_z)
{
	point = create_Vector3D(point_x,point_y,point_z);
}

void line3D::set_direction(Vector3D dir)
{
	pt_error::error_if_false(dir[0]!=0 || dir[1]!=0 || dir[2]!=0, "Direction must be non-zero vector", pt_error::debug);
	direction=dir;
}

void line3D::set_direction(float dir_x, float dir_y, float dir_z)
{
	pt_error::error_if_false(dir_x!=0 || dir_y!=0 || dir_z!=0, "Direction must be non-zero vector", pt_error::debug);
	direction = create_Vector3D(dir_x,dir_y,dir_z);
}


void line3D::least_square_fit_line_to_points_in_3D(vector<Vector3D> points, int dir)
{
	this->set_point_to_center_of_gravity_from_points_in_3D(points);
	this->set_direction_to_point_cloud_variations_given_one_dir(dir, points);
}

void line3D::set_point_to_center_of_gravity_from_points_in_3D(vector<Vector3D> points)
{
	for(int p=0;p<points.size();p++){
		this->point += points[p];
	}
	this->point /= points.size();
	cout<<"set_point_to_center_of_gravity_from_points_in_3D -->"<<this->point<<endl;
}

void line3D::set_direction_to_point_cloud_variations_given_one_dir(int dir, vector<Vector3D> points)
{
	Vector3D R = create_Vector3D(0,0,0);
	float da=0;
	float daa=0;
	for(int p=0;p<points.size();p++){
		da = points[p][dir]-this->point[dir];
		R[0] += (points[p][0]-this->point[0])*da;
		R[1] += (points[p][1]-this->point[1])*da;
		R[2] += (points[p][2]-this->point[2])*da;
		daa += da*da;
	}
	R[0] = R[0]/daa;	//Rxy/Ryy for example...
	R[1] = R[1]/daa;	//Rxy/Ryy for example...
	R[2] = R[2]/daa;	//Rxy/Ryy for example...
	R[dir] = 1;			//Ryy/Ryy = 1;
	this->direction = R;
	cout<<"direction="<<this->direction<<endl;
}

plane3D::plane3D()
{
	point = create_Vector3D(0,0,0);
	normal = create_Vector3D(1,0,0);
	defined=false;

}

plane3D::plane3D(Vector3D pnt, Vector3D norm)
{
	pt_error::error_if_false(norm[0]!=0 || norm[1]!=0 || norm[2]!=0, "Normal must be non-zero vector", pt_error::debug);
	point = pnt;
	normal = norm;
	defined=true;
}

plane3D::plane3D(float point_x, float point_y, float point_z, float normal_x, float normal_y, float normal_z)
{
	pt_error::error_if_false(normal_x!=0 || normal_y!=0 || normal_z!=0, "Normal must be non-zero vector", pt_error::debug);
	point = create_Vector3D(point_x,point_y,point_z);
	normal = create_Vector3D(normal_x,normal_y,normal_z);
	defined=true;
}

plane3D::plane3D(Vector3D point1, Vector3D point2, Vector3D point3) // Construct plane from 3 points, not laying on a straight line
{
	point = point1;
	Vector3D dir1 = create_Vector3D(point2[0]-point1[0],point2[1]-point1[1],point2[2]-point1[2]);
	Vector3D dir2 = create_Vector3D(point3[0]-point1[0],point3[1]-point1[1],point3[2]-point1[2]);
	normal = CrossProduct (dir1, dir2);
	pt_error::error_if_false(normal[0]!=0 || normal[1]!=0 || normal[2]!=0, "Normal must be non-zero vector", pt_error::debug);
	defined=true;
}

Vector3D plane3D::get_point()
{
	return point;
}

Vector3D plane3D::get_normal()
{
	return normal;
}

bool plane3D::is_defined()
{
	return defined;
}

void plane3D::set_point(Vector3D pnt)
{
	point=pnt;
	defined=true;
}

void plane3D::set_point(float point_x, float point_y, float point_z)
{
	point=create_Vector3D(point_x,point_y,point_z);
	defined=true;
}

void plane3D::set_normal(Vector3D norm)
{
	pt_error::error_if_false(norm[0]!=0 || norm[1]!=0 || norm[2]!=0, "Normal must be non-zero vector", pt_error::debug);
	normal=norm;
	defined=true;
}

void plane3D::set_normal(float normal_x, float normal_y, float normal_z)
{
	pt_error::error_if_false(normal_x!=0 || normal_y!=0 || normal_z!=0, "Normal must be non-zero vector", pt_error::debug);
	normal = create_Vector3D(normal_x,normal_y,normal_z);
	defined=true;
}

void plane3D::invert()
{
	normal*=-1;
}

Vector3D plane3D::get_point_of_intersection(line3D line)
{
	return line.get_point()+((point-line.get_point())*normal)/(line.get_direction()*normal)*line.get_direction();
}

line3D plane3D::get_line_of_intersection(plane3D plane)
{
	Vector3D dir=CrossProduct(normal, plane.get_normal());
	float d1=-point*normal;
	float d2=-plane.get_point()*plane.get_normal();
	Vector3D pnt=CrossProduct((d2*normal-d1*plane.get_normal()), dir)/dir.GetSquaredNorm();
	line3D line=line3D(pnt,dir);
	return line;
}

bool convex_last_three_points(vector<Vector2D>::iterator end, bool lower)
{
	line2D line=line2D(*(end-3), *(end-1));
	if (lower) 
		return line.is_point_right_of_line(*(end-2));
	else 
		return line.is_point_left_of_line(*(end-2));
}

vector<Vector2D> get_convex_hull_2D(vector<Vector2D> points)
{
	// GRAHAM SCAN:
	Vector2D left=points.front();
	points.erase(points.begin());
	Vector2D right=points.back();
	points.pop_back();
	
	line2D middle=line2D(left, right);
	Vector2D point;
	vector<Vector2D> hull, upper_hull, lower_hull, upper, lower;
	upper.push_back(right);
	lower.push_back(right);
	while (points.size()!=0) {
		point=points.back();
		points.pop_back();
		if (middle.is_point_left_of_line(point))
			upper.push_back(point);
		else if (middle.is_point_right_of_line(point))
			lower.push_back(point);
	}
	
	lower_hull.push_back(left);
	while (lower.size()!=0) {
		lower_hull.push_back(lower.back());
		lower.pop_back();
		while (lower_hull.size()>=3 && !convex_last_three_points(lower_hull.end(), true))
			lower_hull.erase(lower_hull.end()-2);
	}

	upper_hull.push_back(left);
	while (upper.size()!=0) {
		upper_hull.push_back(upper.back());
		upper.pop_back();
		while (upper_hull.size()>=3 && !convex_last_three_points(upper_hull.end(), false))
			upper_hull.erase(upper_hull.end()-2);
	}
	
	vector<Vector2D>::iterator i;
	for (i=lower_hull.begin(); i!=lower_hull.end()-1; i++)
		hull.push_back(*i);
	for (i=upper_hull.end()-1; i!=upper_hull.begin(); i--)
		hull.push_back(*i);

	return hull;
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

Vector2D create_Vector2D(float x, float y)
{
	Vector2D v;
	v[0] = x;
	v[1] = y;
	return v;
}

Matrix2D outer_product(const Vector2D a, const Vector2D b)
{
	Matrix2D m;
	
	for ( int i  = 0; i < 2; i++ )
	{
		for ( int j = 0; j < 2; j++ )
			{ m[i][j] = a[i] * b[j]; }
	}
	return m;
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

Vector2D mean(const std::vector<Vector2D> & x)
{
	Vector2D sum = create_Vector2D(0.0, 0.0);
	for ( std::vector<Vector2D>::const_iterator itr = x.begin(); itr != x.end(); itr++ )
		{ sum += *itr; }
	sum /= x.size();
	return sum;
}

Vector3D mean(const std::vector<Vector3D> & x)
{
	Vector3D sum = create_Vector3D(0.0, 0.0, 0.0);
	for ( std::vector<Vector3D>::const_iterator itr = x.begin(); itr != x.end(); itr++ )
		{ sum += *itr; }
	sum /= x.size();
	return sum;
}

Matrix2D cov(const std::vector<Vector2D> & x)
{
	const Vector2D xmean = mean(x);

	Matrix2D sx;
	sx.Fill(0.0);
	for ( std::vector<Vector2D>::const_iterator itr = x.begin(); itr != x.end(); itr++ )
		{ sx += outer_product(*itr - xmean, *itr - xmean); }

	sx /= (x.size() - 1);
		
	return sx;
}

Matrix2D cov(const std::vector<Vector2D> & x, const std::vector<Vector2D> & y)
{
	Matrix2D sx = cov(x);
	sx *= (x.size() - 1);

	Matrix2D sy = cov(y);
	sy *= (y.size() - 1);
	
	Matrix2D s = sx + sy;
	s /= (x.size() + y.size() - 2);
	
	return s;
}

Matrix3D cov(const std::vector<Vector3D> & x)
{
	const Vector3D xmean = mean(x);

	Matrix3D sx;
	sx.Fill(0.0);
	for ( std::vector<Vector3D>::const_iterator itr = x.begin(); itr != x.end(); itr++ )
		{ sx += outer_product(*itr - xmean, *itr - xmean); }

	sx /= (x.size() - 1);
		
	return sx;
}

Matrix3D cov(const std::vector<Vector3D> & x, const std::vector<Vector3D> & y)
{
	Matrix3D sx = cov(x);
	sx *= (x.size() - 1);

	Matrix3D sy = cov(y);
	sy *= (y.size() - 1);
	
	Matrix3D s = sx + sy;
	s /= (x.size() + y.size() - 2);
	
	return s;
}

float tsquare(const std::vector<Vector2D> & x, const std::vector<Vector2D> & y)
{
	const float nx = x.size();				// use float to avoid "int/int = int" later
	const float ny = y.size();
	const Vector2D xmean = mean(x);
	const Vector2D ymean = mean(y);
	const Matrix2D s = cov(x, y);
	const Matrix2D s_inv = static_cast<Matrix2D>(s.GetInverse());

	return ((nx * ny) / (nx + ny)) * ((xmean - ymean) * (s_inv * (xmean - ymean)));
}

float tsquare(const std::vector<Vector3D> & x, const std::vector<Vector3D> & y)
{
	const float nx = x.size();				// use float to avoid "int/int = int" later
	const float ny = y.size();
	const Vector3D xmean = mean(x);
	const Vector3D ymean = mean(y);
	const Matrix3D s = cov(x, y);
	const Matrix3D s_inv = static_cast<Matrix3D>(s.GetInverse());

	return ((nx * ny) / (nx + ny)) * ((xmean - ymean) * (s_inv * (xmean - ymean)));
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
