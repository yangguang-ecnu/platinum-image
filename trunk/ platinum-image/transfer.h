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

// *** NOTE: work on this class is at a preliminary stage and may change
// in major ways ***

#ifndef __transfer__
#define __transfer__

#include <FL/Fl_Group.H>
#include <FL/Fl_Slider.H>

#include "color.h"

template <class ELEMTYPE>
    class image_storage;

template <class ELEMTYPE >
class transfer_base
    {
    protected:
        image_storage<ELEMTYPE > * source;
        Fl_Group*  pane;

        //transfer_base (): transfer_base (NULL) {}
        transfer_base (image_storage<ELEMTYPE > * s);
        
        /* void finish(); */
    public:
        virtual ~transfer_base ();

        virtual void get (const ELEMTYPE v, RGBvalue &p) = 0;
        virtual void refresh()
            {}
    };

template <class ELEMTYPE >
class transfer_brightnesscontrast: public transfer_base <ELEMTYPE >
    {
    protected:
        Fl_Slider* intensity_ctrl;
        Fl_Slider* contrast_ctrl;
        ELEMTYPE intensity;
        float contrast;
    public:
        transfer_brightnesscontrast (image_storage <ELEMTYPE > *);
        void get (const ELEMTYPE v, RGBvalue &p);
    };

template <class ELEMTYPE >
class transfer_mapcolor: public transfer_base <ELEMTYPE >
    {
    public:
        transfer_mapcolor (image_storage <ELEMTYPE > * s);
        void get (const ELEMTYPE v, RGBvalue &p);
    };


template <class ELEMTYPE >
class transfer_default: public transfer_base <ELEMTYPE >
    {
    protected:
        Fl_Box *white;
        Fl_Box *black;
    public:
        transfer_default (image_storage <ELEMTYPE > * s);
        void get (const ELEMTYPE v, RGBvalue &p);
        virtual void refresh();
    };


#include "transfer.hxx"

#endif