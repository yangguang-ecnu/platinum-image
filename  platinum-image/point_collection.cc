
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
}

//AF
point_collection::pointStorage::iterator point_collection::begin()
{
	return pointStorage::iterator(thePoints.begin());
}

//AF
point_collection::pointStorage::iterator point_collection::end()
{
	return pointStorage::iterator(thePoints.end());
}

//AF
void point_collection::add(pointStorage::mapped_type point)
{
	if ( thePoints.empty() )
	{
		thePoints[1] = point;
		std::cout << "thePoints is empty" << std::endl;
	}
	else
	{
		pointStorage::reverse_iterator riter;
		riter = thePoints.rbegin();
		std::cout << "riter->first: "<< riter->first << std::endl;
		thePoints[riter->first + 1] = point;
	}
}

//AF
void point_collection::add_pair(pointStorage::key_type index, pointStorage::mapped_type point)
{
	thePoints[index] = point;
}

//AF
point_collection::pointStorage::mapped_type point_collection::get_point (int i)
{
	if ( thePoints.count(i) != 0)
	{	// the point exists
		return thePoints[i];
	}
	else
	{
		throw out_of_range("Unvalid key");
	}

}



//AF
//void point_collection::set_point (pointStorage::key_type index, pointStorage::mapped_type point)
//{
//	pointStorage::mapped_type & p = thePoints[index];
//	p = point;
//}

//AF
//int point_collection::size()
//{
//	return thePoints.size();
//}

//AF
//void point_collection::save_to_file (const std::string)
//{
//	//TODO: implement a solution that uses iterators
//
//	std::cout << "save_to_file is not implemented yet";
//
//}



//#pragma mark *** landmark_collection ***

//AF
//landmark_collection::landmark_collection()
//{
//	std::cout << "landmark_collection" << std::endl;
//
//    // widget=new datawidget<point_collection>(this,"Untitled point");
//
//	// vet inte om jag behöver ovanstående...
//	// landmark_collection ärver ju av point_collection och är därmed av den typen
//	// så därför borde det räcka med att den raden finns i konstruktorn för point_collection
//	// om det nu räcker så behöver jag inte lägga till någon extra metod  datawidget.cc
//	// möjligtvis så måste jag lägga till :point_collection()
//	// vet inte om det behövs eller om defaultkonstruktorn för den klassen jag ärver ifrån
//	// automatiskt körs
//	// -> prova genom att lägga till en std::cout i point_collection-konstruktorn
//}
//
//point_collection::pointStorage::iterator landmark_collection::begin()
//{
//	return pointStorage::iterator(landmarkdata.begin());
//}
//
//point_collection::pointStorage::iterator landmark_collection::end()
//{
//	return pointStorage::iterator(thePoints.end());
//}


void point_collection::save_histogram_to_txt_file(const std::string filename, const std::string separator)
    {
        pt_error::pt_error ("Attempt to save_histogram_to_txt_file on a point_collection object",pt_error::warning);
    }
