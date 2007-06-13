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
#include "image_storage.h"

#include "datamanager.h"
extern datamanager datamanagement;

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

    virtual void highlight (regionofinterest * region) {}     //highlight histogram values for supplied
                                                              //region of interest

    //highlight area indicated by selected threshold
    virtual void render_threshold (unsigned char * image, unsigned int w,unsigned int h) {}; 

    virtual thresholdparvalue get_threshold ()
        {return threshold;}
    virtual bool ready () = 0;                                  //whether histogram has two valid (and compatible)
                                                                //images, and can render
       
    int image_ID (int axis);      //return current image ID, axis 0=h, 1= v and so forth
    };

template <class ELEMTYPE>
class histogram_typed :histogram_base //!features common to histograms of different type pertaining to data type
    {
    image_storage<ELEMTYPE> * images [THRESHOLDMAXCHANNELS];
    ELEMTYPE * i_start,i_end;
    ELEMTYPE max_value, min_value;

    histogram_typed();
    };

template <class ELEMTYPE>
class histogram_1D : public histogram_typed //horizontal 1D graph histogram
    {
    ELEMTYPE hi_low, hi_hi;

    unsigned long render_max;

    int vol_ID;      //ID of the image used

    protected:
        void render_(unsigned char * image, unsigned int w,unsigned int h);
    public:
        histogram_1D (image_storage<ELEMTYPE> * i);
        histogram_1D (ELEMTYPE * start,ELEMTYPE * end);

        ~histogram_1D ();

        void image (int vol);

        void calculate(int number_of_buckets=0);
        thresholdparvalue get_threshold (float h_min,float h_max, float v_min, float v_max, int mode = THRESHOLD_2D_MODE_RECT);
        void highlight (regionofinterest * region);
        bool ready ();
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
        ~histogram_2D ();
        void images (int image_hor,int image_vert);
        void calculate(int number_of_buckets=0);
        thresholdparvalue histogram_2D::get_threshold (float h_min,float h_max, float v_min, float v_max, int mode = THRESHOLD_2D_MODE_RECT);
        void highlight (regionofinterest * region);
        bool ready ();
    };

template <class ELEMTYPE>
histogram_typed<ELEMTYPE >::histogram_typed ():histogram_base()
    {
    i_start  = NULL;
    i_end    = NULL;
    for (int i = 0; i < THRESHOLDMAXCHANNELS; i++)
        { images[i] = NULL; }

    max_value = std::numeric_limits<ELEMTYPE>::max();
    min_value = std::numeric_limits<ELEMTYPE>::min();
    }

// *** histogram_1D ***

template <class ELEMTYPE>
void histogram_1D<ELEMTYPE >::image (int vol)
{
    threshold.id[0]=vol;
    
    calculate();
}

template <class ELEMTYPE>
histogram_1D<ELEMTYPE>::histogram_1D (image_storage<ELEMTYPE> * i):histogram_typed()
    {
    buckets = new unsigned long [max (256,(std::numeric_limits<ELEMTYPE>::max()+std::numeric_limits<ELEMTYPE>::min())/4)];
    }

template <class ELEMTYPE>
histogram_1D<ELEMTYPE>::histogram_1D (ELEMTYPE * start,ELEMTYPE * end ):histogram_typed()
    {
    //these histograms are typically used for stats
    buckets = new unsigned long [256];
    }

template <class ELEMTYPE>
void histogram_1D<ELEMTYPE >::calculate(int new_num_buckets)
{
    if (new_num_buckets !=0 || buckets==NULL)
        {
        if (new_num_buckets !=0)    //change #buckets
            {num_buckets=new_num_buckets;}
        
        hi_low = 0;
        hi_hi =0;
        
        buckets=new unsigned long [num_buckets];
        }
    
    readytorender=false;

    max_value = std::numeric_limits<ELEMTYPE>::min(); //!set initial values to opposite, simplifies the algorithm
    min_value = std::numeric_limits<ELEMTYPE>::max();
    num_distinct_values = 0;
    bucket_max=0;

    //get pointer to source data
    if (threshold.id[0] != 0)
        {images[0] = datamanagement.get_image(threshold.id[0]);}

    if (i_start == NULL)
        {
        i_start = images[0]->begin();
        i_end = images[0]->end();
        }
    
    readytorender=(i_start != NULL);
    
    if (readytorender)
        {
        //reset buckets
        
        for (unsigned short i = 0; i < num_buckets; i++)
            {
            buckets[i]=0;
            }
        
        //ready to calculate, actually
        
        float scalefactor=(num_buckets-1)/vol->get_max_float();
        unsigned short bucketpos;
        
        typename ELEMTYPE * voxpos;
        
        for (voxpos = i_start;voxpos != i_end;++voxpos)
            {
            //calculate distinct value count
            if (buckets[bucketpos] == 0)
                {num_distinct_values++;}

            bucketpos=(*voxpos)*scalefactor;
            //calculate distinct value count
            bucket_max=std::max(buckets[bucketpos]++,bucket_max);

            //calculate min/max
            min_value = max (min_value,*voxpos);
            max_value = min (max_value,*voxpos);
            }
        
        bucket_mean=0;
        for (unsigned short i = 0; i < num_buckets; i++)
            {
            bucket_mean+=buckets[i]/(num_buckets);
            }

        //if # buckets are less than # values, distinct value count will be incorrect
        if (num_buckets < std::numeric_limits<ELEMTYPE>::max()+std::numeric_limits<ELEMTYPE>::min())
            {num_distinct_values = 0}
        }
}

#endif