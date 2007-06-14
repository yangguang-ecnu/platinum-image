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

#ifndef __tensor_algebra__
#define __tensor_algebra__

//tensor algebra type defs, dependencies...
#include "itkVector.h"
#include "itkPoint.h"
#include "itkMatrix.h"

///...and types
typedef itk::Vector<float,3> Vector3D;
//typedef itk::Point<float,3> Point3D;
typedef struct Point3D
    {float x, y, z;};
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

//Following rules can be notes about the rotation matrices below:
// R^(-1)(fi) = R(-fi)
// R^(-1)(fi) = R^T(fi)
// R-total = RzRyRx --< R^(-1) = R^(T)
	class matrix_generator{
	public:
		Matrix3D get_rot_x_matrix_3D(float fi)			//fi in radians
		{
			Matrix3D r;
			r[0][0] = 1;	r[1][0] = 0; 		r[2][0] = 0;
			r[0][1] = 0;	r[1][1] = cos(fi); 	r[2][1] = -sin(fi);
			r[0][2] = 0;	r[1][2] = sin(fi); 	r[2][2] = cos(fi);
			return r;
		}

		Matrix3D get_rot_y_matrix_3D(float fi)			//fi in radians
		{
			Matrix3D r;
			r[0][0] = cos(fi);	r[1][0] = 0; 		r[2][0] = sin(fi);
			r[0][1] = 0;		r[1][1] = 1;		r[2][1] = 0;
			r[0][2] = -sin(fi);	r[1][2] = 0;		r[2][2] = cos(fi);
			return r;
		}

		Matrix3D get_rot_z_matrix_3D(float fi)			//fi in radians
		{
			Matrix3D r;
			r[0][0] = cos(fi);	r[1][0] = -sin(fi);	r[2][0] = 0;
			r[0][1] = sin(fi);	r[1][1] = cos(fi);	r[2][1] = 0;
			r[0][2] = 0;		r[1][2] = 0;		r[2][2] = 1;
			return r;
		}

		Matrix3D get_rot_matrix_3D(float fi_z, float fi_y, float fi_x)	//fi_z/y/x in radians
		{
			return get_rot_z_matrix_3D(fi_z)*get_rot_y_matrix_3D(fi_y)*get_rot_x_matrix_3D(fi_x);
		}


	};

#endif
