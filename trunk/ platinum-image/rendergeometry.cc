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

int rendergeometry::new_rg_ID=1;

Matrix3D rendergeometry::view_to_world_matrix(int viewminsize)
    {
    return dir*renderer_base::display_scale/(viewminsize*zoom);
    }

rendergeometry::rendergeometry()
    {
    //initialize look at to center

    look_at.Fill(0);

    //initialize direction
    dir.SetIdentity();

    //intialize zoom to 100%
    zoom=1;

    id=new_rg_ID++;
    }

int rendergeometry::get_id()
    {
    return id;
    }

void rendergeometry::refresh_viewports()
    {
    rendermanagement.geometry_update_callback(this->id);
    }
	
//AF	
float rendergeometry::distance_to_viewing_plane(Vector3D point)
{
	// distance to viewing plane
	// http://www.math.umn.edu/~nykamp/m2374/readings/planedist/index.html

	Vector3D n = get_n();

	Vector3D v;
	v[0] = point[0] - look_at[0] ;
	v[1] = point[1] - look_at[1] ;
	v[2] = point[2] - look_at[2] ;

	float distance = sqrt( pow((v[0]*n[0] + v[1]*n[1] + v[2]*n[2]), 2) );
	
	return distance;
}

//AF
Vector3D rendergeometry::get_n()
{
	Vector3D direction;
	direction[0] = 0;
	direction[1] = 0;
	direction[2] = 1;

	Vector3D N = dir * direction; 

	float N_magnitude = sqrt(N[0]*N[0] + N[1]*N[1] + N[2]*N[2]);

	Vector3D n;
	n[0] = N[0] / N_magnitude;
	n[1] = N[1] / N_magnitude;
	n[2] = N[2] / N_magnitude;
	
	return n;
}
