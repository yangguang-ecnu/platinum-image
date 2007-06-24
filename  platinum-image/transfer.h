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

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Slider.H>

#include "color.h"
#include "listedfactory.h"
#include "FLTKutilities.h"
#include "points_seq_func1D.h"	//used for spline/linear knots

template <class ELEMTYPE>
class image_storage;

template <class ELEMTYPE>
class histogram_1D;

template <class ELEMTYPE >
class transfer_interpolated;

class transferfactory;
class transferchart;

const std::string tfunction_names[] =
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

	transfer_base (image_storage<ELEMTYPE > * s);

public:
	virtual ~transfer_base ();

	virtual void get (const ELEMTYPE v, RGBvalue &p) = 0;
	virtual void refresh()
	{}
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

class transferfactory //! transfer gets its own object factory type because constructors for templated classes cannot be stored
{
protected:
	int num_items;
public:
	transferfactory ()
	{
		num_items = 0;

		while  (tfunction_names[num_items] != "")
		{ num_items++; }
	}

	~transferfactory()
	{ }

	template <class ELEMTYPE >
		transfer_base<ELEMTYPE > *Create(factoryIdType unique_id,image_storage<ELEMTYPE > * s)
	{
		if (unique_id == tfunction_names [0] )
		{return transfer_default<ELEMTYPE>(s);}

	}

	Fl_Menu * function_menu (Fl_Callback * cb) //! get menu 
	{
		Fl_Menu_Item * fmenu;

		fmenu = new Fl_Menu_Item [num_items];

		for (int m=0; m < num_items; m++)
		{
			init_fl_menu_item(fmenu[m]);

			fmenu[m].label(tfunction_names[m].c_str());
			fmenu[m].callback(cb);
			fmenu[m].argument(m);
			fmenu[m].flags = FL_MENU_RADIO;
		}

		return fmenu;
	}
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
public:
    class transferchart :protected Fl_Widget
    {
protected:
        friend class               transfer_interpolated;
        unsigned long              lookupSize;
        float                      
            lookupStart,
            lookupScale,
            lookupEnd; //redundant, but allows a faster implementation of get(...)
        IMGELEMCOMPTYPE                 * lookup ;
        histogram_1D<ELEMTYPE >         * histogram;
        IMGELEMCOMPTYPE                 * imgdata;
        Fl_RGB_Image			        * histimg;	//keeps the histogram background layer
        points_seq_func1D<float,float>	intensity_knots;		//used for anchor points handling and interpolation 
                                                                //knots are of course also wanted for R,G,B, respectively.
        float leftBound,rightBound;
        
        transferchart (histogram_1D<ELEMTYPE > *, int, int, int, int);
public:
            virtual ~transferchart();
        
        void draw ();
		int handle(int);
        
        virtual void render_lookup (IMGELEMCOMPTYPE lookup [],int lookupSize) = 0;
	};
protected:
    transferchart * chart;
    transfer_interpolated (image_storage <ELEMTYPE > * s);
    //		virtual void recalculate_lookup()=0;
public:
        virtual ~transfer_interpolated();
    void get (const ELEMTYPE v, RGBvalue &p);
};


template <class ELEMTYPE >
class transfer_linear: public transfer_interpolated <ELEMTYPE >
{
protected:
    class transferchart_linear: public transfer_interpolated<ELEMTYPE >::transferchart
    {
public:
        transferchart_linear (histogram_1D<ELEMTYPE > * hi, int x, int y, int w, int h);
        virtual void render_lookup (IMGELEMCOMPTYPE lookup [],int lookupSize);
    };
    
    public:
	transfer_linear (image_storage <ELEMTYPE > * s);
};


template <class ELEMTYPE >
class transfer_spline: public transfer_interpolated <ELEMTYPE >
{
public:
	transfer_spline (image_storage <ELEMTYPE > * s);
};

#endif