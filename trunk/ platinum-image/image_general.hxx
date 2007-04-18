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

template<class ELEMTYPE, int IMAGEDIM>
    class image_scalar; //forward declaration, needed with GCC for unknown reasons

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
image_general<ELEMTYPE, IMAGEDIM>::image_general() : image_storage<ELEMTYPE > ()
    {
    imageptr = NULL;
    minvalue=std::numeric_limits<ELEMTYPE>::min();
    maxvalue=std::numeric_limits<ELEMTYPE>::max();
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::set_parameters (image_general<ELEMTYPE, IMAGEDIM> * from_volume)
    {
    short size [IMAGEDIM];

    for (int d=0; d < IMAGEDIM; d++)
        { size[d]=from_volume->datasize[d]; }

    initialize_dataset(size[0],size[1],size[2]);

    ITKimportfilter=NULL;
    ITKimportimage=NULL;

    volumename = "Copy of " + from_volume->volumename;

    maxvalue        = from_volume->maxvalue;
    minvalue        = from_volume->minvalue;

    origin          = from_volume->origin;
    direction       = from_volume->direction;
    voxel_resize    = from_volume->voxel_resize;

    unit_center     = from_volume->unit_center;
    unit_to_voxel   = from_volume->unit_to_voxel;

    // *ID, from_file, volumename and widget are assigned in image_base constructor
    }

template <class ELEMTYPE, int IMAGEDIM>
image_base * image_general<ELEMTYPE, IMAGEDIM>::alike (imageDataType unit)
    {
    //this alike is called from the template-agnostic image_base class
    image_base * new_volume = NULL;

    switch (unit)
        {
        //TODO: make image_integer (or whatev) class and call that for all int types
        case VOLDATA_BOOL:      new_volume = new image_binary<IMAGEDIM>(this,false);             
            break;
        case VOLDATA_CHAR:      new_volume = new image_integer<char, IMAGEDIM > (this,false);              break;
        case VOLDATA_UCHAR:     new_volume = new image_integer<unsigned char, IMAGEDIM > (this,false);     break;
        case VOLDATA_SHORT:     new_volume = new image_integer<short, IMAGEDIM >(this,false);              break;
        case VOLDATA_USHORT:    new_volume = new image_integer<unsigned short, IMAGEDIM >(this,false);     break;
        case VOLDATA_DOUBLE:    new_volume = new image_scalar<double, IMAGEDIM >(this,false);             break;
        default :
            {
#ifdef _DEBUG
                cout << "image_general::alike: attempting to create alike as unsupported data type (" << unit << ")" << endl;
#endif
            }
            break;
        }

    return new_volume;
    }

//template <class ELEMTYPE, int IMAGEDIM>
//    template <class newType> 
//image_general<newType> * image_general<ELEMTYPE, IMAGEDIM>::alike ()
//    {
//    image_general<newType, IMAGEDIM> * new_volume = NULL;
//
//    new_volume = new image_general<newType, IMAGEDIM>();
//
//    new_volume->initialize_dataset(datasize[0],datasize[1],datasize[2]);
//
//    ITKimportfilter=NULL;
//
//    new_volume->origin          = origin;
//    new_volume->direction       = direction;
//    new_volume->voxel_resize    = voxel_resize;
//
//    new_volume->unit_center     = unit_center;
//    new_volume->unit_to_voxel   = unit_to_voxel;
//
//    return new_volume;
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
        iterator o = begin();
        
        while (i != in->end() && o != end())
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
    template<class SOURCETYPE>
image_general<ELEMTYPE, IMAGEDIM>::image_general(image_general<SOURCETYPE, IMAGEDIM> * old_volume, bool copyData)
    {
    initialize_dataset(old_volume->get_size_by_dim(0), old_volume->get_size_by_dim(1), old_volume->get_size_by_dim(2), false);

    if (copyData)
        { copy_image (old_volume); }
    }

template <class ELEMTYPE, int IMAGEDIM>
image_general<ELEMTYPE, IMAGEDIM>::image_general(itk::SmartPointer< itk::Image<ELEMTYPE, IMAGEDIM > > &i)
    {
    replicate_itk_to_volume(i);
    }

template <class ELEMTYPE, int IMAGEDIM>
image_general<ELEMTYPE, IMAGEDIM>::image_general(int w, int h, int d, ELEMTYPE *ptr)
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
    
    //dimension-indepent loop that may be lifted outside this function
    num_elements=1;
    for (unsigned short i = 0; i < IMAGEDIM; i++) 
        {
        num_elements *= datasize[i];
        }

    imageptr = new ELEMTYPE[num_elements];

    if (ptr!=NULL)
        {memcpy(imageptr,ptr,sizeof(ELEMTYPE)*num_elements);}

    set_parameters();
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::image_has_changed(bool mm_refresh)
    {
    //TODO: some outlandish malfunction in rendererMPR
    //when called with render_thumbnail-generated parameters
    //widget->refresh_thumbnail();

    //data changed, no longer available in a file (not that Mr. Platinum knows of, anyway)
    from_file(false);

    //recalculate min/max
    //with ITK volume data, this is preferrably done with
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
    
    iterator itr = begin();
    while (itr != end())
        {
        val=*itr;
        
        pre_max = max (val, pre_max);
        pre_min = min (val, pre_min);
        
        ++itr;
        }

    //don't change if values don't make sense - 
    //that would be an empty/zero volume
    if (pre_min < pre_max)
        {
        maxvalue=pre_max;
        minvalue=pre_min;
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
    //with ITK volume data, this is preferrably done with
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
    //that would be an empty/zero volume
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

    re_resize=voxel_resize.GetInverse();
    unit_to_voxel=re_resize*datasize_max_norm;
    
    //center of data in unit coordinates where longest edge = 1
    for (unsigned int d=0;d<3;d++)
        {unit_center[d]=voxel_resize[d][d]*datasize[d]/(datasize_max_norm*2);}
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::set_parameters()
    {
    unsigned short datasize_max_norm= max(max((float)datasize[0],(float)datasize[1]),(float)datasize[2]);

    for (unsigned int d=0;d<3;d++)
        {unit_center[d]=(float)datasize[d]/(datasize_max_norm*2);}

    calc_transforms();
    }

template <class ELEMTYPE, int IMAGEDIM>
void  image_general<ELEMTYPE, IMAGEDIM>::replicate_itk_to_volume()
    {
    replicate_itk_to_volume(ITKimportimage);
    }

template <class ELEMTYPE, int IMAGEDIM>
void  image_general<ELEMTYPE, IMAGEDIM>::replicate_itk_to_volume(itk::SmartPointer< itk::Image<ELEMTYPE, IMAGEDIM > > &i)
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
        itk_spacing[d]=voxel_resize[d][d];
        itk_origin[d]=origin[d];
        }

    ITKstart.Fill( 0 );

    ITKregion.SetIndex( ITKstart );
    ITKregion.SetSize(  ITKsize  );
    ITKimportfilter->SetRegion( ITKregion );
    ITKimportfilter->SetOrigin(itk_origin);
    ITKimportfilter->SetSpacing(itk_spacing);

    ITKimportfilter->SetImportPointer( imageptr, num_elements, false);
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
ELEMTYPE image_general<ELEMTYPE, IMAGEDIM>::get_voxel(int x, int y, int z)
    {
    return imageptr[x + datasize[0]*y + datasize[0]*datasize[1]*z];
    }

/*template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_general<ELEMTYPE, IMAGEDIM>::get_voxel(unsigned long offset)
    {
    return imageptr[offset];
    }*/


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::set_voxel(int x, int y, int z, ELEMTYPE voxelvalue)
    {
    imageptr[x + datasize[0]*y + datasize[0]*datasize[1]*z] = voxelvalue;
    }

/*template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::set_voxel(unsigned long offset, ELEMTYPE voxelvalue)
    {
    imageptr[offset] = voxelvalue;
    }*/

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::get_display_voxel(RGBvalue &val,int x, int y, int z)
    {
    val.set_mono(255*(get_voxel (x, y, z)-minvalue)/(maxvalue-minvalue));
    }

template <class ELEMTYPE, int IMAGEDIM>
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
    }

template <class ELEMTYPE, int IMAGEDIM>
float image_general<ELEMTYPE, IMAGEDIM>::get_number_voxel(int x, int y, int z)
    {
    return static_cast<float>(get_voxel (x, y, z));
    }

// Direction=2 -> u=x, v=y, w=z Direction=1 -> u=x, v=z, w=y Direction=0 -> u=y, v=z, w=x
template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_general<ELEMTYPE, IMAGEDIM>::get_voxel_by_dir(int u, int v, int w, int direction)
	{
	if(direction==0)//Loop over x
		return get_voxel(w,u,v);
	if(direction==1)//Loop over y
		return get_voxel(u,w,v);
	if(direction==2)//Loop over z
		return get_voxel(u,v,w);
	return get_voxel(u,v,w);//This should never happen	
	}
	
// Direction=2 -> u=x, v=y, w=z Direction=1 -> u=x, v=z, w=y Direction=0 -> u=y, v=z, w=x
template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::set_voxel_by_dir(int u, int v, int w, ELEMTYPE value, int direction)
	{
	if(direction==0)//Loop over x
		set_voxel(w,u,v,value);
	if(direction==1)//Loop over y
		set_voxel(u,w,v,value);
	if(direction==2)//Loop over z
		set_voxel(u,v,w,value);
	}

template <class ELEMTYPE, int IMAGEDIM>
float image_general<ELEMTYPE, IMAGEDIM>::get_min()
    {
    return minvalue;
    }

template <class ELEMTYPE, int IMAGEDIM>
float image_general<ELEMTYPE, IMAGEDIM>::get_max()
    {
    return maxvalue;
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

    voxel_resize.Fill(0);

    itk_vox_size=i->GetSpacing();
    itk_origin=i->GetOrigin ();
    itk_orientation=i->GetDirection();

    float spacing_min_norm=static_cast<float>(itk_vox_size[0]);
    for (unsigned int d=0;d<IMAGEDIM;d++)
        {
        spacing_min_norm=min(spacing_min_norm,static_cast<float>(itk_vox_size[d]));
        voxel_resize[d][d]=itk_vox_size[d];
        origin[d]=itk_origin[d];

        //orthogonal-only renderer can't handle arbitrary volume orientations
#ifdef RENDER_ORTHOGONALLY_ONLY
        for (unsigned int c=0;c<3;c++)
            {direction[d][c]=round(itk_orientation[d][c]);}
#else
        for (unsigned int c=0;c<3;c++)
            {direction[d][c]=itk_orientation[d][c];}
#endif
        }

    //scale to shortest dimension=1
    voxel_resize/=spacing_min_norm;

    //longest edge
    unsigned short datasize_max_norm=max(max((float)datasize[0],(float)datasize[1]),(float)datasize[2]);

    if (voxel_resize[0][0]==1.0 && voxel_resize[1][1]==1.0 && voxel_resize[2][2]==1.0)
        {
        //cubic voxels may indicate that voxel size info is missing, use
        //heuristic:
        //assume a voxel size that makes the volume as deep as its tallest side,
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

    minvalue            = statsFilter->GetMinimum();
    ELEMTYPE new_max  = statsFilter->GetMaximum();

    //we don't want to lose pixel-data correspondence by scaling chars,
    //don't alter max/min in that case
    //note: scaling is for display only

    //we *could* scale dynamically narrow 8-bit volumes as well,
    //but it's a question if what the viewer expects
    //volumes with just a few classes (20) starting with 0
    //(eg. binary) still get their scaling

    if (new_max - minvalue > 255 || (minvalue ==0 && new_max -minvalue < 20) )
        {
        maxvalue=new_max;
        }
    else
        {
        maxvalue=minvalue + 255;
        }
    }

template <class ELEMTYPE, int IMAGEDIM>
image_binary<IMAGEDIM> * image_general<ELEMTYPE, IMAGEDIM>::threshold(ELEMTYPE low, ELEMTYPE high, bool true_inside_threshold)
{
    image_binary<IMAGEDIM> * output = new image_binary<IMAGEDIM> (this,false);
    
    /*unsigned long i;
	unsigned long n_voxels=get_n_voxels()*/
        ELEMTYPE p;
    
	/*for(i=0; i<n_voxels; i++)
		{
		p=input->get_voxel(i);
		if(p>=low && p<=high)
			output->set_voxel(i,object_value);
		else
			output->set_voxel(i,!object_value);
		}*/
    
    iterator i = begin();
    typename image_binary<IMAGEDIM>::iterator o = output->begin();
    
    while (i != end()) //images are same size and
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

#endif
