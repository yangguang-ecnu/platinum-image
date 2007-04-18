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

#include "transfer.h"

template <class ELEMTYPE >
transfer_base<ELEMTYPE >::transfer_base (image_storage<ELEMTYPE > * s)
    {
    source=s;
    pane = new Fl_Group (0,0,300,80);
    }

template <class ELEMTYPE >
transfer_base<ELEMTYPE >::~transfer_base ()
    {
    delete pane;
    }

/*template <class ELEMTYPE >
void transfer_base::finish ()
    {
    pane->end();
    }*/

// *** transfer_brightnesscontrast ***

template <class ELEMTYPE >
transfer_brightnesscontrast<ELEMTYPE >::transfer_brightnesscontrast (image_storage<ELEMTYPE > * s) : transfer_base(s)
    {
    ELEMTYPE intrange = source->get_max()-source->get_min();
    float contrange =4 * (std::numeric_limits<ELEMTYPE>::max() - std::numeric_limits<ELEMTYPE>::min())/intrange; //4 is arbitrary, the formula gives some basic headroom

    label ("Intensity/contrast");
    intensity = 0;
    contrast = 1.0;

    intensity_ctrl = new Fl_Slider (FL_HORIZONTAL,0,20,300,16,"Intensity");
    intensity_ctrl->value (intensity);
    intensity_ctrl->bounds(-intrange,intrange);

    contrast_ctrl = new Fl_Slider (FL_HORIZONTAL,0,50,300,16,"Contrast");
    intensity_ctrl->value (intensity);
    intensity_ctrl->bounds(-contrange,contrange);
    }

template <class ELEMTYPE >
void transfer_brightnesscontrast<ELEMTYPE >::get (const ELEMTYPE v, RGBvalue &p)
    {
    p.r(v * intensity + contrast);
    p.g(v * intensity + contrast);
    p.b(v * intensity + contrast);
    }

// *** transfer_default ***

template <class ELEMTYPE >
void transfer_default<ELEMTYPE >::get (const ELEMTYPE v, RGBvalue &p)
    {
    p.set_mono(255*(v-source->get_min())/(source->get_max()-source->get_min()));
    }