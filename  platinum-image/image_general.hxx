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
#include "image_label.h"
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

template <class NEWELEM, class TRYELEM, int DIM, template <class, int> class requestedClass >
requestedClass<NEWELEM, DIM> * try_general (image_base* input) //! Helper function to guaranteed_cast
    {
    requestedClass<NEWELEM, DIM> * output = NULL;

    image_general <TRYELEM,DIM>* input_general = dynamic_cast<image_general <TRYELEM, DIM> *> (input) ; //! Try to cast into image_general of try-type

    if (input_general != NULL) //! If cast was successful, input had the tried type and input_general can be used in a call to new class' copy constructor
        {
        output = new requestedClass<NEWELEM, DIM> (input_general,true);
        delete input;
        } 
    return output;
    }

//Some classes have fixed type and only dimensionality templated
//these need the function template right below:

/*template <class ELEM, int DIM, template <int> class requestedClass >
requestedClass<DIM>* guaranteed_cast (image_base* input)
{
    requestedClass<DIM> * output = dynamic_cast<requestedClass<DIM > *>(input);

    //try any data type used in classes with fixed type

    return output;
}

*/

template <template <class, int> class requestedClass, class ELEM, int DIM>
requestedClass<ELEM, DIM>* guaranteed_cast (image_base* input)
    {
    requestedClass<ELEM, DIM> * output = dynamic_cast<requestedClass<ELEM, DIM > *>(input);

    //Try all possible data types

    if (output == NULL)
        { output = try_general <ELEM,unsigned char,DIM,requestedClass >(input); }

    if (output == NULL)
        { output = try_general <ELEM,signed char,DIM,requestedClass >(input); }

    if (output == NULL)
        { output = try_general <ELEM,unsigned short,DIM,requestedClass >(input); }

    if (output == NULL)
        { output = try_general <ELEM,signed short,DIM,requestedClass >(input); }

    if (output == NULL)
        { output = try_general <ELEM,unsigned long,DIM,requestedClass >(input); }

    if (output == NULL)
        { output = try_general <ELEM,signed long,DIM,requestedClass >(input); }

    if (output == NULL)
        { output = try_general <ELEM,float,DIM,requestedClass >(input); }

    if (output == NULL)
        { output = try_general <ELEM,double,DIM,requestedClass >(input); }

    if (output == NULL)
        {throw  (bad_cast());}

    return output;
    }


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

// *** Processing ***

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
	ELEMTYPE err_min_ind=max_val;
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
	err_min_ind=max_val;
	for(j=min_val; j<=max_val; j++)
		{
		sum1+=gauss1hist[j-min_val];
		sum2+=gauss2hist[j-min_val];
		err=sum2+cumsum1-sum1;
		if(err<errMin)
			{
			errMin=err;
			err_min_ind=j+1;
			}
		}
	return err_min_ind;
	}

template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_general<ELEMTYPE, IMAGEDIM>::components_hist_3D()
    {
	ELEMTYPE min_val=this->get_min();
	ELEMTYPE max_val=this->get_max();
	ELEMTYPE err_min_ind=min_val;
	int x,y,z,x0,y0,z0,x2,y2,z2;
	int max_x, max_y, max_z;
	max_x=this->get_size_by_dim(0);
	max_y=this->get_size_by_dim(1);
	max_z=this->get_size_by_dim(2);
	int number_of_voxels=max_x*max_y*max_z;
		
	//Sort points
	// create a counting array, counts, with a member for 
    // each possible discrete value in the input.  
    // initialize all counts to 0.
    int distinct_element_count = max_val - min_val + 1;
    int* counts = new int[distinct_element_count];
	memset(counts, 0, sizeof(int)*(1+max_val-min_val));
    // for each value in the unsorted array, increment the
    // count in the corresponding element of the count array
    typename image_storage<ELEMTYPE >::iterator iter = this->begin();    
    while (iter != this->end()) //images are same size and should necessarily end at the same time
        {
        counts[*iter-min_val]++;
        ++iter;
        }
    int counts0=counts[0];
    // accumulate the counts - the result is that counts will hold
    // the offset into the sorted array for the value associated with that index
	int i,j,k;
    for (j=1; j<distinct_element_count; j++)
		{
        counts[j] += counts[j-1];
		}
    // store the elements in a new ordered array
    int* sorted_index = new int[number_of_voxels];
	ELEMTYPE cur_voxel;
	int rest;
	for (i=number_of_voxels-1, iter=this->begin(); i>=0; i--)
		{
            // decrementing the counts value ensures duplicate values in A
            // are stored at different indices in sorted.
			
			sorted_index[--counts[(*(iter+i))-min_val]] = i;                
        }

    int* par_node = new int[number_of_voxels];
    //Init nodes
    for (j=0; j<number_of_voxels; j++)
		{
        par_node[j]=j;
		}
        
    //Search in decreasing order
    int cur_node;
    int adj_node;
	iter = this->end();
    for (i=number_of_voxels-1; i>=counts0; i--)
		{
        j=sorted_index[i];	 
        cur_node=j;
        
        z=j/(max_x*max_y);
        rest=j-z*(max_x*max_y);
        y=rest/max_x;
        x=rest-y*max_x;
		cur_voxel=this->get_voxel(x,y,z);

        z0=z-1;
        z2=z+1;
        y0=y-1;
        y2=y+1;
        x0=x-1;
        x2=x+1;

        //Later neigbours x2,y2,z2
        if(z2<max_z)
			{
			if(this->get_voxel(x,y,z2)>=cur_voxel)
				{
        		k=x+max_x*(y+z2*max_y);
	        	adj_node=findNode(k, par_node);
	        	if(cur_node!=adj_node)
	        		cur_node=mergeNodes(adj_node,cur_node, par_node);
				}
			}
        if(y2<max_y)
			{
			if(this->get_voxel(x,y2,z)>=cur_voxel)
				{
        		k=x+max_x*(y2+z*max_y);
	        	adj_node=findNode(k, par_node);
	        	if(cur_node!=adj_node)
	        		cur_node=mergeNodes(adj_node,cur_node, par_node);
				}
			}
        if(x2<max_x)
			{
			if(this->get_voxel(x2,y,z)>=cur_voxel)
				{
        		k=x2+max_x*(y+z*max_y);
	        	adj_node=findNode(k, par_node);
	        	if(cur_node!=adj_node)
	        		cur_node=mergeNodes(adj_node,cur_node, par_node);
				}
			}
        //Earlier neighbours x0,y0,z0
        if(z0>=0)
			{
			if(this->get_voxel(x,y,z0)>cur_voxel)
				{
        		k=x+max_x*(y+z0*max_y);
	        	adj_node=findNode(k, par_node);
	        	if(cur_node!=adj_node)
	        		cur_node=mergeNodes(adj_node,cur_node, par_node);
				}
			}
        if(y0>=0)
			{
			if(this->get_voxel(x,y0,z)>cur_voxel)
				{
        		k=x+max_x*(y0+z*max_y);
	        	adj_node=findNode(k, par_node);
	        	if(cur_node!=adj_node)
	        		cur_node=mergeNodes(adj_node,cur_node, par_node);
				}
			}
        if(x0>=0)
			{
			if(this->get_voxel(x0,y,z)>cur_voxel)
				{
        		k=x0+max_x*(y+z*max_y);
	        	adj_node=findNode(k, par_node);
	        	if(cur_node!=adj_node)
	        		cur_node=mergeNodes(adj_node,cur_node, par_node);
				}
			}
		}
    double* x_values = new double[1+max_val-min_val];
    double* y_values = new double[1+max_val-min_val];
    double* diff = new double[1+max_val-min_val];
	memset(diff, 0, sizeof(double)*(1+max_val-min_val));	 
	long cumulative_number_of_objects=0;
	double max=0;

	for (i=0, iter=this->begin(); i<number_of_voxels; i++)
		{
        diff[(*(iter+i))-min_val]++;
        diff[(*(iter+par_node[i]))-min_val]--;
		}
    for(i=max_val; i>=min_val; i--)
		{
        cumulative_number_of_objects+=diff[i];
        y_values[i-min_val]=cumulative_number_of_objects;
        x_values[i-min_val]=i;
        if(cumulative_number_of_objects>max)
        	max=cumulative_number_of_objects;
		}
        
    int min1ind=min_val;
    double max1;
    double min1;
    double dmax=0;
    i=min_val+1;
    while(i<max_val)
		{
        //find max
	    while(y_values[i-min_val]>=y_values[i-1-min_val] && i<max_val)
	        i++;
	    max1=y_values[i-1-min_val];
	    //find following min
	    while(y_values[i-min_val]<=y_values[i-1-min_val] && i<max_val)
	        i++;
	    min1=y_values[i-1-min_val];
	    min1ind=i-1;
	    if(max1-min1>dmax)
			{
	        dmax=max1-min1;
        	err_min_ind=min1ind;	        	
			}
		}

	return err_min_ind;
	}

template <class ELEMTYPE, int IMAGEDIM>
image_label<IMAGEDIM> * image_general<ELEMTYPE, IMAGEDIM>::narrowest_passage_3D(image_binary<IMAGEDIM> * mask, bool object_value)
    {
    image_label<IMAGEDIM> * output = new image_label<IMAGEDIM> (this,false);
	IMGLABELTYPE class1=3;
	IMGLABELTYPE class2=4;
	IMGLABELTYPE undef=1;
	IMGLABELTYPE conflict=2;
	IMGLABELTYPE bkg=0;
	ELEMTYPE min_val=this->get_min();
	ELEMTYPE max_val=this->get_max();
	ELEMTYPE err_min_ind=min_val;
	int x,y,z,x0,y0,z0,x2,y2,z2;
	int max_x, max_y, max_z;
	max_x=this->get_size_by_dim(0);
	max_y=this->get_size_by_dim(1);
	max_z=this->get_size_by_dim(2);
	int number_of_voxels=max_x*max_y*max_z;
	output->erase();
		
	//Sort points
	// create a counting array, counts, with a member for 
    // each possible discrete value in the input.  
    // initialize all counts to 0.
    int distinct_element_count = max_val - min_val + 1;
    int* counts = new int[distinct_element_count];
	memset(counts, 0, sizeof(int)*(1+max_val-min_val));
    int* counts_cum = new int[distinct_element_count];
	memset(counts_cum, 0, sizeof(int)*(1+max_val-min_val));
    // for each value in the unsorted array, increment the
    // count in the corresponding element of the count array
    typename image_storage<ELEMTYPE >::iterator iter = this->begin();    
    while (iter != this->end()) //images are same size and should necessarily end at the same time
        {
        counts[*iter-min_val]++;
        ++iter;
        }
    counts_cum[0]=counts[0];
    // accumulate the counts - the result is that counts will hold
    // the offset into the sorted array for the value associated with that index
	int i,j,k;
    for (j=1; j<distinct_element_count; j++)
		{
        counts_cum[j] = counts_cum[j-1]+counts[j];
		}
    // store the elements in a new ordered array
    int* sorted_index = new int[number_of_voxels];
	ELEMTYPE cur_voxel;
	int rest;
	for (i=number_of_voxels-1, iter=this->begin(); i>=0; i--)
		{
            // decrementing the counts value ensures duplicate values in A
            // are stored at different indices in sorted.
			
			sorted_index[--counts[(*(iter+i))-min_val]] = i;                
        }

    int* par_node = new int[number_of_voxels];
    //Init nodes
    for (j=0; j<number_of_voxels; j++)
		{
        par_node[j]=j;
		}
        
    //Search in decreasing order
    int cur_node;
    int adj_node;
	iter = this->end();
    for (i=number_of_voxels-1; i>=counts[0]; i--)
		{
        j=sorted_index[i];	 
        cur_node=j;
        
        z=j/(max_x*max_y);
        rest=j-z*(max_x*max_y);
        y=rest/max_x;
        x=rest-y*max_x;

		cur_voxel=this->get_voxel(x,y,z);

        z0=z-1;
        z2=z+1;
        y0=y-1;
        y2=y+1;
        x0=x-1;
        x2=x+1;

        //Later neigbours x2,y2,z2
        if(z2<max_z)
			{
			if(this->get_voxel(x,y,z2)>=cur_voxel)
				{
        		k=x+max_x*(y+z2*max_y);
	        	adj_node=findNode(k, par_node);
	        	if(cur_node!=adj_node)
	        		cur_node=mergeNodes(adj_node,cur_node, par_node);
				}
			}
        if(y2<max_y)
			{
			if(this->get_voxel(x,y2,z)>=cur_voxel)
				{
        		k=x+max_x*(y2+z*max_y);
	        	adj_node=findNode(k, par_node);
	        	if(cur_node!=adj_node)
	        		cur_node=mergeNodes(adj_node,cur_node, par_node);
				}
			}
        if(x2<max_x)
			{
			if(this->get_voxel(x2,y,z)>=cur_voxel)
				{
        		k=x2+max_x*(y+z*max_y);
	        	adj_node=findNode(k, par_node);
	        	if(cur_node!=adj_node)
	        		cur_node=mergeNodes(adj_node,cur_node, par_node);
				}
			}
        //Earlier neighbours x0,y0,z0
        if(z0>=0)
			{
			if(this->get_voxel(x,y,z0)>cur_voxel)
				{
        		k=x+max_x*(y+z0*max_y);
	        	adj_node=findNode(k, par_node);
	        	if(cur_node!=adj_node)
	        		cur_node=mergeNodes(adj_node,cur_node, par_node);
				}
			}
        if(y0>=0)
			{
			if(this->get_voxel(x,y0,z)>cur_voxel)
				{
        		k=x+max_x*(y0+z*max_y);
	        	adj_node=findNode(k, par_node);
	        	if(cur_node!=adj_node)
	        		cur_node=mergeNodes(adj_node,cur_node, par_node);
				}
			}
        if(x0>=0)
			{
			if(this->get_voxel(x0,y,z)>cur_voxel)
				{
        		k=x0+max_x*(y+z*max_y);
	        	adj_node=findNode(k, par_node);
	        	if(cur_node!=adj_node)
	        		cur_node=mergeNodes(adj_node,cur_node, par_node);
				}
			}
		}

	    //Find seeds
	    bool* marked=new bool[number_of_voxels];
		memset(marked, 0, sizeof(bool)*number_of_voxels);
		typename image_storage<bool >::iterator mask_iter = mask->begin(); 
		typename image_storage<IMGLABELTYPE >::iterator output_iter = output->begin(); 
		for (i=0; i<number_of_voxels; i++)
			{
			if((*(mask_iter+i))==object_value)
				{
				markRecursive(i,par_node,marked);
				(*(output_iter+i))=class1;
				}
			}

		//NPT image
		ELEMTYPE* npt_array = new ELEMTYPE[number_of_voxels];
		memset(npt_array, 0, sizeof(ELEMTYPE)*number_of_voxels);
	    for (i=number_of_voxels-1; i>=counts[0]; i--)//Skip lowest value
			{
	        j=sorted_index[i];
	        npt_array[j]=getSeedLevel(j, par_node, marked);
			}
	        
	    //Equal neighbour
	    bool* eq_neigh=new bool[number_of_voxels];
	    int n_diff;
	    for (i=number_of_voxels-1; i>=counts[0]; i--)//Skip lowest value
			{
	        j=sorted_index[i];
	        n_diff=0;
	        
	        z=j/(max_x*max_y);
	        rest=j-z*(max_x*max_y);
	        y=rest/max_x;
	        x=rest-y*max_x;

			for(z2=std::max(0,z-1); z2<min(max_z,z+2) && n_diff==0; z2++)
				{
				for(y2=std::max(0,y-1); y2<min(max_y,y+2) && n_diff==0; y2++)
					{
					for(x2=std::max(0,x-1); x2<min(max_x,x+2) && n_diff==0; x2++)
						{
			        	k=x2+max_x*(y2+z2*max_y);
				        if(npt_array[k]!=npt_array[j])
				        	{
				        	n_diff++;
				        	}										
						}
					}
				}
	        if(n_diff==0)
	        	{
				(*(output_iter+j))=class2;
	        	}
			}

	    ELEMTYPE cur_level;
	    int nClass1, nClass2;
		bool changed=true;
		bool changedThis;
		while(changed)
			{
			changed=false;
			for(cur_level=max_val; cur_level>min_val; cur_level--)
				{
				//Loop each level until not changed
				changedThis=true;
				while(changedThis)
					{
					changedThis=false;
			        for (i=counts_cum[cur_level-min_val]-1; i>=counts_cum[cur_level-1-min_val]; i--)
						{
			        	j=sorted_index[i];
				        if((*(output_iter+j))==undef)
				        	{
					        //Check neighbours
				        	z=j/(max_x*max_y);
					        rest=j-z*(max_x*max_y);
					        y=rest/max_x;
					        x=rest-y*max_x;
	
					        nClass1=nClass2=0;

							for(z2=std::max(0,z-1); z2<min(max_z,z+2); z2++)
								{
								for(y2=std::max(0,y-1); y2<min(max_y,y+2); y2++)
									{
									for(x2=std::max(0,x-1); x2<min(max_x,x+2); x2++)
										{
							        	k=x2+max_x*(y2+z2*max_y);
										if((*(output_iter+k))==class1)
											nClass1++;
										else if((*(output_iter+k))==class2)
											nClass2++;										
										}
									}
								}
							if(nClass1>0 && nClass2==0)
								{
								(*(output_iter+j))=class1;
								changed=true;
								changedThis=true;
								}
							else if(nClass2>0 && nClass1==0)
								{
								(*(output_iter+j))=class2;
								changed=true;
								changedThis=true;
								}
							else if(nClass2>0 && nClass1>0)
								{
								(*(output_iter+j))=conflict;
								changed=true;
								changedThis=true;
								}
							}
						}
					}
				}		        
			}  

	return output;
	}

template <class ELEMTYPE, int IMAGEDIM>
int image_general<ELEMTYPE, IMAGEDIM>::findNode(int e, int* par_node)
	{
		if(par_node[e]!=e)
			return findNode(par_node[e], par_node);
		else
			return e;
	}
	
template <class ELEMTYPE, int IMAGEDIM>
int image_general<ELEMTYPE, IMAGEDIM>::mergeNodes(int e1, int e2, int* par_node)
	{
		int res;
		if((*(this->begin()+e1))==(*(this->begin()+e2)))
		{
			res=std::max(e1,e2);
			par_node[min(e1,e2)]=res;
		}
		else if((*(this->begin()+e1))>(*(this->begin()+e2)))
		{
			par_node[e1]=e2;
			res=e2;			
		}
		else
		{
			par_node[e2]=e1;
			res=e1;		
		}
		return res;
	}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::markRecursive(int m, int* par_node, bool* marked)
	{
		if((*(this->begin()+m))>this->get_min())
		{
			if(!marked[m])
			{
				marked[m]=true;
				if(par_node[m]!=m)
					markRecursive(par_node[m], par_node, marked);
			}
		}
	}

template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_general<ELEMTYPE, IMAGEDIM>::getSeedLevel(int m, int* par_node, bool* marked)
	{
		if((*(this->begin()+m))>this->get_min())
		{
			if(marked[m])
				return *(this->begin()+m);
			else if(par_node[m]!=m)
				return getSeedLevel(par_node[m], par_node, marked);
			else
				return 0;
		}
		else
			return 0;
	}

#endif
