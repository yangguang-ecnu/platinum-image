// $Id: points_seq_func1D.hxx 

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

#include "points_seq_func1D.h"

template <class KEY_TYPE, class VALUE_TYPE>
points_seq_func1D<KEY_TYPE, VALUE_TYPE>::points_seq_func1D(KEY_TYPE key_min, VALUE_TYPE val_min, KEY_TYPE key_max, VALUE_TYPE val_max, int nr_points)
{
	float dx=float(key_max-key_min)/float(nr_points);
	float dy=float(val_max-val_min)/float(nr_points);
	for(float x=key_min; x<=key_max; x +=dx)
	{
		themap[x] = float(x - key_min)*dy/dx;
	}
}

template <class KEY_TYPE, class VALUE_TYPE>
void points_seq_func1D<KEY_TYPE, VALUE_TYPE>::insert(KEY_TYPE key, VALUE_TYPE val)
{
	themap[key] = val;
}

template <class KEY_TYPE, class VALUE_TYPE>
void points_seq_func1D<KEY_TYPE, VALUE_TYPE>::erase(KEY_TYPE key)
{
	themap.erase(key);
}

template <class KEY_TYPE, class VALUE_TYPE>
float points_seq_func1D<KEY_TYPE, VALUE_TYPE>::find_dist_to_closest_point2D(KEY_TYPE x, VALUE_TYPE y, KEY_TYPE &closest_key)
{
	float min_dist=1000000;
	float this_dist=0;
	float a=0;
	float b=0;
	map<KEY_TYPE,VALUE_TYPE>::iterator the_iterator;
	for (the_iterator = themap.begin(); the_iterator != themap.end(); ++the_iterator) 
	{
		a = float(the_iterator->first  - x);
		b = float(the_iterator->second - y);
		this_dist = sqrt(a*a+b*b);
		if(this_dist<min_dist)
		{
			min_dist=this_dist;
			closest_key = the_iterator->first;
		}
	}
	return min_dist;
}


template <class KEY_TYPE, class VALUE_TYPE>
double points_seq_func1D<KEY_TYPE, VALUE_TYPE>::get_value_interp_linear(double x)
{	
	double x1,y1,x2,y2;

	map<KEY_TYPE,VALUE_TYPE>::iterator the_iterator;
	for (the_iterator = themap.begin(); the_iterator != themap.end(); ++the_iterator) 
	{
		if(the_iterator->second > x) //means that we just found the value right of x.
		{
			x2 = the_iterator->first;
			y2 = the_iterator->second;
			--the_iterator;
			x1 = the_iterator->first;
			y1 = the_iterator->second;
			break;
		}
	}
	return y1 + (y2-y1)/(x2-x1)*(double(x)-x1); // y1 + dy/dx(=k)*x...
}


template <class KEY_TYPE, class VALUE_TYPE>
float* points_seq_func1D<KEY_TYPE, VALUE_TYPE>::get_spline_derivatives()
{
		float *x = new float[10];
	x[0] = 0;
	x[1] = 1;
	x[2] = 2;
	x[3] = 3;
	x[4] = 4;
	x[5] = 5;
	x[6] = 6;
	x[7] = 7;
	x[8] = 8;
	x[9] = 9;
	float *y = new float[10];
	y[0] = 1;
	y[1] = 2;
	y[2] = 1;
	y[3] = 3;
	y[4] = 4;
	y[5] = 1;
	y[6] = 6;
	y[7] = 7;
	y[8] = 6;
	y[9] = 4;
	float *y2 = new float[10];

	cout<<"spline..."<<endl;
	spline(x,y,10,0,0,y2);
	cout<<"y2"<<y2<<endl;
	cout<<"Spline interpolation = "<<endl;

	for(float i=0.0;i<10;i=i+0.1){
	cout<<splint(x,y,y2,10,i)<<endl;
	}

	float *y2 = new float[100];
	return y2;
}




template <class KEY_TYPE, class VALUE_TYPE>
float points_seq_func1D<KEY_TYPE, VALUE_TYPE>::get_value_interp_spline(float x, float y2[])
{


	return -1;
}




template <class KEY_TYPE, class VALUE_TYPE>
void points_seq_func1D<KEY_TYPE, VALUE_TYPE>::printdata()
{
	cout<<endl<<"printdata()"<<endl;
	map<KEY_TYPE,VALUE_TYPE>::iterator the_iterator;
	for (the_iterator = themap.begin(); the_iterator != themap.end(); ++the_iterator) 
	{
		cout<<"x:"<<the_iterator->first<<"\t"<<"y:"<<the_iterator->second<<endl;
	}
	cout<<".............."<<endl;
}
