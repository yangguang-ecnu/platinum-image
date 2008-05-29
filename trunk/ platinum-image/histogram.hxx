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
    calculate(this->num_buckets);	 //the new number of buckets needs to be sent as argument
}


template <class ELEMTYPE>
void histogram_1D<ELEMTYPE>::resize_to_fit_data()
{
	cout<<"histogram_1D<ELEMTYPE>::resize_to_fit_data()"<<endl;

}

/*
template <>
void histogram_1Dfloat<float>::resize_to_fit_data()
{
	cout<<"histogram_1Dfloat<float>::resize_to_fit_data()"<<endl;
}
*/

template <class ELEMTYPE>
histogram_1D<ELEMTYPE>::histogram_1D (image_storage<ELEMTYPE> * i, int num_buckets):histogram_typed<ELEMTYPE>()
{
    this->images[0] = i;

	if(num_buckets>0){
		resize(num_buckets);
	}else{
		resize(780);//Default # of buckets... i->max() cannot be called since it uses histogram_1D
	}
}

template <class ELEMTYPE>
histogram_1D<ELEMTYPE>::histogram_1D (ELEMTYPE * start,ELEMTYPE * end ):histogram_typed<ELEMTYPE>()
{
    this->i_start = start;
    this->i_end = end;
    
    //these histograms are typically used for stats
    resize (256);
}

//TODO - JK - make this work...
template <class ELEMTYPE>
histogram_1D<ELEMTYPE>::histogram_1D (image_storage<ELEMTYPE> *image_data, image_storage<unsigned char> *image_bin_mask, int num_buckets):histogram_typed<ELEMTYPE>()
{
	cout<<"histogram_1D...masked..."<<endl;

	if(num_buckets >=0){
		this->num_buckets = num_buckets;
		if(this->buckets!=NULL){
			delete this->buckets;
		}
		this->buckets=new unsigned long [this->num_buckets];
	}else{
		pt_error::error("histogram_1D - constructor(masked) - num_buckets=strange...",pt_error::debug);
	}
	this->num_distinct_values = 0;
	this->bucket_max=0;

	//get pointer to source data
	if (this->i_start == NULL)
	{
		this->i_start = image_data->begin().pointer();
		this->i_end = image_data->end().pointer();
	}

	this->readytorender=(this->i_start != NULL);

	if (this->readytorender)
	{
		//reset buckets

		for (unsigned short i = 0; i < this->num_buckets; i++)
		{
			this->buckets[i]=0;
		}


		ELEMTYPE *v; //voxel
		ELEMTYPE v_min = std::numeric_limits<ELEMTYPE>::max();
		ELEMTYPE v_max = std::numeric_limits<ELEMTYPE>::min();
		unsigned char *v_bin;

		for( v = this->i_start, v_bin = image_bin_mask->begin().pointer(); (v != this->i_end) && (v_bin != image_bin_mask->end().pointer()); ++v, ++v_bin){
			if(*v_bin>0){
				if(*v > v_max){
					v_max = *v;
				}
				if(*v < v_min){
					v_min = *v;
				}
			}
		}
//		cout<<"masked hist v_min="<<v_min<<endl;
//		cout<<"masked hist v_max="<<v_max<<endl;
		this->min(v_min);
		this->max(v_max);

		unsigned short bucketpos;

		this->num_elements_in_hist=0;

		for( v = this->i_start, v_bin = image_bin_mask->begin().pointer(); (v != this->i_end) && (v_bin != image_bin_mask->end().pointer()); ++v, ++v_bin){
			if(*v_bin>0){
				bucketpos = intensity_to_bucketpos(*v);
				this->num_elements_in_hist++;
				

				//NOT VERY good to write outside allocated memory
				if(bucketpos>=0 && bucketpos<this->num_buckets){	


					//---------------------------------------
					//------------ PRINT --------------------
					//---------------------------------------
//					cout<<endl;
//					for (unsigned short i = 0; i < this->num_buckets; i++){
//						cout<<this->buckets[i]<<" ";
//					}
//					cout<<endl;
					//---------------------------------------
					//------------ PRINT --------------------
					//---------------------------------------

					//calculate distinct value count
					if (this->buckets[bucketpos] == 0)
					{this->num_distinct_values++;}

					//increment bucket 
					this->buckets[bucketpos]++;

					//update bucket_max
					if (*v != 0 &&  *v != 1) //! feature: ignore 0 and true to get more sensible display scaling
					{ this->bucket_max=std::max(this->buckets[bucketpos],this->bucket_max); }

				}else{
					pt_error::error("histogram_1D<ELEMTYPE >::calculate - bucketpos out of range",pt_error::debug);
					cout<<" histogram_1D<ELEMTYPE >::calculate - bucketpos out of range... bucketpos="<<bucketpos<<endl;
				}
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
histogram_1D<ELEMTYPE >::~histogram_1D ()
{}

template <class ELEMTYPE>
void histogram_1D<ELEMTYPE >::calculate(int new_num_buckets)
    {
	//if new_num_buckets == 0 --> keep the current resolution...

	//cout<<"---histogram_1D<ELEMTYPE >::calculate(int new_num_buckets)---"<<endl;

	if (new_num_buckets !=0 || this->buckets==NULL){
        //resize(...) isn't used here because this function is called from resize,
        //however the above condition will be false in that case

		if (new_num_buckets !=0){    //change #buckets
            this->num_buckets=new_num_buckets;
		}

		delete []this->buckets;  
		this->buckets = NULL;
        this->buckets=new unsigned long [this->num_buckets];
	}

    this->num_distinct_values = 0;
    this->bucket_max=0;

    //get pointer to source data

//    if (this->i_start == NULL) //Always re-read the pointers... they might be lost after some ITK-process for example
  //      {
        // retrieve pointers to image data, iterating pointers are generally a bad idea
        //but this way histograms can be made straight from data pointers when
        //there is not yet an image, such as during load of raw files
        this->i_start = this->images[0]->begin().pointer();
        this->i_end = this->images[0]->end().pointer();
    //    }

    this->readytorender=(this->i_start != NULL);

    if (this->readytorender)
        {
        //reset buckets

        for (unsigned short i = 0; i < this->num_buckets; i++)
            {
            this->buckets[i]=0;
            }

        //ready to calculate, actually

//        float typeMax = std::numeric_limits<ELEMTYPE>::max();
  //      float typeMin = std::numeric_limits<ELEMTYPE>::min();


        unsigned short bucketpos;
        ELEMTYPE * voxel;
		this->num_elements_in_hist=0;

		//calculate min/max
	    this->max_value = std::numeric_limits<ELEMTYPE>::min(); //!set initial values to opposite, simplifies the algorithm
		this->min_value = std::numeric_limits<ELEMTYPE>::max();
		for (voxel = this->i_start;voxel != this->i_end;++voxel)
		{
			this->min_value = std::min (this->min_value,*voxel);
			this->max_value = std::max (this->max_value,*voxel);
		}

		for (voxel = this->i_start;voxel != this->i_end;++voxel)
		{
			bucketpos = intensity_to_bucketpos(*voxel);
			this->num_elements_in_hist++;

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

			}else{
				pt_error::error("histogram_1D<ELEMTYPE >::calculate - bucketpos out of range",pt_error::debug);
				cout<<" histogram_1D<ELEMTYPE >::calculate - bucketpos out of range... bucketpos="<<bucketpos<<endl;
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
void histogram_1D<ELEMTYPE>::render (unsigned char * image, unsigned int width, unsigned int height)
{
    //about FLTK pixel types:  // http://www.fltk.org/articles.php?L466
	typedef IMGELEMCOMPTYPE RGBpixel[RGBpixmap_bytesperpixel];
    RGBpixel * pixels = reinterpret_cast<RGBpixel *>(image);
    
    if (this->buckets != NULL && this->bucket_max > 0){
        for (unsigned int x = 0; x < width; x ++){
            unsigned int b = height-(this->buckets[(x*this->num_buckets)/width] * height)/this->bucket_max;
            unsigned int y;
            for (y = 0; y < b; y ++){                //background
                pixels [x+y*width][RADDR] = pixels [x+y*width][GADDR] = pixels [x+y*width][BADDR] = 0;
                }
            for (y = b; y < height; y ++){            //white
                pixels [x+y*width][RADDR] = pixels [x+y*width][GADDR] = pixels [x+y*width][BADDR] = 255;
                }
		}
	}else{ //empty histogram
        for (unsigned short p = 0; p < width * height; p ++){
            pixels [p][RADDR] = pixels [p][GADDR] = pixels [p][BADDR] = 0;
		}
	}
}

template <class ELEMTYPE>
image_storage<ELEMTYPE> * histogram_1D<ELEMTYPE>::image ()
    {
    return (this->images[0]); //will be NULL if historam uses data pointers
    }

template <class ELEMTYPE>
void histogram_1D<ELEMTYPE>::save_histogram_to_txt_file(std::string filepath, std::string separator)	{
	save_histogram_to_txt_file(filepath, false, NULL, separator);
	}

template <class ELEMTYPE>
void histogram_1D<ELEMTYPE>::save_histogram_to_txt_file(std::string filepath, bool reload_hist_from_image, gaussian *g, std::string separator)	{
		cout<<"save_histogram_to_txt_file(std::string file)..."<<endl;

		if(reload_hist_from_image){
			this->images[0]->min_max_refresh(); //make sure to update image min and max before calling calculate() 
			calculate(); //update histogram statistics...
		}

        float typeMin = std::numeric_limits<ELEMTYPE>::min();
		float typeMax = std::numeric_limits<ELEMTYPE>::max();

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
		myfile<<"\n";

		if(g!=NULL){
			myfile<<"g->amplitude="<<separator<<g->amplitude<<"\n";
			myfile<<"g->center="<<separator<<g->center<<"\n";
			myfile<<"g->sigma="<<separator<<g->sigma<<"\n";
			cout<<"g->evaluate_at(0)="<<g->evaluate_at(0)<<endl;
			cout<<"g->evaluate_at(0.1)="<<g->evaluate_at(0.1)<<endl;
		}

		myfile<<"bucket"<<separator<<"intensity"<<separator<<"bucketvalue\n";
		for(unsigned short i=0; i<this->num_buckets; i++){
			myfile<<i<<separator<<bucketpos_to_intensity(i)<<separator<<this->buckets[i];
			if(g!=NULL){
				myfile<<separator<<g->evaluate_at(bucketpos_to_intensity(i));
			}
			myfile<<"\n";
        }

		myfile.close();
	}


template <class ELEMTYPE>
float histogram_1D<ELEMTYPE>::get_scalefactor(){
	return float(this->max()-this->min())*1.000001/float(this->num_buckets);
	//return float(this->max()-this->min())/float(this->num_buckets-1);
}

template <class ELEMTYPE>
ELEMTYPE histogram_1D<ELEMTYPE>::bucketpos_to_intensity(int bucketpos){
	return this->min() + float(bucketpos)*get_scalefactor();
}

template <class ELEMTYPE>
int histogram_1D<ELEMTYPE>::intensity_to_bucketpos(ELEMTYPE intensity){
	return (intensity - this->min())/get_scalefactor();
}

/*
template <class ELEMTYPE>
void histogram_1D<ELEMTYPE>::smooth_mean(int nr_of_neighbours, int nr_of_times){
	smooth_mean(nr_of_neighbours, nr_of_times, 0, int to);
}
*/

template <class ELEMTYPE>
void histogram_1D<ELEMTYPE>::smooth_mean(int nr_of_neighbours, int nr_of_times, int from_bucket, int to_bucket){
	cout<<"smooth....("<<nr_of_neighbours<<","<<nr_of_times<<")...";

	to_bucket = std::min((unsigned short)to_bucket, this->num_buckets);

	//this variable is filled in the smoothing loop to not affect the smooth input values...
	//the "res-results" are then copied to the buckets[] variable for each iteration...	

	unsigned long * res = new unsigned long[this->num_buckets];
	for(int i=0;i<this->num_buckets;i++){
		res[i] = this->buckets[i];
	}

	if(nr_of_neighbours>=3 && to_bucket > (from_bucket + 3)){
		ELEMTYPE tmp=0;
		int half = int(float(nr_of_neighbours)/2.0);
		int start_filt	= from_bucket + half;
		int end_filt	= to_bucket - (nr_of_neighbours-half);
		int start_sum	= - half;
		int end_sum		= nr_of_neighbours - half;
		cout<<" half="<<half;
		cout<<" start_filt="<<start_filt;
		cout<<" end_filt="<<end_filt;
		cout<<" start_sum="<<start_sum;
		cout<<" end_sum="<<end_sum<<endl;
		cout<<"this->num_buckets="<<this->num_buckets<<endl;

		for(int nr=1;nr<=nr_of_times;nr++){
			for(int i=start_filt;i<end_filt;i++){
				tmp=0;
				for(int j=start_sum;j<end_sum;j++){
					tmp += this->buckets[i+j];
				}
//				cout<<"i="<<i<<" res1="<<res[i];
				res[i] = float(tmp)/float(nr_of_neighbours);
//				cout<<" res2="<<res[i]<<endl;
//				this->buckets[i] = int(float(tmp)/float(nr_of_neighbours));
			}
			for(int i=0;i<this->num_buckets;i++){
//				cout<<"buckets[i]="<<buckets[i]<<" res[i]="<<res[i]<<endl;
				this->buckets[i] = res[i];
			}
		}
		//the start and the end have to be smoothed with a smaller filter....

		if(nr_of_neighbours>5){
//			smooth(5, nr_of_times, from, start_filt+5);
//			smooth(5, nr_of_times, end_filt, to);
		}
	}

	delete res;
}

template <class ELEMTYPE>
ELEMTYPE histogram_1D<ELEMTYPE>::get_intensity_at_histogram_lower_percentile(float percentile, bool ignore_zero_intensity)
{
    cout<<"get_intensity_at_histogram_lower_percentile("<<percentile<<")"<<endl;

	//if histogram comes from masked region... following line wont work....
//	float num_elem_limit = float(this->images[0]->get_num_elements())*percentile;

	unsigned short the_zero_bucket = this->intensity_to_bucketpos(0);
	float num_elem_limit;

	if (ignore_zero_intensity) {num_elem_limit = float(this->num_elements_in_hist-this->buckets[the_zero_bucket])*percentile;}
	else {num_elem_limit = float(this->num_elements_in_hist)*percentile;}

	float sum_elements=0;
	
	

	for (unsigned short i = 0; i < this->num_buckets; i++)
	{
		if (!ignore_zero_intensity || i!=the_zero_bucket) {
			sum_elements += this->buckets[i];
			if(sum_elements>=num_elem_limit){
				return bucketpos_to_intensity(i);
			}
		}
	}

	return bucketpos_to_intensity(this->num_buckets-1);
}

template <class ELEMTYPE>
ELEMTYPE histogram_1D<ELEMTYPE>::get_intensity_at_included_num_pix_from_lower_int(ELEMTYPE lower_int, float num_pix)
{
    cout<<"get_intensity_at_included_num_pix_from_lower_int("<<lower_int<<","<<num_pix<<")"<<endl;

	unsigned short start_bucket = this->intensity_to_bucketpos(lower_int);
    cout<<"start_bucket="<<start_bucket<<endl;
	float sum_elements=0;

	for (unsigned short i = start_bucket; i < this->num_buckets; i++){
		sum_elements += this->buckets[i];
		if(sum_elements>=num_pix){
			return bucketpos_to_intensity(i);
		}
	}

	return bucketpos_to_intensity(this->num_buckets-1);
}

		 
template <class ELEMTYPE>
bool histogram_1D<ELEMTYPE>::is_histogram_bimodal(int start_bucket, int end_bucket, int min_mode_sep, double valley_factor, ELEMTYPE peak_min, int speedup){
	cout<<"isHistogramBimodal..."<<endl;
	this->smooth_mean(5,1,start_bucket,end_bucket);

	bool ret = false;
	ELEMTYPE peak_1 = 0;
	ELEMTYPE peak_2 = 0;
	ELEMTYPE min_peak = 0;
	ELEMTYPE valley = 0;
	for(int i=start_bucket+1; i<=end_bucket-min_mode_sep; i = i+speedup){
		for(int j=i+min_mode_sep; j<end_bucket; j = j+speedup){
			peak_1 = this->get_max_value_in_bucket_range(start_bucket,i);
			peak_2 = this->get_max_value_in_bucket_range(j,end_bucket);
			valley = this->get_min_value_in_bucket_range(i,j);

			min_peak = std::min(peak_1,peak_2);

			if(min_peak > peak_min && valley < valley_factor*double(min_peak)){
				cout<<"start_bucket\ti\tsep\tj\tend_bucket\tpeak_1\tpeak_2\tvalley"<<endl;
				cout<<start_bucket<<"\t"<<i<<"\t"<<min_mode_sep<<"\t"<<j<<"\t";
				cout<<end_bucket<<"\t"<<peak_1<<"\t"<<peak_2<<"\t"<<valley<<endl;
				cout<<"***TRUE***"<<endl;
				return true;
			}

		}
	}
	cout<<"***FALSE***"<<endl;
	return false;
}



template <class ELEMTYPE>
void histogram_1D<ELEMTYPE>::fit_gaussian_to_intensity_range(float &amp, float &center, float &sigma, ELEMTYPE from, ELEMTYPE to, bool print_info)
{
    cout<<"fit_gaussian_to_intensity_range..."<<endl;
	cout<<"this->max()="<<this->max()<<endl;
	cout<<"this->min()="<<this->min()<<endl;
	cout<<"this->num_buckets()="<<this->num_buckets<<endl;

	int from_bucket = intensity_to_bucketpos(from);
	int to_bucket = intensity_to_bucketpos(to);
	cout<<"from_bucket="<<from_bucket<<endl;
	cout<<"to_bucket="<<to_bucket<<endl;

	gaussian g(amp,center,sigma);
	g.amplitude = float(get_max_value_in_bucket_range(from_bucket,to_bucket));
	g.center = get_mean_intensity_in_bucket_range(from_bucket,to_bucket);
	g.sigma = sqrt(get_variance_in_bucket_range(from_bucket,to_bucket));
	int dyn_from_bucket = std::max(from_bucket, intensity_to_bucketpos(g.center-1.5*g.sigma));
	int dyn_to_bucket = std::min(to_bucket, intensity_to_bucketpos(g.center+1.5*g.sigma));

	if(print_info)
	{
	cout<<"**************"<<endl;
	cout<<"**************"<<endl;
	cout<<"from_bucket="<<from_bucket<<endl;
	cout<<"to_bucket="<<to_bucket<<endl;
	cout<<"amp="<<g.amplitude<<endl;
	cout<<"center="<<g.center<<endl;
	cout<<"sigma="<<g.sigma<<endl;
	cout<<"dyn_from_bucket="<<dyn_from_bucket<<endl;
	cout<<"dyn_to_bucket="<<dyn_to_bucket<<endl;
	}

	float tmp;
	int nr_iterations=5;
	for(int i=0;i<nr_iterations;i++){
		g.amplitude = find_better_amplitude(g,dyn_from_bucket,dyn_to_bucket,0.8,1.2,10+i);
		g.center = find_better_center(g,dyn_from_bucket,dyn_to_bucket,0.8,1.2,10+i);
		g.sigma = find_better_sigma(g,dyn_from_bucket,dyn_to_bucket,0.8,1.2,10+i);
		
		//consider shape of gaussian bu dont go outside given limits...
		tmp = std::max(float(from),float(g.center-1.5*g.sigma)); //limits intensity range...
		dyn_from_bucket = std::max(from_bucket, intensity_to_bucketpos(tmp)); //limits bucket range...
		dyn_to_bucket = std::min(to_bucket, intensity_to_bucketpos(g.center+1.5*g.sigma));

	if(print_info)
	{
		cout<<"**************"<<endl;
		cout<<"iter="<<i<<"/"<<nr_iterations<<endl;
		cout<<"g.amplitude="<<g.amplitude<<endl;
		cout<<"g.center="<<g.center<<endl;
		cout<<"g.sigma="<<g.sigma<<endl;
		cout<<"dyn_from_bucket="<<dyn_from_bucket<<endl;
		cout<<"dyn_to_bucket="<<dyn_to_bucket<<endl;
	}
	}

	//set the reference variables again...
	amp = g.amplitude;
	center = g.center;
	sigma = g.sigma;
}

template <class ELEMTYPE>
float histogram_1D<ELEMTYPE>::find_better_amplitude(gaussian g, int from_bucket, int to_bucket, float factor1, float factor2, int nr_steps)
{
	float start = factor1*g.amplitude;
	float end = factor2*g.amplitude;
	float step = (end-start)/float(nr_steps);
	float best = 0;
	double error_sum = 0;
	double error_min = 100000000000000000.0;

	for(g.amplitude=start; g.amplitude<=end; g.amplitude+=step){
//		error_sum = get_least_square_diff(g, from_bucket, to_bucket);
		error_sum = get_least_square_diff_ignore_zeros(g, from_bucket, to_bucket);
		if(error_sum<error_min){
			error_min = error_sum;
			best = g.amplitude;
		}
	}
	return best;
}

template <class ELEMTYPE>
float histogram_1D<ELEMTYPE>::find_better_center(gaussian g, int from_bucket, int to_bucket, float factor1, float factor2, int nr_steps)
{
	float start = factor1*g.center;
	float end = factor2*g.center;
	float step = (end-start)/float(nr_steps);
	float best = 0;
	double error_sum = 0;
	double error_min = 100000000000000000.0;

	for(g.center=start; g.center<=end; g.center+=step){
//		error_sum = get_least_square_diff(g, from_bucket, to_bucket);
		error_sum = get_least_square_diff_ignore_zeros(g, from_bucket, to_bucket);
		if(error_sum<error_min){
			error_min = error_sum;
			best = g.center;
		}
	}
	return best;
}

template <class ELEMTYPE>
float histogram_1D<ELEMTYPE>::find_better_sigma(gaussian g, int from_bucket, int to_bucket, float factor1, float factor2, int nr_steps)
{
	float start = factor1*g.sigma;
	float end = factor2*g.sigma;
	float step = (end-start)/float(nr_steps);
	float best = 0;
	double error_sum = 0;
	double error_min = 100000000000000000.0;

	for(g.sigma=start; g.sigma<=end; g.sigma+=step){
//		error_sum = get_least_square_diff(g, from_bucket, to_bucket);
		error_sum = get_least_square_diff_ignore_zeros(g, from_bucket, to_bucket);
		if(error_sum<error_min){
			error_min = error_sum;
			best = g.sigma;
		}
	}
	return best;
}

template <class ELEMTYPE>
double histogram_1D<ELEMTYPE>::get_least_square_diff(gaussian g, int from_bucket, int to_bucket){
//	cout<<"getError...."<<endl;
	double error = 0;
	float val=0;
	for(int j=from_bucket;j<=to_bucket;j++){
		//it is very important to use the intensity, and not the bucket position...
		val = g.evaluate_at(bucketpos_to_intensity(j));	
		if(j<0){
			error += pow(val - 0, 2);
		}else{
			error += pow(val - float(this->buckets[j]), 2);
		}
	}
	return error;
}

template <class ELEMTYPE>
double histogram_1D<ELEMTYPE>::get_least_square_diff_ignore_zeros(gaussian g, int from_bucket, int to_bucket){
//	cout<<"getError...."<<endl;
	double error = 0;
	float val=0;
	for(int j=from_bucket;j<=to_bucket;j++){
		//it is very important to use the intensity, and not the bucket position...

		val = g.evaluate_at(bucketpos_to_intensity(j));	
		if(this->buckets[j]>0){
			if(j<0){
				error += pow(val - 0, 2);
			}else{
				error += pow(val - float(this->buckets[j]), 2);
			}
		}
	}
	return error;
}


template <class ELEMTYPE>
ELEMTYPE histogram_1D<ELEMTYPE>::get_min_value_in_bucket_range(int from, int to)
{
	int pos=0;
	return get_min_value_in_bucket_range(from, to, pos);
}

template <class ELEMTYPE>
ELEMTYPE histogram_1D<ELEMTYPE>::get_min_value_in_bucket_range(int from, int to, int &min_val_bucket_pos)
{
	ELEMTYPE min_value = std::numeric_limits<ELEMTYPE>::max();

	for(int i=from; i<=to; i++){
		if(this->buckets[i]<max_value){
			min_val_bucket_pos = i;
			min_value = this->buckets[i];
		}
	}

	return min_value;
}



template <class ELEMTYPE>
ELEMTYPE histogram_1D<ELEMTYPE>::get_max_value_in_bucket_range(int from, int to)
{
	int pos=0;
	return get_max_value_in_bucket_range(from, to, pos);
}

template <class ELEMTYPE>
ELEMTYPE histogram_1D<ELEMTYPE>::get_max_value_in_bucket_range(int from, int to, int &max_val_bucket_pos)
{
	ELEMTYPE max_value = std::numeric_limits<ELEMTYPE>::min();

	for(int i=from; i<=to; i++){
		if(this->buckets[i]>max_value){
			max_val_bucket_pos = i;
			max_value = this->buckets[i];
		}
	}

	return max_value;
}

template <class ELEMTYPE>
float histogram_1D<ELEMTYPE>::get_mean_intensity_in_bucket_range(int from, int to)
{
	float intensity_sum=0;
	float num_values=0;
	for(int i=from; i<=to; i++){
		//this number of pixels with this intensity
		intensity_sum += this->buckets[i]*bucketpos_to_intensity(i);
		num_values += this->buckets[i];
	}
	return intensity_sum/num_values;
}

template <class ELEMTYPE>
float histogram_1D<ELEMTYPE>::get_variance_in_bucket_range(int from, int to)
{
	float mean = get_mean_intensity_in_bucket_range(from,to);
	float sum = 0;
	float num_values = 0;
	float intensity=0;
	for(int i=from; i<=to; i++){
		intensity = bucketpos_to_intensity(i);
		//this number of pixels with this (mean-intensity-difference)^2...
		sum += float(this->buckets[i])*(mean-intensity)*(mean-intensity);
		num_values += this->buckets[i];
	}
	return sum/num_values;
}


template <class ELEMTYPE>
float histogram_1D<ELEMTYPE>::get_variance_in_intensity_range(ELEMTYPE from, ELEMTYPE to)
{
	return get_variance_in_bucket_range(intensity_to_bucketpos(from),intensity_to_bucketpos(to));
}

template <class ELEMTYPE>
int histogram_1D<ELEMTYPE>::get_bucket_pos_with_largest_value_in_bucket_range(int from, int to)
{
	int pos=0;
	get_max_value_in_bucket_range(from, to, pos);
	return pos;
}


template <class ELEMTYPE>
int histogram_1D<ELEMTYPE>::get_bucket_pos_with_largest_value_in_intensity_range(ELEMTYPE from, ELEMTYPE to)
{
	int pos=0;
	get_max_value_in_bucket_range(intensity_to_bucketpos(from), intensity_to_bucketpos(to), pos);
	return pos;
}




/*
template<class ELEMTYPE, int IMAGEDIM>
histogram_2Dimage<ELEMTYPE,IMAGEDIM>::histogram_2Dimage<ELEMTYPE,IMAGEDIM>(image_scalar<ELEMTYPE,IMAGEDIM>* i1, image_scalar<ELEMTYPE,IMAGEDIM>* i2)
{
	this->im1 = new image_scalar<ELEMTYPE,IMAGEDIM>(i1,1);
	this->im2 = new image_scalar<ELEMTYPE,IMAGEDIM>(i2,1);
}
*/