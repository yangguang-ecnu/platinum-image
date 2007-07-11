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

class points : public data_base
    {
public:
        typedef std::vector <Vector3D> pointStorage;
protected:
        points();
        
        pointStorage thePoints;
public:
         
        pointStorage::iterator begin();
        pointStorage::iterator end();

        void add (pointStorage::value_type);  //add point
        void remove (pointStorage::iterator); //the opposite of above
        pointStorage::size_type num_points;
    };

#endif
