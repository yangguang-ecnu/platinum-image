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
typedef itk::Vector<int, 3> Vector3Dint;
typedef itk::Point<float,3> Point3D;
typedef std::pair<long, long> HistoPair;
typedef itk::Matrix<float> Matrix3D;

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
void min_normalize (itk::Vector<sType, vDim> &V)     //normalize vector using
                                                     //minimum norm
{
	sType min_norm = V[0];
    
	for (unsigned int d=0;d < vDim;d++)
        {
		min_norm=min(min_norm,V[d]);
        }
	V /=min_norm;
}

template <class sType, unsigned int vDim>
float max_norm (itk::Vector<sType, vDim> &V)     //normalize vector using
                                                     //minimum norm
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
//Following rules can be notes about the rotation matrices below:
// R^(-1)(fi) = R(-fi)
// R^(-1)(fi) = R^T(fi)
// R-total = RzRyRx --< R^(-1) = R^(T)
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

unsigned int get_factorial(unsigned int i);
unsigned int get_permutations(unsigned int n, unsigned int r); //returns pascals triangle values

#endif	//__ptmath.h__