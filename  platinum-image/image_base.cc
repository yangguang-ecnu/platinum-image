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

#include "image_base.h"

#include "bruker.h"

using namespace std;

//#include "fileutils.h"
#include "image_integer.h"
#include "image_complex.h"
#include "viewmanager.h"
#include "datamanager.h"
#include "rendermanager.h"

extern datamanager datamanagement;
extern rendermanager rendermanagement;
extern viewmanager viewmanagement;

static int imagemaxID = 1;

image_base::image_base():data_base()
    {set_parameters ();}
image_base::image_base(image_base* s):data_base(s)
    {
    set_parameters ();
    //setting copy name at the root would be neat,
    //but is not possible since the widget isn't
    //created yet

    name ("Copy of " + s->name());
    }

void image_base::set_parameters ()    
    {
    ostringstream namestream;

    ID = imagemaxID++;

    //constructor: add "Untitled" name and ID
    namestream << "3D image (" << ID << ")";

    widget=new datawidget(DATAHANDLER_VOLUME_3D,ID,namestream.str());
    name(namestream.str());
    }


Vector3D image_base::transform_unit_to_voxel(Vector3D pos)
    {
    Vector3D vox;

    vox=pos+unit_center_;
    vox=unit_to_voxel_*vox;

    return vox;
    }

//enum fileFormatType {FILE_FORMAT_DICOM,FILE_FORMAT_VTK};

// //meta-helper to load templated classes from file format of choice
//template <class imageClass>
//void load_image (imageClass img,fileFormatType format)
//    {
//    if (format == FILE_FORMAT_VTK)
//        {img->load_dataset_from_VTK_file(*file);}
//    if (format == FILE_FORMAT_DICOM)
//        {img->load_dataset_from_DICOM_files(*file);}
//    }

//
// //helper that creates subclass objects from ITK voxel/data type constants
//template <class imageClass>
//imageClass * new_image (fileFormatType format, itk::ImageIOBase::IOPixelType dataType)
//    {
//    imageClass *  newImage = NULL;
//
//    switch (dataType)
//        {
//        case itk::ImageIOBase::UCHAR:
//            newImage = new imageClass<unsigned char>();
//            if (format == FILE_FORMAT_VTK)
//                {((imageClass<unsigned char>*)newImage)->load_dataset_from_VTK_file(*file);}
//            if (format == FILE_FORMAT_DICOM)
//                {((imageClass<unsigned char>*)newImage)->load_dataset_from_DICOM_files(*file);}
//            break;
//        case itk::ImageIOBase::USHORT:
//            newImage = new imageClass<unsigned short>();
//            if (format == FILE_FORMAT_VTK)
//                {((imageClass<unsigned short>*)newImage)->load_dataset_from_VTK_file(*file);}
//            if (format == FILE_FORMAT_DICOM)
//                {((imageClass<unsigned short>*)newImage)->load_dataset_from_DICOM_files(*file);}
//            break;
//        case itk::ImageIOBase::SHORT:
//            newImage = new imageClass<signed short>();
//            if (format == FILE_FORMAT_VTK)
//                {((imageClass<signed short>*)newImage)->load_dataset_from_VTK_file(*file);}
//            if (format == FILE_FORMAT_DICOM)
//                {((imageClass<signed short>*)newImage)->load_dataset_from_DICOM_files(*file);}
//            break;
//        default:
//            cout << "image_base::new_image: unsupported voxel data type (" << vtkIO->GetComponentTypeAsString (componentType) << ")" << endl;
//        }
//
//    return newImage;
//    }

class vtkloader: public imageloader
{
private:
    itk::VTKImageIO::Pointer vtkIO;
    
public:
    vtkloader (const std::vector<std::string>);
    image_base * read (std::vector<std::string>&);
};

class dicomloader: public imageloader
{
private:
    itk::GDCMImageIO::Pointer dicomIO;
    
    vector<string> loaded_series; //! UIDs of the DICOM series loaded during this call
                                  //! to prevent multiple selected frames
                                  //! from loading the same series multiple times
public:
    dicomloader (const std::vector<std::string>);
    image_base * read (std::vector<std::string>&);
};

vtkloader::vtkloader(const std::vector<std::string> f): imageloader(f)
{
    vtkIO = itk::VTKImageIO::New();
}

image_base *vtkloader::read(std::vector<std::string>& files)
    {    
    image_base * result = NULL;

    std::string file = files.front();

    if (vtkIO->CanReadFile (file.c_str()))
        {
        //assumption:
        //File contains image data

        vtkIO->SetFileName(file.c_str());

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
                        ((image_integer<unsigned char>*)result)->load_dataset_from_VTK_file(file);
                        break;
                    case itk::ImageIOBase::USHORT:
                        result = new image_integer<unsigned short>();
                        ((image_integer<unsigned short>*)result)->load_dataset_from_VTK_file(file);
                        break;

                    case itk::ImageIOBase::SHORT:
                        result = new image_integer<short>();
                        ((image_integer<short>*)result)->load_dataset_from_VTK_file(file);
                        break;
                    default:
#ifdef _DEBUG
                        cout << "Load scalar VTK: unsupported component type: " << vtkIO->GetComponentTypeAsString (componentType) << endl;
#endif
						;
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
#ifdef _DEBUG
                std::cout << "image_base::load(...): unsupported pixel type: " << vtkIO->GetPixelTypeAsString(pixelType) << endl;
#endif
				;

            }

        files.erase (files.begin()); 
        }

    return result;
    }

dicomloader::dicomloader (const std::vector<std::string> f): imageloader(f)
{
    dicomIO = itk::GDCMImageIO::New();
}

image_base *dicomloader::read(std::vector<std::string>& files)
{    
    image_base * result = NULL;
    
    for (std::vector<std::string>::const_iterator file = files.begin();file != files.end() && result == NULL;file++) // Repeat until one image has been read
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
                std::cout << labelId << " (" << tagkey << "): ";
                if( dicomIO->GetValueFromTag(tagkey, seriesIdentifier) )
                    {
                    //remove one garbage char at end
                    seriesIdentifier.erase(seriesIdentifier.length()-1,seriesIdentifier.length());
                    //check if another file in the same series was part of the
                    //selection (and loaded)
                    vector<string>::const_iterator series_itr=loaded_series.begin();
                    bool already_loaded=false;
                    
                    if (find(loaded_series.begin(),loaded_series.end(),seriesIdentifier)
                        == loaded_series.end())
                        {loaded_series.push_back(seriesIdentifier);}
                    else
                        {already_loaded = true; }
#ifdef _DEBUG
                    std::cout << seriesIdentifier << endl;
#endif      
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
                                        ((image_integer<unsigned char>*)result)->load_dataset_from_DICOM_files(path_parent(*file),seriesIdentifier);
                                        break;
                                    case itk::ImageIOBase::USHORT:
                                        result = new image_integer<unsigned short>();
                                        ((image_integer<unsigned short>*)result)->load_dataset_from_DICOM_files(path_parent(*file),seriesIdentifier);
                                        break;
                                        
                                    case itk::ImageIOBase::SHORT:
                                        result = new image_integer<short>();
                                        ((image_integer<short>*)result)->load_dataset_from_DICOM_files(path_parent(*file),seriesIdentifier);
                                        break;
                                    default:
#ifdef _DEBUG
                                        cout << "Unsupported component type: " << dicomIO->GetComponentTypeAsString (componentType) << endl;
#endif
										;
                                    }
                                break;
                            case itk::ImageIOBase::COMPLEX:
                                break;
                            default:
#ifdef _DEBUG
                                std::cout << "image_base::load(...): unsupported pixel type: " << dicomIO->GetPixelTypeAsString(pixelType) << endl;
#endif
								;
                                
                            }
                        
                        }//not already loaded
                    
                    } //found series tag
                
                }//series tag exists
            else
                {
                //no series identifier, OK if the intention is to just load 1 frame
                //(DICOM files can only contain 1 frame each)
#ifdef _DEBUG
                std::cout << "(No Value Found in File)";
#endif
                }

            files.clear(); //! if at least one file can be read, the rest are assumed to be part of the same series (or otherwise superfluos)
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
    analyze_hdrloader (std::vector<std::string>);
    image_base * read (std::vector<std::string>&);
};

analyze_hdrloader::analyze_hdrloader(const std::vector<std::string> files): imageloader(files)
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

image_base * analyze_hdrloader::read(std::vector<std::string> &files)
    {
    image_base * newImage = NULL;
	std::string hdr_file = files.front();
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
        }
    
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
		hdr.read((char*)readbuf,36); //Skip
		hdr.read((char*)readbuf,2);
		short endian=buf2short(readbuf);
		bigEndian = ((endian >= 0) && (endian <= 15));
		hdr.read((char*)readbuf,2);
		size[0]=buf2short(readbuf);
		hdr.read((char*)readbuf,2);
		size[1]=buf2short(readbuf);
		hdr.read((char*)readbuf,2);
		size[2]=buf2short(readbuf);
		hdr.read((char*)readbuf,22); //Skip
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

		files.erase (files.begin());
        }

    return newImage;
    }

template <class LOADERTYPE>
void try_loader (std::vector<std::string> &f) //! helper for image_base::load
    {
    if (!f.empty())
        {
        LOADERTYPE loader = LOADERTYPE(f);
        image_base *new_image = NULL; //the eventually loaded image

        do {
            new_image = loader.read(f);
            if (new_image != NULL)
                { datamanagement.add(new_image); }
            } 
		while (new_image !=NULL);
        }
    }

void image_base::load(std::vector<std::string> chosen_files)
    {
    //try Analyze hdr
    try_loader<analyze_hdrloader>(chosen_files);

    //try Bruker
    try_loader<brukerloader>(chosen_files);

    //try VTK
    try_loader<vtkloader>(chosen_files);

    //try DICOM
    try_loader<dicomloader>(chosen_files);

    if ( !chosen_files.empty() )
        {
        //if any files were left, try raw as last resort

        new rawimporter (chosen_files);
        }
    }

