//////////////////////////////////////////////////////////////////////////
//
//  filters.h $Revision $
//
//
//  $LastChangedBy $
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


#ifndef __filters__
#define __filters__

#include <vnl/vnl_matrix.h>


class filter_base
{
public: 
	filter_base(){}
	vnl_matrix<float> data;			//coordinate offsets and weights... for example x,y,z, w (-1,-1, 0, w)
	virtual float apply(float *neighbourhood){return 0;}
};

class filter_median : public filter_base
{
public:
	filter_median(int nx, int ny, int nz=1, int xcenter=-1001, int ycenter=-1001, int zcenter=-1001);
	float apply(float* neighbourhood);
};

class filter_linear : public filter_base
{
public:
	filter_linear(){}
	void set_data_from_floats(float* w, int nx, int ny, int nz, int xcenter, int ycenter, int zcenter);
	void set_data_from_ints(int *w, int nx, int ny, int nz, int xcenter, int ycenter, int zcenter);
	float apply(float *neighbourhood);
	void print();
};


class filter_sobel_2d : public filter_linear
{
public:
	filter_sobel_2d(int orientation=0, int dir=2);
};

class filter_laplace_2d : public filter_linear
{
public:
	filter_laplace_2d(int dir=2, bool eight_connected=true); //four-connected if not eight-connected
};

class filter_laplace_1d : public filter_linear
{
public:
	filter_laplace_1d(int dir=2);
};

class filter_central_difference: public filter_linear
{
public:
	filter_central_difference(int dir=2);
};

class filter_gaussian : public filter_linear
{
public:
	filter_gaussian(int n, int dir, float std_dev=-1, int center=-1001);
};

class filter_mean : public filter_linear
{
public:
	filter_mean(int nx, int ny=1, int nz=1, int xcenter=-1001, int ycenter=-1001, int zcenter=-1001);
};

#endif