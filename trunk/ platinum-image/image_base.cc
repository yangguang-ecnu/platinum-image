// $Id $

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

#include <sstream>
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

enum fileFormatType {FILE_FORMAT_DICOM,FILE_FORMAT_VTK};

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

void image_base::load(std::vector<std::string> files)
    {
    //load mode
    enum  {
        LOMO_NOTSET,
        LOMO_ATOMICFILES, //loading individual volume files, such as VTK
        LOMO_DICOM,       //loading DICOM series
        LOMO_RAW          //loading raw series
        }
load_mode = LOMO_NOTSET;

vector<string> raw_files;
vector<string> loaded_series;   //UIDs of the DICOM series loaded during this call,
//to prevent multiple selected frames
//from loading the same series multiple times

for ( std::vector<string>::iterator file = files.begin(); file != files.end(); ++file )
    {
    image_base *avolume=NULL; //the eventually loaded volume(handler)

    itk::VTKImageIO::Pointer vtkIO = itk::VTKImageIO::New();

    if ( (load_mode == LOMO_ATOMICFILES || load_mode == LOMO_NOTSET) && vtkIO->CanReadFile (file->c_str()))
        {
        load_mode = LOMO_ATOMICFILES;

        //for VTK, we want to know
        //voxel type only

        //assumption:
        //File contains volume data

        vtkIO->SetFileName(file->c_str());

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
                        avolume = new image_integer<unsigned char>();
                        ((image_integer<unsigned char>*)avolume)->load_dataset_from_VTK_file(*file);
                        break;
                    case itk::ImageIOBase::USHORT:
                        avolume = new image_integer<unsigned short>();
                        ((image_integer<unsigned short>*)avolume)->load_dataset_from_VTK_file(*file);
                        break;

                    case itk::ImageIOBase::SHORT:
                        avolume = new image_integer<short>();
                        ((image_integer<short>*)avolume)->load_dataset_from_VTK_file(*file);
                        break;
                    default:
#ifdef _DEBUG
                        cout << "Load scalar VTK: unsupported component type: " << vtkIO->GetComponentTypeAsString (componentType) << endl;
#endif
                    }
                break;

            /*case itk::ImageIOBase::COMPLEX:
                switch (componentType)
                    {
                    case itk::ImageIOBase::UCHAR:
                        avolume = new image_complex<unsigned char>();
                        ((image_scalar<unsigned char>*)avolume)->load_dataset_from_VTK_file(path_parent(*file));
                        break;
                    case itk::ImageIOBase::USHORT:
                        avolume = new image_complex<unsigned short>();
                        ((image_scalar<unsigned short>*)avolume)->load_dataset_from_VTK_file(path_parent(*file));
                        break;

                    case itk::ImageIOBase::SHORT:
                        avolume = new image_complex<short>();
                        ((image_scalar<short>*)avolume)->load_dataset_from_VTK_file(path_parent(*file));
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

            }
        }

    itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();

    //check if it's a DICOM file
    if ( (load_mode == LOMO_DICOM || load_mode == LOMO_NOTSET) && dicomIO->CanReadFile (file->c_str()))
        {
        load_mode = LOMO_DICOM;

        cout << (*file) << " can be read by GDCMImageIO" << endl;

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
                                    avolume = new image_integer<unsigned char>();
                                    ((image_integer<unsigned char>*)avolume)->load_dataset_from_DICOM_files(path_parent(*file),seriesIdentifier);
                                    break;
                                case itk::ImageIOBase::USHORT:
                                    avolume = new image_integer<unsigned short>();
                                    ((image_integer<unsigned short>*)avolume)->load_dataset_from_DICOM_files(path_parent(*file),seriesIdentifier);
                                    break;

                                case itk::ImageIOBase::SHORT:
                                    avolume = new image_integer<short>();
                                    ((image_integer<short>*)avolume)->load_dataset_from_DICOM_files(path_parent(*file),seriesIdentifier);
                                    break;
                                default:
#ifdef _DEBUG
                                    cout << "Unsupported component type: " << dicomIO->GetComponentTypeAsString (componentType) << endl;
#endif
                                }
                            break;
                        case itk::ImageIOBase::COMPLEX:
                            break;
                        default:
#ifdef _DEBUG
                            std::cout << "image_base::load(...): unsupported pixel type: " << dicomIO->GetPixelTypeAsString(pixelType) << endl;
#endif

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

        }//can & will read DICOM


    if (load_mode == LOMO_RAW || load_mode == LOMO_NOTSET)
        {
        //now we start to suspect this is raw
        //build list of files to send to raw importer
        load_mode = LOMO_RAW;

        raw_files.push_back (*file);
        }

    if (avolume != NULL)
        {
        //new volume either way, add to datamanager

        datamanagement.add(avolume);

#ifdef _DEBUG
        viewmanagement.list_connections();
        rendermanagement.listrenderers();
#endif
        }//avolume != NULL


    }//files for() loop

if (load_mode == LOMO_RAW)
    {
    new rawimporter (raw_files);
    }

    }//load (...)

