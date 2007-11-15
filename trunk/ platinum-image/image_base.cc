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

//#include "image_base.h"
#include "datawidget.h"

#include "bruker.h"

#include "image_label.hxx"
#include "image_complex.hxx"

//#include "itkVTKImageIO.h"

using namespace std;

//#include "fileutils.h"
//#include "image_integer.hxx"
#include "viewmanager.h"
#include "datamanager.h"
#include "rendermanager.h"

extern datamanager datamanagement;
extern rendermanager rendermanagement;
extern viewmanager viewmanagement;

image_base::image_base():data_base()
    {set_parameters ();}

image_base::image_base(image_base* const s):data_base(s)
    {
    set_parameters ();
    //setting copy name at the root would be neat,
    //but is not possible since the widget isn't
    //created yet
    
    origin = s->get_origin();
    orientation = s->get_orientation();
	
    name ("Copy of " + s->name());
    }

void image_base::set_parameters ()    
    {
    ostringstream namestream;
    
    origin.Fill(0);
    orientation.SetIdentity();

    //constructor: add "Untitled" name and ID
    namestream << "3D image (" << ID << ")";

    Fl_Group::current(NULL); //evil bugfix: somehwere, sometime, a Group has not been ended
    
	widget=new datawidget<image_base>(this,namestream.str());
    name(namestream.str());
    }

void image_base::rotate(float fi_z,float fi_y,float fi_x) //Is there a good reason for the z,y,x ordering?
{
    matrix_generator mg;
    orientation = mg.get_rot_matrix_3D(fi_z,fi_y,fi_x)*orientation;
}

bool image_base::read_origin_from_dicom_file(std::string dcm_file)
{
	bool succeded = false;
	itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();
    
	if (dicomIO->CanReadFile(dcm_file.c_str()))
        {
		dicomIO->SetFileName(dcm_file.c_str());
		dicomIO->ReadImageInformation();		//get basic DICOM header
		this->origin[0] = float(dicomIO->GetOrigin(0));
		this->origin[1] = float(dicomIO->GetOrigin(1));
		this->origin[2] = float(dicomIO->GetOrigin(2));
		succeded = true;
        }
	return succeded;
}

bool image_base::read_orientation_from_dicom_file(std::string dcm_file)
{
	bool succeded = false;
	itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();
    
	if (dicomIO->CanReadFile(dcm_file.c_str()))
        {
		dicomIO->SetFileName(dcm_file.c_str());
		dicomIO->ReadImageInformation();		//get basic DICOM header
		std::vector<double> a = dicomIO->GetDirection(0);
		std::vector<double> b = dicomIO->GetDirection(1);
		std::vector<double> c = dicomIO->GetDirection(2);
		this->orientation[0][0] = a[0];
		this->orientation[0][1] = b[0];
		this->orientation[0][2] = c[0];
		this->orientation[1][0] = a[1];
		this->orientation[1][1] = b[1];
		this->orientation[1][2] = c[1];
		this->orientation[2][0] = a[2];
		this->orientation[2][1] = b[2];
		this->orientation[2][2] = c[2];
		succeded = true;
        }
	return succeded;
}

void image_base::redraw()
    {
    rendermanagement.data_has_changed(ID);
    }

Matrix3D image_base::get_orientation () const
{
    return orientation;
}

void image_base::set_orientation(const Matrix3D m)
{
    orientation = m;
}

Vector3D image_base::get_origin () const
{
    return origin;
}

void image_base::set_origin(const Vector3D v)
{
    origin = v;
}

void image_base::save_histogram_to_txt_file(const std::string filename, const std::string separator)
    {
        pt_error::pt_error ("Attempt to save_histogram_to_txt_file on a image_base object",pt_error::warning);
    }
	
Vector3D image_base::world_to_voxel(const Vector3D wpos) const
{
	Vector3D vPos = wpos - origin;

	Matrix3D inv_orientation;
	inv_orientation = get_orientation().GetInverse();
	
	Matrix3D inv_voxel_resize;
	inv_voxel_resize = get_voxel_resize().GetInverse();

	vPos = inv_voxel_resize * inv_orientation * vPos;	// the operations are done from right to left
	
	return vPos;
}

class vtkloader: public imageloader
{
private:
    itk::VTKImageIO::Pointer vtkIO;
    
public:
    vtkloader (std::vector<std::string> *);
    image_base * read ();
};

/*
class dicomloader: public imageloader
{
private:
    itk::GDCMImageIO::Pointer dicomIO;
    
	std::vector<std::string> loaded_series; //! UIDs of the DICOM series loaded during this call
                                  //! to prevent multiple selected frames
                                  //! from loading the same series multiple times
public:
    dicomloader (std::vector<std::string> *);
    image_base * read ();
};
*/

vtkloader::vtkloader(std::vector<std::string> * f): imageloader(f)
{
    vtkIO = itk::VTKImageIO::New();
}

image_base *vtkloader::read()
    {    
    image_base * result = NULL;
            
    if (vtkIO->CanReadFile (files->front().c_str()))
        {

        //assumption:
        //File contains image data

        vtkIO->SetFileName(files->front().c_str());

        vtkIO->ReadImageInformation(); 

        itk::ImageIOBase::IOComponentType componentType = vtkIO->GetComponentType();

        //get voxel type
        itk::ImageIOBase::IOPixelType pixelType=vtkIO->GetPixelType();
        
        switch ( pixelType)
            {
            case itk::ImageIOBase::SCALAR:
                //Enumeration values: UCHAR, CHAR, USHORT, SHORT, UINT, INT, ULONG, LONG, FLOAT, DOUBLE

                switch (componentType)
                    {
                    case itk::ImageIOBase::UCHAR:
                        result =  new image_integer<unsigned char>();
                        ((image_integer<unsigned char>*)result)->load_dataset_from_VTK_file(std::string(files->front()));
                        break;
                    case itk::ImageIOBase::USHORT:
                        result = new image_integer<unsigned short>();
                        ((image_integer<unsigned short>*)result)->load_dataset_from_VTK_file(std::string(files->front()));
                        break;
                    case itk::ImageIOBase::SHORT:
                        result = new image_integer<short>();
                        ((image_integer<short>*)result)->load_dataset_from_VTK_file(std::string(files->front()));
                        break;
                    case itk::ImageIOBase::FLOAT: //used for example in complex dixon data imported from deadface format (.df)
                        result = new image_integer<float>();
                        ((image_integer<float>*)result)->load_dataset_from_VTK_file(std::string(files->front()));
                        break;
                    default:
                        pt_error::error("Load scalar VTK: unsupported component type: " + vtkIO->GetComponentTypeAsString (componentType), pt_error::warning);
                    }
                break;

                /*case itk::ImageIOBase::COMPLEX:
                switch (componentType)
                {
                case itk::ImageIOBase::UCHAR:
                result = new image_complex<unsigned char>();
                ((image_scalar<unsigned char>*)result)->load_dataset_from_VTK_file(path_parent(*file));
                break;
                case itk::ImageIOBase::USHORT:
                result = new image_complex<unsigned short>();
                ((image_scalar<unsigned short>*)result)->load_dataset_from_VTK_file(path_parent(*file));
                break;

                case itk::ImageIOBase::SHORT:
                result = new image_complex<short>();
                ((image_scalar<short>*)result)->load_dataset_from_VTK_file(path_parent(*file));
                break;
                default:
                #ifdef _DEBUG
                cout << "Load complex VTK: unsupported component type: " << vtkIO->GetComponentTypeAsString (componentType) << endl;
                #endif
                }*/
                break;
            default:
                pt_error::error("image_base::load(...): unsupported pixel type: " + vtkIO->GetPixelTypeAsString(pixelType), pt_error::warning);
				;

            }
        //file was read - remove from list
        files->erase (files->begin());
        }
    
    /*delete file;*/

    return result;
    }

dicomloader::dicomloader (std::vector<std::string> * f): imageloader(f)
{
    dicomIO = itk::GDCMImageIO::New();
}

image_base * dicomloader::read()
{    
    image_base * result = NULL;
    
    for (std::vector<std::string>::const_iterator file = files->begin();file != files->end() && result == NULL;file++) // Repeat until one image has been read
        {
        if (dicomIO->CanReadFile (file->c_str()))
            {
            dicomIO->SetFileName(file->c_str());

			//get basic DICOM header
            dicomIO->ReadImageInformation();
            
            //get series UID
            std::string seriesIdentifier;
            
            //"0020|000e" - Series Instance UID (series defined by the scanner)
            //series ID identifies the series (out of possibly multiple series in
            //one directory)
            std::string tagkey = "0020|000e";
            
            std::string labelId;
            if( itk::GDCMImageIO::GetLabelFromTag( tagkey, labelId ) )
                {
                //std::cout << labelId << " (" << tagkey << "): ";
                if( dicomIO->GetValueFromTag(tagkey, seriesIdentifier) )
                    {
					
                    //remove one garbage char at end
					seriesIdentifier = seriesIdentifier.c_str();
					
                    //check if another file in the same series was part of the
                    //selection (and loaded)
					std::vector<string>::const_iterator series_itr=loaded_series.begin();
                    bool already_loaded=false;
                    
                    if (find(loaded_series.begin(),loaded_series.end(),seriesIdentifier)
                        == loaded_series.end())
                        {loaded_series.push_back(seriesIdentifier);}
                    else
                        {already_loaded = true; }
                    //std::cout << seriesIdentifier << endl;

                    //get voxel type
                    itk::ImageIOBase::IOPixelType pixelType=dicomIO->GetPixelType();
                    
                    if (!already_loaded) 
                        {
                        itk::ImageIOBase::IOComponentType componentType = dicomIO->GetComponentType();
                        switch ( pixelType)
                            {
                            case itk::ImageIOBase::SCALAR:
                                
                                
                                //Enumeration values: UCHAR, CHAR, USHORT, SHORT, UINT, INT, ULONG, LONG, FLOAT, DOUBLE
                                
                                switch (componentType)
                                    {
                                    case itk::ImageIOBase::UCHAR:
                                        result = new image_integer<unsigned char>();
                                        ((image_integer<unsigned char>*)result)->load_dataset_from_DICOM_filesAF(path_parent(*file),seriesIdentifier);
                                        break;
                                    case itk::ImageIOBase::USHORT:
                                        result = new image_integer<unsigned short>();
                                        ((image_integer<unsigned short>*)result)->load_dataset_from_DICOM_filesAF(path_parent(*file),seriesIdentifier);
                                        break;
                                    case itk::ImageIOBase::SHORT:
                                        result = new image_integer<short>();
                                        ((image_integer<short>*)result)->load_dataset_from_DICOM_filesAF(path_parent(*file),seriesIdentifier);
                                        break;
									case itk::ImageIOBase::FLOAT:
                                        result = new image_integer<float>();
                                        ((image_integer<float>*)result)->load_dataset_from_DICOM_filesAF(path_parent(*file),seriesIdentifier);
                                        break;
                                    default:
                                        pt_error::error("dicomloader::read() --> Unsupported component type: " + dicomIO->GetComponentTypeAsString (componentType), pt_error::warning);
										;
                                    }
                                break;
                            case itk::ImageIOBase::COMPLEX:
                                break;
                            default:
                                pt_error::error("image_base::load(...): unsupported pixel type: " + dicomIO->GetPixelTypeAsString(pixelType),pt_error::warning);
								;
                                
                            }
                        
                        }//not already loaded
                    
                    } //found series tag
                
                }//series tag exists
            else
                {
                //no series identifier, OK if the intention is to just load 1 frame
                //(DICOM files can only contain 1 frame each)

                pt_error::error("(No Value Found in File)",pt_error::notice);
                }

            files->clear(); //! if at least one file can be read, the rest are assumed to be part of the same series (or otherwise superfluos)
            }
        }
    
    return result;
}

class analyze_hdrloader: public imageloader 
{
private:
	int buf2int(unsigned char* buf);
	short buf2short(unsigned char* buf);
public:
    analyze_hdrloader (std::vector<std::string> *);
    image_base * read ();
};

analyze_hdrloader::analyze_hdrloader(std::vector<std::string> * files): imageloader(files)
	{
	}

int analyze_hdrloader::buf2int(unsigned char* buf)
	{
	int res=buf[0]<<24|buf[1]<<16|buf[2]<<8|buf[3];
	return res;
	}

short analyze_hdrloader::buf2short(unsigned char* buf)
	{
	short res=buf[0]<<8|buf[1];
	return res;
	}

image_base * analyze_hdrloader::read()
    {
    image_base * newImage = NULL;
	std::string hdr_file = files->front();
	std::string img_file = hdr_file;
	std::string img_name;
    
    unsigned int pos;
    
    pos=img_file.rfind(".hdr",img_file.length()-1);
    
    if (pos !=std::string::npos)
        {
        img_file.erase(pos,img_file.length());
        pos=img_file.rfind("/",img_file.length()-1);
        img_name = img_file.substr(pos+1);
        img_file += ".img";

        if (file_exists (img_file) && file_exists (hdr_file))
            {
            short size[3];
            Vector3D voxelsize;
            bool bigEndian;
            bool isSigned;
            bool isFloat;
            int bitDepth;

            std::ifstream hdr (std::string(hdr_file).c_str(),ios::in);
            int count=0;
            unsigned char readbuf[100];
            hdr.read((char*)readbuf,4);
            int sizeof_hdr=buf2int(readbuf);
            count+=4;
			hdr.ignore(36); //Skip
            hdr.read((char*)readbuf,2);
            short endian=buf2short(readbuf);
            bigEndian = ((endian >= 0) && (endian <= 15));
            hdr.read((char*)readbuf,2);
            size[0]=buf2short(readbuf);
            hdr.read((char*)readbuf,2);
            size[1]=buf2short(readbuf);
            hdr.read((char*)readbuf,2);
            size[2]=buf2short(readbuf);
			hdr.ignore(22); //Skip
            hdr.read((char*)readbuf,2);
            short datatype=buf2short(readbuf);
            switch (datatype) {	      
                case 2:
                    isSigned=false;
                    isFloat=false;
                    bitDepth=8;
                    break;
                case 4:
                    isSigned=true;
                    isFloat=false;
                    bitDepth=16;
                    break;
                case 8:
                    isSigned=true;
                    isFloat=false;
                    bitDepth=32;
                    break; 
                case 16:
                    isSigned=true;
                    isFloat=true;
                    bitDepth=32;
                    break; 
                case 128:
                    isSigned=false;
                    isFloat=false;
                    bitDepth=24;
                    break; 
                default:
                    isSigned=false;
                    isFloat=false;
                    bitDepth=8;					// DT_UNKNOWN
                }

            voxelsize.Fill (1);

            if(isFloat)
                {
                newImage = allocate_image<image_scalar> (
                    isFloat,
                    isSigned,
                    bitDepth,
                    std::vector<std::string > (1,img_file), 
                    size[0], size[1],
                    bigEndian,
                    0, 
                    voxelsize);
                }
            else
                {
                newImage = allocate_image<image_integer> (
                    isFloat,
                    isSigned,
                    bitDepth,
                    std::vector<std::string > (1,img_file), 
                    size[0], size[1],
                    bigEndian,
                    0, 
                    voxelsize);
                }
            hdr.close();
            newImage->name(img_name);

            //file was read - remove from list
            files->erase (files->begin());
            }
        }

    return newImage;
    }

class analyze_objloader: public imageloader 
{
public:
    analyze_objloader (std::vector<std::string> *);
    image_base * read ();
};

analyze_objloader::analyze_objloader(std::vector<std::string> * files): imageloader(files)
	{
	}

image_base * analyze_objloader::read()
    {
    image_base * newImage = NULL;
	std::string obj_file = files->front();
	std::string img_name = obj_file;
    
    unsigned int pos;
    
    pos=img_name.rfind(".obj",img_name.length()-1);
    
    if (pos !=std::string::npos)
        {
        img_name.erase(pos,img_name.length());
		pos=img_name.rfind("/",img_name.length()-1);
		img_name = img_name.substr(pos+1);
    
		if (file_exists (obj_file))
			{
			short size[3];
			
			int nObjects=0;
			int skip=0;

			int corr=4;//3;//-1; ???????????????????????
			std::ifstream fobj (std::string(obj_file).c_str());
			unsigned char  header[20];
			fobj.read((char*)header,20);

			int data[5];
			int i;
			for(i=0; i<5; i++)
				{
				data[i]=header[i*4+0]<<24 | header[i*4+1]<<16 | header[i*4+2]<<8 | header[i*4+3];
				}
			size[0]=data[1];
			//size[1]=data[3];//Analyze6
			//size[2]=data[2];//Analyze6
			size[1]=data[2];//Analyze7
			size[2]=data[3];//Analyze7
			nObjects=data[4];
			skip=nObjects*152+corr;//TODO: This is not correct		
			int n_vox=size[0]*size[1]*size[2];

			unsigned char* buf=new unsigned char[n_vox];
			memset(buf,0,n_vox);

			// Start reading run-lenght encoded data
			fobj.ignore(skip);
			int l,b;
			int j;
			i=0;
			while(fobj.good())     // loop while extraction from file is possible
				{				
				l=fobj.get();	
				if(fobj.good())
					{
					b=fobj.get();
					for(j=0; j<l; j++)
						{
						buf[i]=b;
						i++;
						}
					}
				}

			Vector3D voxelsize;
			voxelsize.Fill(1);
			newImage = new image_label<>(buf, n_vox, size[0], size[1], voxelsize);

			fobj.close();
			newImage->name(img_name);

			files->erase (files->begin());
			}
		}

    return newImage;
    }

template <class LOADERTYPE>
void image_base::try_loader (std::vector<std::string> * f) //! helper for image_base::load
{
	if (!f->empty())
	{
		LOADERTYPE loader = LOADERTYPE(f);
		image_base *new_image = NULL; //the eventually loaded image

		do {
			new_image = loader.read();
			if (new_image != NULL)
			{ datamanagement.add(new_image); }
		} 
		while (new_image !=NULL && !f->empty());
	}
}

void image_base::load( std::vector<std::string> f)
    {
    std::vector<std::string> chosen_files(f);
    
    //try Analyze obj
    try_loader<analyze_objloader>(&chosen_files);

    //try Analyze hdr
    try_loader<analyze_hdrloader>(&chosen_files);

    //try Bruker
    try_loader<brukerloader>(&chosen_files);

    //try VTK
    try_loader<vtkloader>(&chosen_files);

    //try DICOM
    try_loader<dicomloader>(&chosen_files);

    if ( !chosen_files.empty() )
        {
        //if any files were left, try raw as last resort

        rawimporter::create(chosen_files);
        }
    }

