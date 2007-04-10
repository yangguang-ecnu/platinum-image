//////////////////////////////////////////////////////////////////////////
//
//  global constants and macros
//
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

#ifndef __global_h__
#define __global_h__

#define PLATINUM_MAJOR_VER 0
#define PLATINUM_MINOR_VER 1

#define FLTK_LABEL_SIZE 14
#define FLTK_SMALL_LABEL 11
#define FLTK_SCROLLBAR_SIZE 16

#define BUTTONHEIGHT 25
//#define TOOLAREAWIDTH 200

#define DATALISTINITHEIGHT 300

#define NOT_FOUND_ID -1
#define NA 0 // not applicable

#define RGBpixmap_bytesperpixel 3
#define RGBApixmap_bytesperpixel 4

#define RPIXEL 0
#define GPIXEL 1
#define BPIXEL 2

//#define RGBvaluePtr unsigned char*
//#define RGBAvaluePtr unsigned char*
//#define RGBvalue unsigned char[RGBpixmap_bytesperpixel]
//#define RGBAvalue unsigned char[RGBApixmap_bytesperpixel]

struct RGBvalue
    {
    unsigned short r;
    unsigned short g;
    unsigned short b;
    };

struct RGBAvalue
    {
    unsigned short r;
    unsigned short g;
    unsigned short b;
    unsigned short a;
    };

#define FILEPOSTYPE char

#define VOLUMEMAXDIMENSION 3

#endif