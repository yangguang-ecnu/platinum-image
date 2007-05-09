//////////////////////////////////////////////////////////////////////////
//
//  image_generalfile.hxx $Revision $
//
//  File I/O functions implemented in image_general
//
//  $LastChangedBy $
//

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

#include "fileutils.h"

template <class ELEMTYPE, int IMAGEDIM>
image_general<ELEMTYPE, IMAGEDIM>::image_general(std::vector<std::string> files, long width, long height, bool bigEndian, long headerSize, Vector3D voxelSize, unsigned int startFile,unsigned int increment)
//raw constructor
{
    unsigned long sliceSize = width * height;
    string dirname;
    volume_load_mode mode = undefined;
    
    //TODO: start & increment values
    
    //set voxel size, will be used to calculate
    //unit_to_voxel when initialize_dataset is called
    this->voxel_resize.Fill(0);
    for (int d=0;d < 3;d++)
        { voxel_resize[d][d] = voxelSize[d]; }
    
    struct stat fileStats;
    
    if (stat(files.front().c_str(), &fileStats) == 0)
        {
        if (files.size() == 1)
            {
            string dirpath = files.front();

            if (S_ISDIR(fileStats.st_mode))
                {
                vector<std::string> dir_files = get_dir_entries (dirpath);

                if (dir_files.size() > 0)
                    {
                    dirname = path_end (dirpath);
                    
                    files = dir_files;
                    
                    mode = multifile;
                    }
                }
            else
                {
                mode = single_file;
                dirname = path_parent (files.front());
                }
            }
        }
    else
        {
        //total failure, TODO: throw exception
#ifdef _DEBUG
        cout << "Opening raw: couldn't get stat for \"" << files.front() << "\"" << endl;
#endif
        }

    if (mode == single_file)
        {
        //One multi-slice file
        
        unsigned long fileSize = fileStats.st_size;
        unsigned long depth = (fileSize - headerSize) / (sliceSize*sizeof (ELEMTYPE));
        
        initialize_dataset(width, height, depth);
        ELEMTYPE* writepointer = this->imageptr;
        this->num_elements = datasize[0]*datasize[1]*datasize[2];
        
        ifstream stackFile (files.front().c_str(), ios::in | ios::binary);
        stackFile.seekg (headerSize);
        
        stackFile.read ((FILEPOSTYPE*)writepointer, sizeof (ELEMTYPE) * datasize[0]*datasize[1]*datasize[2]);
        
        adjust_endian (this->imageptr, this->num_elements,bigEndian);
        
        stackFile.close();
        }
    
    if (mode == multifile)
        {
        unsigned long depth = files.size();
        initialize_dataset(width, height, depth);
        ELEMTYPE* writepointer = this->imageptr;
        this->num_elements = datasize[0]*datasize[1]*datasize[2];
        
        std::vector<std::string>::iterator fileItr = files.begin();
        
        while (fileItr != files.end() && writepointer < (this->imageptr + this->num_elements))
            {
            //Multiple files, each containing one slice
            ifstream imageFile ((*fileItr).c_str(), ios::in | ios::binary);
            
            if (imageFile.good()) //check that everything is OK for reading
                {
                //header is skipped at the start of each file
                //Note: header size is in bytes (FILEPOSTYPE) regardless of image data type           
                imageFile.seekg (headerSize);
                
                imageFile.read ((FILEPOSTYPE*)writepointer, sizeof (ELEMTYPE) * sliceSize);
                writepointer +=sliceSize;
                
                imageFile.close();
                }
            }
        adjust_endian (this->imageptr, this->num_elements,bigEndian);
        }
    
    if (this->imageptr != NULL)
        {
        name_from_path (files.front());
        
        image_has_changed(true);
        }
    else
        {
        //TODO: if imageptr == NULL at this point, volume has not been read and error has
        //to be reported somehow
        }
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::load_dataset_from_DICOM_files(string dir_path,string seriesIdentifier)
    {  
    typedef itk::GDCMSeriesFileNames                NamesGeneratorType;

    itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();

    NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
    nameGenerator->SetLoadPrivateTags ( true ); 
    nameGenerator->SetUseSeriesDetails( false );    //no details - crucial because existing
    //UID has to match seriesIdentifier
    nameGenerator->SetDirectory(dir_path.c_str());

    typedef std::vector< std::string > FileNamesContainer; 
    FileNamesContainer fileNames; 
    fileNames = nameGenerator->GetFileNames( seriesIdentifier );     

    // *** initialize reader ***

    typename theSeriesReaderType::Pointer reader = theSeriesReaderType::New();
    reader->SetFileNames( fileNames );

    reader->SetImageIO( dicomIO );

    typename theImagePointer image = theImageType::New();

    // *** do reading ***

    try{
        reader->Update();
        }
    catch (itk::ExceptionObject &ex)
        {
        cout<<ex<<endl;
        }

    image = reader->GetOutput();

    // *** transfer image data to our platform's data structure ***
    replicate_itk_to_volume(image);

    // *** get DICOM metadata ***

    ostringstream namestring;
    const string tagkey []=
        {
        "0018|0020",
        "0018|1030",
        "0018|0080",
        "0018|0081",
        "0018|1314",
        "0018|9005",
        "0008|1030",
        "0008|103E",
        "0010|0030"
        };

    const unsigned int num_name_tags = 9;

    std::string namebase_tag;

    std::string labelId;

    //set name in the form
    //study description-series description-patient's birth date
    //# for empty fields
    for (unsigned int t=0;t <num_name_tags ; t++)
        {
        namebase_tag="";

        if (t >0)
            {namestring<< "/";}

        if( dicomIO->GetValueFromTag(tagkey[t],namebase_tag ) && namebase_tag!="")
            {
            namestring << namebase_tag;}
        else
            {namestring << "#";} 
        }

    name(namestring.str());

    this->from_file(true);
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::save_image_to_VTK_file(string file_path)
    {
    //replicate volume to ITK image and save it as VTK file

    typename theImageType::DirectionType itk_orientation;

    for (unsigned int d=0;d<3;d++)
        {
        for (unsigned int c=0;c<3;c++)
            {itk_orientation[d][c]=this->direction[d][c];}
        }

    make_image_an_itk_reader();
    typename theImagePointer output_image=ITKimportfilter->GetOutput();

    output_image->SetDirection(itk_orientation);

    typename theWriterType::Pointer writer = theWriterType::New();   //default file type is VTK
    writer->SetFileName( file_path.c_str() );
    writer->SetInput(output_image);

    try{
        writer->Update();
        }
    catch (itk::ExceptionObject &ex){
        cout<<"Exception thrown saving file ("<<file_path<<")"<<ex;
        }
    }

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::load_dataset_from_VTK_file(string file_path)
    {
    typename theReaderType::Pointer r = theReaderType::New();
    itk::VTKImageIO::Pointer VTKIO = itk::VTKImageIO::New();

    r->SetFileName(file_path.c_str());

    r->SetImageIO( VTKIO );

    typename theImagePointer image = theImageType::New();
    image = r->GetOutput();
    r->Update();
    typename theSizeType s = image->GetBufferedRegion().GetSize();

    replicate_itk_to_volume(image);

    this->name_from_path (file_path);
    }