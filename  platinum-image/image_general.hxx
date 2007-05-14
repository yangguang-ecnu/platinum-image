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

#ifndef __image_general_hxx__
#define __image_general_hxx__

#include "rawimporter.h"
#include "image_scalar.h"

#include "image_binary.h"
#include "fileutils.h"

#include <iostream>
#include <fstream>

#include "itkImageRegionIterator.h"	//most basic, fastest pixel order...
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkDICOMImageIO2.h"
#include "itkDICOMSeriesFileNames.h"
#include "itkVTKImageIO.h"
#include "itkImageSliceIteratorWithIndex.h"
#include "itkStatisticsImageFilter.h"

#define RENDER_ORTHOGONALLY_ONLY

using namespace std;

//GCC does not support templated typedefs
//we bow our heads in appreciation and define them with macros instead :P
#define theImageType itk::Image<ELEMTYPE,IMAGEDIM>
#define theImagePointer theImageType::Pointer
#define theIteratorType itk::ImageRegionIterator<theImageType >
#define theSeriesReaderType itk::ImageSeriesReader<theImageType >
#define theReaderType itk::ImageFileReader<theImageType >
#define	theWriterType itk::ImageFileWriter<theImageType >
#define theSizeType theImageType::RegionType::SizeType
#define theStatsFilterType itk::StatisticsImageFilter<theImageType >
#define theStatsFilterPointerType theStatsFilterType::Pointer

template <class ELEMTYPE, int IMAGEDIM>
template <class sourceType>
void image_general<ELEMTYPE, IMAGEDIM>::set_parameters (image_general<sourceType, IMAGEDIM> * sourceImage)
    {
    //this function only works when image dimensionality matches

    short size [IMAGEDIM];

    for (int d=0; d < IMAGEDIM; d++)
        { size[d]=sourceImage->get_size_by_dim(d); }

    initialize_dataset(size[0],size[1],size[2]);

    ITKimportfilter=NULL;
    ITKimportimage=NULL;

    this->maxvalue        = sourceImage->get_max();
    this->minvalue        = sourceImage->get_min();

    this->voxel_resize    = sourceImage->get_voxel_resize();

    this->unit_center_     = sourceImage->unit_center();
    this->unit_to_voxel_   = sourceImage->unit_to_voxel();

    // *ID, from_file, imagename and widget are assigned in image_base constructor
    }

template <class ELEMTYPE, int IMAGEDIM>
image_base * image_general<ELEMTYPE, IMAGEDIM>::alike (imageDataType unit)
    {
    //this alike is called from the template-agnostic image_base class
    image_base * new_image = NULL;

    switch (unit)
        {
        case VOLDATA_BOOL:      new_image = new image_binary<IMAGEDIM>(this,false);             
            break;
        case VOLDATA_CHAR:      new_image = new image_integer<char, IMAGEDIM > (this,false);              break;
        case VOLDATA_UCHAR:     new_image = new image_integer<unsigned char, IMAGEDIM > (this,false);
            break;
        case VOLDATA_SHORT:     new_image = new image_integer<short, IMAGEDIM >(this,false);              break;
        case VOLDATA_USHORT:    new_image = new image_integer<unsigned short, IMAGEDIM >(this,false);     break;
        case VOLDATA_DOUBLE:    new_image = new image_scalar<double, IMAGEDIM >(this,false);             break;
        default :
            {
#ifdef _DEBUG
                cout << "image_general::alike: attempting to create alike as unsupported data type (" << unit << ")" << endl;
#endif
            }
            break;
        }

    return new_image;
    }

//template <class ELEMTYPE, int IMAGEDIM>
//    template <class newType> 
//image_general<newType> * image_general<ELEMTYPE, IMAGEDIM>::alike ()
//    {
//    image_general<newType, IMAGEDIM> * new_image = NULL;
//
//    new_image = new image_general<newType, IMAGEDIM>();
//
//    new_image->initialize_dataset(datasize[0],datasize[1],datasize[2]);
//
//    ITKimportfilter=NULL;
//
//    new_image->origin          = origin;
//    new_image->direction       = direction;
//    new_image->voxel_resize    = voxel_resize;
//
//    new_image->unit_center_     = unit_center;
//    new_image->unit_to_voxel   = unit_to_voxel_;
//
//    return new_image;
//    }

template <class ELEMTYPE, int IMAGEDIM>
    template <class inType>
void image_general<ELEMTYPE, IMAGEDIM>::copy_image (image_general<inType, IMAGEDIM> * in)
    {
    if ( same_size (in))
        {
        /*for (unsigned long c= 0; c < static_cast<unsigned short>( in->get_size_by_dim(0)*in->get_size_by_dim(1)*in-> get_size_by_dim(2)); c++)
            {
            set_voxel (c,in->get_voxel(c));
            }*/
        
        typename image_general<inType, IMAGEDIM>::iterator i = in->begin();
        typename image_storage<ELEMTYPE >::iterator o = this->begin();
        
        while (i != in->end() && o != this->end())
            {
            *o = *i;
            
            ++i; ++o;
            }
        }
    else
        {     
        //total failure, TODO: throw exception
#ifdef _DEBUG
        cout << "Copying image data: image sizes don't match" << endl;
#endif
        }
    }

template <class ELEMTYPE, int IMAGEDIM>
    image_general<ELEMTYPE, IMAGEDIM>::image_general():image_storage<ELEMTYPE>()
    {   
    voxel_resize.SetIdentity();
    }

template <class ELEMTYPE, int IMAGEDIM>
    template<class SOURCETYPE>
    image_general<ELEMTYPE, IMAGEDIM>::image_general(image_general<SOURCETYPE, IMAGEDIM> * old_image, bool copyData) : image_storage<ELEMTYPE > (old_image) //copy constructor
    {
    initialize_dataset(old_image->get_size_by_dim(0), old_image->get_size_by_dim(1), old_image->get_size_by_dim(2), NULL);

    if (copyData)
        { copy_image (old_image); }

    set_parameters(old_image);
    }

template <class ELEMTYPE, int IMAGEDIM>
image_general<ELEMTYPE, IMAGEDIM>::image_general(itk::SmartPointer< itk::Image<ELEMTYPE, IMAGEDIM > > &i) : image_storage<ELEMTYPE >()
    {
    replicate_itk_to_image(i);
    }

template <class ELEMTYPE, int IMAGEDIM>
image_general<ELEMTYPE, IMAGEDIM>::image_general(int w, int h, int d, ELEMTYPE *ptr) : image_storage<ELEMTYPE >()
    {
    initialize_dataset( w,  h,  d, NULL);
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::initialize_dataset(int w, int h, int d)
//this definition catches the virtual
//initialize_dataset in image_base
    {
    initialize_dataset( w,  h,  d, NULL);
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::initialize_dataset(int w, int h, int d,  ELEMTYPE *ptr)
    {
    datasize[0] = w; datasize[1] = h; datasize[2] = d;
    
    //dimension-independent loop that may be lifted outside this function
    this->num_elements=1;
    for (unsigned short i = 0; i < IMAGEDIM; i++) 
        {
        this->num_elements *= datasize[i];
        }

    this->imageptr = new ELEMTYPE[this->num_elements];

    if (ptr!=NULL)
        {memcpy(this->imageptr,ptr,sizeof(ELEMTYPE)*this->num_elements);}

    set_parameters();
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::image_has_changed(bool mm_refresh)
    {
    //TODO: some outlandish malfunction in rendererMPR
    //when called with render_thumbnail-generated parameters
    //widget->refresh_thumbnail();

    //data changed, no longer available in a file (not that Mr. Platinum knows of, anyway)
    this->from_file(false);

    //recalculate min/max
    //with ITK image data, this is preferrably done with
    //StatisticsImageFilter

    ELEMTYPE val;

    ELEMTYPE pre_max=std::numeric_limits<ELEMTYPE>::min();
    ELEMTYPE pre_min=std::numeric_limits<ELEMTYPE>::max();

    /*for (int z=0; z < datasize[2]; z++)
        {
        for (int y=0; y < datasize[1]; y++)
            {
            for (int x=0; x < datasize[0]; x++)
                {  
                val=get_voxel(x,y,z);

                pre_max = max (val, pre_max);
                pre_min = min (val, pre_min); 
                }
            }
        }*/
    
    typename image_storage<ELEMTYPE>::iterator itr = this->begin();
    while (itr != this->end())
        {
        val=*itr;
        
        pre_max = max (val, pre_max);
        pre_min = min (val, pre_min);
        
        ++itr;
        }

    //don't change if values don't make sense - 
    //that would be an empty/zero image
    if (pre_min < pre_max)
        {
        this->maxvalue=pre_max;
        this->minvalue=pre_min;
        }

    //clear ITK connection
    ITKimportfilter = NULL;
    if (ITKimportimage.IsNotNull())
        {ITKimportimage->Delete();}
    }

/*image_general<ELEMTYPE, IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::min_max_refresh()
    {
    //recalculate min/max
    //with ITK image data, this is preferrably done with
    //StatisticsImageFilter

    ELEMTYPE val;

    ELEMTYPE pre_max=std::numeric_limits<ELEMTYPE>::min();
    ELEMTYPE pre_min=std::numeric_limits<ELEMTYPE>::max();

    for (int z=0; z < datasize[2]; z++)
        {
        for (int y=0; y < datasize[1]; y++)
            {
            for (int x=0; x < datasize[0]; x++)
                {  
                val=get_voxel(x,y,z);

                pre_max = max (val, pre_max);
                pre_min = min (val, pre_min); 
                }
            }
        }

    //don't change if values don't make sense - 
    //that would be an empty/zero image
    if (pre_min < pre_max)
        {
        maxvalue=pre_max;
        minvalue=pre_min;
        }
    }*/

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::calc_transforms ()
    {
    Matrix3D re_resize;
    unsigned short datasize_max_norm= max(max((float)datasize[0],(float)datasize[1]),(float)datasize[2]);

    re_resize=this->voxel_resize.GetInverse();
    this->unit_to_voxel_=re_resize*datasize_max_norm;
    
    //center of data in unit coordinates where longest edge = 1
    for (unsigned int d=0;d<3;d++)
        {this->unit_center_[d]=this->voxel_resize[d][d]*datasize[d]/(datasize_max_norm*2);}
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::set_parameters()
    {
    unsigned short datasize_max_norm= max(max((float)datasize[0],(float)datasize[1]),(float)datasize[2]);

    for (unsigned int d=0;d<3;d++)
        {this->unit_center_[d]=(float)datasize[d]/(datasize_max_norm*2);}

    calc_transforms();
    }

template <class ELEMTYPE, int IMAGEDIM>
void  image_general<ELEMTYPE, IMAGEDIM>::replicate_itk_to_image()
    {
    replicate_itk_to_image(ITKimportimage);
    }

template <class ELEMTYPE, int IMAGEDIM>
void  image_general<ELEMTYPE, IMAGEDIM>::replicate_itk_to_image(itk::SmartPointer< itk::Image<ELEMTYPE, IMAGEDIM > > &i)
    {
    //i.IsNotNull () could be used to catch
    //uninitialized use. In that case it should throw an exception

    typename theSizeType ITKsize = (i->GetLargestPossibleRegion()).GetSize();

    i->SetBufferedRegion(i->GetLargestPossibleRegion());

    initialize_dataset(ITKsize[0], ITKsize[1], ITKsize[2],i->GetBufferPointer());

    set_parameters (i);
    }

template <class ELEMTYPE, int IMAGEDIM>
typename theImagePointer image_general<ELEMTYPE, IMAGEDIM>::itk_image()
    {
    if (ITKimportimage.IsNull ())
        {
        if (ITKimportfilter.IsNull ())
            { make_image_an_itk_reader();}

        ITKimportimage = ITKimportfilter->GetOutput();
        }

    return ITKimportimage;
    }

template <class ELEMTYPE, int IMAGEDIM>
typename itk::ImportImageFilter< ELEMTYPE, IMAGEDIM >::Pointer image_general<ELEMTYPE, IMAGEDIM>::itk_import_filter()
    {
    if (ITKimportfilter.IsNull ())
        {
        make_image_an_itk_reader();
        }

    return ITKimportfilter;
    }

template <class ELEMTYPE, int IMAGEDIM>
void  image_general<ELEMTYPE, IMAGEDIM>::make_image_an_itk_reader()
    {
    ITKimportfilter = itk::ImportImageFilter< ELEMTYPE, IMAGEDIM >::New();

    typename itk::ImportImageFilter< ELEMTYPE, IMAGEDIM >::SizeType   ITKsize;
    typename itk::ImportImageFilter< ELEMTYPE, IMAGEDIM >::IndexType  ITKstart;
    typename itk::ImportImageFilter< ELEMTYPE, IMAGEDIM >::RegionType ITKregion;

    float itk_spacing[IMAGEDIM],itk_origin[3];

    //convert geometry variables to ITK-friendly types
    ITKsize[0]  = datasize[0];  // size along X
    ITKsize[1]  = datasize[1];  // size along Y
    ITKsize[2]  = datasize[2];  // size along Z

    for (unsigned int d=0;d<3;d++)
        {
        itk_spacing[d]=this->voxel_resize[d][d];
        itk_origin[d]=this->origin[d];
        }

    ITKstart.Fill( 0 );

    ITKregion.SetIndex( ITKstart );
    ITKregion.SetSize(  ITKsize  );
    ITKimportfilter->SetRegion( ITKregion );
    ITKimportfilter->SetOrigin(itk_origin);
    ITKimportfilter->SetSpacing(itk_spacing);

    ITKimportfilter->SetImportPointer( this->imageptr, this->num_elements, false);
    }

template <class ELEMTYPE, int IMAGEDIM>
unsigned short image_general<ELEMTYPE, IMAGEDIM>::get_size_by_dim(int dim)
    {
    return datasize [dim];
    }

template <class ELEMTYPE, int IMAGEDIM>
unsigned short image_general<ELEMTYPE, IMAGEDIM>::get_size_by_dim_and_dir(int dim, int direction)
    {
	return get_size_by_dim((dim+direction+1)%IMAGEDIM);
    }

template <class ELEMTYPE, int IMAGEDIM>
bool image_general<ELEMTYPE, IMAGEDIM>::same_size (image_base * other)
    {
    for (unsigned int d=0;d < IMAGEDIM; d++)
        {
        if (datasize[d] != other->get_size_by_dim(d))
            {
            return false;
            }
        }
    return true;
    }

template <class ELEMTYPE, int IMAGEDIM>
    Matrix3D image_general<ELEMTYPE, IMAGEDIM>::get_voxel_resize ()
    {
    return voxel_resize;
    }

template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_general<ELEMTYPE, IMAGEDIM>::get_voxel(int x, int y, int z)
    {
    return this->imageptr[x + datasize[0]*y + datasize[0]*datasize[1]*z];
    }

/*template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_general<ELEMTYPE, IMAGEDIM>::get_voxel(unsigned long offset)
    {
    return imageptr[offset];
    }*/


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::set_voxel(int x, int y, int z, ELEMTYPE voxelvalue)
    {
    this->imageptr[x + datasize[0]*y + datasize[0]*datasize[1]*z] = voxelvalue;
    }

/*template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::set_voxel(unsigned long offset, ELEMTYPE voxelvalue)
    {
    imageptr[offset] = voxelvalue;
    }*/

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::get_display_voxel(RGBvalue &val,int x, int y, int z)
    {
    this->tfunction->get(get_voxel (x, y, z),val);
    //val.set_mono(255*(get_voxel (x, y, z)-minvalue)/(maxvalue-minvalue));
    }

/*template <class ELEMTYPE, int IMAGEDIM>
unsigned char image_general<ELEMTYPE, IMAGEDIM>::get_display_voxel(int x, int y, int z)
    {
    if (maxvalue != minvalue)
        {
        if (maxvalue - minvalue == 255)
            {
            //direct mapping to chars, don't scale
            return static_cast<unsigned char>(get_voxel (x, y, z)-minvalue);
            }
        else               
            {return static_cast<unsigned char>(255*(get_voxel (x, y, z)-minvalue)/(maxvalue-minvalue));}
        }
    return 0;
    }*/

template <class ELEMTYPE, int IMAGEDIM>
float image_general<ELEMTYPE, IMAGEDIM>::get_number_voxel(int x, int y, int z)
    {
    return static_cast<float>(get_voxel (x, y, z));
    }

template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_general<ELEMTYPE, IMAGEDIM>::get_voxel_by_dir(int u, int v, int w, int direction)
{	
	if(direction==0)//Loop over x
		return get_voxel(w,u,v);
	if(direction==1)//Loop over y
		return get_voxel(v,w,u);
	return get_voxel(u,v,w); //Loop over z	
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::set_voxel_by_dir(int u, int v, int w, ELEMTYPE value, int direction)
{
	if(direction==0)//Loop over x
		set_voxel(w,u,v,value);
	else if(direction==1)//Loop over y
		set_voxel(v,w,u,value);
	else
		set_voxel(u,v,w,value);//Loop over z
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::testpattern()
    {
    cout << "Testpattern: " << datasize[2] << " " << datasize[1] << " " << datasize[0] << endl;
    //gradient test pattern
    for (int z=0; z < datasize[2]; z++)
        for (int y=0; y < datasize[1]; y++)
            for (int x=0; x < datasize[0]; x++)
                {
                set_voxel(x,y,z, int(float(x+y+z)*255.0/float(datasize[2]+datasize[1]+datasize[0])));
                }
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::set_parameters(itk::SmartPointer< itk::Image<ELEMTYPE, IMAGEDIM > > &i)
    {
    typename itk::Image<ELEMTYPE,IMAGEDIM>::SpacingType           itk_vox_size;
    typename itk::Image<ELEMTYPE,IMAGEDIM>::PointType             itk_origin;
    typename itk::Image<ELEMTYPE,IMAGEDIM>::DirectionType         itk_orientation;

    this->voxel_resize.SetIdentity();

    itk_vox_size=i->GetSpacing();
    itk_origin=i->GetOrigin ();
    itk_orientation=i->GetDirection();

    float spacing_min_norm=static_cast<float>(itk_vox_size[0]);
    for (unsigned int d=0;d<IMAGEDIM;d++)
        {
        if (itk_vox_size[d] > 0)
            {
            voxel_resize[d][d]=itk_vox_size[d];
            spacing_min_norm=min(spacing_min_norm,static_cast<float>(itk_vox_size[d]));
            }
        this->origin[d]=itk_origin[d];

        //orthogonal-only renderer can't handle arbitrary image orientations
#ifdef RENDER_ORTHOGONALLY_ONLY
        for (unsigned int c=0;c<3;c++)
            {this->direction[d][c]=round(itk_orientation[d][c]);}
#else
        for (unsigned int c=0;c<3;c++)
            {direction[d][c]=itk_orientation[d][c];}
#endif
        }

    //scale to shortest dimension=1. If condition is not met,
    //voxel_resize will be an identity matrix
    if (spacing_min_norm > 0)
        {voxel_resize/=spacing_min_norm;}

    //longest edge
    unsigned short datasize_max_norm=max(max((float)datasize[0],(float)datasize[1]),(float)datasize[2]);

    if (voxel_resize[0][0]==1.0 && voxel_resize[1][1]==1.0 && voxel_resize[2][2]==1.0)
        {
        //cubic voxels may indicate that voxel size info is missing, use
        //heuristic:
        //assume a voxel size that makes the image as deep as its tallest side,
        //without voxel z size exceeding 4

        voxel_resize.Fill(0);
        voxel_resize[0][0]=1;
        voxel_resize[1][1]=1;
        voxel_resize[2][2]=min(datasize_max_norm/(float)datasize[2],(float)4);
        }

    calc_transforms ();

    // *** max/min ***

    typename theStatsFilterPointerType statsFilter = theStatsFilterType::New();
    statsFilter->SetInput(i);
    statsFilter->Update();

    this->minvalue            = statsFilter->GetMinimum();
    ELEMTYPE new_max  = statsFilter->GetMaximum();

    //we don't want to lose pixel-data correspondence by scaling chars,
    //don't alter max/min in that case
    //note: scaling is for display only

    //we *could* scale dynamically narrow 8-bit images as well,
    //but it's a question if what the viewer expects
    //images with just a few classes (20) starting with 0
    //(eg. binary) still get their scaling

    if (new_max - this->minvalue > 255 || (this->minvalue ==0 && new_max - this->minvalue < 20) )
        {
        this->maxvalue=new_max;
        }
    else
        {
        this->maxvalue = this->minvalue + 255;
        }
    }

template <class ELEMTYPE, int IMAGEDIM>
image_binary<IMAGEDIM> * image_general<ELEMTYPE, IMAGEDIM>::threshold(ELEMTYPE low, ELEMTYPE high, bool true_inside_threshold)
	{
    image_binary<IMAGEDIM> * output = new image_binary<IMAGEDIM> (this,false);
        
    typename image_storage<ELEMTYPE >::iterator i = this->begin();
    typename image_binary<IMAGEDIM>::iterator o = output->begin();
    
    while (i != this->end()) //images are same size and
                       //should necessarily end at the same time
        {
        if(*i>=low && *i<=high)
            {*o=true_inside_threshold;}
        else
            {*o=!true_inside_threshold;}

        ++i; ++o;
        }
    
    return output;
	}

template <class ELEMTYPE, int IMAGEDIM>
//int image_general<ELEMTYPE, IMAGEDIM>::gauss_fit2(image_binary<IMAGEDIM> *mask, bool object_value)
ELEMTYPE image_general<ELEMTYPE, IMAGEDIM>::gauss_fit2()
    {
	ELEMTYPE min_val=this->get_min();
	ELEMTYPE max_val=this->get_max();
	ELEMTYPE errMinInd=max_val;
	double* hist=new double[1+max_val-min_val];
	memset(hist, 0, sizeof(double)*(1+max_val-min_val));

    typename image_storage<ELEMTYPE >::iterator i = this->begin();
    
    while (i != this->end()) //images are same size and should necessarily end at the same time
        {
        hist[*i-min_val]++;
        ++i;
        }

	//Test all threshold values
	double sum2_error_min=-10000001;
	ELEMTYPE sum2_error_min_ind=min_val;
	double sum1=0;
	double sum2=0;
	double sum1_2=0;
	double sum2_2=0;
	double n1=0;
	double n2=0;
	double cumSum=0;
	double cumSum_2=0;
	double cumN=0;
	double sum1opt=0;
	double sum2opt=0;
	double sum1_2opt=0;
	double sum2_2opt=0;
	double n1opt=0;
	double n2opt=0;
	double std1opt=0;
	double std2opt=0;
	double mean1opt=0;
	double mean2opt=0;

	unsigned long j;
	for(j=min_val; j<=max_val; j++)
		{
		cumSum+=j*hist[j-min_val];
		cumSum_2+=j*j*hist[j-min_val];
		cumN+=hist[j-min_val];
		}

	for(j=min_val; j<=max_val; j++)
		{
		//Compute mean and std
		sum1+=j*hist[j-min_val];
		sum1_2+=j*j*hist[j-min_val];
		n1+=hist[j-min_val];
		sum2=cumSum-sum1;
		sum2_2=cumSum_2-sum1_2;
		n2=cumN-n1;

		if(n1>1 && n2>1)
			{
			double mean1=sum1/n1;
			double mean2=sum2/n2;
			double std1=sqrt((sum1_2-n1*mean1*mean1)/(n1-1));
			double std2=sqrt((sum2_2-n2*mean2*mean2)/(n2-1));
			double p1=n1/(n1+n2);
			double p2=n2/(n1+n2);
			if(std1>0 && std2>0)
				{
				double sum2error=p1*log(std1)+p2*log(std2)-p1*log(p1)-p2*log(p2);
				if(sum2error<sum2_error_min || sum2_error_min<-10000000)
					{
					sum2_error_min=sum2error;
					sum2_error_min_ind=j+1;
					sum1opt=sum1;
					sum2opt=sum2;
					sum1_2opt=sum1_2;
					sum2_2opt=sum2_2;
					n1opt=n1;
					n2opt=n2;
					std1opt=std1;
					std2opt=std2;
					mean1opt=mean1;
					mean2opt=mean2;
					}    
				}
			}
		}

	double* gauss1hist=new double[1+max_val-min_val];
	double* gauss2hist=new double[1+max_val-min_val];
	double* gausstothist=new double[1+max_val-min_val];
	memset(gauss1hist, 0, sizeof(double)*(1+max_val-min_val));
	memset(gauss2hist, 0, sizeof(double)*(1+max_val-min_val));
	memset(gausstothist, 0, sizeof(double)*(1+max_val-min_val));

	//Compute mean and std
	sum1=sum1opt;
	sum2=sum2opt;
	sum1_2=sum1_2opt;
	sum2_2=sum2_2opt;
	n1=n1opt;
	n2=n2opt;

	if(n1>1 && n2>1)
		{
		double sum2diffMin=0;
		double mean1=sum1/n1;
		double mean2=sum2/n2;
		double std1=sqrt((sum1_2-n1*mean1*mean1)/(n1-1));
		double std2=sqrt((sum2_2-n2*mean2*mean2)/(n2-1));
		double sqrt2pi=sqrt(2*PI);
		double sumGauss1=0;
		double sumGauss2=0;
		for(j=min_val; j<=max_val; j++)
			{
			gauss1hist[j-min_val]=exp(-(j-mean1)*(j-mean1)/(2*std1*std1))/(std1*sqrt2pi);
			gauss2hist[j-min_val]=exp(-(j-mean2)*(j-mean2)/(2*std2*std2))/(std2*sqrt2pi);
			sumGauss1+=gauss1hist[j-min_val];
			sumGauss2+=gauss2hist[j-min_val];
			}
		for(j=min_val; j<=max_val; j++)
			{
			gauss1hist[j-min_val]*=n1/sumGauss1;
			gauss2hist[j-min_val]*=n2/sumGauss2;
			gausstothist[j-min_val]=gauss1hist[j-min_val]+gauss2hist[j-min_val];
			sum2diffMin+=(gausstothist[j-min_val]-hist[j-min_val])*(gausstothist[j-min_val]-hist[j-min_val]);
			}
		double dN1=n1opt*0.5;
		double dN2=n2opt*0.5;
		double dM1=std1opt;
		double dM2=std2opt;
		double dS1=std1opt/2;
		double dS2=std2opt/2;
		double sum2diff1=0;
		double sum2diff2=0;
		n1=n1opt;
		n2=n2opt;
		mean1=mean1opt;
		mean2=mean2opt;
		std1=std1opt;
		std2=std2opt;
		double minDN1=n1opt*0.001;
		double minDN2=n2opt*0.001;
		double minDS1=0.5;
		double minDS2=0.5;
		double minDM1=0.5;
		double minDM2=0.5;
		double decrease=0.5;
	
		while(dN1>=minDN1 || dN2>=minDN2 || dM1>=minDM1 || dM2>=minDM2 || dS1>=minDS1 || dS2>=minDS2)
			{
			if(dN1>=minDN1)
				{
				//Test n1
				n1=n1opt-dN1;
				n2=n2opt;
				mean1=mean1opt;
				mean2=mean2opt;
				std1=std1opt;
				std2=std2opt;
				sum2diff1=0;
				sum2diff2=0;
				sumGauss1=0;
				sumGauss2=0;
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]=exp(-(j-mean1)*(j-mean1)/(2*std1*std1))/(std1*sqrt2pi);
					gauss2hist[j-min_val]=exp(-(j-mean2)*(j-mean2)/(2*std2*std2))/(std2*sqrt2pi);
					sumGauss1+=gauss1hist[j-min_val];
					sumGauss2+=gauss2hist[j-min_val];
					}
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]*=n1/sumGauss1;
					gauss2hist[j-min_val]*=n2/sumGauss2;
					gausstothist[j-min_val]=gauss1hist[j-min_val]+gauss2hist[j-min_val];
					sum2diff1+=(gausstothist[j-min_val]-hist[j-min_val])*(gausstothist[j-min_val]-hist[j-min_val]);
					}
				n1=n1opt+dN1;
				sumGauss1=0;
				sumGauss2=0;
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]=exp(-(j-mean1)*(j-mean1)/(2*std1*std1))/(std1*sqrt2pi);
					gauss2hist[j-min_val]=exp(-(j-mean2)*(j-mean2)/(2*std2*std2))/(std2*sqrt2pi);
					sumGauss1+=gauss1hist[j-min_val];
					sumGauss2+=gauss2hist[j-min_val];
					}
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]*=n1/sumGauss1;
					gauss2hist[j-min_val]*=n2/sumGauss2;
					gausstothist[j-min_val]=gauss1hist[j-min_val]+gauss2hist[j-min_val];
					sum2diff2+=(gausstothist[j-min_val]-hist[j-min_val])*(gausstothist[j-min_val]-hist[j-min_val]);
					}
				if(sum2diff1<sum2diffMin)
					{
					if(sum2diff1<sum2diff2)
						{
						sum2diffMin=sum2diff1;
						n1opt-=dN1;
						}
					else
						{
						sum2diffMin=sum2diff2;
						n1opt+=dN1;
						}
					}
				else if(sum2diff2<sum2diffMin)
					{
					sum2diffMin=sum2diff2;
					n1opt+=dN1;
					} 
				else
					{
					dN1*=decrease;
					}
				}
	
			if(dN2>=minDN2)
				{
				//Test n2
				n1=n1opt;
				n2=n2opt-dN2;
				mean1=mean1opt;
				mean2=mean2opt;
				std1=std1opt;
				std2=std2opt;
				sum2diff1=0;
				sum2diff2=0;
				sumGauss1=0;
				sumGauss2=0;
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]=exp(-(j-mean1)*(j-mean1)/(2*std1*std1))/(std1*sqrt2pi);
					gauss2hist[j-min_val]=exp(-(j-mean2)*(j-mean2)/(2*std2*std2))/(std2*sqrt2pi);
					sumGauss1+=gauss1hist[j-min_val];
					sumGauss2+=gauss2hist[j-min_val];
					}
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]*=n1/sumGauss1;
					gauss2hist[j-min_val]*=n2/sumGauss2;
					gausstothist[j-min_val]=gauss1hist[j-min_val]+gauss2hist[j-min_val];
					sum2diff1+=(gausstothist[j-min_val]-hist[j-min_val])*(gausstothist[j-min_val]-hist[j-min_val]);
					}
				n2=n2opt+dN2;
				sumGauss1=0;
				sumGauss2=0;
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]=exp(-(j-mean1)*(j-mean1)/(2*std1*std1))/(std1*sqrt2pi);
					gauss2hist[j-min_val]=exp(-(j-mean2)*(j-mean2)/(2*std2*std2))/(std2*sqrt2pi);
					sumGauss1+=gauss1hist[j-min_val];
					sumGauss2+=gauss2hist[j-min_val];
					}
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]*=n1/sumGauss1;
					gauss2hist[j-min_val]*=n2/sumGauss2;
					gausstothist[j-min_val]=gauss1hist[j-min_val]+gauss2hist[j-min_val];
					sum2diff2+=(gausstothist[j-min_val]-hist[j-min_val])*(gausstothist[j-min_val]-hist[j-min_val]);
					}
				if(sum2diff1<sum2diffMin)
					{
					if(sum2diff1<sum2diff2)
						{
						sum2diffMin=sum2diff1;
						n2opt-=dN2;
						}
					else
						{
						sum2diffMin=sum2diff2;
						n2opt+=dN2;
						}
					}
				else if(sum2diff2<sum2diffMin)
					{
					sum2diffMin=sum2diff2;
					n2opt+=dN2;
					} 
				else
					{
					dN2*=decrease;
					}
				}

			if(dM1>=minDM1)
				{
				//Test m1
				n1=n1opt;
				n2=n2opt;
				mean1=mean1opt-dM1;
				mean2=mean2opt;
				std1=std1opt;
				std2=std2opt;
				sum2diff1=0;
				sum2diff2=0;
				sumGauss1=0;
				sumGauss2=0;
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]=exp(-(j-mean1)*(j-mean1)/(2*std1*std1))/(std1*sqrt2pi);
					gauss2hist[j-min_val]=exp(-(j-mean2)*(j-mean2)/(2*std2*std2))/(std2*sqrt2pi);
					sumGauss1+=gauss1hist[j-min_val];
					sumGauss2+=gauss2hist[j-min_val];
					}
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]*=n1/sumGauss1;
					gauss2hist[j-min_val]*=n2/sumGauss2;
					gausstothist[j-min_val]=gauss1hist[j-min_val]+gauss2hist[j-min_val];
					sum2diff1+=(gausstothist[j-min_val]-hist[j-min_val])*(gausstothist[j-min_val]-hist[j-min_val]);
					}
				mean1=mean1opt+dM1;
				sumGauss1=0;
				sumGauss2=0;
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]=exp(-(j-mean1)*(j-mean1)/(2*std1*std1))/(std1*sqrt2pi);
					gauss2hist[j-min_val]=exp(-(j-mean2)*(j-mean2)/(2*std2*std2))/(std2*sqrt2pi);
					sumGauss1+=gauss1hist[j-min_val];
					sumGauss2+=gauss2hist[j-min_val];
					}
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]*=n1/sumGauss1;
					gauss2hist[j-min_val]*=n2/sumGauss2;
					gausstothist[j-min_val]=gauss1hist[j-min_val]+gauss2hist[j-min_val];
					sum2diff2+=(gausstothist[j-min_val]-hist[j-min_val])*(gausstothist[j-min_val]-hist[j-min_val]);
					}
				if(sum2diff1<sum2diffMin && mean1opt-dM1>min_val)
					{
					if(sum2diff1<sum2diff2)
						{
						sum2diffMin=sum2diff1;
						mean1opt-=dM1;
						}
					else
						{
						sum2diffMin=sum2diff2;
						mean1opt+=dM1;
						}
					}
				else if(sum2diff2<sum2diffMin)
					{
					sum2diffMin=sum2diff2;
					mean1opt+=dM1;
					} 
				else
					{
					dM1*=decrease;
					}
				}

			if(dM2>=minDM2)
				{
				//Test m2
				n1=n1opt;
				n2=n2opt;
				mean1=mean1opt;
				mean2=mean2opt-dM2;
				std1=std1opt;
				std2=std2opt;
				sum2diff1=0;
				sum2diff2=0;
				sumGauss1=0;
				sumGauss2=0;
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]=exp(-(j-mean1)*(j-mean1)/(2*std1*std1))/(std1*sqrt2pi);
					gauss2hist[j-min_val]=exp(-(j-mean2)*(j-mean2)/(2*std2*std2))/(std2*sqrt2pi);
					sumGauss1+=gauss1hist[j-min_val];
					sumGauss2+=gauss2hist[j-min_val];
					}
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]*=n1/sumGauss1;
					gauss2hist[j-min_val]*=n2/sumGauss2;
					gausstothist[j-min_val]=gauss1hist[j-min_val]+gauss2hist[j-min_val];
					sum2diff1+=(gausstothist[j-min_val]-hist[j-min_val])*(gausstothist[j-min_val]-hist[j-min_val]);
					}
				mean2=mean2opt+dM2;
				sumGauss1=0;
				sumGauss2=0;
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]=exp(-(j-mean1)*(j-mean1)/(2*std1*std1))/(std1*sqrt2pi);
					gauss2hist[j-min_val]=exp(-(j-mean2)*(j-mean2)/(2*std2*std2))/(std2*sqrt2pi);
					sumGauss1+=gauss1hist[j-min_val];
					sumGauss2+=gauss2hist[j-min_val];
					}
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]*=n1/sumGauss1;
					gauss2hist[j-min_val]*=n2/sumGauss2;
					gausstothist[j-min_val]=gauss1hist[j-min_val]+gauss2hist[j-min_val];
					sum2diff2+=(gausstothist[j-min_val]-hist[j-min_val])*(gausstothist[j-min_val]-hist[j-min_val]);
					}
				if(sum2diff1<sum2diffMin && mean2opt-3*std2>min_val)
					{
					if(sum2diff1<sum2diff2)
						{
						sum2diffMin=sum2diff1;
						mean2opt-=dM2;
						}
					else
						{
						sum2diffMin=sum2diff2;
						mean2opt+=dM2;
						}
					}
				else if(sum2diff2<sum2diffMin)
					{
					sum2diffMin=sum2diff2;
					mean2opt+=dM2;
					} 
				else
					{
					dM2*=decrease;
					}
				}

			if(dS1>=minDS1)
				{
				//Test s1
				n1=n1opt;
				n2=n2opt;
				mean1=mean1opt;
				mean2=mean2opt;
				std1=std1opt-dS1;
				std2=std2opt;
				sum2diff1=0;
				sum2diff2=0;
				sumGauss1=0;
				sumGauss2=0;
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]=exp(-(j-mean1)*(j-mean1)/(2*std1*std1))/(std1*sqrt2pi);
					gauss2hist[j-min_val]=exp(-(j-mean2)*(j-mean2)/(2*std2*std2))/(std2*sqrt2pi);
					sumGauss1+=gauss1hist[j-min_val];
					sumGauss2+=gauss2hist[j-min_val];
					}
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]*=n1/sumGauss1;
					gauss2hist[j-min_val]*=n2/sumGauss2;
					gausstothist[j-min_val]=gauss1hist[j-min_val]+gauss2hist[j-min_val];
					sum2diff1+=(gausstothist[j-min_val]-hist[j-min_val])*(gausstothist[j-min_val]-hist[j-min_val]);
					}
				std1=std1opt+dS1;
				sumGauss1=0;
				sumGauss2=0;
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]=exp(-(j-mean1)*(j-mean1)/(2*std1*std1))/(std1*sqrt2pi);
					gauss2hist[j-min_val]=exp(-(j-mean2)*(j-mean2)/(2*std2*std2))/(std2*sqrt2pi);
					sumGauss1+=gauss1hist[j-min_val];
					sumGauss2+=gauss2hist[j-min_val];
					}
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]*=n1/sumGauss1;
					gauss2hist[j-min_val]*=n2/sumGauss2;
					gausstothist[j-min_val]=gauss1hist[j-min_val]+gauss2hist[j-min_val];
					sum2diff2+=(gausstothist[j-min_val]-hist[j-min_val])*(gausstothist[j-min_val]-hist[j-min_val]);
					}
				if(sum2diff1<sum2diffMin)
					{
					if(sum2diff1<sum2diff2)
						{
						sum2diffMin=sum2diff1;
						std1opt-=dS1;
						}
					else
						{
						sum2diffMin=sum2diff2;
						std1opt+=dS1;
						}
					}
				else if(sum2diff2<sum2diffMin)
					{
					sum2diffMin=sum2diff2;
					std1opt+=dS1;
					} 
				else
					{
					dS1*=decrease;
					}
				}

			if(dS2>=minDS2)
				{
				//Test s2
				n1=n1opt;
				n2=n2opt;
				mean1=mean1opt;
				mean2=mean2opt;
				std1=std1opt;
				std2=std2opt-dS2;
				sum2diff1=0;
				sum2diff2=0;
				sumGauss1=0;
				sumGauss2=0;
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]=exp(-(j-mean1)*(j-mean1)/(2*std1*std1))/(std1*sqrt2pi);
					gauss2hist[j-min_val]=exp(-(j-mean2)*(j-mean2)/(2*std2*std2))/(std2*sqrt2pi);
					sumGauss1+=gauss1hist[j-min_val];
					sumGauss2+=gauss2hist[j-min_val];
					}
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]*=n1/sumGauss1;
					gauss2hist[j-min_val]*=n2/sumGauss2;
					gausstothist[j-min_val]=gauss1hist[j-min_val]+gauss2hist[j-min_val];
					sum2diff1+=(gausstothist[j-min_val]-hist[j-min_val])*(gausstothist[j-min_val]-hist[j-min_val]);
					}
				std2=std2opt+dS2;
				sumGauss1=0;
				sumGauss2=0;
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]=exp(-(j-mean1)*(j-mean1)/(2*std1*std1))/(std1*sqrt2pi);
					gauss2hist[j-min_val]=exp(-(j-mean2)*(j-mean2)/(2*std2*std2))/(std2*sqrt2pi);
					sumGauss1+=gauss1hist[j-min_val];
					sumGauss2+=gauss2hist[j-min_val];
					}
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]*=n1/sumGauss1;
					gauss2hist[j-min_val]*=n2/sumGauss2;
					gausstothist[j-min_val]=gauss1hist[j-min_val]+gauss2hist[j-min_val];
					sum2diff2+=(gausstothist[j-min_val]-hist[j-min_val])*(gausstothist[j-min_val]-hist[j-min_val]);
					}
				if(sum2diff1<sum2diffMin/* && mean2opt-3*std2>min_val*/)
					{
					if(sum2diff1<sum2diff2)
						{
						sum2diffMin=sum2diff1;
						std2opt-=dS2;
						}
					else
						{
						sum2diffMin=sum2diff2;
						std2opt+=dS2;
						}
					}
				else if(sum2diff2<sum2diffMin && mean2opt-3*std2>min_val)
					{
					sum2diffMin=sum2diff2;
					std2opt+=dS2;
					} 
				else
					{
					dS2*=decrease;
					}
				}
			}
		}

	double maxGauss=0;
	double cumsum1=0;
	sum1=0;
	sum2=0;
	for(j=min_val; j<=max_val; j++)
		{
		if(gausstothist[j-min_val]>maxGauss)
			maxGauss=gausstothist[j-min_val];
		cumsum1+=gauss2hist[j-min_val];
		
		}
	double err;
	double errMin=cumsum1;
	errMinInd=max_val;
	for(j=min_val; j<=max_val; j++)
		{
		sum1+=gauss1hist[j-min_val];
		sum2+=gauss2hist[j-min_val];
		err=sum2+cumsum1-sum1;
		if(err<errMin)
			{
			errMin=err;
			errMinInd=j+1;
			}
		}
	return errMinInd;
	}

#endif
