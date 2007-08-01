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
const float renderer_base::display_scale = 25;


renderer_base::renderer_base()
    {
    maxrendererID++;
    identitet=maxrendererID;

    std::cout << "renderer_base-kontruktÃ¶r" << std::endl;

    imagestorender=NULL;
    imagestorender_id=0;

    wheretorender=NULL;
    wheretorender_id=0;
    }
void renderer_base::connect_geometry (rendergeometry * g)
    {
    wheretorender= g;
    wheretorender_id=g->get_id();
    }
void renderer_base::connect_combination (rendercombination * c)
    {
    imagestorender=c;
    imagestorender_id=c->get_id();
    }

int renderer_base::combination_id()
    {
    return imagestorender_id;
    }

void renderer_base::look_at(float x, float y, float z,float zoom)
    {
    wheretorender->look_at[0]=x;
    wheretorender->look_at[1]=y;
    wheretorender->look_at[2]=z;

    if (zoom > 0)
        //0 indicates no selection
        {wheretorender->zoom=zoom;}
    }

void renderer_base::move( float pan_x, float pan_y, float pan_z)
{
    //arguments are physical coordinates,   
    
    Vector3D pan;
    
    /*const bool use_constraints=false;
    
    const float min_zoom=0.5;
    const float max_zoom=10;
    float max_pan=0.5;           //in local coordinates; means panning to composite image edge
    
    if ((wheretorender->zoom*zoom_d >= min_zoom && wheretorender->zoom*zoom_d <= max_zoom ) || !use_constraints)*/
    
    pan.Fill(0);
    pan[0]=pan_x;
    pan[1]=pan_y;
    pan[2]=pan_z;
    
    wheretorender->look_at+=pan;
    
    /*if (use_constraints)
        {
        for (int d=0;d<3;d++)
            {
            if (fabs(wheretorender->look_at[d]) > max_pan)                        
                //if look_at is outside max_pan
                //in any direction,
                //set to max_pan maintaining sign   
                
                {wheretorender->look_at[d]=max_pan*
                wheretorender->look_at[d]/fabs(wheretorender->look_at[d]);}   
            }
        }*/
}

void renderer_base::move_view (int vsize, int pan_x, int pan_y, int pan_z, float zoom_d)
{
    //also relative to view orientation
    //zoom is a multiple of previous zoom value
    
    Vector3D pan;

    pan.Fill(0);
    pan[0]=pan_x*renderer_base::display_scale/(float)(vsize * wheretorender->zoom);
    pan[1]=pan_y*renderer_base::display_scale/(float)(vsize * wheretorender->zoom);
    pan[2]=pan_z;
    
    wheretorender->zoom*=zoom_d;
    wheretorender->look_at+=wheretorender->dir*pan;
}

void renderer_base::move_voxels (int x,int y,int z)
{
    Vector3D dir;
    dir[0] = x; dir[1] = y; dir[2] = z;
    
    //combinations of images are rendered (which may have varying voxel sizes, 
    //positions and orientation), here it is defined that
    //move_voxels will move by the voxels of the topmost image
    
    image_base* image = imagestorender->top_image();
    
    dir = wheretorender->dir*dir;
    Matrix3D reOrient;
    reOrient = image->get_orientation().GetInverse();
    
    dir = reOrient* image->get_voxel_resize() * image->get_orientation() * dir;
            
    move(dir[0],dir[1],dir[2]);
}


/*std::vector<float> renderer_base::get_values(Vector3D unitPos) const
{
    std::vector<float> v;
    Vector3D tlb;
    for (rendercombination::iterator itr = imagestorender->begin(); itr != imagestorender->end();itr++)
        {
        image_base * image = dynamic_cast<image_base *> (itr->pointer);
        
        if (image != NULL)
            {
            //center-based to top-left-back,
            //add particular image's geometric center
            tlb=unitPos+image->unit_center();
            tlb=image->unit_to_voxel()*tlb;
            if ( tlb[0] >= 0 && tlb[1] >= 0 && tlb[2] >= 0 && tlb[0] < image->get_size_by_dim(0) && tlb[1] < image->get_size_by_dim(1) && tlb[2] < image->get_size_by_dim(2))
                {
                v.push_back(image->get_number_voxel(tlb[0],tlb[1],tlb[2]));
                }
            else
                {v.push_back(-1);}
            }
        }
    }
    return v;*/

std::map<std::string,float> renderer_base::get_values_view(int vx, int vy,int sx,int sy) const
{
    //virtual function, MSVC gets hickups without namespace spec however :(
    return renderer_base::get_values_world(view_to_world(vx,vy,sx,sy));
    }

std::map<std::string,float> renderer_base::get_values_world(Vector3D worldPos) const
{
    std::map<std::string,float> m;

    for (rendercombination::iterator itr = imagestorender->begin(); itr != imagestorender->end();itr++)
        {
        image_base * image = dynamic_cast<image_base *> (itr->pointer);
        
        if (image != NULL)
            {
            Vector3D vPos = image->world_to_voxel(worldPos);

            /*#ifdef _DEBUG
            std::cout << "x:" << vPos[0] << "y:" << vPos[1] << "z:" << vPos[2] << std::endl;
            #endif*/
            
            if ( vPos[0] >= 0 && vPos[1] >= 0 && vPos[2] >= 0 && vPos[0] < image->get_size_by_dim(0) && vPos[1] < image->get_size_by_dim(1) && vPos[2] < image->get_size_by_dim(2))
                {
                m[image->name()] = (image->get_number_voxel(vPos[0],vPos[1],vPos[2]));
                }
            
            }
        }
    
    return m;    
}