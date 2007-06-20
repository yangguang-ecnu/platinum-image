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
    requestedClass<NEWELEM, DIM> * output = NULL; //dynamic_cast<requestedClass<NEWELEM, DIM> *> (input);

    image_general <TRYELEM,DIM>* input_general = dynamic_cast<image_general <TRYELEM, DIM> *> (input) ; //! Try to cast into image_general of try-type

    if (input_general != NULL) //! If cast was successful, input had the tried type and input_general can be used in a call to new class' copy constructor
        {
        output = new requestedClass<NEWELEM, DIM> (input_general,true);
        //delete input;
        } 
    return output;
    }

template <template <class, int> class requestedClass, class ELEM, int DIM>
requestedClass<ELEM, DIM>* scalar_copycast (image_base* input)
    {
    requestedClass<ELEM, DIM> * output = NULL; //dynamic_cast<requestedClass<ELEM, DIM > *>(input);

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

    /*if (output == NULL)
        {throw  (bad_cast());}*/

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

    //initialize_dataset(size[0],size[1],size[2]);

    ITKimportfilter=NULL;
    ITKimportimage=NULL;

    this->maxvalue        = sourceImage->get_max();
    this->minvalue        = sourceImage->get_min();

    this->voxel_resize    = sourceImage->get_voxel_resize();
	//origin & direction are copied in data_base... 

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
                std::cout << "image_general::alike: attempting to create alike as unsupported data type (" << unit << ")" << std::endl;
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

/*template <class ELEMTYPE, int IMAGEDIM>
    template <class inType>
void image_general<ELEMTYPE, IMAGEDIM>::copy_image (image_general<inType, IMAGEDIM> * in)
    {
    if ( same_size (in))
        {  
        typename image_general<inType, IMAGEDIM>::iterator i = in->begin();
        typename image_storage<ELEMTYPE >::iterator o = this->begin();
        
        while (i != in->end() && o != this->end())
            {
            *o = *i;
            
            ++i; ++o;
            }
		this->image_has_changed();
        }
    else
        {     
        //total failure, TODO: throw exception
#ifdef _DEBUG
        cout << "Copying image data: image sizes don't match" << std::endl;
#endif
        }
    }*/

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
        { copy_data (old_image,this); }

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

	voxel_resize.SetIdentity(); //A default value of the voxel dimension is...

    //dimension-independent loop that may be lifted outside this function
    this->num_elements=1;
    for (unsigned short i = 0; i < IMAGEDIM; i++) 
        {
        this->num_elements *= datasize[i];
        }

    this->imagepointer( new ELEMTYPE[this->num_elements] );

    if (ptr!=NULL) //memcpy is bad karma! Use copy_data(in, out) whenever you know your (input) datatype!
        {
        memcpy(this->imagepointer(),ptr,sizeof(ELEMTYPE)*this->num_elements);
        
        image_has_changed(true);
        }
        
    set_parameters();
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::image_has_changed(bool stat_refresh)
    {
    //TODO: some outlandish malfunction in rendererMPR
    //when called with render_thumbnail-generated parameters
    //widget->refresh_thumbnail();

    //data changed, no longer available in a file (not that Mr. Platinum knows of, anyway)
    this->from_file(false);

    //recalculate min/max
    if(stat_refresh)
        {
        this->stats_refresh();

        //refresh transfer function
        this->tfunction->refresh();
        }

    //clear ITK connection
    ITKimportfilter = NULL;
    if (ITKimportimage.IsNotNull())
        {ITKimportimage->Delete();}
    }

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
    pt_error::error_if_false (i.IsNotNull (), "Attempting to create ITK image from unitialized image object", pt_error::fatal ); 

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

    ITKimportfilter->SetImportPointer( this->imagepointer(), this->num_elements, false);
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
	void image_general<ELEMTYPE, IMAGEDIM>::set_voxel_resize(float dx, float dy, float dz)
	{
	voxel_resize[0][0]=dx;	
	voxel_resize[1][1]=dy;	
	voxel_resize[2][2]=dz;	
	}


template <class ELEMTYPE, int IMAGEDIM>
bool image_general<ELEMTYPE, IMAGEDIM>::get_voxel_resize_from_dicom_file(std::string dcm_file)
{
	bool succeded = false;
	itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();

	if (dicomIO->CanReadFile(dcm_file.c_str()))
	{
		dicomIO->SetFileName(dcm_file.c_str());
		dicomIO->ReadImageInformation();		//get basic DICOM header
		this->voxel_resize.SetIdentity();
		this->voxel_resize[0][0] = dicomIO->GetSpacing(0);
		this->voxel_resize[1][1] = dicomIO->GetSpacing(1);
		this->voxel_resize[2][2] = dicomIO->GetSpacing(2);
		succeded = true;
	}
	return succeded;
}


template <class ELEMTYPE, int IMAGEDIM>
    Matrix3D image_general<ELEMTYPE, IMAGEDIM>::get_voxel_resize ()
    {
    return voxel_resize;
    }

template <class ELEMTYPE, int IMAGEDIM>
bool image_general<ELEMTYPE, IMAGEDIM>::is_voxelpos_within_image_3D(int vp_x, int vp_y, int vp_z)
	{
	return (vp_x>=0 && vp_y>=0 && vp_z>=0 && vp_x<datasize[0] && vp_y<datasize[1] && vp_z<datasize[2]);
	}

template <class ELEMTYPE, int IMAGEDIM>
bool image_general<ELEMTYPE, IMAGEDIM>::is_voxelpos_within_image_3D(Vector3D vp)
	{
	return is_voxelpos_within_image_3D(vp[0],vp[1],vp[2]);
	}

template <class ELEMTYPE, int IMAGEDIM>
bool image_general<ELEMTYPE, IMAGEDIM>::is_voxelpos_inside_image_border_3D(int vp_x, int vp_y, int vp_z, int dist)
	{
	return (vp_x>=dist && vp_y>=dist && vp_z>=dist && vp_x<datasize[0]-dist && vp_y<datasize[1]-dist && vp_z<datasize[2]-dist);
	}

template <class ELEMTYPE, int IMAGEDIM>
bool image_general<ELEMTYPE, IMAGEDIM>::is_voxelpos_inside_image_border_3D(Vector3D vp, int dist)
	{
	return is_voxelpos_inside_image_border_3D(vp[0],vp[1],vp[2], dist);
	}



template <class ELEMTYPE, int IMAGEDIM>
bool image_general<ELEMTYPE, IMAGEDIM>::is_physical_pos_within_image_3D(Vector3D phys_pos)
	{
	//cout<<" *** is_physical_pos_within_image_3D(Vector3D phys_pos)... *** "<<std::endl;
	//ev. phys_pos = origin + direction*voxel_resize*voxel_pos
	//figure out if (direction*X = phys_pos_of_first_corner) --> are then all X-components positive? and...
	//figure out if (direction*Y = phys_pos_of_last_corner) --> are then all Y-components positive?
	Vector3D phys_pos2 = phys_pos - this->origin; //now we only need to find out the scalar product of...
	Matrix3D dir_inv;
	dir_inv = this->direction.GetInverse();
	Vector3D X = dir_inv*phys_pos2;
//	cout<<"X="<<X<<std::endl;
	bool ret=false;
	if(X[0]>=0 && X[1]>=0 && X[2]>=0)
	{
		phys_pos2 = phys_pos - get_physical_pos_for_voxel(datasize[0]-1,datasize[1]-1,datasize[2]-1);
		X = dir_inv*phys_pos2;
//		cout<<"X2="<<X<<std::endl;
		if(X[0]<=0 && X[1]<=0 && X[2]<=0)
		{
			ret = true;
		}
	}
	
	return ret;
	}


template <class ELEMTYPE, int IMAGEDIM>
Vector3D image_general<ELEMTYPE, IMAGEDIM>::get_voxelpos_from_physical_pos_3D(Vector3D phys_pos)
	{
	Matrix3D a = this->direction*voxel_resize;
	a = a.GetInverse();
	return a*(phys_pos - this->origin);
	}

template <class ELEMTYPE, int IMAGEDIM>
Vector3D image_general<ELEMTYPE, IMAGEDIM>::get_voxelpos_integers_from_physical_pos_3D(Vector3D phys_pos)
	{
	Vector3D vox = get_voxelpos_from_physical_pos_3D(phys_pos);
	Vector3D vox2;
	vox2[0] = round<float>(vox[0]);
	vox2[1] = round<float>(vox[1]);
	vox2[2] = round<float>(vox[2]);
	return vox2;
	}

template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_general<ELEMTYPE, IMAGEDIM>::get_voxel(int x, int y, int z)
    {
    return this->dataptr[x + datasize[0]*y + datasize[0]*datasize[1]*z];
    }

template <class ELEMTYPE, int IMAGEDIM> //JK3
ELEMTYPE image_general<ELEMTYPE, IMAGEDIM>::get_voxel_in_physical_pos(Vector3D phys_pos)
    {
	Vector3D v = get_voxelpos_from_physical_pos_3D(phys_pos);
	return (is_voxelpos_within_image_3D(v[0],v[1],v[2]))? get_voxel(v[0],v[1],v[2]):0;
    }

template <class ELEMTYPE, int IMAGEDIM> //JK3
ELEMTYPE image_general<ELEMTYPE, IMAGEDIM>::get_voxel_in_physical_pos_mean_3D_interp26(Vector3D phys_pos)
    {
	Vector3D v = get_voxelpos_from_physical_pos_3D(phys_pos);
	Vector3D cv;
	cv[0]=int(v[0]); cv[1]=int(v[1]); cv[2]=int(v[2]);	//center voxel to interpolate around...

	ELEMTYPE sum=0;
	Vector3D tmp;
	for(int i=-1;i<=1;i++){
		for(int j=-1;j<=1;j++){
			for(int k=-1;k<=1;k++){
				tmp[0]=cv[0]+i;
				tmp[1]=cv[1]+j;
				tmp[2]=cv[2]+k;
				if(is_voxelpos_within_image_3D(tmp[0],tmp[1],tmp[2]))
					{
					sum += get_voxel(tmp[0],tmp[1],tmp[2]);
					}
				}
			}
		}
	ELEMTYPE res = sum/26.0;
	std::cout<<"v="<<v<<" cv="<<cv<<" res="<<res<<std::endl;

    return res;
    }

template <class ELEMTYPE, int IMAGEDIM> //JK3
ELEMTYPE image_general<ELEMTYPE, IMAGEDIM>::get_voxel_in_physical_pos_26NB_weighted(Vector3D phys_pos, float w1, float w2, float w3, float w4)
    {
	Vector3D v = get_voxelpos_from_physical_pos_3D(phys_pos);
	Vector3D cv;
	cv[0]=int(v[0]); cv[1]=int(v[1]); cv[2]=int(v[2]);	//center voxel to interpolate around...

	ELEMTYPE sum=0;
	Vector3D tmp;
	ELEMTYPE res = 0;
	
	if(is_voxelpos_within_image_3D(cv[0],cv[1],cv[2])){

		//Center Pixel
		sum += w1*get_voxel(cv[0],cv[1],cv[2]);

		//Side Neighbours
		sum += w2*get_voxel(cv[0]-1,cv[1],cv[2]);	//left
		sum += w2*get_voxel(cv[0]+1,cv[1],cv[2]);	//right
		sum += w2*get_voxel(cv[0],cv[1]-1,cv[2]);	//top
		sum += w2*get_voxel(cv[0],cv[1]+1,cv[2]);	//bottom
		sum += w2*get_voxel(cv[0],cv[1],cv[2]-1);	//under
		sum += w2*get_voxel(cv[0],cv[1],cv[2]+1);	//over

		//Edge Neighbours
		for(int k=-1;k<=1;k=k+2){	//"under" and "over" layers...
			sum += w3*get_voxel(cv[0]-1,cv[1],cv[2]+k);
			sum += w3*get_voxel(cv[0]+1,cv[1],cv[2]+k);
			sum += w3*get_voxel(cv[0],cv[1]-1,cv[2]+k);
			sum += w3*get_voxel(cv[0],cv[1]+1,cv[2]+k);
		}
		//"center" layer...
		sum += w3*get_voxel(cv[0]-1,cv[1]-1,cv[2]);
		sum += w3*get_voxel(cv[0]-1,cv[1]+1,cv[2]);
		sum += w3*get_voxel(cv[0]+1,cv[1]-1,cv[2]);
		sum += w3*get_voxel(cv[0]+1,cv[1]+1,cv[2]);


		//Corner Neighbours
		for(int i=-1;i<=1;i=i+2){
			for(int j=-1;j<=1;j=j+2){
				for(int k=-1;k<=1;k=k+2){
					tmp[0]=cv[0]+i;	tmp[1]=cv[1]+j;	tmp[2]=cv[2]+k;
					if(is_voxelpos_within_image_3D(tmp[0],tmp[1],tmp[2])){
						sum += w4*get_voxel(tmp[0],tmp[1],tmp[2]);
					}
				}
			}
		}
	}
	res = sum/(w1 + 6.0*w2 + 12*w3 + 8*w4);
	std::cout<<"v="<<v<<" cv="<<cv<<" res="<<res<<std::endl;

    return res;
    }






/*template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_general<ELEMTYPE, IMAGEDIM>::get_voxel(unsigned long offset)
    {
    return imagepointer()[offset];
    }*/

template <class ELEMTYPE, int IMAGEDIM>
Vector3D image_general<ELEMTYPE, IMAGEDIM>::get_physical_pos_for_voxel(int x, int y, int z)
{
	Vector3D vox_pos; vox_pos[0]=x; vox_pos[1]=y; vox_pos[2]=z;
	Vector3D phys_pos;
	phys_pos = this->origin + this->direction*this->voxel_resize*vox_pos;
	return phys_pos;
}



template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::set_voxel(int x, int y, int z, ELEMTYPE voxelvalue)
    {
    this->dataptr[x + datasize[0]*y + datasize[0]*datasize[1]*z] = voxelvalue;
    }

/*template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::set_voxel(unsigned long offset, ELEMTYPE voxelvalue)
    {
    imagepointer()[offset] = voxelvalue;
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
double image_general<ELEMTYPE, IMAGEDIM>::get_num_diff_1storder_central_diff_3D(int x, int y, int z, int direction)
{	
	if(direction==0)
		return 0.5*double(get_voxel(x+1,y,z)-get_voxel(x-1,y,z));
	if(direction==1)
		return 0.5*double(get_voxel(x,y+1,z)-get_voxel(x,y-1,z));
	return 0.5*double(get_voxel(x,y,z+1)-get_voxel(x,y,z-1));
}

template <class ELEMTYPE, int IMAGEDIM>
double image_general<ELEMTYPE, IMAGEDIM>::get_num_diff_2ndorder_central_diff_3D(int x, int y, int z, int direction1, int direction2)
{	
	if(direction2==0)
		return 0.5*(get_num_diff_1storder_central_diff_3D(x+1,y,z,direction1)-get_num_diff_1storder_central_diff_3D(x-1,y,z,direction1));
	if(direction2==1)
		return 0.5*(get_num_diff_1storder_central_diff_3D(x,y+1,z,direction1)-get_num_diff_1storder_central_diff_3D(x,y-1,z,direction1));
	return 0.5*(get_num_diff_1storder_central_diff_3D(x,y,z+1,direction1)-get_num_diff_1storder_central_diff_3D(x,y,z-1,direction1));
}

template <class ELEMTYPE, int IMAGEDIM>
double image_general<ELEMTYPE, IMAGEDIM>::get_num_diff_3rdorder_central_diff_3D(int x, int y, int z, int direction1, int direction2, int direction3)
{	
	if(direction3==0)
		return 0.5*(get_num_diff_2ndorder_central_diff_3D(x+1,y,z,direction1,direction2)-get_num_diff_2ndorder_central_diff_3D(x-1,y,z,direction1,direction2));
	if(direction3==1)
		return 0.5*(get_num_diff_2ndorder_central_diff_3D(x,y+1,z,direction1,direction2)-get_num_diff_2ndorder_central_diff_3D(x,y-1,z,direction1,direction2));
	return 0.5*(get_num_diff_2ndorder_central_diff_3D(x,y,z+1,direction1,direction2)-get_num_diff_2ndorder_central_diff_3D(x,y,z-1,direction1,direction2));
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
void image_general<ELEMTYPE, IMAGEDIM>::set_value_to_voxels_in_region(int x, int y, int z, int dx, int dy, int dz, ELEMTYPE value)
{
	for (int k=z; k < z+dz; k++){
		for (int j=y; j < y+dy; j++){
			for (int i=x; i < x+dx; i++){
                set_voxel(i,j,k,value);
			}
		}
	}
}


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::testpattern()
    {
    std::cout << "Testpattern: " << datasize[2] << " " << datasize[1] << " " << datasize[0] << std::endl;
    //gradient test pattern
    for (int z=0; z < datasize[2]; z++)
        for (int y=0; y < datasize[1]; y++)
            for (int x=0; x < datasize[0]; x++)
                {
                set_voxel(x,y,z, int(float(x+y+z)*255.0/float(datasize[2]+datasize[1]+datasize[0])));
                }

	this->image_has_changed(true);
    }

//JK3
template <class ELEMTYPE, int IMAGEDIM>
template<class TARGETTYPE> 
void image_general<ELEMTYPE, IMAGEDIM>::resample_into_this_image_NN(image_general<TARGETTYPE, 3> * new_image)
{
	std::cout<<"image_general::resample_into_this_image"<<std::endl;
	this->print_geometry();
	new_image->print_geometry();

	Vector3D vox_pos;
	Vector3D phys_pos;
	float res;
    for (int z=0; z < new_image->datasize[2]; z++)
        {
        for (int y=0; y < new_image->datasize[1]; y++)
            {
            for (int x=0; x < new_image->datasize[0]; x++)
                {  
				vox_pos[0] = x;	vox_pos[1] = y;	vox_pos[2] = z;
				phys_pos = new_image->origin + new_image->voxel_resize*new_image->direction*vox_pos;
//				cout<<"phys_pos="<<phys_pos<<std::endl;
				res = get_voxel_in_physical_pos_26NB_weighted(phys_pos,10,2,0,0);
//				res = get_voxel_in_physical_pos_mean_3D_interp26(phys_pos);
//				res = get_voxel_in_physical_pos(phys_pos);
//				cout<<"res="<<res<<std::endl;
				new_image->set_voxel(x,y,z,res);
                }
            }
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
void image_general<ELEMTYPE, IMAGEDIM>::set_geometry(float ox,float oy,float oz,float dx,float dy,float dz,float fi_x,float fi_y,float fi_z)
{
	this->origin[0]=ox;
	this->origin[1]=oy;
	this->origin[2]=oz;
	this->set_voxel_resize(dx,dy,dz);
	this->direction.SetIdentity();
	this->rotate(fi_z,fi_y,fi_x);
}
			

template <class ELEMTYPE, int IMAGEDIM>
bool image_general<ELEMTYPE, IMAGEDIM>::get_geometry_from_dicom_file(std::string dcm_file)
{
	std::cout<<"get_geometry_from_dicom_file"<<std::endl;
	bool b1 = this->get_origin_from_dicom_file(dcm_file);
	bool b2 = this->get_direction_from_dicom_file(dcm_file);
	bool b3 = this->get_voxel_resize_from_dicom_file(dcm_file);
	return b1&&b2&&b3;
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::print_geometry()
{
	std::cout<<"************"<<std::endl;
	std::cout<< this->name()<<"->print_geometry()"<<std::endl;
	std::cout<<"datasize: (";
	for(int i=0;i<IMAGEDIM;i++)
		std::cout<<datasize[i]<<",";
	std::cout<<")"<<std::endl;
	std::cout<<"origin:"<<this->origin<<std::endl;
	std::cout<<"voxel_resize:"<<std::endl<<get_voxel_resize()<<std::endl;
	std::cout<<"direction:"<<std::endl;
	std::cout<<this->direction[0][0]<<" "<<this->direction[1][0]<<" "<<this->direction[2][0]<<std::endl;
	std::cout<<this->direction[0][1]<<" "<<this->direction[1][1]<<" "<<this->direction[2][1]<<std::endl;
	std::cout<<this->direction[0][2]<<" "<<this->direction[1][2]<<" "<<this->direction[2][2]<<std::endl;
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::print_physical_corner_coords()
{
	std::cout<<"***************************************"<<std::endl;
	std::cout<<name()<<"->print_physical_corner_coords()"<<std::endl;
	std::cout<<		get_physical_pos_for_voxel(0,				0,				datasize[2]-1);
	std::cout<<" "<<	get_physical_pos_for_voxel(datasize[0]-1,	0,				datasize[2]-1);
	std::cout<<std::endl;
	std::cout<<		get_physical_pos_for_voxel(0,				datasize[1]-1,	datasize[2]-1);
	std::cout<<" "<<	get_physical_pos_for_voxel(datasize[0]-1,	datasize[1]-1,	datasize[2]-1);
	std::cout<<std::endl;
	std::cout<<std::endl;
	std::cout<<		get_physical_pos_for_voxel(0,				0,				0);
	std::cout<<" "<<	get_physical_pos_for_voxel(datasize[0]-1,	0,				0);
	std::cout<<std::endl;
	std::cout<<		get_physical_pos_for_voxel(0,				datasize[1]-1,	0);
	std::cout<<" "<<	get_physical_pos_for_voxel(datasize[0]-1,	datasize[1]-1,	0);
	std::cout<<std::endl;
}

#endif