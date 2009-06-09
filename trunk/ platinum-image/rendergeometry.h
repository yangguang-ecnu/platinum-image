//////////////////////////////////////////////////////////////////////////
//
//  Rendergeometry $Revision$
///
/// Rendergeometry stores rendering coordinates (look at, orientation
/// and zoom) for renderers
///
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

#ifndef __rendergeometry__
#define __rendergeometry__

#include "ptmath.h"
#include "global.h"

class image_base;

class rendergeometry 
    {
    private:
        int id;
        static int new_rg_ID;                   //ID counter for new geometry objects

	public:
        rendergeometry();

        // *** NOTE: some render parameter constraints (like not zooming in/out at insane levels) are implemented in renderer_base::move

		Vector3D look_at;     //center viewpoint, this will be the middle of the rendering and pivot point for the slice direction.
        Matrix3D dir;       //view to composite image matrix    //only direction (no scaling) = normal of slice plane
        float zoom;  //user-determined magnification, multiplied with scale to obtain the actual rendering scale
					//ZOOM_CONSTANT/max(phys_span_x,phys_span_y)*rectangular_score;
					//rectangular_score = (vp_side_max/vp_side_min) and span = image span, from view direction

		int get_id();

		Matrix3D view_to_world_matrix(int viewminsize);
        void refresh_viewports();   //refresh viewports using this combination
		
		float distance_to_viewing_plane(Vector3D point);

		Vector3D get_N();	// return normal vector
		Vector3D get_n();	// return unit normal vector

		Vector3D get_X();	// return global vector for viewport x-dir
		Vector3D get_Y();	// return global vector for viewport y-dir

		line3D get_physical_line_of_intersection(rendergeometry *rg2);
		line2D get_physical_line_of_intersection_projected(rendergeometry *rg2);

		Matrix3D get_scan_line_slop_matrix(image_base *the_image_pointer, float rgb_min_norm_div_by_zoom_constant);
	};
#endif