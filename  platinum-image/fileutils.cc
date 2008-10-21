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

#include "fileutils.h"

#include <iostream>

#ifdef WIN32
//Windows

#include "dirent.h"

#else
//UNIX
#include <dirent.h>
#endif

void trailing_slash (string &s)
    {
    if (*s.rbegin() != '/')
        {
        s.append("/");
        }
    }

vector<string> get_dir_entries(string path, bool full_path)
    {
    // *** POSIX ***

    vector<string> f;
    dirent *ep;
    char cpath[MAXPATHLENGTH];
    strcpy (cpath,path.c_str());

    DIR * dp = opendir (cpath);

	if(full_path){
		while (ep = readdir (dp))
			f.push_back (path + ep->d_name);
	}else{
		while (ep = readdir (dp))
			f.push_back (ep->d_name);
	}

    (void) closedir (dp);

    return f;
    }

vector<string> get_dir_entries_ending_with(string path, string ending)
    {
    // *** POSIX ***

    vector<string> f;
    dirent *ep;
    char cpath[MAXPATHLENGTH];
    strcpy (cpath,path.c_str());

    DIR * dp = opendir (cpath);

	while (ep = readdir (dp)){
		string res = string(ep->d_name);
		int s = ending.size();
		int from = res.size()-s;
		if(from<0){from=0;}

		string this_ending = res.substr(from,s);
//		cout<<"res="<<res<<endl;
//		cout<<"this_ending="<<this_ending<<endl;
		
		if(this_ending == ending)
		{
			f.push_back (ep->d_name);
		}
	}

    (void) closedir (dp);
    return f;
    }


string path_end (string file_path)
    {
    unsigned int pos;

    pos=file_path.rfind("/",file_path.length()-2);
    
    if (pos !=string::npos)
        {
        file_path.erase(0,pos+1);
        }
    
    return file_path;
  
    }

string path_parent (string file_path)
    {
    unsigned int pos;

	//try "/"
    pos=file_path.rfind("/",file_path.length()-1);

	//try "\" (Windows paths might use these)
	if(pos ==string::npos){
	   pos=file_path.rfind("\\",file_path.length()-1);
	}
    
    if (pos !=string::npos)
        {
        file_path.erase(pos+1,file_path.length());
        }
    
    return file_path;
    }

bool file_exists (string file_path)
{
    struct stat fileStats;
    
    if (stat(file_path.c_str(), &fileStats) == 0)
        {
        if (!S_ISDIR(fileStats.st_mode))
            {return true;}
        else
            {return false;}
        }
    
    return false;

    //return (access(file_path.c_str(), F_OK) == 1);
}

bool dir_exists (string file_path)
{
    struct stat fileStats;
    
    if (stat(file_path.c_str(), &fileStats) == 0)
        {
        if (S_ISDIR(fileStats.st_mode))
            {return true;}
        else
            {return false;}
        }
    
    return false;
}

void create_dir(string dir_path)
{
	itksys::SystemTools::MakeDirectory( dir_path.c_str() );
}

void add_to_string_vector_if_not_present(vector<string> &v, string s)
{
	if(s!=""){
		bool present=false;
		for(int i=0; i<v.size() && !present; i++){
			if(v[i] == s){
				present=true;
			}
		}
		if(!present){
//			cout<<"******"<<endl;
			v.push_back(s);
		}
	}
}

//------------- Dicom specific file handling ----------------------


vector<string> get_dicom_files_in_dir(string dir_path, bool full_path)
{
//	cout<<"get_dicom_files_in_dir..."<<endl;
	itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();

	vector<string> dcm_files;
	vector<string> all_files = get_dir_entries(dir_path,true);

	for(int i=0;i<all_files.size();i++)
	{
//		cout<<"*i="<<i<<" "<<all_files[i]<<endl;
		if(dicomIO->CanReadFile(all_files[i].c_str()))
		{
//			cout<<"****dcm***"<<endl;
			dcm_files.push_back(all_files[i]);
		}
	}
	return dcm_files;
}

string get_first_dicom_file_in_dir(string dir_path, bool full_path)
{
	vector<string> v = get_dicom_files_in_dir(dir_path, full_path);
//	cout<<"v.size()="<<v.size()<<endl;
	if(v.size()>0){
		return v[0];
	}
	return "";
}

string get_second_dicom_file_in_dir(string dir_path, bool full_path)
{
	vector<string> v = get_dicom_files_in_dir(dir_path, full_path);
//	cout<<"v.size()="<<v.size()<<endl;
	if(v.size()>1){
		return v[1];
	}
	return "";
}

vector<string> get_dicom_files_with_dcm_tag_value(vector<string> files, string dcm_tag, string tag_val)
{
	vector<string> v;
	for(int i=0;i<files.size();i++){
		if( get_dicom_tag_value(files[i], dcm_tag)==tag_val ){
			v.push_back(files[i]);
		}
	}
	return v;
}

int get_number_of_dicom_files_in_dir(string dir_path)
{
return get_dicom_files_in_dir(dir_path).size();
}

string get_dicom_tag_value(string file_path, string dcm_tag, bool remove_garbage_char)
{
//	cout<<"get_dicom_tag_value...("<<file_path<<","<<dcm_tag<<")"<<endl;
	itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();
	string dcmdata = "";

	if(dicomIO->CanReadFile(file_path.c_str()))
	{
		dicomIO->SetFileName(file_path.c_str());
		dicomIO->ReadImageInformation();		//get basic DICOM header
		dicomIO->GetValueFromTag(dcm_tag,dcmdata);
		if(remove_garbage_char){
			remove_string_ending(dcmdata, " ");
		}
//		cout<<"CanReadFile... (dcmdata="<<dcmdata<<")"<<endl;	
	}
	return dcmdata;
}



bool does_dir_contain_dcmfile_with_tag_value(string dir_path, string dcm_tag, string tag_val, bool recursive_search)
{
//	cout<<"does_dir_contain_dcmfile_with_tag_value..."<<endl;

	vector<string> dcm_files = get_dicom_files_in_dir(dir_path, true);
	itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();
	string dcmdata = "";
	if(recursive_search){
		vector<string> dirs = subdirs(dir_path);
		bool result;
		for(int i=0;i<dirs.size();i++){
			result = does_dir_contain_dcmfile_with_tag_value(dirs[i], dcm_tag, tag_val, recursive_search);
			if(result){
				return true;
			}
		}

	}

	for(int i=0;i<dcm_files.size();i++){
		dicomIO->SetFileName(dcm_files[i].c_str());
		dicomIO->ReadImageInformation();		//get basic DICOM header
		dicomIO->GetValueFromTag(dcm_tag,dcmdata);
		remove_string_ending(dcmdata," "); //removes eventual last garbage char
		//cout<<"tag_val=("<<tag_val<<") dcmdata=("<<dcmdata<<")"<<endl;
		if(dcmdata == tag_val){
			return true;
		}
	}
	return false;
}


string find_first_sub_dir_containing_dcm_file_with_tag_value(string dir_path, string dcm_tag, string tag_val, bool recursive_search)
{
//	cout<<"find_first_sub_dir_containing_dcm_file_with_tag_value...("<<dir_path<<")"<<endl;
	vector<string> dirs = subdirs(dir_path);
	string result="";

	for(int i=0;i<dirs.size();i++){
		cout<<"dirs[i]="<<dirs[i]<<endl;
		if(recursive_search){
			result = find_first_sub_dir_containing_dcm_file_with_tag_value(dirs[i], dcm_tag, tag_val, recursive_search);
			if(result != ""){
				return result;
			}
		}
		if(does_dir_contain_dcmfile_with_tag_value(dirs[i],dcm_tag,tag_val,false)){
//			cout<<"***"<<endl;
			return dirs[i];
		}
	}
	return "";
}

//----------------------------------------------work in progress-----/SO------------------
TagValueDirInfo get_number_of_dirs_and_first_sub_dir_containing_dcm_file_with_tag_value(string dir_path, string dcm_tag, string tag_val)
{
	vector<string> dirs = subdirs(dir_path);
	TagValueDirInfo tagVal;
	tagVal.firstDir="";
	tagVal.similarDirs = 0;

	for(int i=0;i<dirs.size();i++)
	{
		cout<<"dirs[i]="<<dirs[i]<<endl;

		if(does_dir_contain_dcmfile_with_tag_value(dirs[i],dcm_tag,tag_val,false))
		{
			tagVal.similarDirs++;

			if (tagVal.firstDir == "")
			{
			tagVal.firstDir = dirs[i];
			}
		}
	}
	cout << "* * * Number of similarDirs = " << tagVal.similarDirs << endl;
	cout << "* * * firstDir = " << tagVal.firstDir << endl;
	return tagVal;
}
//-----------------------------------------------------------------------------------------------------------

vector<string> list_dicom_tag_values_for_this_ref_tag_value(vector<string> files, string dcm_tag, string dcm_tag_val, string dcm_ref_tag)
{
//	cout<<"list_dicom_tag_values_for_this_ref_tag_value..."<<endl;
//	cout<<"dcm_tag="<<dcm_tag<<endl;
//	cout<<"dcm_tag_val=("<<dcm_tag_val<<")"<<endl;
//	cout<<"dcm_ref_tag="<<dcm_ref_tag<<endl;

	//for example dcm_tag = "series ID"
	//for example dcm_tag_val = 1.231434.3432545
	//for example ref_tag_val = "echo time"..

	vector<string> vec;
	vector<string> values_present;
	string tmp;
	string tmp2;

	for(int i=0;i<files.size();i++){
		tmp2 = get_dicom_tag_value(files[i], dcm_tag);
//		cout<<"tmp2=("<<tmp2<<")"<<endl;
		tmp2 = tmp2.c_str();	//remove last garbage char...
//		cout<<"tmp3=("<<tmp2<<")"<<endl;
		if(dcm_tag_val == tmp2)
		{
			tmp = get_dicom_tag_value(files[i], dcm_ref_tag);
//			cout<<"tmp="<<tmp<<endl;
			add_to_string_vector_if_not_present(values_present, tmp);
		}
	}
	return values_present;
}

vector<string> list_dicom_tag_values_in_dir(string dir_path, string dcm_tag, bool recursive_search, bool exclusive_values)
{
	cout<<"list_dicom_tag_values_in_dir("<<dir_path<<") "<<endl;
	vector<string> tags;
	vector<string> tags2;
	if(recursive_search){
		vector<string> dirs = subdirs(dir_path);
		for(int i=0;i<dirs.size();i++){
			tags2 = list_dicom_tag_values_in_dir(dirs[i], dcm_tag, recursive_search, exclusive_values);
			for(int j=0;j<tags2.size();j++){
				if(exclusive_values){
					add_to_string_vector_if_not_present(tags,tags2[j]);
				}else{
					tags.push_back(tags2[j]);
				}
//				cout<<"tags.size()="<<tags.size()<<endl; 
			}
		}
	}

	vector<string> files = get_dicom_files_in_dir(dir_path, true);
	string tmp;
	for(int i=0;i<files.size();i++){
		tmp = get_dicom_tag_value(files[i], dcm_tag);
//		cout<<"tmp="<<tmp<<endl;
		if(exclusive_values){
			add_to_string_vector_if_not_present(tags,tmp);
		}else{
			tags.push_back(tmp);
		}
	}
	return tags;
}

bool does_dicom_file_tag_contain(string file_path, string dcm_tag, string content)
{
	string tag = get_dicom_tag_value(file_path, dcm_tag, true);
	int pos = tag.find(content);
	cout<<file_path<<" "<<dcm_tag<<" "<<tag<<" "<<content<<" "<<pos<<" "<<tag.size()<<endl;
	if( pos>=0 && pos<tag.size() ){
		return true;
	}
	return false;
}


bool is_dicom_file_magnitude_image(string file_path)
{
	return does_dicom_file_tag_contain(file_path, DCM_IMAGE_TYPE, DCM_MAGNITUDE_SUBSTRING);
}

bool is_dicom_file_real_image(string file_path)
{
	return does_dicom_file_tag_contain(file_path, DCM_IMAGE_TYPE, DCM_REAL_SUBSTRING);
}

bool is_dicom_file_imaginary_image(string file_path)
{
	return does_dicom_file_tag_contain(file_path, DCM_IMAGE_TYPE, DCM_IMAGINARY_SUBSTRING);
}

string get_elemtype_in_dicom_file(string file_path)
{
    itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();
	string result = "undefined";

	if(dicomIO->CanReadFile(file_path.c_str())){
		dicomIO->SetFileName(file_path.c_str());
		dicomIO->ReadImageInformation();	//get basic DICOM header

//		itk::ImageIOBase::IOPixelType pixelType=dicomIO->GetPixelType(); //scalar
		itk::ImageIOBase::IOComponentType componentType = dicomIO->GetComponentType();

		switch(componentType){	//Enumeration values: UCHAR, CHAR, USHORT, SHORT, UINT, INT, ULONG, LONG, FLOAT, DOUBLE
			case itk::ImageIOBase::UCHAR:
				result = "uchar";
			break;
			case itk::ImageIOBase::CHAR:
				result = "char";
			break;
			case itk::ImageIOBase::USHORT:
				result = "ushort";
			break;
			case itk::ImageIOBase::SHORT:
				result = "short";
			break;
			case itk::ImageIOBase::UINT:
				result = "uint";
			break;
			case itk::ImageIOBase::INT:
				result = "int";
			break;
			case itk::ImageIOBase::ULONG:
				result = "ulong";
			break;
			case itk::ImageIOBase::LONG:
				result = "long";
			break;
			case itk::ImageIOBase::FLOAT:
				result = "float";
			break;
			case itk::ImageIOBase::DOUBLE:
				result = "double";
			break;
		}
	}

	return result;
}



//------------- String handling functions ----------------------

bool does_string_end_with(string s, string ending)
{
	if(s.find_last_of(ending) == s.size()-1)
	{
		return true;
	}
	return false;
}

bool remove_file_lastname(string &s, int max_no_lastname_chars)
{
	unsigned int last = s.find_last_of(".");
//	cout<<"last="<<s.find_last_of(".")<<endl;
//	cout<<"size="<<s.size()<<endl;
	if( last >= (s.size() -max_no_lastname_chars -1) )
	{
		s = s.substr(0,last);
		return true;
	}
	return false;
}

bool remove_string_ending(string &s, string ending)
{
//	cout<<"remove_string_ending"<<endl;
	if(does_string_end_with(s,ending)){
//		cout<<"s.size()="<<s.size()<<endl;
//		cout<<"ending.size()="<<ending.size()<<endl;
//		cout<<"s="<<s<<endl;
		s = s.substr(0,s.size()-ending.size());
//		cout<<"s="<<s<<endl;
		return true;
	}
	return false;
}

string replace_last_substring(string s, string val, string replacement){
//	cout<<"replace_last_substring..."<<endl;
	cout<<"s="<<s<<endl;
	int last = s.find_last_of(val);
	string s2 = s;
	string a = s.substr(0,last-val.size()+1);
	string b = replacement;
	cout<<"s2="<<s2<<endl;
	string c = s2.substr(last+1,s2.size()-1-last);
	cout<<"last="<<last<<endl;
	cout<<"a="<<a<<endl;
	cout<<"b="<<b<<endl;
	cout<<"c="<<c<<endl;
	return a+b+c;
}

vector<string> subdirs(string dir_path, bool fullpath)
    {
    trailing_slash(dir_path);

    vector<string> result = get_dir_entries(dir_path,false);
	for(int i=0;i<result.size();i++){
//		cout<<"res="<<result[i]<<endl;
	}

    vector<string>::iterator dirs = result.begin();

    while (result.size() > 0 && dirs != result.end())
        {
        //sort out items which are not directories
        //or circular references
		cout<<"*dirs="<<*dirs<<endl;

	    if (*dirs == "." || *dirs == ".." || !dir_exists(dir_path + *dirs) )
            {
            dirs=result.erase(dirs);
            }
        else
            {
				if(fullpath){
					(*dirs).insert(0,dir_path);
				}
            trailing_slash(*dirs);

            ++dirs; 

            }
        }
    return result;
    }


vector<string> subdirs_where_name_contains(string dir_path, string name_substring)
{
	vector<string> sub = subdirs(dir_path);
	vector<string> sub2;

	for(int i=0;i<sub.size();i++){
//		cout<<"sub[i].find(name_substring)="<<sub[i].find(name_substring)<<endl;
		if( sub[i].find(name_substring) < sub[i].size() ){
			sub2.push_back(sub[i]);
		}
	}
	return sub2;
}

vector<string> get_files_in_dir_where_name_contains(string dir_path, string name_substring)
{
	vector<string> files = get_dir_entries(dir_path,false);
	vector<string> files2;

	for(int i=0;i<files.size();i++){
//		cout<<"files[i]="<<dir_path+files[i]<<endl;
		if(file_exists(dir_path+files[i])){
//			cout<<"files[i].find(name_substring)="<<files[i].find(name_substring)<<endl;
			if( files[i].find(name_substring) < files[i].size() ){
//				cout<<"***"<<endl;
				files2.push_back(files[i]);
			}
		}
	}
	return files2;
}

string int2str(int i){
return templ_to_string(i);
}

string float2str(float f){
return templ_to_string(f);
}

void split(const std::string & s, char c, std::vector<std::string> & v)
{
	string::size_type i = 0;
	string::size_type j = s.find(c);
	
	while ( j != string::npos )
	{
		v.push_back(s.substr(i, j - i));
		i = ++j;
		j = s.find(c, j);
		
		if ( j == string::npos )
		{	// there are no more delimiters. get the last part of s
			v.push_back(s.substr(i));
		}
	}
}

