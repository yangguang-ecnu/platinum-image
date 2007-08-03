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
    
    pos=file_path.rfind("/",file_path.length()-1);
    
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
