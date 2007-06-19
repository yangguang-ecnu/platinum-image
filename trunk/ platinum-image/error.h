//////////////////////////////////////////////////////////////////////////
//
//  Error $Revision$
//
//  Exceptions and error handling
//
//  $LastChangedBy$
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

#ifndef __error__
#define __error__

#include <string>

class pt_error : public std::exception 
    {
    public:
        typedef enum {
            notice,   //! would have just been written to cout
            debug,    //! problem, which pertains only to developers
            warning,  //! non-serious problem of interest to users 
            serious,  //! error affecting result
            fatal     //! error affecting result so that the program cannot continue
            } errorLevel;
    private:
        std::string _M_msg;
        errorLevel level;

    public:
        explicit 
            pt_error(const std::string& __arg,errorLevel l = serious);

        virtual 
            ~pt_error() throw() { }

        virtual const char* 
            what() const throw();

        static void error_if_true (bool condition, std::string message, pt_error::errorLevel l = serious);
        static void error_if_false (bool condition, std::string message, pt_error::errorLevel l = serious)
            {error_if_true (!condition, message, l);}
        static void error_if_null (const void * p, std::string message, pt_error::errorLevel l = fatal)
            {error_if_true (p == NULL, message, l);}
    };


#endif __error__