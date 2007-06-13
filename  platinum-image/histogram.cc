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

#include "histogram.h"

// *** histogram_base ***

void histogram_base::clear_pixmap (uchar * image, unsigned int w,unsigned int h)
    {
    //clear image
    unsigned int isize=w*h*RGBpixmap_bytesperpixel;
    for (unsigned int i=0;i < isize; i+=RGBpixmap_bytesperpixel)
        {
        image[i]=image[i+1]=image[i+2]=0;
        }
    }

histogram_base::histogram_base ()
    {
    buckets =NULL;

    num_distinct_values =0;
    }

histogram_base::~histogram_base ()
    {
    if (buckets != NULL)
        {delete []buckets; }
    }

int histogram_base::image_ID (int axis)
    {
    return threshold.get_id (axis);
    }

// *** histogram_2D ***

void histogram_2D_plot::images (int image_hor,int image_vert)
    {
    threshold.id[0]=image_hor;
    threshold.id[1]=image_vert;

    calculate();
    }

bool histogram_2D_plot::ready ()
    {
    return (datamanagement.get_image(threshold.id[0]) != NULL && datamanagement.get_image(threshold.id[1]) != NULL );
    }

void histogram_2D_plot::calculate(int foo)
    {
    image_base * vol_v= datamanagement.get_image(threshold.id[1]);
    image_base * vol_h= datamanagement.get_image(threshold.id[0]);

    if (vol_v != NULL && vol_h != NULL)
        {
        imagesdifferinsize= ! vol_v->same_size(vol_h);
        }
    }

void histogram_2D_plot::render_(uchar * image, unsigned int w,unsigned int h)
    {
    image_base * vol_v= datamanagement.get_image(threshold.id[1]);
    image_base * vol_h= datamanagement.get_image(threshold.id[0]);

    if (!imagesdifferinsize)
        {
        float pixfactor_h=w/vol_h->get_max_float(),
            pixfactor_v=h/vol_v->get_max_float();
        //position scaled based on char (0-255)
        //display pixel input 
        //TODO: take min value into account

        unsigned short vol_size[VOLUMEMAXDIMENSION];

        for (unsigned int d=0;d < VOLUMEMAXDIMENSION; d++)
            {
            vol_size[d]=vol_h->get_size_by_dim(d);
            }
        unsigned int pixpos_x, pixpos_y;
        unsigned short voxpos [3];
        const unsigned char pix_intensity = 255/5; //TEST: intensity of single voxel in point-based histogram
        voxpos[2]=0;
        do {
            voxpos[1]=0;
            do {
                voxpos[0]=0;
                do {
                    pixpos_x=h-vol_h->get_number_voxel(voxpos[0],voxpos[1],voxpos[2])*pixfactor_h;
                    pixpos_y=vol_v->get_number_voxel(voxpos[0],voxpos[1],voxpos[2])*pixfactor_v;

                    image[(pixpos_x+w*pixpos_y)*RGBpixmap_bytesperpixel]+=pix_intensity;
                    image[(pixpos_x+w*pixpos_y)*RGBpixmap_bytesperpixel+1]+=pix_intensity;
                    image[(pixpos_x+w*pixpos_y)*RGBpixmap_bytesperpixel+2]+=pix_intensity;

                    } while (++voxpos[0] < vol_size[0]);
                } while (++voxpos[1] < vol_size[1]);
            } while (++voxpos[2] < vol_size[2]);
        }
    }

histogram_2D::~histogram_2D () {
    if (buckets != NULL)
        {delete []highlight_data;}
    }

void histogram_2D::images (int image_hor,int image_vert)
    {
    threshold.id[0]=image_hor;
    threshold.id[1]=image_vert;

    calculate();
    }

bool histogram_2D::ready ()
    {
    return readytorender;
    }

thresholdparvalue histogram_2D::get_threshold (float h_min,float h_max, float v_min, float v_max, int mode)
    {
    threshold.mode=mode;

    image_base * vol_v= datamanagement.get_image(threshold.id[1]);
    image_base * vol_h= datamanagement.get_image(threshold.id[0]);

    if (vol_h == NULL)
        {threshold.id[0]=NOT_FOUND_ID;}

    if (vol_v == NULL)
        {threshold.id[1]=NOT_FOUND_ID;}

    //TODO: account for nonzero image minimum

    if (vol_v != NULL && vol_h != NULL)
        {
        threshold.low [0]=h_min*vol_h->get_max_float();
        threshold.low [1]=v_min*vol_v->get_max_float();

        threshold.high [0]=h_max*vol_h->get_max_float();
        threshold.high [1]=v_max*vol_v->get_max_float();
        }
    else
        {
        threshold.low [0]=threshold.high [0]=threshold.low [1]=threshold.high [1]=0;
        }

    return threshold;
    }

void histogram_2D::calculate(int new_num_buckets)
    {
    if (new_num_buckets !=0 || buckets==NULL)
        {
        if (new_num_buckets !=0)    //change #buckets
            {num_buckets=new_num_buckets;}

        //highlight_data is expected to be created and deleted
        //along with buckets

        if (buckets != NULL)
            {
            delete [] highlight_data;
            highlight_data=NULL;

            delete [] buckets; 
            buckets=NULL;
            }

        buckets=new unsigned long [num_buckets*num_buckets];
        highlight_data=new bool [num_buckets*num_buckets];
        }

    readytorender=false;

    image_base * vol_v= datamanagement.get_image(threshold.id[1]);
    image_base * vol_h= datamanagement.get_image(threshold.id[0]);

    if (vol_v != NULL && vol_h != NULL)
        {
        //true if images have same dimensions
        readytorender=vol_v->same_size(vol_h);

        //reset buckets
        for (unsigned short j = 0; j < num_buckets; j++)
            {
            for (unsigned short i = 0; i < num_buckets; i++)
                {
                buckets[i+j*num_buckets]=0;
                highlight_data[i+j*num_buckets]=false;
                }
            }
        }

    if (readytorender)
        {
        //ready to calculate, actually

        float scalefactor_h=(num_buckets-1)/vol_h->get_max_float(),
            scalefactor_v=(num_buckets-1)/vol_v->get_max_float();
        float value_h, value_v;
        unsigned short bucketpos_x, bucketpos_y;
        unsigned short voxpos [3];
        unsigned short vol_size[VOLUMEMAXDIMENSION];

        for (unsigned int d=0;d < VOLUMEMAXDIMENSION; d++)
            {
            vol_size[d]=vol_h->get_size_by_dim(d);
            }

        bucket_max=0;
        voxpos[2]=0;
        do {
            voxpos[1]=0;
            //in_region_z= voxpos[2] >= region_min[2] && voxpos[2] <= region_max[2];
            do {
                voxpos[0]=0;
                //in_region_y= voxpos[1] >= region_min[1] && voxpos[1] <= region_max[1];
                do {
                    value_h=vol_h->get_number_voxel(voxpos[0],voxpos[1],voxpos[2]);
                    value_v=vol_v->get_number_voxel(voxpos[0],voxpos[1],voxpos[2]);

                    bucketpos_x=value_h*scalefactor_h;
                    bucketpos_y=value_v*scalefactor_v;

                    bucket_max=std::max(buckets[bucketpos_x+bucketpos_y*num_buckets]++,bucket_max);

                    } while (++voxpos[0] < vol_size[0]);
                } while (++voxpos[1] < vol_size[1]);
            } while (++voxpos[2] < vol_size[2]);

        //heuristic for histogram calibration:
        //calculate mean histogram intensity and use twice that as max
        bucket_mean=0;
        for (unsigned short j = 0; j < num_buckets; j++)
            {
            for (unsigned short i = 0; i < num_buckets; i++)
                {
                bucket_mean+=buckets[i+j*num_buckets]/(num_buckets*num_buckets);
                }
            }


        render_max = bucket_mean;
        //render_max=(float)((float)(vol_size[0]*vol_size[1]*vol_size[2])/(num_buckets*num_buckets*4));
        //render_max=(float)((float)(vol_size[0]*vol_size[1]*vol_size[2])/(float)(bucket_max));

#ifdef _DEBUG
        std::cout << "bucket_max: " << bucket_max << std::endl << "render_max: " << render_max << std::endl;
#endif
        }
    }

    void histogram_2D::highlight (regionofinterest * region)
        {
        image_base * vol_v= datamanagement.get_image(threshold.id[1]);
        image_base * vol_h= datamanagement.get_image(threshold.id[0]);

        if (readytorender!=true)
            {calculate();}

        //if images matched, etc, histogram is ready to be highlighted
        if (readytorender==true)
            {
            //reset highlight
            for (long j = 0; j < num_buckets; j++)
                {
                for (long i = 0; i < num_buckets; i++)
                    {
                    highlight_data[i+j*num_buckets]=false;
                    }
                }

            float scalefactor_h=(num_buckets-1)/vol_h->get_max_float(),
                scalefactor_v=(num_buckets-1)/vol_v->get_max_float();
            float value_h, value_v;
            unsigned short bucketpos_x, bucketpos_y;
            unsigned short voxpos [3];

            unsigned short region_min[THRESHOLDMAXCHANNELS], region_max[THRESHOLDMAXCHANNELS]; //integer values of ROI boundaries

            if (region != NULL)
                {
                unsigned short vol_size[VOLUMEMAXDIMENSION];

                for (unsigned int d=0;d < VOLUMEMAXDIMENSION; d++)
                    {
                    vol_size[d]=vol_h->get_size_by_dim(d);
                    }

                for (unsigned int d=0;d < 3; d++)
                    {
                    region_min[d]=std::min(std::max ((float)0,region->start[d]),(float)vol_size[d]-1);
                    region_max[d]=std::max((float)0,std::min ((float)vol_size[d]-1,region->start[d]+region->size[d]));
                    }
                }

            voxpos[2]=region_min[2];
            do {
                voxpos[1]=region_min[1];
                do {
                    voxpos[0]=region_min[0];
                    do {
                        value_h=vol_h->get_number_voxel(voxpos[0],voxpos[1],voxpos[2]);
                        value_v=vol_v->get_number_voxel(voxpos[0],voxpos[1],voxpos[2]);

                        bucketpos_x=value_h*scalefactor_h;
                        bucketpos_y=value_v*scalefactor_v;

                        highlight_data [bucketpos_x+bucketpos_y*num_buckets]=true; 

                        } while (++voxpos[0] < region_max[0]);
                    } while (++voxpos[1] < region_max[1]);
                } while (++voxpos[2] < region_max[2]);
            }
        }

    void histogram_2D::render_(uchar * image, unsigned int w,unsigned int h)
        {
        float slope_x, slope_y;

        slope_x = num_buckets/(float)w;
        slope_y = num_buckets/(float)h;

        unsigned char intensity_value;
        float intensity_scale= 255.0/(float)render_max;

        if (readytorender)
            {
            for (unsigned short y=1; y < h; y++)
                {
                for (unsigned short x=0; x < w; x++)
                    {
                    float hist_pos_x, hist_pos_y;
                    int hist_index;

                    hist_pos_x=slope_x*x;
                    hist_pos_y=slope_y*y;

                    hist_index=((unsigned short) hist_pos_x)+((unsigned short) hist_pos_y)*num_buckets;
                    intensity_value=std::min((float)buckets [hist_index]*(float)intensity_scale, (float)255.0);

                    image[(x+(h-y)*w)*RGBpixmap_bytesperpixel]=(highlight_data [hist_index] ? intensity_value/2 : intensity_value);
                    image[(x+(h-y)*w)*RGBpixmap_bytesperpixel+1]=(highlight_data [hist_index] ? intensity_value/2 + 127 : intensity_value);
                    image[(x+(h-y)*w)*RGBpixmap_bytesperpixel+2]=(highlight_data [hist_index] ? intensity_value/2 + 127 : intensity_value);
                    }
                }
            }
        }
    