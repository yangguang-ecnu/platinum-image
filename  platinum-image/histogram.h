//////////////////////////////////////////////////////////////////////////
//
//  Classes doing the processing of histograms in 1-3D $Revision$
//  (separated from user interface)
//
//  $LastChangedBy$
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

template <class ELEMTYPE>
class image_storage;

#include "global.h"

struct regionofinterest
    {
    Vector3D start, size;

    ROImode type;   //determines how these (voxel) coordinates should be interpreted
    };

class histogram_base
    {
    protected:
        unsigned long * buckets ;
        unsigned long num_distinct_values;
        
        unsigned short num_buckets;     //buckets per dimension, ie. actual #buckets = num_buckets^2 for 2D histogram
        unsigned long bucket_max;
        unsigned long bucket_mean;

        bool readytorender;
        
        void clear_pixmap (unsigned char * image, unsigned int w,unsigned int h);
        virtual void render_ (unsigned char * image, unsigned int width,unsigned int height) = 0;
        thresholdparvalue threshold;

        histogram_base();
    public:
        virtual ~histogram_base();
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

    virtual void render_threshold (unsigned char * image, unsigned int w,unsigned int h) {}; 

    virtual thresholdparvalue get_threshold ()
        {return threshold;}
    virtual bool ready ();                                  //whether histogram has two valid (and compatible)
                                                                //images, and can render
       
    int image_ID (int axis);      //return current image ID, axis 0=h, 1= v and so forth

    unsigned long num_values ()
        {return num_distinct_values;}
    };

template <class ELEMTYPE>
class histogram_typed : public histogram_base //!features common to histograms of different type pertaining to data type
    {
    protected:
        image_storage<ELEMTYPE> * images [THRESHOLDMAXCHANNELS];
        ELEMTYPE * i_start, *i_end;
        ELEMTYPE max_value, min_value;
    public:
        histogram_typed();

        ELEMTYPE min ()
            {return min_value;}
        ELEMTYPE max ()
            {return max_value;}
    };

template <class ELEMTYPE>
class histogram_1D : public histogram_typed<ELEMTYPE> //horizontal 1D graph histogram
    {
    ELEMTYPE hi_low, hi_hi;

    unsigned long render_max;

    protected:
        void render_(unsigned char * image, unsigned int w,unsigned int h)
            {}
        void resize (unsigned long);
    public:
        histogram_1D (image_storage<ELEMTYPE> * i);
        histogram_1D (ELEMTYPE * start,ELEMTYPE * end );

        ~histogram_1D () {}
        
        void render (unsigned char * image, unsigned int width,unsigned int height);

    //void image (int vol);

    void calculate(int number_of_buckets=0);
    //thresholdparvalue get_threshold (float h_min,float h_max, float v_min, float v_max, int mode = THRESHOLD_2D_MODE_RECT);
    virtual bool ready ()
        {return this->readytorender;}   
    };


class histogram_2D_plot : public histogram_base  //TEST: histographic plot
    {
    bool imagesdifferinsize;
    unsigned short vol_size[VOLUMEMAXDIMENSION];

    protected:
        void render_(unsigned char * image, unsigned int w,unsigned int h);
    public:
        void images (int image_hor,int image_vert);
        void calculate(int number_of_buckets=0);

        bool ready ();
    };

class histogram_2D : public histogram_base
    {
    unsigned long render_max;

    bool * highlight_data;      //highlighting of region of interest

    int vol_h_ID,vol_v_ID;      //IDs of the two images used

    protected:
        void render_(unsigned char * image, unsigned int w,unsigned int h);
    public:
        histogram_2D ();
        ~histogram_2D ();
        void images (int image_hor,int image_vert);
        void calculate(int number_of_buckets=0);
        thresholdparvalue histogram_2D::get_threshold (float h_min,float h_max, float v_min, float v_max, int mode = THRESHOLD_2D_MODE_RECT);
        void highlight (regionofinterest * region); //highlight histogram values for supplied
                                                    //region of interest
        bool ready ();
    };

#endif