//////////////////////////////////////////////////////////////////////////////////
//
//  reslice routines for MPR $Revision$
///
/// arbitrary 2D slices rendered by scanline
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

#define _rendererMPR_cc_
#include "rendererMPR.h"

#define MAXRENDERVOLUMES 50

//Use arbitrary-angle rendering code
#define USE_ARBITRARY

extern datamanager datamanagement;

using namespace std;

template<class T>                   
T signed_ceil(T & x){   //ceil that returns rounded absolute upwards
    return (x < 0 ? floor (x) : ceil (x));
    }

vector<float> rendererMPR::get_values(int vx, int vy,int sx,int sy) const
    {
    //virtual function, MSVC gets hickups without namespace spec however :(
    return renderer_base::get_values(view_to_world(vx,vy,sx,sy));
    }

void rendererMPR::connect_image(int vHandlerID)
    {
    //TEST: wrapper, this should be done directly by rendermanagement
    imagestorender->add_image(vHandlerID);
    }

Vector3D rendererMPR::view_to_world(int vx, int vy,int sx,int sy) const
{
    Vector3D view,unit;
    vector<float> v;
    
    view[0]=vx-sx/2;
    view[1]=vy-sy/2;
    view[2]=0;
    
    //transform to unit coordinates
    unit=wheretorender->view_to_world(min (sx,sy))*view;
    unit=wheretorender->look_at+unit;
    
    return unit;
}

Vector3D rendererMPR::view_to_voxel(int vx, int vy,int sx,int sy,int imageID) const
{
    if (imageID != -1)
        {
        return datamanagement.get_image(imageID)->transform_unit_to_voxel (view_to_world(vx, vy,sx,sy));
        }
    else
        {    
        image_base * image = imagestorender->top_image();
        if (image !=NULL)
            {
            return image->transform_unit_to_voxel (view_to_world(vx, vy,sx,sy));
            }
        else
            {
            //no image there
            return Vector3D();
            }
        }
    //return Vector3D();
}

int rendererMPR::renderer_type()
{
    return RENDERER_MPR;
}

void rendererMPR::render_thumbnail (unsigned char *rgb, int rgb_sx, int rgb_sy, int image_ID)
{
    rendercombination r = rendercombination (image_ID);
    rendergeometry g = rendergeometry ();
    
    render_( rgb, rgb_sx, rgb_sy,&g,&r,NULL);
}

void rendererMPR::render_threshold (unsigned char *rgba, int rgb_sx, int rgb_sy, thresholdparvalue * threshold)
{
    render_( rgba, rgb_sx, rgb_sy,wheretorender,imagestorender,threshold);
}

void rendererMPR::render_position(unsigned char *rgb, int rgb_sx, int rgb_sy)
{
    render_( rgb, rgb_sx, rgb_sy,wheretorender,imagestorender,NULL);
}

//render orthogonal slices using memory-order scanline
void rendererMPR::render_(uchar *pixels, int rgb_sx, int rgb_sy,rendergeometry * where,rendercombination * what,thresholdparvalue * threshold)
{
    if (what->empty())
        {
        //*** no images: exit ***
        return;
        }
    
    //*** Variables ***
    
    float scale; //voxel to pixel ratio, determined so that dataset's longest edge
                 //fits into renderer's unit space (this includes z-direction of course)
                 //multiplied with zoom it gives the V/P ratio to render with
                 //also taking into account current zoom level
    
    int vol_count; //number of images to render in this call
    
    // *** Volume setup ***
    
    Vector3D voxel_offset[MAXRENDERVOLUMES]; //offset (translation) from origin in image
    
    float rgb_min_norm=min(float(rgb_sx),float(rgb_sy));
    
    //renderer_max_norm=0;
    /*vol_count=0;
    
    while (what->image_remaining(vol_count))
        { vol_count++; }*/
    vol_count = static_cast<int> (std::distance (what->begin(), what->end()));
    
    // *** Pixmap rendering parameters ***
    
    Vector3D screen_center;
    
    screen_center[0]=rgb_sx; 
    screen_center[1]=rgb_sy; 
    screen_center[2]=0;
    
    screen_center/=2;
    
    Vector3D unit_screen_center=screen_center/rgb_min_norm; //dividing by min_norm resolves problem of
                                                            //keeping proportions with varying pixmap size
    
    //fill background color
    //standard MIN blending leaves background white, instead background is marked
    //with zero intensity in G+B channels, and tinted in a second pass
    
    blendmode blend_mode;
    if (threshold == NULL)
        { blend_mode = what->blend_mode(); }
    else
        { blend_mode = RENDER_THRESHOLD; }
    
    switch (blend_mode)
        {
        case RENDER_THRESHOLD:
            for (long p=0; p < rgb_sx*rgb_sy*RGBApixmap_bytesperpixel; p +=RGBApixmap_bytesperpixel )
                {
                pixels[p] = pixels[p + 1] = pixels[p + 2]= pixels[p + 3]=0;
                }
            break;
        case BLEND_MIN:
            for (long p=0; p < rgb_sx*rgb_sy*RGBpixmap_bytesperpixel; p +=RGBpixmap_bytesperpixel )
                {
                pixels[p] = 255;
                pixels[p + 1] = pixels[p + 2] = 0;
                }
            break;
        default:
            for (long p=0; p < rgb_sx*rgb_sy*RGBpixmap_bytesperpixel; p +=RGBpixmap_bytesperpixel )
                {pixels[p] = pixels[p + 1] = pixels[p + 2]=0;}
        }
    
    
    // *** Per-image render loop ***
    
    Vector3D data_size;
    
    //for (int the_image=0;the_image <vol_count;the_image++)
    int the_image = 0;
    for (rendercombination::iterator pairItr = what->begin();pairItr != what->end();pairItr++)  
        {
        image_base *the_image_pointer, *the_other_image_pointer;
        
        the_image_pointer = dynamic_cast<image_base *> (pairItr->pointer);
        
        //skip non-images, for now
        if (the_image_pointer != NULL )
            { 
            
            for(int d=0; d<3; d++)
                {data_size[d]=the_image_pointer->get_size_by_dim(d);}
            
            scale = rgb_min_norm/(max(data_size[0],max(data_size[1],data_size[2])));
            
            
            if (blend_mode == RENDER_THRESHOLD)
                {
                the_image_pointer       = datamanagement.get_image (threshold->id[0]);
                the_other_image_pointer = datamanagement.get_image (threshold->id[1]);
                }
            else
                { the_image_pointer=(image_base *)the_image_pointer; }
            
            // *** loop variables common to scanline and orthogonal renderer ***
            
            //color for tint mode
            int tint_r=(((the_image % 3) ==0) ^ (the_image > 2));
            int tint_g=(((the_image % 3) ==1) ^ (the_image > 2));
            int tint_b=(((the_image % 3) ==2) ^ (the_image > 2));
            
            //pixel fill start & end points, used in common blend mode code
            long fill_x_start,
                fill_x_end,
                fill_y_start,
                fill_y_end;
            
            //position to read in voxel data grid
            Vector3D vox;
            
            //RGB value in loop
            RGBvalue value = RGBvalue();
            bool threshold_value = false;
            
            Matrix3D render_dir=where->dir;
            
            //render_dir*=the_image_pointer->direction;
            
            Vector3D start,end;
            
            //end of direction vectors and precomputed variables
            //derived from these
            
            //start position in image
            voxel_offset[the_image]=(the_image_pointer->unit_to_voxel())*(where->look_at+the_image_pointer->unit_center()-((render_dir*unit_screen_center)/where->zoom));
            
#ifndef USE_ARBITRARY
            //transform start to pixel units & voxel space which the renderer
            //is based on, since it allows us to step through voxel space for
            //each rendered pixel with integers representing the position
            
            start=the_image_pointer->get_voxel_resize()*voxel_offset[the_image]*where->zoom*scale;
            
            Matrix3D reverse_dir;
            Vector3D view_offset;
            
            reverse_dir=render_dir.GetTranspose();
            view_offset=reverse_dir*(-start);
            
            //end point of rendering box (in reality a slice)
            //since coordinates are pixel-sized, this box will
            //always be permutations of
            //pixmap width, pixmap height and 0
            Vector3D view_box;
            
            view_box[0]=rgb_sx;
            view_box[1]=rgb_sy;
            view_box[2]=0;
            
            end=start+render_dir*view_box;
            
            //swap start and end values so voxels will still be traversed in correct order
            for (int d=0;d < 3;d++)
                {
                if (start[d] > end[d])
                    {t_swap (start[d],end[d]);}
                }
            
            // *** Rendering ***
            
            //Optimization strategies:
            
            //1. process voxel data in memory order for optimal cache use
            //2. fill entire voxels (at > 1 times magnification in x or y direction)
            // in one pass rather than through several scanlines
            //3. store frequent calculations
            
            //long rgb_x,rgb_y; //pixmap position
            
            //long view_fill_dir_y = render_dir(0,1)+render_dir(1,1)+render_dir(2,1); //directions in which pixmap is filled
            //long view_fill_dir_x = render_dir(0,0)+render_dir(1,0)+render_dir(2,0);
            
            //long rgb_x_next,rgb_y_next;
            
            //loop variables - integer position to avoid truncation artifacts
            
            long vp [3];
            
            vp [2] = static_cast<long>(start[2]);
            
            //cached values for render loop
            
            //size in pixels of rendered voxel
            float vp_delta[3];
            
            //actual rendered voxel size at a particular point (3 dimensions for consistency)
            int rgb_delta [3];
            for (int d = 0;d < 3; d++)
                {rgb_delta [d] = 0;}
            
            //voxel axis of x and y render directions
            int rgb_x_index,rgb_y_index;
            
            for (int d = 0;d < 3; d++)
                {
                if (render_dir [d][0] != 0)
                    {rgb_x_index = d; }
                if (render_dir [d][1] != 0)
                    {rgb_y_index = d; }
                }
            
            for (int d = 0; d <3; d ++)
                {
                //if (d == rgb_x_index || d == rgb_y_index)
                {vp_delta[d] = max ((float)1,scale*where->zoom*the_image_pointer->get_voxel_resize()[d][d]);}
                // else
                // {vp_delta[d] = 0; }
                }
            
            
            //long pixpos_comp [3][2];
            
            // *** Do rendering ***
            
            //most of the expressions here stay constant at some time and can be stored to speed things u
            //each loop must be passed through at least once, hence do{}while loops
            
            do{ //iterate z
                /*vox[2]=floor (vp [2]/(scale*where->zoom*the_image_pointer->get_voxel_resize()[2][2]));*/
                if (vp_delta [2] != 0)
                    {vox[2] = vp [2]/vp_delta[2];}
                
                //pixpos_comp[2][0]=vp [2]*render_dir[2][0];
                //pixpos_comp[2][1]=vp [2]*render_dir[2][1];
                
                vp [1]=static_cast<long>(start[1]);
                
                do{ //iterate y
                    /*vox[1]=floor (vp [1]/(scale*where->zoom*the_image_pointer->get_voxel_resize()[1][1]));*/
                    if (vp_delta [1] != 0)
                        {vox[1]= vp [1]/vp_delta[1];}
                    
                    //pixpos_comp[1][0]=vp [1]*render_dir[1][0];
                    //pixpos_comp[1][1]=vp [1]*render_dir[1][1];
                    
                    vp [0]=static_cast<long>(start[0]);
                    do{ //iterate x
                        
                        //position in voxel space
                        
                        /*vox[0]=floor (vp [0]/(scale*where->zoom*the_image_pointer->get_voxel_resize()[0][0]));*/
                        if (vp_delta [0] != 0)
                            {vox[0]=vp [0]/vp_delta[0];}
                        
                        rgb_delta [rgb_x_index] = max (1, (int)(vox[0] * vp_delta [rgb_x_index] - vp [rgb_x_index]));
                        rgb_delta [rgb_y_index] = max(1, (int)(vox[1] * vp_delta [rgb_y_index] - vp [rgb_y_index]));
                        
                        
                        
                        //TODO: flytta 1 och 2 till yttre loopar
                        /*vp_delta [0] = max ((long)1,(long)((vox[0]+1) * the_image_pointer->get_voxel_resize()[0][0]*scale) - vp[0]);
                        vp_delta [1] = max ((long)1,(long)((vox[1]+1) * the_image_pointer->get_voxel_resize()[1][1]*scale) - vp[1]);
                        vp_delta [2] = max ((long)1,(long)((vox[2]+1) * the_image_pointer->get_voxel_resize()[2][2]*scale) - vp[2]);*/
                        
                        //pixpos_comp[0][0]=vp [0]*render_dir[0][0];
                        //pixpos_comp[0][1]=vp [0]*render_dir[0][1];
                        
                        //position in rgb pixmap
                        
                        /*rgb_x=pixpos_comp[0][0]+pixpos_comp[1][0]+pixpos_comp[2][0]+ view_offset[0];
                        rgb_y=pixpos_comp[0][1]+pixpos_comp[1][1]+pixpos_comp[2][1]+ view_offset[1];*/
                        
                        //the position to which the current voxel stretches in rgb pixmap
                        //at where->zoom*scale below 1, this max not be an entire pixel step
                        //hence vp is incremented by at least 1 in their respective loops
                        
                        //float rgb_dir_x = render_dir[0][0]+render_dir[1][0]+render_dir[2][0] == (-1) ? -.5 : 0;
                        //float rgb_dir_y = render_dir[0][1]+render_dir[1][1]+render_dir[2][1] == (-1) ? -.5 : 0;
                        
                        //rgb_x_next=static_cast<long>((render_dir[0][0]*(vox[0]+1) *the_image_pointer->get_voxel_resize()[0][0]+render_dir[1][0]*(vox[1]+1)*the_image_pointer->get_voxel_resize()[1][1]+render_dir[2][0]*(vox[2]+1)*the_image_pointer->get_voxel_resize()[2][2])*scale*where->zoom);
                        
                        //rgb_y_next=static_cast<long>((render_dir[0][1]*(vox[0]+1)*the_image_pointer->get_voxel_resize()[0][0]+render_dir[1][1]*(vox[1]+1)*the_image_pointer->get_voxel_resize()[1][1]+render_dir[2][1]*(vox[2]+1)*the_image_pointer->get_voxel_resize()[2][2])*scale*where->zoom);
                        
                        
                        //#ifdef _DEBUG
                        //                    for (int d =0;d< 3 && print;d++)
                        //                        {cout << "vp [" << d << "] = " << vp[d] << "; ";}
                        //                    cout << endl;
                        //#endif
                        
                        // X
                        fill_x_start    = vp [rgb_x_index];
                        fill_x_end      = fill_x_start + vp_delta [rgb_x_index];
                        
                        if (render_dir [rgb_x_index][0] < 0)
                            {
                            //reverse coordinates, view_offset will compensate for the reversed direction
                            fill_x_start    =  - fill_x_start;
                            fill_x_end      =  - fill_x_end;
                            }
                        
                        fill_x_start    += view_offset[0];
                        fill_x_end      += view_offset[0];
                        
                        fill_x_start    = max ((long)0, min ((long)rgb_sx, fill_x_start));
                        fill_x_end      = max ((long)0, min ((long)rgb_sx, fill_x_end));
                        
                        if (fill_x_start > fill_x_end)
                            {t_swap (fill_x_start, fill_x_end); }
                        
                        // Y
                        fill_y_start    = vp [rgb_y_index];
                        fill_y_end      = fill_y_start + vp_delta [rgb_y_index];
                        
                        if (render_dir [rgb_y_index][1] < 1)
                            {
                            fill_y_start    =  - fill_y_start;
                            fill_y_end      =  - fill_y_end;
                            }
                        
                        fill_y_start    += view_offset[1];
                        fill_y_end      += view_offset[1];
                        
                        fill_y_start    = max ((long)1, min ((long)rgb_sy, fill_y_start));
                        fill_y_end      = max ((long)1, min ((long)rgb_sy, fill_y_end));
                        
                        if (fill_y_start > fill_y_end)
                            {t_swap (fill_y_start, fill_y_end); }
                        
                        
                        //    if (render_dir [rgb_x_index][0] < 0)
                        //        {
                        //        fill_x_start  = max ((long)0, min ((long)rgb_sx, (long)(rgb_sx - vp [rgb_x_index])+ view_offset[0])));
                        //        }
                        
                        //long fill_x_start
                        //    = max ((long)0, min ((long)rgb_sx, (long)(vp [rgb_x_index]+ view_offset[0])));
                        //long fill_x_end
                        //    = min ((long)rgb_sx, max((long)0, (long)(vp [rgb_x_index] + vp_delta [rgb_x_index]+ view_offset[0])));
                        
                        //long fill_y_start
                        //    = max ((long)0, min ((long)rgb_sy, (long)(vp [rgb_y_index]+ view_offset[1])) );
                        //long fill_y_end
                        //    = min ((long)rgb_sy, max((long)0, (long)(vp [rgb_y_index] + vp_delta [rgb_y_index]+ view_offset[1])));
                        
                        /*long fill_y_start
                            = max ((long)0, min ((long)rgb_sy, (long)(rgb_y)));
                        long fill_y_end
                            = min ((long)rgb_sy, max((long)0, (long)(rgb_y_next)));
                        
                        long fill_x_start
                            = max ((long)0, min ((long)rgb_sx, (long)(rgb_x)));
                        long fill_x_end
                            = min ((long)rgb_sx, max((long)0, (long)(rgb_x_next)));*/
                        
#else
                        
                        start = voxel_offset[the_image];
                        
                        //set slope to size of render plane in unit coordinates
                        
                        Vector3D slope_x, slope_y;
                        
                        Matrix3D pix_to_vox;
                        pix_to_vox = the_image_pointer->get_voxel_resize().GetInverse();
                        
                        slope_x.Fill(0);
                        slope_x[0]=1;
                        
                        slope_x = render_dir * slope_x;
                        slope_x = pix_to_vox * slope_x;
                        slope_x /= (scale*where->zoom);
                        
                        slope_y.Fill(0);
                        slope_y[1]=1;
                        
                        slope_y = render_dir * slope_y;
                        slope_y = pix_to_vox * slope_y;
                        slope_y /= (scale*where->zoom);
                        
                        //center slice onscreen
                        
                        int nonsquare_offset=(rgb_sx-rgb_sy)/2;
                        
                        if (nonsquare_offset > 0)
                            {
                            //if positive, image is portrait orientation
                            start-=slope_x*nonsquare_offset;
                            }
                        
                        else
                            {
                            start+=slope_y*nonsquare_offset;
                            }
                        
                        // Render loop
                        
                        //1. iterate Y and determine new position of horizontal scanline
                        //2. check that position is within data bounds
                        //3. render pixel
                        //4. move along scanline (iterate x)
                        //5. repeat
                        
                        for ( fill_y_start=0; fill_y_start < rgb_sy; fill_y_start++)
                            {
                            fill_y_end=fill_y_start+1;
                            vox=start+slope_y*fill_y_start;
                            
                            for ( fill_x_start=0; fill_x_start < rgb_sx; fill_x_start++)
                                {
                                //if ((vox[0]>0) && (vox[0]<data_size[0])
                                
                                //    && (vox[1]>0) && (vox[1]<data_size[1])
                                //    && (vox[2]>0) && (vox[2]<data_size[2]))
                                
                                //    {
                                fill_x_end=fill_x_start+1;
                                
#endif
                                //get actual value in data, this has been scaled to fit the range of unsigned char
                                if (vox[0] >= 0 && vox[1] >= 0 && vox[2] >= 0 && vox[0] < data_size[0] && vox[1] < data_size[1] && vox[2] < data_size[2])
                                    {
                                    if (blend_mode == RENDER_THRESHOLD)
                                        {
                                        float t_value [2];
                                        
                                        t_value[0] = the_image_pointer->get_number_voxel(vox[0],vox[1],vox[2]);
                                        t_value[1] = the_other_image_pointer->get_number_voxel(vox[0],vox[1],vox[2]);
                                        
                                        //rect threshold
                                        threshold_value = ( t_value[0] > threshold->low[0] && t_value[0] < threshold->high[0] &&
                                                            t_value[1] > threshold->low[1] && t_value[1] < threshold->high[1]);
                                        
                                        if (threshold_value && threshold->mode==THRESHOLD_2D_MODE_OVAL)
                                            {
                                            //oval threshold
                                            //value = value && (sqrt(powf((t_value[0]-((threshold->high[0]+threshold->low[0])/2.0))/((threshold->high[0]-threshold->low[0])/(threshold->high[1]-threshold->low[1])),2.0)+powf(t_value[1]-((threshold->high[1]+threshold->low[1])/2.0),2.0) ) <= (threshold->high[1]+threshold->low[1])/2.0);
                                            threshold_value = (sqrt(powf((t_value[0]-((threshold->high[0]+threshold->low[0])/2.0))/((threshold->high[0]-threshold->low[0])/(threshold->high[1]-threshold->low[1])),2.0)+powf(t_value[1]-((threshold->high[1]+threshold->low[1])/2.0),2.0) ) <= (threshold->high[1]+threshold->low[1])/2.0);
                                            }
                                        }
                                    else
                                        { the_image_pointer->get_display_voxel(value,vox[0],vox[1],vox[2]);}
                                    
                                    
                                    for (long rgb_fill_y=fill_y_start; (rgb_fill_y <  fill_y_end);rgb_fill_y++)
                                        {
                                        for (long rgb_fill_x=fill_x_start;(rgb_fill_x < fill_x_end);rgb_fill_x++)
                                            {
                                            switch (blend_mode)
                                                {
                                                case BLEND_OVERWRITE:
                                                    value.write(pixels+RGBpixmap_bytesperpixel * (rgb_fill_x+rgb_sx*rgb_fill_y));
                                                    break;
                                                    
                                                case BLEND_MIN:
                                                    if (pixels[RGBpixmap_bytesperpixel *
                                                        
                                                        (rgb_fill_x+rgb_sx*rgb_fill_y)] >= value.mono())
                                                        
                                                        {
                                                        //we can assume that the R value represents total pixel intensity
                                                        //because previous pixel value was set with the same mode
                                                        
                                                        //more than or equal (>=) above is important because
                                                        //we want to replace the background even for the r=255 case
                                                        //to mask out background color
                                                        
                                                        value.write(pixels+RGBpixmap_bytesperpixel * (rgb_fill_x+rgb_sx*rgb_fill_y));
                                                        }
                                                    break;
                                                    
                                                case BLEND_MAX:
                                                    if (pixels[RGBpixmap_bytesperpixel *
                                                        (rgb_fill_x+rgb_sx*rgb_fill_y)] < value.mono())
                                                        {
                                                        //we can assume that the R value represents total pixel intensity
                                                        //because previous pixel value was set with the same mode
                                                        
                                                        value.write(pixels+RGBpixmap_bytesperpixel * (rgb_fill_x+rgb_sx*rgb_fill_y));
                                                        }
                                                    break;
                                                    
                                                case BLEND_AVG:
                                                    {
                                                        /*unsigned char prevval[3];
                                                        for (int c=0;c < 3;c++)
                                                        {prevval[c]=pixels[RGBpixmap_bytesperpixel * (rgb_fill_x+rgb_sx*rgb_fill_y) + c]; }
                                                        pixels[RGBpixmap_bytesperpixel * (rgb_fill_x+rgb_sx*rgb_fill_y)] = prevval[0] + value/vol_count;
                                                        pixels[RGBpixmap_bytesperpixel * (rgb_fill_x+rgb_sx*rgb_fill_y) + 1] = prevval[1] + value/vol_count;
                                                        pixels[RGBpixmap_bytesperpixel * (rgb_fill_x+rgb_sx*rgb_fill_y) + 2] = prevval[2] + value/vol_count;*/
                                                        
                                                        pixels[RGBpixmap_bytesperpixel * (rgb_fill_x+rgb_sx*rgb_fill_y)] += value.r()/vol_count;
                                                        pixels[RGBpixmap_bytesperpixel * (rgb_fill_x+rgb_sx*rgb_fill_y) + 1] += value.g()/vol_count;
                                                        pixels[RGBpixmap_bytesperpixel * (rgb_fill_x+rgb_sx*rgb_fill_y) + 2] += value.b()/vol_count;
                                                    }
                                                    break;
                                                    
                                                case BLEND_TINT:
                                                    pixels[RGBpixmap_bytesperpixel *
                                                        (rgb_fill_x+rgb_sx*rgb_fill_y)] += tint_r*value.mono();
                                                    pixels[RGBpixmap_bytesperpixel *
                                                        (rgb_fill_x+rgb_sx*rgb_fill_y) + 1] += tint_g*value.mono();
                                                    pixels[RGBpixmap_bytesperpixel *
                                                        (rgb_fill_x+rgb_sx*rgb_fill_y) + 2] += tint_b*value.mono();
                                                    break;
                                                    
                                                case RENDER_THRESHOLD:
                                                    if (threshold_value)
                                                        {
                                                        RGBAvalue value = RGBAvalue (IMGELEMCOMPMAX,0,0, IMGELEMCOMPMAX);
                                                        value.write(pixels+RGBApixmap_bytesperpixel * (rgb_fill_x+rgb_sx*rgb_fill_y));
                                                        }
                                                    break;
                                                default:
                                                    {
                                                        //suppress GCC enum warning
                                                    }
                                                } // switch (blend_mode)
                                            
                                            } //rgb_fill_x loop
                                        
                                        } //rgb_fill_y loop
                                    
                                    } //if within data bounds
#ifndef USE_ARBITRARY
                                
                                vp [0]+=rgb_delta[0];
                                }while (vp [0] < end[0] && rgb_delta [0] != 0 );
                            
                            vp [1]+=rgb_delta[1];
                                }while (vp [1] < end[1] && rgb_delta [1] != 0 );
                        
                        vp [2]+=rgb_delta[2];
                            }while (vp [2] < end[2] && rgb_delta [2] != 0 );
                    
#else
                    vox+=slope_x;
                } //fill_x_start loop
            } //fill_y_start loop
#endif
            if (blend_mode== BLEND_MIN)
                {
                for (long p=0; p < rgb_sx*rgb_sy*RGBpixmap_bytesperpixel; p +=RGBpixmap_bytesperpixel )
                    {
                    if (pixels[p] != pixels[p+1])
                        {
                        //unaltered background, fill with final background color
                        
                        pixels[p] = pixels[p + 1] = 0;
                        pixels[p + 2] = 127;
                        }
                    }
                }
            
        } //the_image_pointer != NULL
        
        the_image++;
        }   //per-image loop
    
    }//render_ function

void rendererMPR::fill_rgbimage_with_value(unsigned char *rgb, int x, int y, int w, int h, int rgb_sx, int value){

    for (int rgb_y=y; rgb_y < y+h; rgb_y++){

        for (int rgb_x=x; rgb_x < x+w; rgb_x++){

            rgb[RGBpixmap_bytesperpixel * (rgb_x+rgb_sx*rgb_y)] = value;

            rgb[RGBpixmap_bytesperpixel * (rgb_x+rgb_sx*rgb_y) + 1] = value;

            rgb[RGBpixmap_bytesperpixel * (rgb_x+rgb_sx*rgb_y) + 2] = value;

            }

        }

    }
