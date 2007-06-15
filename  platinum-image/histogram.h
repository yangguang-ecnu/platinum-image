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

    virtual void render_threshold (unsigned char * image, unsigned int w,unsigned int h) {}; 

    virtual thresholdparvalue get_threshold ()
        {return threshold;}
    virtual bool ready ();                                  //whether histogram has two valid (and compatible)
                                                                //images, and can render
       
    int image_ID (int axis);      //return current image ID, axis 0=h, 1= v and so forth
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
    };

template <class ELEMTYPE>
class histogram_1D : public histogram_typed<ELEMTYPE> //horizontal 1D graph histogram
    {
    ELEMTYPE hi_low, hi_hi;

    unsigned long render_max;

    int vol_ID;      //ID of the image used

    protected:
        void render_(unsigned char * image, unsigned int w,unsigned int h)
            {}
    public:
        histogram_1D (image_storage<ELEMTYPE> * i);
    histogram_1D (ELEMTYPE * start,ELEMTYPE * end );

            ~histogram_1D () {}

        //void image (int vol);

        void calculate(int number_of_buckets=0);
        //thresholdparvalue get_threshold (float h_min,float h_max, float v_min, float v_max, int mode = THRESHOLD_2D_MODE_RECT);
        virtual bool ready ()
            {return this->readytorender;};   
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
        void highlight (regionofinterest * region); //highlight histogram values for supplied
                                                    //region of interest
        bool ready ();
    };

template <class ELEMTYPE>
histogram_typed<ELEMTYPE>::histogram_typed()
    {
    i_start  = NULL;
    i_end    = NULL;
    for (int i = 0; i < THRESHOLDMAXCHANNELS; i++)
        { images[i] = NULL; }

    max_value = std::numeric_limits<ELEMTYPE>::max();
    min_value = std::numeric_limits<ELEMTYPE>::min();
    }

// *** histogram_1D ***

/*template <class ELEMTYPE>
void histogram_1D<ELEMTYPE >::image (int vol)
{
    this->threshold.id[0]=vol;
    
    calculate();
}*/

template <class ELEMTYPE>
histogram_1D<ELEMTYPE>::histogram_1D (image_storage<ELEMTYPE> * i):histogram_typed<ELEMTYPE>()
    {
    this->images[0] = i;
    
    this->buckets = new unsigned long [std::max (static_cast<unsigned long>(256),static_cast<unsigned long>((std::numeric_limits<ELEMTYPE>::max()+std::numeric_limits<ELEMTYPE>::min())/4.0))];
    
    calculate();
    }

template <class ELEMTYPE>
histogram_1D<ELEMTYPE>::histogram_1D (ELEMTYPE * start,ELEMTYPE * end ):histogram_typed<ELEMTYPE>()
    {
    this->i_start = start;
    this->i_end = end;
    
    //these histograms are typically used for stats
    this->buckets = new unsigned long [256];
    
    calculate();
    }

template <class ELEMTYPE>
void histogram_1D<ELEMTYPE >::calculate(int new_num_buckets)
{
    if (new_num_buckets !=0 || this->buckets==NULL)
        {
        if (new_num_buckets !=0)    //change #buckets
            {this->num_buckets=new_num_buckets;}
        
        hi_low = 0;
        hi_hi =0;
        
        this->buckets=new unsigned long [this->num_buckets];
        }
    
    this->readytorender=false;

    this->max_value = std::numeric_limits<ELEMTYPE>::min(); //!set initial values to opposite, simplifies the algorithm
    this->min_value = std::numeric_limits<ELEMTYPE>::max();
    this->num_distinct_values = 0;
    this->bucket_max=0;

    //get pointer to source data
    //since histogram_typed is instantiated with a particular type, the commented-out image reference acquisition below may not work at all.
    /*if (this->threshold.id[0] != 0)
        {
        image_base * i = datamanagement.get_image(this->threshold.id[0]);
        this->images[0] = dynamic_cast<image_storage<ELEMTYPE>>(i);
        }*/

    if (this->i_start == NULL)
        {
        // retrieve pointers to image data, iterating pointers are generally a bad idea
        //but this way histograms can be made straight from data pointers when
        //there is not yet an image, such as during load of raw files
        this->i_start = this->images[0]->begin().pointer();
        this->i_end = this->images[0]->end().pointer();
        }
    
    this->readytorender=(this->i_start != NULL);
    
    if (this->readytorender)
        {
        //reset buckets
        
        for (unsigned short i = 0; i < this->num_buckets; i++)
            {
            this->buckets[i]=0;
            }
        
        //ready to calculate, actually
        
        float scalefactor=(this->num_buckets-1)/(std::numeric_limits<ELEMTYPE>::max()+std::numeric_limits<ELEMTYPE>::min());
        unsigned short bucketpos;
        
        ELEMTYPE * voxpos;

        for (voxpos = this->i_start;voxpos != this->i_end;++voxpos)
            {
            bucketpos=((*voxpos)-std::numeric_limits<ELEMTYPE>::min())*scalefactor;
            //calculate distinct value count
            this->bucket_max=std::max(this->buckets[bucketpos]++,this->bucket_max);

            //calculate distinct value count
            if (this->buckets[bucketpos] == 0)
                {this->num_distinct_values++;}

            //calculate min/max
            this->min_value = std::max (this->min_value,*voxpos);
            this->max_value = std::min (this->max_value,*voxpos);
            }

        this->bucket_mean=0;
        for (unsigned short i = 0; i < this->num_buckets; i++)
            {
            this->bucket_mean+=this->buckets[i]/(this->num_buckets);
            }

        //if # buckets are less than # values, distinct value count will be incorrect
        if (this->num_buckets < std::numeric_limits<ELEMTYPE>::max()+std::numeric_limits<ELEMTYPE>::min())
            {this->num_distinct_values = 0;}
        }
}

#endif