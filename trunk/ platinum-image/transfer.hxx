//$Id$

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

#ifndef __transfer_hxx__
#define __transfer_hxx__

#include "transfer.h"

#include "datawidget.h"
#include "transfer_interpolated.hxx"

#include <sstream>

template <class ELEMTYPE >
transfer_base<ELEMTYPE >::transfer_base (image_storage<ELEMTYPE > * s)
    {
    source=s;
	if(source->widget!=NULL)
		pane = pt_error::error_if_null(dynamic_cast<datawidget<image_base>*>(source->widget)->reset_tf_controls(),"Trying to reset_tf_controls on widget of other type than datawidget<image_base>",pt_error::fatal);
	else
		pane = NULL;
    }

template <class ELEMTYPE >
transfer_base<ELEMTYPE >::~transfer_base ()
    {
    }

template <class ELEMTYPE >
void transfer_base<ELEMTYPE >::redraw_image_cb( Fl_Widget* o, void* p )
    {
    REDRAWCALLBACKPTYPE theImage = reinterpret_cast<REDRAWCALLBACKPTYPE > (p);
    theImage->redraw();
    }

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
    contrast_ctrl->value (contrast);
    contrast_ctrl->bounds(-contrange,contrange);

    this->pane->end();
    }

template <class ELEMTYPE >
void transfer_brightnesscontrast<ELEMTYPE >::get (const ELEMTYPE v, RGBvalue &p)
    {
    p.set_mono(v * this->intensity + this->contrast);
    }


// *** transfer_mapcolor ***

template <class ELEMTYPE >
transfer_mapcolor<ELEMTYPE >::transfer_mapcolor  (image_storage<ELEMTYPE > * s):transfer_base<ELEMTYPE >(s)
    {
    //pane->choice ();
    Fl_Group * frame = this->pane;

    frame->resize(0,0,270,90);
    frame->resizable(NULL);

        int top = 5;
        int left = 5;
        
        for (int c=0;c<20;c++)
            {
            RGBvalue vcolor = RGBvalue();
            std::ostringstream label;

            if (c > 0 && c % 8 == 0)
                {
                left = 5;
                top+=30;
                }

            label << c ;

            Fl_Box * b = new Fl_Box (frame->x()+left,frame->y()+top,15,15);

            b->copy_label(label.str().c_str());
            label.flush();

            b->align(FL_ALIGN_RIGHT);
            b->box(FL_DOWN_BOX);
            b->labelsize (9);

            get(c,vcolor);

            b->color(fl_rgb_color(vcolor.r(), vcolor.g(),vcolor.b()));
            /*if (vcolor.mono() < 80)
                { b->color(FL_WHITE); }*/

            left += 30;
            }


    frame->end();

    this->update();
    }

template <class ELEMTYPE >
void transfer_mapcolor<ELEMTYPE>::get (ELEMTYPE v, RGBvalue &p)
    {
    //TODO: repeat colors for v > IMGLABELMAX with modulo
    switch (static_cast<unsigned char>(v))
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
            p.set_rgb (0,255,255);
            break;

            // *** darker ***
        case 8:
            p.set_rgb (192,0,0);
            break;
        case 9:
            p.set_rgb (0,0,192);
            break;
        case 10:
            p.set_rgb (0,192,0);
            break;
        case 11:
            p.set_rgb (192,0,192);
            break;
        case 12:
            p.set_rgb (192,192,0);
            break;
        case 13:
            p.set_rgb (0,192,192);
            break;

            // *** lighter ***
        case 14:
            p.set_rgb (255,192,192);
            break;
        case 15:
            p.set_rgb (192,192,255);
            break;
        case 16:
            p.set_rgb (192,255,192);
            break;
        case 17:
            p.set_rgb (255,192,255);
            break;
        case 18:
            p.set_rgb (255,255,192);
            break;
        case IMGLABELMAX:
            p.set_rgb (192,255,255);
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
	if(this->pane == NULL)
		return;
    this->pane->resize(0,0,270,35);
    this->pane->resizable(NULL);

    //pane->choice ();

    // *** FLUID ***
        { Fl_Box* o = white = new Fl_Box(80, 10, 15, 15, "high");
        o->box(FL_THIN_DOWN_BOX);
        o->color(FL_WHITE);
        o->align(FL_ALIGN_RIGHT);
        }
        { Fl_Box* o = black = new Fl_Box(15, 10, 15, 15, "low");
        o->box(FL_THIN_DOWN_BOX);
        o->color(FL_FOREGROUND_COLOR);
        o->align(FL_ALIGN_RIGHT);
        }
    // *** end of FLUID ***

    this->pane->end();

    this->update();
    }

/*
template <>	//JK2 image_complex testing
void transfer_default<std::complex<float> >::get (const std::complex<float> v, RGBvalue &p)
    {
    p.set_mono(255);
//    p.set_mono(255*(v- this->source->get_min())/(this->source->get_max()- this->source->get_min()));
    }
*/

template <class ELEMTYPE >
void transfer_default<ELEMTYPE >::get (const ELEMTYPE v, RGBvalue &p)
	{
		if(this->source->get_max()>this->source->get_min())	{
			p.set_mono(255*(v- this->source->get_min())/(this->source->get_max()- this->source->get_min()));
		}else{
			p.set_mono(0);
		}
	}


template <class ELEMTYPE >
std::string templ_to_string (ELEMTYPE t)
    {
    std::ostringstream output;
    output.flags( std::ios::boolalpha | std::ios::dec );
    output.fill(' ');

    //a true templated function would *not* cast to float,
    //instead use a specialization for bool and unsigned char,
    //however that causes a problem with the current structure (see below)
    output << static_cast<float>(t);

    return output.str();
    }

//template specialization seems to be treated as regular function body,
//and creates a link error for being multiply defined here

/*template <>
std::string templ_to_string (unsigned char t)
    {
    std::ostringstream output;
    output.flags( std::ios::boolalpha | std::ios::dec );
    output.fill(' ');

    output << static_cast<unsigned char>(t);

    return output.str();
    }*/

template <class ELEMTYPE >
void transfer_default<ELEMTYPE >::update()
    {
    std::string label = templ_to_string (this->source->get_min());
    black->copy_label(label.c_str());

    label = templ_to_string (this->source->get_max());
    white->copy_label(label.c_str());
    }

#endif