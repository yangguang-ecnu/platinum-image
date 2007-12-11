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

//template <class IMAGEDIM>
//    class image_binary;

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
        ELEMTYPE * i_start, *i_end;		//needs modification when > 1D histograms are handled
        ELEMTYPE max_value, min_value;
    public:
        histogram_typed();

        ELEMTYPE min()							//returns histogram (and also image) "intensity min"
            {return min_value;}
        void min (ELEMTYPE new_min)			
            {min_value=new_min;}
        ELEMTYPE max ()							//returns histogram (and also image) "intensity max"
            {return max_value;}
        void max (ELEMTYPE new_max)			
            {max_value=new_max;}
    };


class gaussian{
public:
	gaussian(float amp, float cent, float sig);
	~gaussian(void);
	float amplitude;	
	float center;
	float sigma;		//standard deviation
	float evaluate_at(int x);
};


template <class ELEMTYPE>
class histogram_1D : public histogram_typed<ELEMTYPE> //horizontal 1D graph histogram
    {
    unsigned long render_max;

    protected:
        void render_(unsigned char * image, unsigned int w, unsigned int h)
            {}
    public:
        histogram_1D (image_storage<ELEMTYPE> * i, int num_buckets=-1);
        histogram_1D (ELEMTYPE * start,ELEMTYPE * end );
        histogram_1D (image_storage<ELEMTYPE> *image_data, image_storage<unsigned char> *image_bin_mask, int num_buckets);

        ~histogram_1D ();
        
        void resize (unsigned long); //JK - Should maybe be protected...
		virtual void resize_to_fit_data(); //Make sure alla integer values have a bucket, JK - Should maybe be protected...
        void render(unsigned char * image, unsigned int width, unsigned int height);

	    //void image (int vol);

		void calculate(int number_of_buckets=0);
		void print_histogram_content();
		//thresholdparvalue get_threshold (float h_min,float h_max, float v_min, float v_max, int mode = THRESHOLD_2D_MODE_RECT);
		virtual bool ready ()
			{return this->readytorender;}   
		image_storage<ELEMTYPE> * image ();

		void save_histogram_to_txt_file(std::string filepath, bool reload_hist_from_image=true, gaussian *g=NULL, std::string separator="\t");
		
		ELEMTYPE bucketpos_to_intensity(int bucketpos);
		int intensity_to_bucketpos(ELEMTYPE intensity);

//		void smooth_mean(int nr_of_neighbours=3, int nr_of_times=1);
		void smooth_mean(int nr_of_neighbours, int nr_of_times, int from, int to);

		//------ Fitting of gaussian functions ------
		void fit_gaussian_to_intensity_range(float &amp, float &center, float &sigma, ELEMTYPE from, ELEMTYPE to);
		float find_better_amplitude(gaussian g, int from_bucket, int to_bucket, float factor1=0.8, float factor2=1.2, int nr_steps=10);
		float find_better_center(gaussian g, int from_bucket, int to_bucket, float factor1=0.8, float factor2=1.2, int nr_steps=10);
		float find_better_sigma(gaussian g, int from_bucket, int to_bucket, float factor1=0.8, float factor2=1.2, int nr_steps=10);
		double get_least_square_diff(gaussian g, int from_bucket, int to_bucket);
		int get_max_value_in_bucket_range(int from, int to);
		int get_max_value_in_bucket_range(int from, int to, int &max_val_bucket_pos);
		float get_mean_intensity_in_bucket_range(int from, int to);
		float get_variance_in_bucket_range(int from, int to);
    };


// This class extends histogram_1D<class ELEMTYPE> just to implement one function
// if this function should be specialized in the "_1D" class the whole class would need 
// to be specialized for float...
/*
template <class ELEMTYPE>
class histogram_1Dfloat : public histogram_1D<float>
{
	void resize_to_fit_data(); //Make sure alla integer values have a bucket, JK - Should maybe be protected...

};
*/

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