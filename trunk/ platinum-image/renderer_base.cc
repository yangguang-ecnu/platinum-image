// $Id$

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

#include "renderer_base.h"

int renderer_base::maxrendererID = 0;

using namespace std;

renderer_base::renderer_base()
    {
    maxrendererID++;
    id=maxrendererID;

    the_rc=NULL;
    rc_id=0;

    the_rg=NULL;
    rg_id=0;
    }

void renderer_base::connect_geometry (rendergeometry * g)
    {
    the_rg= g;
    rg_id=g->get_id();
    }
	
void renderer_base::connect_combination(rendercombination *rc)
    {
    the_rc=rc;
    rc_id=rc->get_id();
    }

int renderer_base::combination_id()
    {
    return rc_id;
    }
	
int renderer_base::geometry_id() const
{
	return rg_id;
}

void renderer_base::look_at(float x, float y, float z,float zoom)
    {
    the_rg->look_at[0]=x;
    the_rg->look_at[1]=y;
    the_rg->look_at[2]=z;

    if (zoom > 0)
        //0 indicates no selection
        {the_rg->zoom=zoom;}
    }

void renderer_base::move( float pan_x, float pan_y, float pan_z)
{
    //arguments are physical coordinates,   
    
    Vector3D pan;
    
    /*const bool use_constraints=false;
    
    const float min_zoom=0.5;
    const float max_zoom=10;
    float max_pan=0.5;           //in local coordinates; means panning to composite image edge
    
    if ((the_rg->zoom*zoom_d >= min_zoom && the_rg->zoom*zoom_d <= max_zoom ) || !use_constraints)*/
    
    pan.Fill(0);
    pan[0]=pan_x;
    pan[1]=pan_y;
    pan[2]=pan_z;
    
    the_rg->look_at+=pan;
    
    /*if (use_constraints)
        {
        for (int d=0;d<3;d++)
            {
            if (fabs(the_rg->look_at[d]) > max_pan)                        
                //if look_at is outside max_pan
                //in any direction,
                //set to max_pan maintaining sign   
                
                {the_rg->look_at[d]=max_pan*
                the_rg->look_at[d]/fabs(the_rg->look_at[d]);}   
            }
        }*/
}

void renderer_base::move_view (int vsize, int pan_x, int pan_y, int pan_z, float zoom_d)
{
    //also relative to view orientation
    //zoom is a multiple of previous zoom value
    
    Vector3D pan;

    pan.Fill(0);
	
//    pan[0]=pan_x*renderer_base::display_scale/(float)(vsize * the_rg->zoom);
//    pan[1]=pan_y*renderer_base::display_scale/(float)(vsize * the_rg->zoom);
    pan[0]=pan_x*ZOOM_CONSTANT/(float)(vsize * the_rg->zoom);
    pan[1]=pan_y*ZOOM_CONSTANT/(float)(vsize * the_rg->zoom);
    pan[2]=pan_z;
    
//	cout<<"zoom="<<the_rg->zoom<<"-->";
    the_rg->zoom*=zoom_d;
//	cout<<the_rg->zoom<<endl;
    the_rg->look_at+=the_rg->dir*pan;
}

void renderer_base::move_voxels (int x,int y,int z)
{
    Vector3D dir;
    dir[0] = x; dir[1] = y; dir[2] = z;
    
    //combinations of images are rendered (which may have varying voxel sizes, 
    //positions and orientation), here it is defined that
    //move_voxels will move by the voxels of the topmost image
    
    image_base* image = the_rc->top_image();
    
    dir = the_rg->dir*dir;
    Matrix3D reOrient;
    reOrient = image->get_orientation().GetInverse();
    
    dir = reOrient* image->get_voxel_resize() * image->get_orientation() * dir;
            
    move(dir[0],dir[1],dir[2]);
}

std::vector<int> renderer_base::world_to_view(rendergeometry *g, int sx, int sy, const Vector3D wpos)
{
    std::vector<int> view;
    Vector3D toView = wpos;
    int vmin = std::min(sx,sy);
    //float wtvCenterScale = renderer_base::display_scale/((float)vmin*g->zoom*2);
   
    Matrix3D world_to_view_matrix;
    world_to_view_matrix = g->view_to_world_matrix(vmin).GetInverse();
    
    toView = world_to_view_matrix * (toView - g->look_at);
    	
    view.push_back(round(toView[0]+sx/2.0));
    view.push_back(round(toView[1]+sy/2.0));

//	cout<<"world_to_view(sx="<<sx<<" sy="<<sy<<" wpos="<<wpos<<") --"<<view[0]<<" "<<view[1]<<endl;

    return view;
}

std::vector<float> renderer_base::world_dir_to_view_dir (rendergeometry * g,int sx,int sy,const Vector3D w_dir)
{
    std::vector<float> view;
	int vmin = std::min (sx,sy);
   
    Matrix3D world_to_view_matrix;
    world_to_view_matrix = g->view_to_world_matrix(vmin).GetInverse();
    
    Vector3D toView = world_to_view_matrix * (w_dir);
    	
    view.push_back(toView[0]);
    view.push_back(toView[1]);
    return view;
}


std::vector<int> renderer_base::world_to_view (int sx,int sy,const Vector3D wpos) const
{
    return world_to_view(the_rg,sx,sy,wpos);
}

std::map<std::string,float> renderer_base::get_values_view(int vx, int vy,int sx,int sy) const 
{
    //virtual function, MSVC gets hickups without namespace spec however :(
    return renderer_base::get_values_world(view_to_world(vx,vy,sx,sy));
}

std::map<std::string,float> renderer_base::get_values_world(Vector3D worldPos) const
{
    std::map<std::string,float> m;

    for (rendercombination::iterator itr = the_rc->begin(); itr != the_rc->end();itr++)
        {
        image_base * image = dynamic_cast<image_base *> (itr->pointer);
        
        if (image != NULL)
            {
            Vector3D vPos = image->world_to_voxel(worldPos);
            
            if ( vPos[0] >= 0 && vPos[1] >= 0 && vPos[2] >= 0 && vPos[0] < image->get_size_by_dim(0) && vPos[1] < image->get_size_by_dim(1) && vPos[2] < image->get_size_by_dim(2))
                {
                m[image->name()] = (image->get_number_voxel(vPos[0],vPos[1],vPos[2]));
                }
            
            }
        }
    
    return m;    
}