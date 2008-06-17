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

#ifndef __voxel_tools_hxx__
#define __voxel_tools_hxx__

#include "voxel_tools.h"

template <class ELEMTYPE>
voxel<ELEMTYPE>::voxel(){
	position = create_Vector3D(0,0,0);
	value = 0;
}

template <class ELEMTYPE>
voxel<ELEMTYPE>::voxel(Vector3Dint pos, ELEMTYPE val){
//	cout<<"voxel(Vector3Dint pos, ELEMTYPE val)"<<endl;
	position = pos;
	value = val;
}

template <class ELEMTYPE>
voxel<ELEMTYPE>::voxel(voxel<ELEMTYPE> &v)
{
//	cout<<"voxel(voxel<ELEMTYPE> &v)"<<endl;
	position = v.position;
	value = v.value;
}

template <class ELEMTYPE>
void voxel<ELEMTYPE>::print()
{
	cout<<"("<<this->position<<")"<<this->value<<endl;
}




//---------------- A function for comparing voxels is needed for the "voxel_set" class ---------------------------------------------

template <class ELEMTYPE>
bool voxel_compare_func(const voxel<ELEMTYPE>* v1, const voxel<ELEMTYPE>* v2)
	{
		//sort on value first, then on z,y,x coordinates...
		if(v1->value < v2->value){
//			cout<<"less..."<<endl;
			return true;
		}else if(v1->value == v2->value){
			if(v1->position[2] < v2->position[2]){
				return true;
			}else if(v1->position[2] == v2->position[2]){
				if(v1->position[1] < v2->position[1]){
					return true;
				}else if(v1->position[1] == v2->position[1]){
					return (v1->position[0] < v2->position[0]);
				}
			}
		}
		return false;
	}


template <class ELEMTYPE>
bool voxel_comparator<ELEMTYPE>::operator () (const voxel<ELEMTYPE>* v1, const voxel<ELEMTYPE>* v2) const
{
	return voxel_compare_func<ELEMTYPE>(v1,v2);
}


/*
template <class ELEMTYPE>
class voxel_comparator
{
public:
	bool operator()(const voxel<ELEMTYPE>* v1, const voxel<ELEMTYPE>* v2) const
	{
		return voxel_compare_func<ELEMTYPE>(v1,v2);
	}
};
*/




//---------------- voxel_set -  A class for handling sorted sets of voxels ---------------------------------------


template <class ELEMTYPE>
voxel_set<ELEMTYPE>::voxel_set() : set< voxel<ELEMTYPE>*, voxel_comparator<ELEMTYPE> >()
{
//	accumulated_mean=0;
}

template<class ELEMTYPE>
void voxel_set<ELEMTYPE>::insert_voxel(voxel<ELEMTYPE> *v, bool update_accumulated_mean)
{
//	cout<<"voxel_set<ELEMTYPE>::insert_voxel()"<<endl;
	this->insert(v);

/*	if( this->size()==0 && update_accumulated_mean){
		accumulated_mean = v->value;
		cout<<"acc_mean ="<<update_accumulated_mean<<endl;
	}else if( update_accumulated_mean){
		double n = this->size();
		accumulated_mean =  ((n-1.0)*accumulated_mean + double(v->value))/(n);
		cout<<"acc_mean ="<<accumulated_mean<<endl;
	}
*/
}

template<class ELEMTYPE>
void voxel_set<ELEMTYPE>::insert_these(voxel_set<ELEMTYPE> &vs)
{
	for(int i=0;i<vs.size();i++){
		this->insert_voxel(vs.erase_lowest());
	}
}


template<class ELEMTYPE>
void voxel_set<ELEMTYPE>::erase_these(voxel_set<ELEMTYPE> &vs)
{
	cout<<"voxel_set<ELEMTYPE>::erase_these(voxel_set<ELEMTYPE> &vs)"<<endl;

	set<voxel<ELEMTYPE>*,voxel_comparator<ELEMTYPE> >::iterator res_it_end;
	voxel_set<ELEMTYPE> res;
	int max_num = std::max(this->size(),vs.size());
	for(int i=0;i<max_num;i++){
		res.insert( new voxel<unsigned short>(create_Vector3Dint(-1,-1,-1),-i) );
	}

//	res_it_end = set_intersection(this->begin(), this->end(), vs.begin(), vs.end(), res.begin(), voxel_compare_func<ELEMTYPE>);
	res_it_end = set_difference(this->begin(), this->end(), vs.begin(), vs.end(), res.begin(), voxel_compare_func<ELEMTYPE>);

	this->clear();
//	voxel<ELEMTYPE>* v;
	set<voxel<ELEMTYPE>*,voxel_comparator<ELEMTYPE> >::iterator it = res.begin();
	while(it != res_it_end){
//		v = *it;
//		v->print();
		this->insert(*it);
		it++;
	}
	res.clear();
}

template<class ELEMTYPE>
void voxel_set<ELEMTYPE>::and(voxel_set<ELEMTYPE> &vs)
{
	cout<<"voxel_set<ELEMTYPE>::and(voxel_set<ELEMTYPE> &vs)"<<endl;

	set<voxel<ELEMTYPE>*,voxel_comparator<ELEMTYPE> >::iterator res_it_end;
	voxel_set<ELEMTYPE> res;
	int max_num = std::min(this->size(),vs.size());
	for(int i=0;i<max_num;i++){
		res.insert( new voxel<unsigned short>(create_Vector3Dint(-1,-1,-1),-i) );
	}

	res_it_end = set_intersection(this->begin(), this->end(), vs.begin(), vs.end(), res.begin(), voxel_compare_func<ELEMTYPE>);
//	res_it_end = set_difference(this->begin(), this->end(), vs.begin(), vs.end(), res.begin(), voxel_compare_func<ELEMTYPE>);

	this->clear();
	set<voxel<ELEMTYPE>*,voxel_comparator<ELEMTYPE> >::iterator it = res.begin();
	while(it != res_it_end){
		this->insert(*it);
		it++;
	}
	res.clear();
}

template<class ELEMTYPE>
void voxel_set<ELEMTYPE>::move_lowest_to(voxel_set<ELEMTYPE> &vs)
{
	if(this->size()>0){
		vs.insert(this->erase_lowest());
	}
}

template<class ELEMTYPE>
void voxel_set<ELEMTYPE>::move_highest_to(voxel_set<ELEMTYPE> &vs)
{
	if(this->size()>0){
		vs.insert(this->erase_highest());
	}

//	accumulated_mean =  this->get_calculated_mean();
}



template<class ELEMTYPE>
voxel<ELEMTYPE>* voxel_set<ELEMTYPE>::erase_lowest()
{
//	cout<<"voxel_set<ELEMTYPE>::erase_lowest()"<<endl;
	voxel<ELEMTYPE>* tmp = NULL;

	if(this->size()>0){
		tmp = *this->begin();
		this->erase(this->begin());
	}

//	accumulated_mean =  this->get_calculated_mean();
	return tmp;
}

template<class ELEMTYPE>
voxel<ELEMTYPE>* voxel_set<ELEMTYPE>::erase_highest()
{
//	cout<<"voxel_set<ELEMTYPE>::erase_highest()"<<endl;
	voxel<ELEMTYPE>* tmp = NULL;

	if(this->size()>0){
		set<voxel<ELEMTYPE>*,voxel_comparator<ELEMTYPE> >::iterator it = this->end();
		it--;
		tmp = *it;
		this->erase(it);
	}

//	accumulated_mean =  this->get_calculated_mean();
	return tmp;
}

template<class ELEMTYPE>
void voxel_set<ELEMTYPE>::erase_all()
{
	cout<<"voxel_set<ELEMTYPE>::erase_all()"<<endl;
	while(this->size()>0){
		this->erase_highest();
	}
}

template<class ELEMTYPE>
double voxel_set<ELEMTYPE>::get_calculated_mean()
{
//	cout<<"voxel_set<ELEMTYPE>::get_calculated_mean()"<<endl;
	double sum=0;

	set<voxel<ELEMTYPE>*,voxel_comparator<ELEMTYPE> >::iterator it;
	voxel<ELEMTYPE>* tmp;
	for(it = this->begin(); it!=this->end(); it++ ) {
		tmp = *it;
		sum += tmp->value;
	}  
	return sum/double(this->size());
}

/*
template<class ELEMTYPE>
double voxel_set<ELEMTYPE>::get_accumulated_mean()
{
	return accumulated_mean;
}
*/

template<class ELEMTYPE>
voxel<ELEMTYPE>* voxel_set<ELEMTYPE>::get_median_voxel()
{
	if(this->size()>0){
		voxel<ELEMTYPE>* median;
		set<voxel<ELEMTYPE>*,voxel_comparator<ELEMTYPE> >::iterator it = this->begin();

//		it++(this->size()/2);
		for(int i=0;i<this->size()/2;i++){
			it++;
		}

		median = *it;
		return median;
	}
	return NULL;
}


template<class ELEMTYPE>
void voxel_set<ELEMTYPE>::set_values_to_voxels_in_this_set_using_image_data(image_general<ELEMTYPE,3> *im)
{
	set<voxel<ELEMTYPE>*,voxel_comparator<ELEMTYPE> >::iterator it;
	voxel<ELEMTYPE>* tmp;
	for(it = this->begin(); it!=this->end(); it++ ) {
		tmp = *it;
		tmp->value = im->get_voxel(tmp->position[0],tmp->position[1],tmp->position[2]);
	}  
}



template<class ELEMTYPE>
void voxel_set<ELEMTYPE>::print_all()
{
	cout<<"voxel_set<ELEMTYPE>::print()  (n="<<this->size()<<")"<<endl;
	set<voxel<ELEMTYPE>*,voxel_comparator<ELEMTYPE> >::iterator it;
	voxel<ELEMTYPE>* tmp;
	for(it = this->begin(); it!=this->end(); it++ ) {
		tmp = *it;
		cout<<"("<<tmp->position<<") "<<float(tmp->value)<<endl; //cast to float to not print "unsigned char" as "characters"
	}  
}



//#include "voxel_tools_process.hxx"

#endif
