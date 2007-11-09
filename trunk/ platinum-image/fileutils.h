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

#define MAXPATHLENGTH 512

//S_ISDIR is not defined in MSVC
#ifndef S_ISDIR
#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif

void trailing_slash (std::string &s);                           //! ensures an accurate directory path,
                                                                //! adds a trailing slash ( / ) if
                                                                //! there wasn't one already

std::vector<std::string> get_dir_entries (std::string path);    //return string vector listing directory contents
std::vector<std::string> get_dir_entries_ending_with(std::string path, std::string ending);    //return string vector listing directory contents that end with (ending)

std::string path_parent (std::string);                          //!get parent (full path except file/indicated dir)
std::string path_end (std::string);                             //!get file/dir name pointed to without path
std::vector<std::string> subdirs (std::string dir_path);       //!get immediate subdirectories (full paths)

bool file_exists (std::string file_path);   //! return whether file exists. //! NOTE: returns false for existing directory
bool dir_exists (std::string file_path);                              


//------------- String handling functions ----------------------

bool does_string_end_with(std::string s, std::string ending);
bool remove_file_lastname(std::string &s, int max_no_lastname_chars=3);

std::string int2str(int i);
std::string float2str(float f);

template <class ELEMTYPE >
std::string templ_to_string (ELEMTYPE t)
    {
    std::ostringstream output;
    output.flags( std::ios::boolalpha | std::ios::dec );
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
std::string templ_to_string (unsigned char t)
    {
    std::ostringstream output;
    output.flags( std::ios::boolalpha | std::ios::dec );
    output.fill(' ');

    output << static_cast<unsigned char>(t);

    return output.str();
    }*/


#endif