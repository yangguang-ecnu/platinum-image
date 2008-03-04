
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

//#include "point_base.h"
#include "datawidget.h"

point_collection::point_collection() : data_base()
{
    //start empty
    widget=new datawidget<point_collection>(this,"Untitled point");
	active = -1;
}

point_collection::~point_collection() {}

point_collection::pointStorage::iterator point_collection::begin()
{
	return pointStorage::iterator(thePoints.begin());
}

point_collection::pointStorage::iterator point_collection::end()
{
	return pointStorage::iterator(thePoints.end());
}

void point_collection::add(pointStorage::mapped_type point)
{
	pointStorage::key_type index = 1;
	
	if ( !thePoints.empty() )
	{
		pointStorage::reverse_iterator riter;
		riter = thePoints.rbegin();
		index = riter->first + 1;
	}
	
	add_pair (index, point);
}

void point_collection::remove(pointStorage::key_type index)
{
	if ( thePoints.size() > 0 && thePoints.count(index) != 0)
	{	// the point exists
		pointStorage::iterator itr =  thePoints.find(index);
		thePoints.erase(itr);
	}
	else
	{
		pt_error::error("point_collection::remove...",pt_error::warning);
		throw out_of_range("Unvalid key");
	}
}

void point_collection::add_pair(pointStorage::key_type index, pointStorage::mapped_type point)
{
	thePoints[index] = point;
}

point_collection::pointStorage::mapped_type point_collection::get_point (int i)
{
	if ( thePoints.size() > 0 && thePoints.count(i) != 0)
	{	// the point exists
		return thePoints[i];
	}
	else
	{
		pt_error::error("point_collection::get_point...",pt_error::warning);
		throw out_of_range("Unvalid key");
	}
}

void point_collection::set_active(int a)
{
	active = a;
}

int point_collection::get_active()
{
	return active;
}

void point_collection::clear()
{
	thePoints.clear();
}

bool point_collection::contains( pointStorage::key_type index )
{
	if ( thePoints.count(index) != 0 )
		{ return true; }

	return false;
}

bool point_collection::empty()
{
	if ( thePoints.empty() )
		{ return true; }
	return false;
}

int point_collection::size()
{
	return thePoints.size();
}

void point_collection::save_histogram_to_txt_file(const std::string filename, const std::string separator)
    {
        pt_error::pt_error ("Attempt to save_histogram_to_txt_file on a point_collection object",pt_error::warning);
    }
