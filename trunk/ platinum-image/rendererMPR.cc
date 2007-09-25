//////////////////////////////////////////////////////////////////////////////////
//
//  RendererMPR $Revision$
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

void rendererMPR::connect_image(int vHandlerID)
    {
    //TEST: wrapper, this should be done directly by rendermanagement
    imagestorender->add_data(vHandlerID);
	}

Vector3D rendererMPR::view_to_world(int vx, int vy,int sx,int sy) const
{
    Vector3D viewCentered,world;
    vector<float> v;
    float viewmin = std::min (sx,sy);
    
    viewCentered[0]=vx-sx/2;
    viewCentered[1]=vy-sy/2;
    viewCentered[2]=0;
    
    //transform to world coordinates
    world=wheretorender->view_to_world_matrix(viewmin)*viewCentered;
    world=world+wheretorender->look_at;
    
    return world;
}

Vector3D rendererMPR::view_to_voxel(int vx, int vy,int sx,int sy,int imageID) const
{
    if (imageID != -1)
        {
        return datamanagement.get_image(imageID)->world_to_voxel (view_to_world(vx, vy,sx,sy));
        }
    else
        {    
        Vector3D v;
        
        image_base * image = imagestorender->top_image();
        
        if (image !=NULL)
            {
            v = image->world_to_voxel (view_to_world(vx, vy,sx,sy));
            
            for (int d = 0;d < 3;d++)
                {if (v[d] >= image->get_size_by_dim(d))
                    {       
                    Vector3D n;
                    n.Fill (-1);
                    
                    return n;
                    }
                }
            
            return v;
            }
        else
            {
            //no image there
            Vector3D n;
            n.Fill (-1);
            
            return n;
            }
        }
    //return Vector3D();
}

bool rendererMPR::supports_mode (int m)
{
    switch (m)
        {
        case BLEND_OVERWRITE:
        case BLEND_MAX:
        case BLEND_MIN:
        case BLEND_AVG:
        case BLEND_TINT:
        case RENDER_THRESHOLD:
            return true;
            break;
        default:
            return false;
        }
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
        
    int vol_count; //number of images to render in this call
    
    // *** Volume setup ***
    
    Vector3D voxel_offset[MAXRENDERVOLUMES]; //offset (translation) from origin in image
    
    float rgb_min_norm=min(float(rgb_sx),float(rgb_sy));
    
    vol_count = static_cast<int> (std::distance (what->begin(), what->end()));
    
    // *** Pixmap rendering parameters ***
    
    Vector3D screen_center;
    
    screen_center[0]=rgb_sx; 
    screen_center[1]=rgb_sy; 
    screen_center[2]=0;
    
    screen_center/=2;
    
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
       
    #pragma mark *** Per-image render loop ***
    
    Vector3D data_size;
    
    int the_image = 0;

    for (rendercombination::iterator pairItr = what->begin();pairItr != what->end();pairItr++)  
        { 
        image_base *the_image_pointer, *the_other_image_pointer;
        
		pt_error::error_if_null(pairItr->pointer,"Rendered data object is NULL");//Crash here when closing an image

        the_image_pointer = dynamic_cast<image_base *> (pairItr->pointer);
        
        bool OKrender = the_image_pointer != NULL;
        
        if (blend_mode == RENDER_THRESHOLD)
            {
            the_image_pointer       = datamanagement.get_image (threshold->id[0]);
            the_other_image_pointer = datamanagement.get_image (threshold->id[1]);
            
            OKrender = the_image_pointer != NULL && the_other_image_pointer != NULL;
            }
        
        //render images in first pass, points in second
        if (OKrender )
            { 
            
            for(int d=0; d<3; d++)
                {data_size[d]=the_image_pointer->get_size_by_dim(d);}
            
            const float scale = rgb_min_norm / display_scale; //constant = the number of mms that should fit inside a viewport at zoom 1
            
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
            
            //RGB value in loop
            RGBvalue value = RGBvalue();
            bool threshold_value = false;
            
            Vector3D start,end;
            
            //end of direction vectors and precomputed variables
            //derived from these
            
            //start position in image
                  
            voxel_offset[the_image] = the_image_pointer->world_to_voxel(where->look_at-((where->dir*screen_center)/(where->zoom*scale)));
            
            start = voxel_offset[the_image];
                        
            //set slope to size of render plane in unit coordinates
                        
            Matrix3D slope;
            slope = the_image_pointer->get_voxel_resize().GetInverse();
            
            Matrix3D revDir;
            
            revDir = the_image_pointer->get_orientation().GetInverse();
            slope = revDir * slope;
            
            slope/= where->zoom * scale;
            
            Vector3D slope_x;
            slope_x.Fill(0);
            slope_x[0] = 1;
            slope_x = slope * where->dir * slope_x;
            
            Vector3D slope_y;
            slope_y.Fill(0);
            slope_y[1] = 1;
            slope_y = slope * where->dir * slope_y;
            
            //position to read in voxel data grid
            Vector3D vox;
            
            // Render loop
            
            //1. iterate Y and determine new position of horizontal scanline
            //2. check that position is within data bounds
            //3. render pixel
            //4. move along scanline (iterate x)
            //5. repeat
             
            for ( fill_y_start=0; fill_y_start < rgb_sy; fill_y_start++)
                {
                fill_y_end=fill_y_start+1;
                vox=start+slope_y*(float)fill_y_start;
                
                for ( fill_x_start=0; fill_x_start < rgb_sx; fill_x_start++)
                    {
                  
                    fill_x_end=fill_x_start+1;
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
                    vox+=slope_x;
                    } //fill_x_start loop
                    } //fill_y_start loop
            
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
    
    #pragma mark *** per-point render loop ***
    
    for (rendercombination::iterator pairItr = what->begin();pairItr != what->end();pairItr++)  
        {
        point *pointPointer = dynamic_cast<point *> (pairItr->pointer);
        
        //NOTE: dynamic_cast to point means that point_collections are excluded,
        //for now
	
		
				
		//AF --- begin ---

		
		point_collection * points;		
		if ( !dynamic_cast<point *> (pairItr->pointer) )
		{
			//std::cout << "------> " << "This is not a point! Maybe a point_collection?" << std::endl;
			if ( points = dynamic_cast<point_collection *> (pairItr->pointer) )
			{
				//std::cout << "------> " << "It is a point_collection!" << std::endl;
			}
		}


		if (points != NULL )
		{
			point_collection::pointStorage::iterator iter;
			for (iter = points->begin(); iter != points->end(); iter++)
			{
				//ofs << iter->first << "   " << iter->second << std::endl;
				Vector3D point = points->get_point(iter->first);
				draw_cross(pixels, rgb_sx, rgb_sy, where, point);
				
			}
		}


		//AF --- end ---
        
		
		
		
        if (pointPointer != NULL )
            {
            std::vector<int> loc = world_to_view (where,rgb_sx,rgb_sy,pointPointer->get_origin());
            
            //TODO: color according to distance to viewing plane
            //http://www.math.umn.edu/~nykamp/m2374/readings/planedist/index.html

            
            //draw cross
            for (int d = -2;d <= 2; d++)
                {
                pixels[RGBpixmap_bytesperpixel *
                    (loc[0]+d+rgb_sx*loc[1]) + RADDR] = 255;
                pixels[RGBpixmap_bytesperpixel *
                    (loc[0]+d+rgb_sx*loc[1]) + GADDR] = 0;
                pixels[RGBpixmap_bytesperpixel *
                    (loc[0]+d+rgb_sx*loc[1]) + BADDR] = 0;   
                
                pixels[RGBpixmap_bytesperpixel *
                    (loc[0]+rgb_sx*(loc[1]+d)) + RADDR] = 255;
                pixels[RGBpixmap_bytesperpixel *
                    (loc[0]+rgb_sx*(loc[1]+d)) + GADDR] = 0;
                pixels[RGBpixmap_bytesperpixel *
                    (loc[0]+rgb_sx*(loc[1]+d)) + BADDR] = 0;     
                }
            }
        }//point rendering loop
    
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

//AF
void rendererMPR::draw_cross(uchar *pixels, int rgb_sx, int rgb_sy, rendergeometry * where, Vector3D point)
{
	int default_size = 2;
	int on_spot_size = 4;
	float default_threshold = 10.0;
	float on_spot_threshold = 0.5;
	float distance = where->distance_to_viewing_plane(point);
	float alpha = abs(distance - default_threshold) / default_threshold;

	vector<int> default_rgb(3);	
	default_rgb[0] = 255;
	default_rgb[1] = 255;
	default_rgb[2] = 0;
	
	vector<int> on_spot_rgb(3);
	on_spot_rgb[0] = 255;
	on_spot_rgb[1] = 0;
	on_spot_rgb[2] = 0;

	

	std::vector<int> loc = world_to_view(where, rgb_sx, rgb_sy, point);

	if ( distance > default_threshold )
		{ return; }
		
	int size = default_size;
	vector<int> rgb = default_rgb;
	
	if ( distance < on_spot_threshold )
	{
		size = on_spot_size;
		rgb = on_spot_rgb;
	}
		
	for (int d = -size; d <= size; d++)
	{
		float current_horizontal_r = pixels[RGBpixmap_bytesperpixel * (loc[0]+d+rgb_sx*loc[1]) + RADDR];
		float current_horizontal_g = pixels[RGBpixmap_bytesperpixel * (loc[0]+d+rgb_sx*loc[1]) + GADDR];
		float current_horizontal_b = pixels[RGBpixmap_bytesperpixel * (loc[0]+d+rgb_sx*loc[1]) + BADDR];

		float current_vertical_r = pixels[RGBpixmap_bytesperpixel * (loc[0]+rgb_sx*(loc[1]+d)) + RADDR];
		float current_vertical_g = pixels[RGBpixmap_bytesperpixel * (loc[0]+rgb_sx*(loc[1]+d)) + GADDR];
		float current_vertical_b = pixels[RGBpixmap_bytesperpixel * (loc[0]+rgb_sx*(loc[1]+d)) + BADDR];
		
		pixels[RGBpixmap_bytesperpixel * (loc[0]+d+rgb_sx*loc[1]) + RADDR] = rgb[0] * alpha + current_horizontal_r * (1 - alpha);
		pixels[RGBpixmap_bytesperpixel * (loc[0]+d+rgb_sx*loc[1]) + GADDR] = rgb[1] * alpha + current_horizontal_g * (1 - alpha);
		pixels[RGBpixmap_bytesperpixel * (loc[0]+d+rgb_sx*loc[1]) + BADDR] = rgb[2] * alpha + current_horizontal_b * (1 - alpha);
				
		pixels[RGBpixmap_bytesperpixel * (loc[0]+rgb_sx*(loc[1]+d)) + RADDR] = rgb[0] * alpha + current_vertical_r * (1 - alpha);
		pixels[RGBpixmap_bytesperpixel * (loc[0]+rgb_sx*(loc[1]+d)) + GADDR] = rgb[1] * alpha + current_vertical_g * (1 - alpha);
		pixels[RGBpixmap_bytesperpixel * (loc[0]+rgb_sx*(loc[1]+d)) + BADDR] = rgb[2] * alpha + current_vertical_b * (1 - alpha);     
	}
}


