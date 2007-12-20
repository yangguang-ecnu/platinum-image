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

template <class VOXTYPE, template <class,int=3 > class IMGCLASS>
void try_allocate (image_base* &i,unsigned int voxel_type, std::vector<std::string> files, long width, long height, bool bigEndian, long headerSize, Vector3D voxelSize)
{
    if (i == NULL && sizeof (VOXTYPE) == voxel_type/8 )
        i = new IMGCLASS<VOXTYPE> (files, width, height, bigEndian, headerSize, voxelSize);
}

template <template <class,int=3 > class IMGCLASS>
image_base* allocate_image (bool floatType, bool signedType, unsigned int voxel_type, std::vector<std::string> files, long width, long height, bool bigEndian, long headerSize, Vector3D voxelSize)
{
    image_base* output = NULL;
    
    if (floatType)
        {
        try_allocate<float,IMGCLASS>(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
        try_allocate<double,IMGCLASS>(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
        try_allocate<long double,IMGCLASS>(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
        }
    else
        {
        if (signedType)
            {
            try_allocate<signed char,IMGCLASS >(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
            try_allocate<signed short,IMGCLASS >(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
            try_allocate<signed long,IMGCLASS >(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
            try_allocate<signed long long,IMGCLASS >(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
            }
        else
            {
            try_allocate<unsigned char,IMGCLASS >(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
            try_allocate<unsigned short,IMGCLASS >(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
            try_allocate<unsigned long,IMGCLASS >(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
            try_allocate<long long,IMGCLASS >(output, voxel_type, files, width, height, bigEndian, headerSize, voxelSize);
            }
        }
    
    return output;
}

template <class ELEMTYPE>
ELEMTYPE * load_pixels (unsigned long &num_elements,std::vector<std::string> files, bool bigEndian, long headerSize = 0, unsigned int startFile= 1,unsigned int increment = 1)
    {
    ELEMTYPE * output = NULL;
    num_elements = 0;

    //unsigned long sliceSize = width * height;
    std::string dirname;
    image_load_mode mode = undefined;
    
    //TODO: start & increment values
    
    struct stat fileStats;
    
    if (stat(files.front().c_str(), &fileStats) == 0)
        {
        if (files.size() == 1)
            {
            std::string dirpath = files.front();

            if (S_ISDIR(fileStats.st_mode))
                {
					std::vector<std::string> dir_files = get_dir_entries (dirpath);

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
        pt_error::error ("Opening raw: couldn't get stat for \"" + files.front() + "\"",pt_error::fatal);
        }

    //size of *first* file
    unsigned long dataSize = fileStats.st_size - headerSize; //!header size always in bytes

    if (mode == single_file)
        { num_elements = dataSize/sizeof(ELEMTYPE); }

    if (mode == multifile)
        { num_elements = files.size()*(dataSize/sizeof(ELEMTYPE)); }

    output = new ELEMTYPE [num_elements];
    ELEMTYPE * writepointer = output;

    if (mode == single_file)
        {
        //One multi-slice file
        
        //unsigned long depth = (fileSize - headerSize) / (sliceSize*sizeof (ELEMTYPE));
        
        //initialize_dataset(width, height, depth);
        
        std::ifstream stackFile (files.front().c_str(), std::ios::in | std::ios::binary);
        stackFile.seekg (headerSize);
        
        stackFile.read (reinterpret_cast<FILEPOSTYPE*>(writepointer), sizeof (ELEMTYPE) * num_elements);
        
        adjust_endian (output, num_elements,bigEndian);
        
        stackFile.close();
        }
    
    if (mode == multifile)
        {
        //unsigned long depth = files.size();
        //initialize_dataset(width, height, depth);
        
        std::vector<std::string>::iterator fileItr = files.begin();
        
        while (fileItr != files.end() && writepointer < (output + num_elements))
            {
            //Multiple files, each containing one slice
            std::ifstream imageFile ((*fileItr).c_str(), std::ios::in | std::ios::binary);
            
            if (imageFile.good()) //check that everything is OK for reading
                {
                //header is skipped at the start of each file
                //Note: header size is in bytes (FILEPOSTYPE) regardless of image data type           
                imageFile.seekg (headerSize);
                
                imageFile.read (reinterpret_cast<FILEPOSTYPE*>(writepointer), sizeof (ELEMTYPE) * dataSize);
                writepointer +=dataSize;
                
                imageFile.close();
                }
            }
        adjust_endian (output, num_elements,bigEndian);
        }

    return output;
    }

template <class ELEMTYPE, int IMAGEDIM>
image_general<ELEMTYPE, IMAGEDIM>::image_general(ELEMTYPE * inData, unsigned long numElements, long width, long height, Vector3D voxelSize)
//raw from data constructor
    {
    if (inData != NULL)
        {
        //set voxel size, will be used to calculate
        //unit_to_voxel when initialize_dataset is called

        initialize_dataset (width,height,numElements/(width * height),inData);

        this->voxel_size = voxelSize;
        
        //image_has_changed(true);
        }
    else
        {
        throw ("Image was not created",pt_error::fatal);
        }
    }

template <class ELEMTYPE, int IMAGEDIM>
image_general<ELEMTYPE, IMAGEDIM>::image_general(std::vector<std::string> files, long width, long height, bool bigEndian, long headerSize, Vector3D voxelSize, unsigned int startFile,unsigned int increment)
//raw from file constructor
    {
    unsigned long numElements;
    ELEMTYPE * data = load_pixels<ELEMTYPE>(numElements, files,bigEndian,headerSize,  startFile,increment);

    if (data != NULL)
        {
        //set voxel size, will be used to calculate
        //unit_to_voxel when initialize_dataset is called
        
        initialize_dataset (width,height,numElements/(width * height),data);
        
        this->voxel_size = voxelSize;
        
		delete[] data; //To avoid memory leak !!!!!!!
        name_from_path (files.front());

        //image_has_changed(true);
        }
    else
        {
        //TODO: if imageptr == NULL at this point, image has not been read and error has
        //to be reported somehow
        }
    }

/*

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::load_dataset_from_DICOM_files(std::string dir_path,std::string seriesIdentifier)
    {  
	std::cout << "--- load_dataset_from_DICOM_files" << std::endl;



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

	if(fileNames.size()>0){
		//dcmIO->SetFileName(fileNames[0].c_str());					//JK1
	}
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
        std::cout<<ex<<std::endl;
        }

    image = reader->GetOutput();

//	reader->	//JK1 - Load meta data from dicom reader...

    // *** transfer image data to our platform's data structure ***
    replicate_itk_to_image(image);

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

	this->meta.read_metadata_from_dcm_file(fileNames[0].c_str());	//JK1 - Loads meta data from first dicom file in vector...
    }
*/

/*
template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::load_dataset_from_DICOM_files2(std::string dir_path,std::string seriesIdentifier)
{
	std::cout << "--- load_dataset_from_DICOM_files2" << std::endl;


//	 message from ITK/Examples/IO/DicomSeriesReadImageWrite.cxx
//   
//   The DICOM classes illustrated here are OBSOLETE and are
//   scheduled for being removed from the toolkit.
//
//   Please refer to DicomSeriesReadImageWrite2.cxx instead, where the GDCM
//   classes are used. GDCM classes are the ones currently recommended for
//   performing reading and writing of DICOM images.




//  typedef itk::Image<unsigned short,5> ImageNDType;
//  typedef itk::ImageSeriesReader<ImageNDType> ReaderType;

	typedef itk::ImageSeriesReader<theImageType> ReaderType;

	itk::DICOMImageIO2::Pointer io = itk::DICOMImageIO2::New();
	itk::DICOMSeriesFileNames::Pointer names = itk::DICOMSeriesFileNames::New();
	names->SetDirectory(dir_path.c_str());
	  
	typename ReaderType::Pointer reader = ReaderType::New();
//	reader->SetFileNames(names->GetFileNames(seriesIdentifier));
	//  reader->SetFileNames(names->GetFileNames());

    std::vector<std::string> fileNames; 
    fileNames = names->GetFileNames(seriesIdentifier);     
	reader->SetFileNames(fileNames);


	reader->SetImageIO(io);
	std::cout << names;

	//  FilterWatcher watcher(reader);

	try{
	//    if (1){reader->ReverseOrderOn();}
		reader->Update();
		reader->GetOutput()->Print(std::cout);
	}catch (itk::ExceptionObject &ex){
		std::cout << ex;
	}

	typename theImagePointer image = theImageType::New();
	image = reader->GetOutput();
	replicate_itk_to_image(image);

    this->from_file(true);
	this->meta.read_metadata_from_dcm_file(fileNames[0].c_str());	//JK1 - Loads meta data from first dicom file in vector...

	std::cout << fileNames[0].c_str() << std::endl;
	
	this->name( this->meta.get_name() );
	
	this->read_geometry_from_dicom_file( fileNames[0].c_str() );

}
*/

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::load_dataset_from_DICOM_filesAF(std::string dir_path,std::string seriesIdentifier)
{ 
 
//	std::cout<< "--- load_dataset_from_DICOM_filesAF" << std::endl;
	std::cout<<"dir_path="<<dir_path<<std::endl;
	std::cout<<"seriesIdentifier="<<seriesIdentifier<<std::endl;
	dir_path = path_parent(dir_path);
	std::cout<<"path_parent(dir_path)="<<dir_path<<std::endl;
	
    typedef itk::GDCMSeriesFileNames NamesGeneratorType;
    NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
    nameGenerator->SetUseSeriesDetails ( false );			// no details - crucial because existing
    nameGenerator->SetLoadPrivateTags ( true ); 
    nameGenerator->SetDirectory ( dir_path.c_str() );

    typedef std::vector< std::string > FileNamesContainer; 
	FileNamesContainer fileNames = nameGenerator->GetFileNames ( seriesIdentifier ); 
//    fileNames = nameGenerator->GetFileNames ( seriesIdentifier );     

	std::cout<<"Number of files in series..."<<fileNames.size()<<std::endl;

	load_dataset_from_these_DICOM_files(fileNames);
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::load_dataset_from_these_DICOM_files(vector<string> fileNames){
	if(fileNames.size()==1){
		typename theReaderType::Pointer reader = theReaderType::New();

		itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();
		reader->SetImageIO ( dicomIO );
		reader->SetFileName ( fileNames[0].c_str() );
		try { reader->Update(); }
		catch (itk::ExceptionObject &ex)
			{ std::cout << ex << std::endl; }

		typename theImagePointer image = theImageType::New();
		image = reader->GetOutput();

		// *** transfer image data to our platform's data structure ***
		replicate_itk_to_image(image);

	}else if(fileNames.size()>1){
		typename theSeriesReaderType::Pointer reader = theSeriesReaderType::New();

		itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();
		reader->SetImageIO ( dicomIO );
		reader->SetFileNames ( fileNames );

		try { reader->Update(); }
		catch (itk::ExceptionObject &ex)
			{ std::cout << ex << std::endl; }

		typename theImagePointer image = theImageType::New();
		image = reader->GetOutput();

		// *** transfer image data to our platform's data structure ***
		replicate_itk_to_image(image);

	}else{
		pt_error::error("load_dataset_from_DICOM_filesAF--> fileNames.size()==0",pt_error::debug);
	}


    this->from_file(true);
	this->meta.read_metadata_from_dcm_file(fileNames[0].c_str());	//JK1 - Loads meta data from first dicom file in vector...
	this->name( this->meta.get_name() );
	this->read_geometry_from_dicom_file ( fileNames[0].c_str() );			// use the first file name in the vector
	//this->read_geometry_from_dicom_file ( fileNames.back().c_str() );		// use the last file name in the vector

}


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::load_dataset_from_all_DICOM_files_in_dir(std::string dir_path)
    {  
		cout<<"load_dataset_from_all_DICOM_files_in_dir...("<<dir_path<<")"<<endl;
		
		//-- Remove final "\\" of dir_path...
		while(dir_path.find_last_of("\\")==dir_path.size()-1){
			cout<<"substring...ing..."<<endl;
			dir_path = dir_path.substr(0,dir_path.size()-1);
		}
//		cout<<"dir_path="<<dir_path<<endl;

		if(!dir_exists(dir_path)){
			pt_error::error("load_dataset_from_all_DICOM_files_in_dir - No Such Dir ("+dir_path+")",pt_error::debug);
		}else{
			itk::DICOMImageIO2::Pointer dicomIO = itk::DICOMImageIO2::New();
			itk::DICOMSeriesFileNames::Pointer nameGenerator = itk::DICOMSeriesFileNames::New();
			nameGenerator->SetDirectory( dir_path.c_str() );

			typedef vector<string> seriesIdContainer;
			const seriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();
			seriesIdContainer::const_iterator seriesItr = seriesUID.begin();
			seriesIdContainer::const_iterator seriesEnd = seriesUID.end();

			nameGenerator->SetFileNameSortingOrderToSortByImageNumber();//in order...
			typedef vector<string> fileNamesContainer;
			fileNamesContainer fileNames;
			fileNames = nameGenerator->GetFileNames();					//stores all dicom filenames...

			cout<<"fileNames.size()"<<fileNames.size()<<endl;

			//-------------READER-------------
			typename theSeriesReaderType::Pointer reader = theSeriesReaderType::New();
			reader->SetFileNames( fileNames );
			reader->SetImageIO( dicomIO );
			try{
				reader->Update();
			}catch (itk::ExceptionObject &ex){
				cout<<ex<<endl;
			}

			// *** transfer image data to our platform's data structure ***
			typename theImagePointer image = theImageType::New();
			image = reader->GetOutput();
			replicate_itk_to_image(image);

			this->name("Dicomfiles");

			this->from_file(true);

			this->meta.read_metadata_from_dcm_file(fileNames[0].c_str());	//JK1 - Loads meta data from first dicom file in vector...
		}
	}



template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::save_to_VTK_file(const std::string file_path, const bool useCompression)
    {
	cout<<"save_to_VTK_file..."<<endl;
    typename theWriterType::Pointer writer = theWriterType::New();   //default file type is VTK
    writer->SetFileName( file_path.c_str() );
    writer->SetInput(get_image_as_itk_output());
	if(useCompression){
		writer->UseCompressionOn();
	}

    try{
        writer->Update();
        }
    catch (itk::ExceptionObject &ex){
        pt_error::error("Exception thrown saving file (" +file_path + ")", pt_error::warning);
		std::cout<<ex<<std::endl;
        }
    }


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::save_to_DCM_file(const std::string file_path, const bool useCompression, const bool anonymize)
    {
	cout<<"save_to_DCM_file..."<<endl;    //port image to ITK image and save it as DCM file

	//--------------------------------------------------------
	//--- If dicom image has tag "DCM_IMAGE_ORIENTATION_PATIENT" a/b/c/d/e/f
	//--- This is read as 
	//		a d j
	//		b e	k
	//		c f l
	//--- GDCMImageIO saves the direcional conines transposed compared to the ITK use....
	//--- Idea - Transpose rotation matrix before saving... and again after...

	//--------------------------------------------------------
	this->orientation = this->orientation.GetTranspose();
	//--------------------------------------------------------

    typename theWriterType::Pointer writer = theWriterType::New();
	typename itk::OrientedImage<ELEMTYPE, IMAGEDIM >::Pointer image = get_image_as_itk_output();
    writer->SetFileName( file_path.c_str() );
    writer->SetInput(image);
	if(useCompression){
		writer->UseCompressionOn();
	}

	if(anonymize){
		typedef itk::GDCMImageIO ImageIOType;
		ImageIOType::Pointer gdcmImageIO = ImageIOType::New();
		writer->SetImageIO( gdcmImageIO );
		typedef itk::MetaDataDictionary   DictionaryType;
		DictionaryType & dictionary = image->GetMetaDataDictionary();

		//TODO - make sure all interesting data in "metadata-object" is saves....
		itk::EncapsulateMetaData<std::string>( dictionary, DCM_PATIENT_NAME, "Anonymized" );
		itk::EncapsulateMetaData<std::string>( dictionary, DCM_PATIENT_ID, "Anonymized" );
		itk::EncapsulateMetaData<std::string>( dictionary, DCM_PATIENT_BIRTH_DATE, "Anonymized" );
		// The two lines below don't work... 
		// The brutal rotation of the orientation matrix was used before and after saving instead...
		//itk::EncapsulateMetaData<std::string>( dictionary, DCM_IMAGE_POSITION_PATIENT, "1\\2\\3" );
		//itk::EncapsulateMetaData<std::string>( dictionary, DCM_IMAGE_ORIENTATION_PATIENT, this->get_orientation_as_dcm_string() );
	}

    try{
        writer->Update();
    }catch (itk::ExceptionObject &ex){
        pt_error::error("Exception thrown saving file (" +file_path + ")", pt_error::warning);
		std::cout<<ex<<std::endl;
    }

	//--------------------------------------------------------
	this->orientation = this->orientation.GetTranspose(); //transpose again  "=back"
	//--------------------------------------------------------
}




template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::save_to_TIF_file_series_3D(const std::string file_path_base)
{
	char buf[10];
	for(int z=0;z<datasize[2];z++)
	{
		image_scalar<ELEMTYPE, IMAGEDIM> *slc = new image_scalar<ELEMTYPE, IMAGEDIM>();
		slc->initialize_dataset(datasize[0],datasize[1],1);
		slc->copy_slice_from_3D(this,z,0,2);
		slc->scale(); //0...256
		slc->data_has_changed(true);

		sprintf(buf,"%04i",z);
		image_scalar<unsigned char,3> *slc2 = new image_scalar<unsigned char,3>(slc);
		slc2->save_uchar2D_to_TIF_file(file_path_base, string(buf));
		delete slc;
		delete slc2;
	}
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::save_uchar2D_to_TIF_file(const std::string file_path_base, const std::string slice)
{
	typedef itk::ImageFileWriter<itk::OrientedImage<unsigned char,3> >	theTifWriterType;

	string s = file_path_base+"_"+slice+".tif";
	theTifWriterType::Pointer writer = theTifWriterType::New();
	writer->SetFileName(s.c_str());
	writer->SetInput(get_image_as_itk_output());
	try{
		writer->Update();
	}catch (itk::ExceptionObject &ex){
		cout<<"Exception thrown saving file.....("<<s<<")"<<ex;
	}

//	this->clear_itk_porting();
}


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::load_dataset_from_VTK_file(string file_path)
    {
	if(file_exists(file_path)){
		typename theReaderType::Pointer r = theReaderType::New();
		itk::VTKImageIO::Pointer VTKIO = itk::VTKImageIO::New();

		r->SetFileName(file_path.c_str());

		r->SetImageIO( VTKIO );

		typename theImagePointer image = theImageType::New();
		image = r->GetOutput();
		r->Update();
		typename theSizeType s = image->GetBufferedRegion().GetSize();

		replicate_itk_to_image(image);

		this->name_from_path (file_path);
	}else{
		pt_error::error("image_general::load_dataset_from_VTK_file()--> file does not exist...",pt_error::debug);
	}
    }