//////////////////////////////////////////////////////////////////////////
//
//  points_seq_func1D $Revision: 131 $
///
/// point collection class specialized on ordered 2D points
///
/// The points_seq_func1D class is templated over key (x-values) and value (y-values) types.
/// It uses the STL map-class to keep a list of sorted (x,y) pairs.
/// inserts, erasing, find_closest_point, interpolation (lin/spline) functions are available
/// this class is intended to be a sub class to data_base....
//
//  $LastChangedBy: arvid.r $
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

#ifndef __points_seq_func1D__
#define __points_seq_func1D__

#include "point_base.h"



template <class KEY_TYPE, class VALUE_TYPE>
class points_seq_func1D : public point_base{
private:
	map<KEY_TYPE,VALUE_TYPE> themap;

protected:

public:
	points_seq_func1D(){};
	points_seq_func1D(KEY_TYPE key_min, VALUE_TYPE val_min, KEY_TYPE key_max, VALUE_TYPE val_max, int nr_points);

	void insert(KEY_TYPE key, VALUE_TYPE val);
	void erase(KEY_TYPE key);

	float find_dist_to_closest_point2D(KEY_TYPE x, VALUE_TYPE y, KEY_TYPE &closest_key);
	double get_value_interp_linear(double x);	//for now, the x value is assumed to be in the domain


	float* get_spline_derivatives();
	float get_value_interp_spline(float x, float y2[]);

	void printdata();
};

#include "points_seq_func1D.hxx"

#endif
