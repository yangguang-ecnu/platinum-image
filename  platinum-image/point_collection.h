//////////////////////////////////////////////////////////////////////////
//
//  point_base $Revision$
//
//  Untemplated base class for point collections
//
//  $LastChangedBy$

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

#ifndef __point_base__
#define __point_base__

#include "data_base.h"

class point_collection : public data_base
{
	public:
        //typedef std::vector <Vector3D> pointStorage;
		typedef std::map<int, Vector3D> pointStorage;
			
	protected:        
        pointStorage thePoints;
		
	public:
        point_collection();						//AF - moved from protection
         
        pointStorage::iterator begin();
        pointStorage::iterator end();

        void add (pointStorage::mapped_type);  //add point
        void remove (pointStorage::iterator); //the opposite of above
		void add_pair (pointStorage::key_type, pointStorage::mapped_type);
		
        pointStorage::size_type num_points;
		
		
//		Vector3D & get_point(int i);			//AF
		pointStorage::mapped_type get_point(int i);			//AF
//		void set_point(pointStorage::key_type, pointStorage::mapped_type);
//		int size();								//AF
		//void save_to_file (const std::string);	//AF		
        
        virtual void redraw ()
            {
            pt_error::error("Attempting redraw() on point_collection, not implemented yet");
            }
        
        virtual void save_to_VTK_file (const std::string)
            {
            pt_error::error("Attempting save_to_VTK_file on point_collection, not implemented yet");
            }
			
		virtual	void save_histogram_to_txt_file(const std::string filename, const std::string separator=";");
			
    };
	
#endif
