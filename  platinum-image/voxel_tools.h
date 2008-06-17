//////////////////////////////////////////////////////////////////////////
//
//   voxel_tools $Revision:$
///
///  Handling of datastructures as "voxel_class" sorted sets of voxels...
///
//   $LastChangedBy: joel.kullberg $
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

#ifndef __voxel_tools__
#define __voxel_tools__

#include <set>
//#include <stack>
//#include <queue>
//#include "image_general.h"

using namespace std;


// --------------------------- "Voxel class" för various algorithms... --------------------
template <class ELEMTYPE>
class voxel{
public:
	voxel();
	voxel(Vector3Dint pos, ELEMTYPE val);
	voxel(voxel<ELEMTYPE> &v);
	~voxel(){cout<<"~voxel()..."<<endl;}

	Vector3Dint position;
	ELEMTYPE value;

	void print();

	bool operator==(const voxel<ELEMTYPE> &v)
		{ return (position == v.position && value == v.value); }
	bool operator!=(const voxel<ELEMTYPE> &v)
		{ return (position != v.position && value != v.value); }
	bool operator<(const voxel<ELEMTYPE> &v)
		{ return value<v.value; }
	bool operator>(const voxel<ELEMTYPE> &v)
		{ return value>v.value; }

	friend std::ostream &operator<<(std::ostream &ut, const voxel<ELEMTYPE> &V)
		{
        ut<<v.position<<" "<<v.value;
        return ut;
        }
};


//---------------- A function for comparing voxels is needed for the "voxel_set" class -------------------------------------

template <class ELEMTYPE>
bool voxel_compare_func(const voxel<ELEMTYPE>* v1, const voxel<ELEMTYPE>* v2);

template <class ELEMTYPE>
class voxel_comparator
{
public:
	bool operator()(const voxel<ELEMTYPE>* v1, const voxel<ELEMTYPE>* v2) const;
};



//---------------- voxel_set -  A class for handling sorted sets of voxels ---------------------------------------




template<class ELEMTYPE>
class voxel_set : public set<voxel<ELEMTYPE>*, voxel_comparator<ELEMTYPE> >
{
	private:
//		double accumulated_mean; //TODO... implement consistent mean buffering...

    public:
        voxel_set();
		void insert_voxel(voxel<ELEMTYPE> *v, bool update_accumulated_mean=true);
		void insert_these(voxel_set<ELEMTYPE> &vs);
//		void erase(voxel<ELEMTYPE> v);
		void erase_these(voxel_set<ELEMTYPE> &vs);
		void and(voxel_set<ELEMTYPE> &vs);
		void move_lowest_to(voxel_set<ELEMTYPE> &vs);
		void move_highest_to(voxel_set<ELEMTYPE> &vs);
		voxel<ELEMTYPE>* erase_lowest(); //first
		voxel<ELEMTYPE>* erase_highest();//i.e. last
		void erase_all(); //removes pointers from vector without calling theri constructors...
		double get_calculated_mean();
//		double get_accumulated_mean();
		voxel<ELEMTYPE>* get_median_voxel();

		void set_values_to_voxels_in_this_set_using_image_data(image_general<ELEMTYPE,3> *im);

		void print_all();
		//--------------
/*
float get_mean(vector<voxel<unsigned short>*> &v){
	float sum=0;
	for(int i=0;i<v.size();i++){
		sum += v[i]->value;
	}
	return sum/float(v.size());
}
*/

};

//-----------------------------------------------------------------
//-----------------------------------------------------------------

#endif
