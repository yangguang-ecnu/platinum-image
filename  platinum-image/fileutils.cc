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

void trailing_slash (std::string &s)
    {
    if (*s.rbegin() != '/')
        {
        s.append("/");
        }
    }

std::vector<std::string> get_dir_entries (std::string path)
    {
    // *** POSIX ***

    std::vector<std::string> f;
    dirent *ep;
    char cpath[MAXPATHLENGTH];
    strcpy (cpath,path.c_str());

    DIR * dp = opendir (cpath);

    while (ep = readdir (dp))
        f.push_back (ep->d_name);

    (void) closedir (dp);

    return f;
    }

std::vector<std::string> get_dir_entries_ending_with(std::string path, std::string ending)
    {
    // *** POSIX ***

    std::vector<std::string> f;
    dirent *ep;
    char cpath[MAXPATHLENGTH];
    strcpy (cpath,path.c_str());

    DIR * dp = opendir (cpath);

	while (ep = readdir (dp)){
		std::string res = std::string(ep->d_name);
		int s = ending.size();
		int from = res.size()-s;
		if(from<0){from=0;}

		std::string this_ending = res.substr(from,s);
//		std::cout<<"res="<<res<<std::endl;
//		std::cout<<"this_ending="<<this_ending<<std::endl;
		
		if(this_ending == ending)
		{
			f.push_back (ep->d_name);
		}
	}

    (void) closedir (dp);
    return f;
    }


std::string path_end (std::string file_path)
    {
    unsigned int pos;

    pos=file_path.rfind("/",file_path.length()-2);
    
    if (pos !=std::string::npos)
        {
        file_path.erase(0,pos+1);
        }
    
    return file_path;
  
    }

std::string path_parent (std::string file_path)
    {
    unsigned int pos;

	//try "/"
    pos=file_path.rfind("/",file_path.length()-1);

	//try "\" (Windows paths might use these)
	if(pos ==std::string::npos){
	   pos=file_path.rfind("\\",file_path.length()-1);
	}
    
    if (pos !=std::string::npos)
        {
        file_path.erase(pos+1,file_path.length());
        }
    
    return file_path;
    }

bool file_exists (std::string file_path)
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

bool dir_exists (std::string file_path)
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

bool does_string_end_with(std::string s, std::string ending)
{
	if(s.find_last_of(ending) == s.size()-1)
	{
		return true;
	}
	return false;
}

bool remove_file_lastname(std::string &s, int max_no_lastname_chars)
{
	unsigned int last = s.find_last_of(".");
	std::cout<<"last="<<s.find_last_of(".")<<std::endl;
	std::cout<<"size="<<s.size()<<std::endl;
	if( last >= (s.size() -max_no_lastname_chars -1) )
	{
		s = s.substr(0,last);
		return true;
	}
	return false;
}

std::vector<std::string> subdirs (std::string dir_path)
    {
    trailing_slash(dir_path);

    std::vector<std::string> result = get_dir_entries (dir_path);

    std::vector<std::string>::iterator dirs = result.begin();

    while (result.size() > 0 && dirs != result.end())
        {
        //sort out items which are not directories
        //or circular references
        if (*dirs == "." || *dirs == ".." || !dir_exists(dir_path + *dirs) )
            {
            result.erase(dirs);
            }
        else
            {
            (*dirs).insert(0,dir_path);
            trailing_slash(*dirs);

            ++dirs; 

            }
        }
    return result;
    }


std::string int2str(int i){
return templ_to_string(i);
}

std::string float2str(float f){
return templ_to_string(f);
}

