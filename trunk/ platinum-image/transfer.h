//////////////////////////////////////////////////////////////////////////
//
//  Transfer
//
//  Transfer function classes
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

// *** NOTE: work on this class is in a preliminary stage and may change
// in major ways ***

#ifndef __transfer__
#define __transfer__

#include "color.h"
#include <FL/Fl_Group.H>

template <class ELEMTYPE >
class transfer_base
    {
    protected:
        Fl_Group*  pane;
        transfer_base ();
        ~transfer_base ();
        /* void finish(); */
    public:
        virtual void get (const ELEMTYPE v, RGBvalue &p);
    };

template <class ELEMTYPE >
class transfer_brightnesscontrast: public transfer_base <ELEMTYPE >
    {
    public:
        transfer_brightnesscontrast ();
        void get (const ELEMTYPE v, RGBvalue &p);
    };

#include "transfer.hxx"

#endif