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

#include "rendergeometry.h"

#include "rendermanager.h"
extern rendermanager rendermanagement;


//-----------------------------------------------------
int rendergeometry_base::new_rg_ID=1;

rendergeometry_base::rendergeometry_base()
{
	id=new_rg_ID++;
}

int rendergeometry_base::get_id()
{
    return id;
}


//-----------------------------------------------------

rendergeom_image::rendergeom_image():rendergeometry_base()
{
    look_at.Fill(0);	    //initialize look at to center
    dir.SetIdentity();		//initialize direction
    zoom=1;					//intialize zoom to 100%
}

Matrix3D rendergeom_image::view_to_world_matrix(int viewminsize)
{
//    return dir*renderer_base::display_scale/(viewminsize*zoom);
    return dir*ZOOM_CONSTANT/(viewminsize*zoom);
}

void rendergeom_image::refresh_viewports()
{
    rendermanagement.geometry_update_callback(this->id);
}
	
float rendergeom_image::distance_to_viewing_plane(Vector3D point)
{
	Vector3D v;
	v = point - look_at;
	Vector3D n = get_n();
	float distance = abs ( v * n);
	return distance;
}

Vector3D rendergeom_image::get_N()
{
	Vector3D direction = create_Vector3D(0, 0, 1);
	return dir * direction;
}

Vector3D rendergeom_image::get_n()
{
	Vector3D N = get_N();
	return N / N.GetNorm();
}

Vector3D rendergeom_image::get_X()
{
	return dir * create_Vector3D(1, 0, 0);
}

Vector3D rendergeom_image::get_Y()
{
	return dir * create_Vector3D(0, 1, 0);
}

line3D rendergeom_image::get_physical_line_of_intersection(rendergeom_image *rg2)
{
	plane3D p = plane3D(this->look_at,this->get_n());
	plane3D p2 = plane3D(rg2->look_at,rg2->get_n());
	return p.get_line_of_intersection(p2);
}

line2D rendergeom_image::get_physical_line_of_intersection_projected(rendergeom_image *rg2)
{
	line3D l = get_physical_line_of_intersection(rg2);
	plane3D p = plane3D(this->look_at,this->get_n());
	return p.get_projected_line(l);
}

Matrix3D rendergeom_image::get_scan_line_slop_matrix(image_base *the_image_pointer, float rgb_min_norm_div_by_zoom_constant)
{
	Matrix3D orientation_inv = the_image_pointer->get_orientation().GetInverse();
	Matrix3D inv_size = the_image_pointer->get_voxel_resize().GetInverse();
	return (inv_size * orientation_inv)/(this->zoom * rgb_min_norm_div_by_zoom_constant);
}
