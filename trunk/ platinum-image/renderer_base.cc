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

renderer_base::renderer_base()
    {
    maxrendererID++;
    identitet=maxrendererID;

    std::cout << "renderer_base-kontruktör" << std::endl;

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
void renderer_base::move( float pan_x, float pan_y, float pan_z, float zoom_d)
    {
    //arguments are relative unit screen coordinates, ie. pan_x==1 moves wheretorender->look_at
    //to the right the distance of image's longest edge
    //also relative to view orientation, and pan_z is independent of zoom (which is intuitively right)

    Vector3D pan,final_pan;

    const bool use_constraints=true;

    const float min_zoom=0.5;
    const float max_zoom=10;
    float max_pan=0.5;           //in local coordinates; means panning to composite image edge

    if ((wheretorender->zoom*zoom_d >= min_zoom && wheretorender->zoom*zoom_d <= max_zoom ) || !use_constraints)
        {wheretorender->zoom*=zoom_d;}

    pan.Fill(0);
    pan[0]=pan_x/wheretorender->zoom;
    pan[1]=pan_y/wheretorender->zoom;
    pan[2]=pan_z; //ensures that z step is <= one voxel, not too elegant tho
    //pan[2]=pan_z*voxel_resize[the_image][2][2]/data_max_norm[the_image]; //move the equivalent of 1 step in z-resolution
 
    wheretorender->look_at+=wheretorender->dir*pan;

    if (use_constraints)
        {
        for (int d=0;d<3;d++)
            {
            /*Vector3D test_pan;

            test_pan.Fill(0);
            test_pan[d]=pan[d]; //set test_pan vector to direction d component only

            test_pan=wheretorender->look_at+wheretorender->dir*test_pan;

            if (test_pan.GetNorm() > max_pan/wheretorender->zoom)   //ensure that panning the given
            //distance in direction d won't take
            //the visible area outside a box with
            //side length max_pan*2
            {pan[d]=0;}*/

            if (fabs(wheretorender->look_at[d]) > max_pan)                        
                //if look_at is outside max_pan
                //in any direction,
                //set to max_pan maintaining sign   
                
                {wheretorender->look_at[d]=max_pan*
                wheretorender->look_at[d]/fabs(wheretorender->look_at[d]);}   
            }
        }
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

std::map<std::string,float> renderer_base::get_values_world(Vector3D unitPos) const
{
    std::map<std::string,float> m;
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
                m[image->name()] = (image->get_number_voxel(tlb[0],tlb[1],tlb[2]));
                }
            //else
            //    {m.push_back(-1);}
            }
        }
    return m;    
}