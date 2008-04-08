//////////////////////////////////////////////////////////////////////////
//
//  header collecting mathematical functions used
//  throughout the platform
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

#ifndef __ptmath__
#define __ptmath__

#define PI 3.1415926536

#include <limits>	//for example...  numeric_limits<float>

//tensor algebra type defs, dependencies...
#include "itkVector.h"
#include "itkPoint.h"
#include "itkMatrix.h"

#include "fileutils.h"

///...and types
typedef itk::Vector<float,3> Vector3D;
typedef itk::Vector<int,3> Vector3Dint;
typedef itk::Point<float,3> Point3D;
typedef std::pair<long, long> HistoPair;
typedef itk::Matrix<float> Matrix3D;

typedef itk::Vector<float,2> Vector2D;
typedef itk::Matrix<float,2,2> Matrix2D;


class line2D{

public:
	line2D();
	line2D(Vector2D pnt1, Vector2D pnt2);
	line2D(float x1, float y1, float x2, float y2);

	Vector2D get_point();
	Vector2D get_direction();
	void set_point(Vector2D pnt);
	void set_point(float point_x, float point_y);
	void set_direction(Vector2D dir);
	void set_direction(float dir_x, float dir_y);

	bool is_point_left_of_line(Vector2D pnt);
	bool is_point_right_of_line(Vector2D pnt);
	bool is_point_left_of_line(float x, float y);
	bool is_point_right_of_line(float x, float y);

protected:
	Vector2D point;
	Vector2D direction;
};

class line3D{ //TODO: A future plan in to put this under data_base and connect useful rendering to it...

public:
	line3D();
	line3D(Vector3D pnt, Vector3D dir);
	line3D(float point_x, float point_y, float point_z, float dir_x, float dir_y, float dir_z);

	Vector3D get_point();
	Vector3D get_direction();
	void set_point(Vector3D pnt);
	void set_point(float point_x, float point_y, float point_z);
	void set_direction(Vector3D dir);
	void set_direction(float dir_x, float dir_y, float dir_z);

	void least_square_fit_line_to_points_in_3D(vector<Vector3D> points, int dir);

protected:
	Vector3D point;
	Vector3D direction;
	void set_point_to_center_of_gravity_from_points_in_3D(vector<Vector3D> points);
	void set_direction_to_point_cloud_variations_given_one_dir(int dir, vector<Vector3D> points); //note that point needs to be set to center of gravity...
};

class plane3D{
public:
	plane3D();
	plane3D(Vector3D pnt, Vector3D norm);
	plane3D(float point_x, float point_y, float point_z, float normal_x, float normal_y, float normal_z);
	plane3D(Vector3D point1, Vector3D point2, Vector3D point3);
	
	Vector3D get_point();
	Vector3D get_normal();
	bool is_defined();
	void set_point(Vector3D pnt);
	void set_point(float point_x, float point_y, float point_z);
	void set_normal(Vector3D norm);
	void set_normal(float normal_x, float normal_y, float normal_z);

	void invert();
	Vector3D get_point_of_intersection(line3D line);
	line3D get_line_of_intersection(plane3D plane);
	line2D get_projected_line(line3D line);

protected:
	Vector3D point;
	Vector3D normal;
	bool defined; //Undefined planes can be used as default values for functions taking a varying number of planes as arguemt
};

bool convex_last_three_points(vector<Vector2D>::iterator end, bool lower); // used by get_convex_hull_2D
vector<Vector2D> get_convex_hull_2D(vector<Vector2D> points); // points should be sorted, first by x, then by y (first and second values in Vector2D)

//not typical tensor functions, just for lack of a better place

template<class T>                   
void t_swap(T & t1, T & t2){        //templated swap functions that's less
                                    //(not at all) picky on input
	T tmp(t1); 
	t1 = t2; 
	t2  = tmp; 
}

template <class T>
T round (T f)   //round to nearest integer
{return floor (f + 0.5); }

template <class T>
T abs_ceil (T f)   //round to nearest higher, ignoring sign
{
	T sign = (f < 0 ? -1 : 1);
	return sign*ceil (f*sign);
}

template <class sType, unsigned int vDim>
float min_norm (itk::Vector<sType, vDim> &V)     //normalize vector using
                                                     //minimum norm
{
	sType m = V[0];
    
	for (unsigned int d=0;d < vDim;d++)
        {
		min_norm=min(min_norm,V[d]);
        }
    return m;
}

template <class sType, unsigned int vDim>
void min_normalize (itk::Vector<sType, vDim> &V)     //normalize vector using minimum norm
{
	sType min_norm = V[0];
    
	for (unsigned int d=0;d < vDim;d++)
        {
		min_norm=min(min_norm,V[d]);
        }
	V /=min_norm;
}

template <class sType, unsigned int vDim>
float max_norm (itk::Vector<sType, vDim> &V)
{
	sType m = V[0];
    
	for (unsigned int d=0;d < vDim;d++)
        {
		m=max(m,V[d]);
        }
    
    return m;
}


template <class C, class D> void adjust_endian (D* data, C size,bool DataIsBigEndian)
//alter endian if supplied value
//and system value don't match
{
	bool RunsOnBigEndian;
    
#ifdef _MSC_VER
	short word = 0x4321;
	RunsOnBigEndian = (*(char *)& word) != 0x21 ;
#else
	RunsOnBigEndian = (htonl(1)==1 );
#endif
    
	unsigned int swapSize = sizeof (D);
    
	if (RunsOnBigEndian ^ DataIsBigEndian)
        {
		for (C p =0;p < size;p++)
            {
			unsigned char * b = (unsigned char *) &(data[p]);
			register int i = 0;
			register int j = swapSize-1;
            
			while (i<j)
                {
				t_swap(b[i], b[j]);
				i++, j--;
                }
            }
        }
}


//templated string to num conversion
template <class T>
bool dec_from_string(T& outType, const std::string& inString)
    {
    std::istringstream convstream(inString);
    return !(convstream >> std::dec >> outType).fail();
    }


/*
 bool has_only_positive_or_zero_components(Vector3D v)
 {
     for(int i=0;i<=2;i++)
         {
         if(v[i]<0)
             return false;
         }
     return true;
 }
 
 bool has_only_negative_or_zero_components(Vector3D v)
 {
     for(int i=0;i<=2;i++)
         {
         if(v[i]>0)
             return false;
         }
     return true;
 }
 */

/*
class matrix_generator{
public:
	Matrix3D get_rot_x_matrix_3D(float fi);			//fi in radians
	Matrix3D get_rot_y_matrix_3D(float fi);			//fi in radians
	Matrix3D get_rot_z_matrix_3D(float fi);			//fi in radians
	
	//rotation examples based on the "basic" image processing coordinate system
	//(x-->right, y-->down and z--> the view direction of the screen)
	//+fi_z rotates the image volume: "Counterclockwise" of the z-direction
	//+fi_y rotates the image volume: "Counterclockwise" of the y-direction
	//+fi_x rotates the image volume: "Counterclockwise" of the x-direction
	Matrix3D get_rot_matrix_3D(float fi_z, float fi_y, float fi_x);	//fi_z/y/x in radians
	Matrix3D get_rot_matrix_3D(int fi_z, int fi_y, int fi_x);	//fi_z/y/x in degrees
};
*/

//Following rules can be noted about the rotation matrices below:
// R^(-1)(fi) = R(-fi)
// R^(-1)(fi) = R^T(fi)
// R-total = RzRyRx --> R^(-1) = R^(T)

//rotation examples based on the "basic" image processing coordinate system
//(x-->right, y-->down and z--> the view direction of the screen)
//+fi_z rotates the image volume: "Counterclockwise" of the z-direction
//+fi_y rotates the image volume: "Counterclockwise" of the y-direction
//+fi_x rotates the image volume: "Counterclockwise" of the x-direction

Matrix3D create_rot_x_matrix_3D(float fi_rad);									//fi in radians
Matrix3D create_rot_y_matrix_3D(float fi_rad);									//fi in radians
Matrix3D create_rot_z_matrix_3D(float fi_rad);									//fi in radians
Matrix3D create_rot_matrix_3D(float fi_x_rad, float fi_y_rad, float fi_z_rad);	//fi_x/y/z in radians




///////////////////////////////////////////////////////
// Given x and y vectors of length nSteps, returns 
// a vector of second derivatives, y2 (natural cubic spline).  Based 
// on the routine "spline" from Numerical Recipes section 3.3.
// First, call pt_spline once to generate y2.
// Then call pt_splint (using y2) to  interpolate values...

void pt_spline1D(float x[],float y[],int n,float yp1,float ypn,float y2[]);
float pt_splint1D(float xa[],float ya[],float y2a[],int n,float x);




// Tricubic interpolation using method described in:
// F. Lekien, J.E. Marsden
// Tricubic Interpolation in Three Dimensions
// International Journal for Numerical Methods in Engineering, 63 (3), 455-471, 2005
// ...used in the function: image_scalar.interpolate_tricubic_3D...

extern const int A_tricubic[64][64]; //defined in ptmath.cc

template <class T>
void print_type_limits()
{
	std::cout<<std::numeric_limits<T>::min()<<"\t";
	std::cout<<std::numeric_limits<T>::max()<<"\t";
	std::cout<<std::numeric_limits<T>::digits<<std::endl;
}

void print_datatype_numerical_limits();
double get_random_number_in_span(double min, double max);
Vector3D create_Vector3D(float x, float y, float z);
Vector3Dint create_Vector3Dint(int x, int y, int z);
Vector2D create_Vector2D(float x, float y);


Matrix3D outer_product(const Vector3D a, const Vector3D b);

unsigned int get_factorial(unsigned int i);
unsigned int get_permutations(unsigned int n, unsigned int r); //returns pascals triangle values
unsigned int get_smallest_power_above(unsigned int this_val, unsigned int power_base=2); //e.g. this_val=10 (base=2) --> 16

Vector2D mean(const std::vector<Vector2D> & x);
Vector3D mean(const std::vector<Vector3D> & x);
Matrix2D cov(const std::vector<Vector2D> & x);
Matrix2D cov(const std::vector<Vector2D> & x, const std::vector<Vector2D> & y);
Matrix3D cov(const std::vector<Vector3D> & x);
Matrix3D cov(const std::vector<Vector3D> & x, const std::vector<Vector3D> & y);
float tsquare(const std::vector<Vector2D> & x, const std::vector<Vector2D> & y);		// Hotelling's two-sample t-square statistic
float tsquare(const std::vector<Vector3D> & x, const std::vector<Vector3D> & y);		// Hotelling's two-sample t-square statistic

// Some parts of the implementation of some the following algorithms were inspired by
// "Numerical Recipes in C", Second Edition, Press, Teukolsky, Vetterling, Flannery
double invF(const double p, const double a, const double b);				// Inverse cumulative F distribution function
double invIncompleteBeta(const double p, const double a, const double b);	// Inverse incomplete beta function
double logGamma(const double x);											// Logarithm of the Gamma function
double incompleteBeta(const double a, const double b, const double x);		// Incomplete beta function
double incompleteBetaCF(const double a, const double b, const double x);	// Continued fraction for incomplete beta function



#endif	//__ptmath.h__
