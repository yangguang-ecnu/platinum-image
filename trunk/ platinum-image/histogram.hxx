//$Id$

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

// *** histogram_typed ***

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
void histogram_1D<ELEMTYPE>::resize (unsigned long newNum)
{
    this->num_buckets = newNum;
    
    this->buckets = new unsigned long [this->num_buckets];
    
    calculate();
}

template <class ELEMTYPE>
histogram_1D<ELEMTYPE>::histogram_1D (image_storage<ELEMTYPE> * i):histogram_typed<ELEMTYPE>()
{
    this->images[0] = i;
    
    resize (std::max (static_cast<unsigned long>(256),static_cast<unsigned long>((std::numeric_limits<ELEMTYPE>::max()+std::numeric_limits<ELEMTYPE>::min())/4.0)));
}

template <class ELEMTYPE>
histogram_1D<ELEMTYPE>::histogram_1D (ELEMTYPE * start,ELEMTYPE * end ):histogram_typed<ELEMTYPE>()
{
    this->i_start = start;
    this->i_end = end;
    
    //these histograms are typically used for stats
    resize (256);
}

template <class ELEMTYPE>
void histogram_1D<ELEMTYPE >::calculate(int new_num_buckets)
{
    if (new_num_buckets !=0 || this->buckets==NULL)
        //resize(...) isn't used here because this function is called from resize,
        //however the above condition will be false in that case
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
        
        float typeMax = std::numeric_limits<ELEMTYPE>::max();
        float typeMin = std::numeric_limits<ELEMTYPE>::min();
        
        
        float scalefactor=(this->num_buckets-1)/(typeMax-typeMin);
        unsigned short bucketpos;
        
        ELEMTYPE * voxpos;
        
        for (voxpos = this->i_start;voxpos != this->i_end;++voxpos)
            {
            bucketpos=((*voxpos)-std::numeric_limits<ELEMTYPE>::min())*scalefactor;
            
            //calculate distinct value count
            if (this->buckets[bucketpos] == 0)
                {this->num_distinct_values++;}
            
            //increment bucket and update bucket_max
            this->bucket_max=std::max(this->buckets[bucketpos]++,this->bucket_max);
            
            //calculate min/max
            this->min_value = std::min (this->min_value,*voxpos);
            this->max_value = std::max (this->max_value,*voxpos);
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

template <class ELEMTYPE>
void histogram_1D<ELEMTYPE>::render (unsigned char * image, unsigned int width,unsigned int height)
{
    //about FLTK pixel types:
    // http://www.fltk.org/articles.php?L466
    
    typedef int RGBpixel[RGBpixmap_bytesperpixel];
    
    if (this->buckets != NULL)
        {
        RGBpixel * pixels = reinterpret_cast<RGBpixel *>(image);
        
        for (unsigned int x = 0; x < width; x ++)
            {
            unsigned int b = (this->buckets[(x*this->num_buckets)/width] * height)/this->bucket_max;
            unsigned int y;
            
            for (y = 0; y < b; y ++)
                {
                pixels [x+y*width][RADDR] = 255;
                pixels [x+y*width][GADDR] = 255;
                pixels [x+y*width][BADDR] = 255;
                }
            for (y = b; y < height; y ++)
                {
                pixels [x+y*width][RADDR] = 0;
                pixels [x+y*width][GADDR] = 0;
                pixels [x+y*width][BADDR] = 0;
                }
            }
        } 
}
