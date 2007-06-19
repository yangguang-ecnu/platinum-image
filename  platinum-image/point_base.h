//////////////////////////////////////////////////////////////////////////
//
//  point_base
//
//  Untemplated base class for point collections
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

#ifndef __point_base__
#define __point_base__

#include "data_base.h"

class point_base : public data_base
    {
    };

///////////////////////////////////////
// The structure is planned as follows
// data_base
//	point_base
//		points							//multiple points
//			point						//a single point is a subclass of points
//			points_unstructured			//point cloud (e.g. seed points)
//			points_seq					//e.g. contour points
//				points_seq_closed		//e.g. contour points
//				points_seq_func			//e.g. contour points
//			points_mesh					//points in a mesh structure...

#endif
