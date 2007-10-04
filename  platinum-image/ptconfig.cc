// $Id:$

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

#include "ptconfig.h"

#include <iostream>
#include <fstream>


void pt_config::initialize(string filename){
	
	if(file_exists(filename)){
		ConfigFile cf = ConfigFile(filename,"=","//");	
	}else{
		std::ofstream out( filename.c_str() );

		out<<"Platinum default configuration file" <<endl;
		out<<endl;
		out<<"latest_path = . \t//Latest path used from platinum" <<endl;

		out.close();
	}
}



/*
std::string errornames[] =
    {
    //array allows error names to be printed to strings and whatnot
    "Notice",	
    "Debug error",
    "Warning",
    "Serious error",
    "Fatal error"
    };

pt_error::pt_error(const std::string& __arg, errorLevel l)  : std::exception(), _M_msg(__arg)
    { 
    level = l;
    std::cout << "pt_error exception: (" << errornames [l] << ")" << std::endl << __arg << std::endl;
    }

const char* pt_error::what() const throw()
    {
    return _M_msg.c_str();
    }

bool pt_error::error_if_true (bool condition, std::string message, pt_error::errorLevel l)
    {
    if (condition == true)
        {
        throw pt_error (message, l);
        }
    return condition;
    }

void pt_error::error ( std::string message, pt_error::errorLevel l)
    {
	//Simplifies debugging in "release mode"
//	std::ofstream myfile;
//	myfile.open("debug.txt");
//	myfile<<message<<std::endl;
//	myfile.close();

    if (l >= serious)
        {
        throw pt_error (message, l);
        }
    else
        {
        std::cerr << errornames[l] << ": " << message << std::endl;
        }
    }


const std::string pt_error::level_name (pt_error::errorLevel l)
    {
    return errornames[l];
    }

*/