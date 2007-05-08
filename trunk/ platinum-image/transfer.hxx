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
    pane = source->widget->reset_tf_controls();
    }

template <class ELEMTYPE >
transfer_base<ELEMTYPE >::~transfer_base ()
    {
    }

/*template <class ELEMTYPE >
void transfer_base::finish ()
    {
    pane->end();
    }*/

// *** transfer_brightnesscontrast ***

template <class ELEMTYPE >
transfer_brightnesscontrast<ELEMTYPE >::transfer_brightnesscontrast (image_storage<ELEMTYPE > * s) : transfer_base<ELEMTYPE >(s)
    {
    ELEMTYPE intrange = this->source->get_max() - this->source->get_min();
    float contrange =4 * (std::numeric_limits<ELEMTYPE>::max() - std::numeric_limits<ELEMTYPE>::min())/intrange; //4 is arbitrary, the formula gives some basic headroom

    this->pane->label ("Intensity/contrast");
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


// *** transfer_mapcolor ***

template <class ELEMTYPE >
transfer_mapcolor<ELEMTYPE >::transfer_mapcolor  (image_storage<ELEMTYPE > * s):transfer_base<ELEMTYPE >(s)
    {
    this->pane->resize(0,0,270,35);
    this->pane->resizable(NULL);

    this->pane->begin();

        {
        int top = 5;
        int left = 5;
        RGBvalue vcolor = RGBvalue();
        std::ostringstream label;

        for (int c=0;c<20;c++)
            {


            if (c > 0 && c % 8 == 0)
                {
                left = 5;
                top+=25;
                }

            label << c ;

            Fl_Box * b = new Fl_Box (left,top,left+ 15, top + 15);

            b->copy_label(label.str().c_str());
            label.flush();

            get(c,vcolor);

            b->color(fl_rgb_color(vcolor.r(), vcolor.g(),vcolor.b()));
            if (vcolor.mono() < 80)
                { b->color(FL_WHITE); }

            left += 20;
            }

        }

    this->pane->end();

    this->refresh();
    }

template <class ELEMTYPE >
void transfer_mapcolor<ELEMTYPE >::get (const ELEMTYPE v, RGBvalue &p)
    {
    switch (v)
        {
        case 0 :
            p.set_rgb (0,0,0);
            break;
        case 1:
            p.set_rgb (255,255,255);
            break;
        case 2:
            p.set_rgb (255,0,0);
            break;
        case 3:
            p.set_rgb (0,0,255);
            break;
        case 4:
            p.set_rgb (0,255,0);
            break;
        case 5:
            p.set_rgb (255,0,255);
            break;
        case 6:
            p.set_rgb (255,255,0);
            break;
        case 7:
            p.set_rgb (255,0,255);
            break;

            // *** darker ***
        case 8:
            p.set_rgb (127,0,0);
            break;
        case 9:
            p.set_rgb (0,0,127);
            break;
        case 10:
            p.set_rgb (0,127,0);
            break;
        case 11:
            p.set_rgb (127,0,127);
            break;
        case 12:
            p.set_rgb (127,127,0);
            break;
        case 13:
            p.set_rgb (127,0,127);
            break;

            // *** lighter ***
        case 14:
            p.set_rgb (255,127,127);
            break;
        case 15:
            p.set_rgb (127,127,255);
            break;
        case 16:
            p.set_rgb (127,255,127);
            break;
        case 17:
            p.set_rgb (255,127,255);
            break;
        case 18:
            p.set_rgb (255,255,127);
            break;
        case 19:
            p.set_rgb (255,127,255);
            break;

        default:
            //gray indicates that the scale is exceeded
            p.set_rgb (127,127,127);
        }
    }

// *** transfer_default ***

template <class ELEMTYPE >
transfer_default<ELEMTYPE >::transfer_default  (image_storage<ELEMTYPE > * s):transfer_base<ELEMTYPE >(s)
    {
    this->pane->resize(0,0,270,35);
    this->pane->resizable(NULL);

    this->pane->begin();

    // *** FLUID ***
        { Fl_Box* o = white = new Fl_Box(10, 10, 15, 15, "high");
        o->box(FL_THIN_DOWN_BOX);
        o->color(FL_WHITE);
        o->align(FL_ALIGN_RIGHT);
        }
        { Fl_Box* o = black = new Fl_Box(90, 10, 15, 15, "low");
        o->box(FL_THIN_DOWN_BOX);
        o->color(FL_FOREGROUND_COLOR);
        o->align(FL_ALIGN_RIGHT);
        }
    // *** end of FLUID ***

    this->pane->end();

    this->refresh();
    }

template <class ELEMTYPE >
void transfer_default<ELEMTYPE >::get (const ELEMTYPE v, RGBvalue &p)
    {
    p.set_mono(255*(v- this->source->get_min())/(this->source->get_max()- this->source->get_min()));
    }

template <class ELEMTYPE >
void transfer_default<ELEMTYPE >::refresh()
    {
    std::ostringstream label;
    label.flags( std::ios::boolalpha | std::ios::dec );

    label << this->source->get_min();
    black->copy_label(label.str().c_str());
    label.flush();

    label << this->source->get_max();
    white->copy_label(label.str().c_str());
    }