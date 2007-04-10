//////////////////////////////////////////////////////////////////////////
//
//  Classes doing the processing of histograms in 1-3D
//  (separated from user interface)
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

#ifndef __histogram__
#define __histogram__

#include "ptmath.h"
#include "threshold.h"

#include "global.h"

enum ROImode {ROI_RECT, ROI_CIRCLE};

struct regionofinterest
    {
    Vector3D start, size;

    ROImode type;   //determines how these (voxel) coordinates should be interpreted
    };

class histogram_base
    {
    protected:
        unsigned long * buckets ;
        unsigned short num_buckets;     //buckets per dimension, ie. actual #buckets = num_buckets^2
        void clear_pixmap (unsigned char * image, unsigned int w,unsigned int h);
        virtual void render_ (unsigned char * image, unsigned int width,unsigned int height) = 0;
        thresholdparvalue threshold;
    public:
        histogram_base();
        virtual ~histogram_base() {};
        void render(unsigned char * image, unsigned int width,unsigned int height)   //use calculated data to
                                                                                     //render the histogram image
            {
            clear_pixmap (image, width, height);
            render_(image, width, height);
            }
          
        virtual void calculate(int number_of_buckets=0) {}      //do variance, max, choose number of
                                                                //buckets and the like 
                                                                //omitting the num_buckets
                                                                //parameter uses current stored resolution

    virtual void highlight (regionofinterest * region) {}     //highlight histogram values for supplied
                                                              //region of interest

    //highlight area indicated by selected threshold
    virtual void render_threshold (unsigned char * image, unsigned int w,unsigned int h) {}; 

    virtual thresholdparvalue get_threshold ()
        {return threshold;}
    virtual bool ready () = 0;                                  //whether histogram has two valid (and compatible)
                                                                //volumes, and can render
       
    int volume_ID (int axis);      //return current volume ID, axis 0=h, 1= v and so forth
    };

class histogram_2D_plot : public histogram_base  //TEST: histographic plot
    {
    bool volumesdifferinsize;
    unsigned short vol_size[VOLUMEMAXDIMENSION];

    protected:
        void render_(unsigned char * image, unsigned int w,unsigned int h);
    public:
        void volumes (int volume_hor,int volume_vert);
        void calculate(int number_of_buckets=0);

        bool ready ();
    };

class histogram_2D : public histogram_base
    {
    unsigned long bucket_max;
    unsigned long bucket_mean;
    unsigned long render_max;

    bool * highlight_data;      //highlighting of region of interest

    int vol_h_ID,vol_v_ID;      //IDs of the two volumes used
    bool readytorender;

    protected:
        void render_(unsigned char * image, unsigned int w,unsigned int h);
    public:
        ~histogram_2D ();
        void volumes (int volume_hor,int volume_vert);
        void calculate(int number_of_buckets=0);
        thresholdparvalue histogram_2D::get_threshold (float h_min,float h_max, float v_min, float v_max, int mode = THRESHOLD_2D_MODE_RECT);
        void highlight (regionofinterest * region);
        bool ready ();
    };

#endif