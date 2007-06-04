//////////////////////////////////////////////////////////////////////////
//
//   Bruker $Revision$
///
///  Bruker file format import
//
//   based on the description at
//   http://imaging.mrc-cbu.cam.ac.uk/imaging/FormatBruker
//   and
//   http://www.sph.sc.edu/comd/rorden/bru2anz.html
//
//   $LastChangedBy$
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

#ifndef __bruker__
#define __bruker__

#include <string>
#include <vector>

class image_base;

class imageloader
{
public:
    imageloader(const std::vector<std::string> f)
        { }
    ~imageloader()
        { }
};

class brukerloader: public imageloader 
{
private:
    std::string                 session;
    std::vector<std::string>    runs;
    std::vector<std::string>    reconstructions;
    
    void get_reconstructions(std::string run_dir_path);
    
public:
    brukerloader (std::vector<std::string>);
    image_base * read (std::vector<std::string>&);
};

#endif