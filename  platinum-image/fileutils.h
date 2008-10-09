//////////////////////////////////////////////////////////////////////////
//
//  FileUtils
//
//  File & directory functions
//	String handling
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

#ifndef __fileutils__
#define __fileutils__

#include <string>
#include <vector>
#include <sys/stat.h>
#include <sstream>

#include "itkGDCMImageIO.h"
#include <itksys/SystemTools.hxx>  //JK -- allows for example creation of new folder...
#include "global.h"

using namespace std;

#define MAXPATHLENGTH 512

//S_ISDIR is not defined in MSVC
#ifndef S_ISDIR
#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif


void trailing_slash (string &s);                           //! ensures an accurate directory path,
                                                                //! adds a trailing slash ( / ) if
                                                                //! there wasn't one already

vector<string> get_dir_entries(string path, bool full_path=false);    //return string vector listing directory contents
vector<string> get_dir_entries_ending_with(string path, string ending);    //return string vector listing directory contents that end with (ending)

string path_parent(string);                         //!get parent (full path except file/indicated dir)
string path_end(string);                            //!get file/dir name pointed to without path
vector<string> subdirs(string dir_path, bool fullpath=true);			//!get immediate subdirectories (full paths)
vector<string> subdirs_where_name_contains(string dir_path, string name_substring);			//!get immediate subdirectories (full paths)
vector<string> get_files_in_dir_where_name_contains(string dir_path, string name_substring);			//!get immediate subdirectories (full paths)

bool file_exists(string file_path);   //! return whether file exists. //! NOTE: returns false for existing directory
bool dir_exists(string file_path);                              
void create_dir(string dir_path);

//------------- String vector specific handling ----------------------

void add_to_string_vector_if_not_present(vector<string> &v, string s);

//------------- Dicom specific file handling ----------------------

vector<string>	get_dicom_files_in_dir(string dir_path, bool full_path=false);
string			get_first_dicom_file_in_dir(string dir_path, bool full_path=false);
string			get_second_dicom_file_in_dir(string dir_path, bool full_path=false); //can for example be used for loading image 2 in dual echo sequences...
vector<string>	get_dicom_files_with_dcm_tag_value(vector<string> files, string dcm_tag, string tag_val);
int				get_number_of_dicom_files_in_dir(string dir_path);
string			get_dicom_tag_value(string file_path, string dcm_tag, bool remove_garbage_char=true);
bool			does_dir_contain_dcmfile_with_tag_value(string dir_path, string dcm_tag, string tag_val, bool recursive_search=false);
string			find_first_sub_dir_containing_dcm_file_with_tag_value(string dir_path, string dcm_tag, string tag_val, bool recursive_search=false);
vector<string>	list_dicom_tag_values_for_this_ref_tag_value(vector<string> files, string dcm_tag, string dcm_tag_val, string dcm_ref_tag);
vector<string>	list_dicom_tag_values_in_dir(string dir_path, string dcm_tag, bool recursive_search=false, bool exclusive_values=false);

bool			does_dicom_file_tag_contain(string file_path, string tag, string content);
bool			is_dicom_file_magnitude_image(string file_path);
bool			is_dicom_file_real_image(string file_path);
bool			is_dicom_file_imaginary_image(string file_path);
string			get_elemtype_in_dicom_file(string file_path);

//------------- String handling functions ----------------------

bool does_string_end_with(string s, string ending);
bool remove_file_lastname(string &s, int max_no_lastname_chars=3);
bool remove_string_ending(string &s, string ending=" ");
string replace_last_substring(string s, string val, string replacement="");

string int2str(int i);
string float2str(float f);

template <class ELEMTYPE >
string templ_to_string (ELEMTYPE t)
    {
    ostringstream output;
	output.flags( ios::boolalpha | ios::dec );
    output.fill(' ');

    //a true templated function would *not* cast to float,
    //instead use a specialization for bool and unsigned char,
    //however that causes a problem with the current structure (see below)
    output << static_cast<float>(t);

    return output.str();
    }

//template specialization seems to be treated as regular function body,
//and creates a link error for being multiply defined here

/*template <>
string templ_to_string (unsigned char t)
    {
    ostringstream output;
    output.flags( ios::boolalpha | ios::dec );
    output.fill(' ');

    output << static_cast<unsigned char>(t);

    return output.str();
    }*/





// split a delimited string into multiple strings
// example: "aaa;bbb;ccc" -> "aaa" "bbb" "ccc"
void split(const std::string & s, char c, std::vector<std::string> & v);
	



#endif