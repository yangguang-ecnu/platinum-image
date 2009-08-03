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

#ifndef __ptvector__
#define __ptvector__

#include <vector>
#include <math.h>
using namespace std;

/* Base starts here */
template<class ELEMTYPE>
class pt_vector : public vector<ELEMTYPE>{
protected:
	
public:
	pt_vector(int);
	~pt_vector(void);
	void config_x_axis(double resolution, double start);
	ELEMTYPE get_maximum_in_range(int from, int to);
	ELEMTYPE get_minimum_in_range(int from, int to);
	void mean_value_smoothing(int filter_size);
	double get_mean_in_range(int from, int to);
	double get_slope_at_location(int location);
	double distance_between_points(int x1, int x2);
	double get_mean_in_range_for_vector(int from, int to, vector<ELEMTYPE> vec);
	double area_between_points(int x1, int x2);

	void normalize_to_maximum();

	double x_res, x_axis_start;
	


private:
	
	
};

template<class ELEMTYPE>
pt_vector<ELEMTYPE>::pt_vector(int start_size) : vector<ELEMTYPE>(start_size){
	x_res = 1.0;
	x_axis_start = 0;
}

/* Sets the scaling of the x axis */
template<class ELEMTYPE>
void pt_vector<ELEMTYPE>::config_x_axis(double resolution, double start){
	
	x_res = resolution;
	x_axis_start = start;
}

/* Looks in a specified range and returns the maximum value found */
template<class ELEMTYPE>
ELEMTYPE pt_vector<ELEMTYPE>::get_maximum_in_range(int from, int to){
	
	ELEMTYPE maximum = numeric_limits<ELEMTYPE>::min();
	if(size() < from)
		return NULL;
	maximum = at(from);
	for(int i = from; i<=to && i<size(); i++)
		if(at(i) > maximum)
			maximum = at(i);
	return maximum;
}

/* Looks in a specified range and returns the minimum value found */
template<class ELEMTYPE>
ELEMTYPE pt_vector<ELEMTYPE>::get_minimum_in_range(int from, int to){
	
	ELEMTYPE minimum = numeric_limits<ELEMTYPE>::max();
	if(size() < from)
		return NULL;
	minimum = at(from);
	for(int i = from; i<=to && i<size(); i++)
		if(at(i) < minimum)
			minimum = at(i);

	return minimum;
}

/* Returns the slope of the curve at a specified location */
template<class ELEMTYPE>
double pt_vector<ELEMTYPE>::get_slope_at_location(int location){
	
	int start;

	if(size() < 2)
		return 0;

	start = location -1;
	if(start < 0){
		start++;
		location++;
	}
	
	return at(location) - at(start);
	//Safest to return double because of unknown datatype in calculation
}

/* Returns the slope of the curve at a specified location */
template<class ELEMTYPE>
void pt_vector<ELEMTYPE>::mean_value_smoothing(int filter_size){
	
	vector<ELEMTYPE> queue;
	vector<ELEMTYPE>::iterator first_element;
	int pos;
	pos = 0;
	if(size() < filter_size){
		return;
	}
	int i;
	for(i = 0; i< filter_size; i++){
		queue.push_back(at(0));
	}
	for(i = 0; i<= filter_size; i++){
		queue.push_back(at(i));
	}
	first_element = queue.begin();

	while(pos < size()){
		at(pos) = get_mean_in_range_for_vector(0, queue.size(), queue);
		
		queue.erase(first_element);
		first_element = queue.begin();

		pos++;
		if(pos+filter_size < size()){
			queue.push_back(at(pos+filter_size));
		}else{
			queue.push_back(at(size()-1));
		}
		
	}

}


template<class ELEMTYPE>
double pt_vector<ELEMTYPE>::get_mean_in_range(int from, int to){
	
	double mean;

	mean = 0;
	for(int i = from; i < to; i++)
		mean +=(at(i)/(to-from+1));

	return mean;
}
template<class ELEMTYPE>
double pt_vector<ELEMTYPE>::get_mean_in_range_for_vector(int from, int to, vector<ELEMTYPE> vec){
	
	double mean;

	mean = 0;
	for(int i = from; i < to; i++)
		mean +=(vec.at(i)/(to-from+1));

	return mean;
}

template<class ELEMTYPE>
double pt_vector<ELEMTYPE>::distance_between_points(int x1, int x2){
	
	return abs(x1-x2)*x_res;
}

template<class ELEMTYPE>
double pt_vector<ELEMTYPE>::area_between_points(int x1, int x2){
	double area_approx;
	area_approx = 0;
	for(int i = x1; i < x2; i++){
		area_approx += abs(at(i))*(x_res);//h*b
	}
	return area_approx; //same unit as x_res is given in
}

/* This does not work yet... */
template<class ELEMTYPE>
void pt_vector<ELEMTYPE>::normalize_to_maximum(){
	ELEMTYPE min = get_minimum_in_range(0,size()-1);
	ELEMTYPE max = get_maximum_in_range(0,size()-1);

	ELEMTYPE new_max = numeric_limits<ELEMTYPE>::max();
	ELEMTYPE new_min = numeric_limits<ELEMTYPE>::min();

	long double dy = max-min;
	ELEMTYPE dy_new = new_max-new_min;
	ELEMTYPE shift = new_min - min;
	cout << "innan: " << at(23);
	for(int i = 0; i < size(); i++){
		at(i) = (ELEMTYPE) floor( (at(i)+shift)*(dy_new/dy) );
	}
	cout << "   efter" << at(23) << endl;
}

/* Base ends here */

/* Scalar starts here */
template<class ELEMTYPE>
class pts_vector : public pt_vector<ELEMTYPE>{
public: 
	pts_vector(int);
	~pts_vector(void);

};

template<class ELEMTYPE>
pts_vector<ELEMTYPE>::pts_vector(int start_size) : pt_vector<ELEMTYPE>(start_size){
	
}
/* Scalar ends here */
/* Complex starts here */
template<class ELEMTYPE>
class ptc_vector : public pt_vector<ELEMTYPE>{
public:
	ptc_vector(int);
	~ptc_vector(void);

};



template<class ELEMTYPE>
ptc_vector<ELEMTYPE>::ptc_vector(int start_size) : pt_vector<ELEMTYPE>(start_size){
	
}
/* Complex ends here */

#endif	//__pt_vector.h__