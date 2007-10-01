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
//	cout<<"void histogram_1D<ELEMTYPE>::resize("<<newNum<<")"<<endl;

    this->num_buckets = newNum;

	if(this->buckets != NULL){	//JK - Prevent memory loss....
//		cout<<"...delete buckets...."<<endl;
		delete []this->buckets;  
		this->buckets = NULL;
	}
    this->buckets = new unsigned long [this->num_buckets];

	//resize() is called from the constructor
    calculate();	
}

template <class ELEMTYPE>
histogram_1D<ELEMTYPE>::histogram_1D (image_storage<ELEMTYPE> * i):histogram_typed<ELEMTYPE>()
{
    this->images[0] = i;
//	cout<<"histogram_1D constructor..."<<endl;
    
//    resize (std::max (static_cast<unsigned long>(256),static_cast<unsigned long>((std::numeric_limits<ELEMTYPE>::max()+std::numeric_limits<ELEMTYPE>::min())/4.0)));

//	if(i->get_max()>i->get_min()){
//		resize(static_cast<unsigned long>(i->get_max()-i->get_min()));//JK-hist Default number of buckets...
//	}else{
		resize(500);//JK-hist Default number of buckets...
//	}
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
//	cout<<"---histogram_1D<ELEMTYPE >::calculate(int new_num_buckets)---"<<endl;

	if (new_num_buckets !=0 || this->buckets==NULL){
        //resize(...) isn't used here because this function is called from resize,
        //however the above condition will be false in that case

		if (new_num_buckets !=0){    //change #buckets
            this->num_buckets=new_num_buckets;
		}

        this->buckets=new unsigned long [this->num_buckets];
	}

//    this->max_value = std::numeric_limits<ELEMTYPE>::min(); //!set initial values to opposite, simplifies the algorithm
//    this->min_value = std::numeric_limits<ELEMTYPE>::max();
    this->num_distinct_values = 0;
    this->bucket_max=0;

    //get pointer to source data

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

//        float scalefactor=(this->num_buckets-1)/(typeMax-typeMin);
//        float scalefactor=(this->num_buckets-1)/(imageMax-imageMin); //JK-hist
        float scalefactor = float(this->max()-this->min())/float(this->num_buckets-1); //JK-hist

//		cout<<"calculate - this->num_buckets="<<this->num_buckets<<endl;
//		cout<<"calculate - max()="<<max()<<endl;
//		cout<<"calculate - min()="<<min()<<endl;
//		cout<<"calculate - scalefactor="<<scalefactor<<endl;

        unsigned short bucketpos;
        ELEMTYPE * voxel;

		for (voxel = this->i_start;voxel != this->i_end;++voxel)
		{
			//bucketpos=((*voxel)-std::numeric_limits<ELEMTYPE>::min())*scalefactor; 
			//bucketpos=((*voxel)-typeMin)*scalefactor; //JK
			//bucketpos=((*voxel)-imageMin)*scalefactor; //JK-hist
			bucketpos=((*voxel)-this->min())/scalefactor; //JK-hist

			//NOT VERY good to write outside allocated memory
			if(bucketpos>=0 && bucketpos<this->num_buckets){	

				//calculate distinct value count
				if (this->buckets[bucketpos] == 0)
				{this->num_distinct_values++;}

				//increment bucket 
				this->buckets[bucketpos]++;

				//update bucket_max
				if (*voxel != 0 &&  *voxel != 1) //! feature: ignore 0 and true to get more sensible display scaling
				{ this->bucket_max=std::max(this->buckets[bucketpos],this->bucket_max); }

				//calculate min/max
				this->min_value = std::min (this->min_value,*voxel);
				this->max_value = std::max (this->max_value,*voxel);
			}else{
//				pt_error::error("histogram_1D<ELEMTYPE >::calculate - bucketpos out of range",pt_error::debug);
//				cout<<" histogram_1D<ELEMTYPE >::calculate - bucketpos out of range... bucketpos="<<bucketpos<<endl;
			}
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
    else
        {
        //no calculation was done, set sensible values
        this->max_value = std::numeric_limits<ELEMTYPE>::max(); 
        this->min_value = std::numeric_limits<ELEMTYPE>::min();
        }
    }

template <class ELEMTYPE>
void histogram_1D<ELEMTYPE>::render (unsigned char * image, unsigned int width,unsigned int height)
{
    //about FLTK pixel types:
    // http://www.fltk.org/articles.php?L466
    
typedef IMGELEMCOMPTYPE RGBpixel[RGBpixmap_bytesperpixel];

    RGBpixel * pixels = reinterpret_cast<RGBpixel *>(image);
 /*   
	//JK2-tmp-fix....
    if (this->buckets != NULL && this->bucket_max > 0)
        {
        for (unsigned int x = 0; x < width; x ++)
            {
            unsigned int b = height-(this->buckets[(x*this->num_buckets)/width] * height)/this->bucket_max;
            unsigned int y;

            for (y = 0; y < b; y ++)
                {
                //background
                pixels [x+y*width][RADDR] = pixels [x+y*width][GADDR] = pixels [x+y*width][BADDR] = 0;
                }
            for (y = b; y < height; y ++)
                {
                //white
                pixels [x+y*width][RADDR] = pixels [x+y*width][GADDR] = pixels [x+y*width][BADDR] = 255;
                }
            }
        }
    else
        { //empty histogram
*/
        for (unsigned short p = 0; p < width * height; p ++)
            {
            pixels [p][RADDR] = pixels [p][GADDR] = pixels [p][BADDR] = 0;
            }
//        }
}

template <class ELEMTYPE>
image_storage<ELEMTYPE> * histogram_1D<ELEMTYPE>::image ()
    {
    return (this->images[0]); //will be NULL if historam uses data pointers
    }


template <class ELEMTYPE>
void histogram_1D<ELEMTYPE>::save_histogram_to_txt_file(std::string filepath, std::string separator)
	{
		cout<<"save_histogram_to_txt_file(std::string file)..."<<endl;

		this->images[0]->min_max_refresh(); //make sure to update image min and max before calling calculate() 
		calculate(); //update histogram statistics...

        float typeMin = std::numeric_limits<ELEMTYPE>::min();
		float typeMax = std::numeric_limits<ELEMTYPE>::max();
//        float scalefactor=(this->num_buckets-1)/(typeMax-typeMin);
        float scalefactor=float((this->max()-this->min()))/float(this->num_buckets);

		ofstream myfile;
		myfile.open(filepath.c_str());
		myfile<<"num_buckets"<<separator<<this->num_buckets<<"\n";
		myfile<<"min_value"<<separator<<this->min()<<"\n";
		myfile<<"max_value"<<separator<<this->max()<<"\n";
		myfile<<"num_distinct_values"<<separator<<this->num_distinct_values<<"\n";
		myfile<<"bucket_mean"<<separator<<this->bucket_mean<<"\n";
		myfile<<"bucket_max"<<separator<<this->bucket_max<<"\n";
		myfile<<"readytorender"<<separator<<this->readytorender<<"\n";
		myfile<<"typeMin"<<separator<<typeMin<<"\n";
		myfile<<"typeMax"<<separator<<typeMax<<"\n";
		myfile<<"scalefactor"<<separator<<scalefactor<<"\n";
		myfile<<"\n";

		//unsigned short bucketpos; //=i   (bucketpos=((*voxel)-typeMin)*scalefactor;) --->
		ELEMTYPE voxel=0;																	// = float(i)/scalefactor + typeMin;

		myfile<<"bucket"<<separator<<"intensity"<<separator<<"bucketvalue\n";
		for(unsigned short i=0; i<this->num_buckets; i++){
//			voxel = float(i)/scalefactor + typeMin;
			voxel = this->min() + float(i)*scalefactor;
			myfile<<i<<separator<<voxel<<separator<<this->buckets[i]<<"\n";
        }

		myfile.close();
	}
