//////////////////////////////////////////////////////////////////////////
//
//  Transfer $Revision$
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

#include <FL/Fl_Box.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Slider.H>

#include "color.h"
#include "listedfactory.h"

template <class ELEMTYPE>
    class image_storage;

class transferfactory;

const std::string tfunctions[] =
    {"Default",
    "Brightness/contrast",
    "Labels",
    "Linear",
    "Spline","" };

class transfer_manufactured //! Sub-base class that holds the static factory object
    {
    static transferfactory factory;
    };

template <class ELEMTYPE >
class transfer_base: public transfer_manufactured
    {
    protected:
        image_storage<ELEMTYPE > * source;
        Fl_Group*  pane;

        //transfer_base (): transfer_base (NULL) {}
        transfer_base (image_storage<ELEMTYPE > * s);
        
    public:
        virtual ~transfer_base ();

        virtual void get (const ELEMTYPE v, RGBvalue &p) = 0;
        virtual void refresh()
            {}
    };

class transferfactory //! transfer gets its own object factory type because constructors for templated classes cannot be stored
    {
    protected:
        Fl_Menu * fmenu;
    public:
        transferfactory ()
            {
            int num_items = 0;

            while  (tfunctions[num_items] != "")
                { num_items++; }

            fmenu = new Fl_Menu_Item [num_items];

            for (int m=0; m < num_items; m++)
                {
                /*dir_menu_items[m].label(preset_direction_labels[m]);
            dir_menu_items[m].callback(&set_direction_callback);
            dir_menu_items[m].user_data(cbp);
            dir_menu_items[m].flags= FL_MENU_RADIO;*/
                }
            }

        ~transferfactory()
            { delete [] fmenu; }

        template <class ELEMTYPE >
            transfer_base<ELEMTYPE > *Create(factoryIdType unique_id,image_storage<ELEMTYPE > * s)
            {
            if (unique_id == tfunctions [0] )
                {return transfer_default<ELEMTYPE>(s);}
            }

        Fl_Menu * function_menu ()
            {
            
            }
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
    class transfer_interpolated: public transfer_base <ELEMTYPE >
    {
protected:
        IMGELEMCOMPTYPE lookup [0xFFFFFFF]; //accomodates unsigned long
public:
        transfer_interpolated (image_storage <ELEMTYPE > * s);
        void get (const ELEMTYPE v, RGBvalue &p);
    };

template <class ELEMTYPE >
    class transfer_linear: public transfer_interpolated <ELEMTYPE >
    {
public:
        transfer_linear (image_storage <ELEMTYPE > * s);
    };

template <class ELEMTYPE >
    class transfer_spline: public transfer_interpolated <ELEMTYPE >
    {
public:
        transfer_spline (image_storage <ELEMTYPE > * s);
    };
    
#include "transfer.hxx"
#include "transfer_interpolated.hxx"

#endif