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
	Fl_Group* frame = this->pane;
	frame->resize(0,0,270,85);
	frame->callback(transfer_base<ELEMTYPE >::redraw_image_cb);
	frame->user_data( static_cast<REDRAWCALLBACKPTYPE>(this->source) );

	min = this->source->get_min();
	max = this->source->get_max();
    ELEMTYPE intrange = max-min;

//	brightness = float(intrange)/2.0; //JK

//	float contrast_min = 0;
  //  float contrast_max = PI/2.0;
//    contrast = PI/4.0;

	//JK - GUI modification
	int xx = frame->x();
	int yy = frame->y();
	int ww = frame->w();
	int w2 = 50;
	int dy = 20;

	min_ctrl = new Fl_Value_Slider(xx+w2,yy+5,ww-w2,15,"Min");
	min_ctrl->type(FL_HOR_SLIDER);
	min_ctrl->align(FL_ALIGN_LEFT);
    min_ctrl->value(min);
    min_ctrl->bounds(min,max);
	min_ctrl->step(float(intrange)/100.0);
	min_ctrl->precision(2);
	min_ctrl->callback(slider_cb,this);

	max_ctrl = new Fl_Value_Slider(xx+w2,yy+5+dy,ww-w2,15,"Max");
	max_ctrl->type(FL_HOR_SLIDER);
	max_ctrl->align(FL_ALIGN_LEFT);
    max_ctrl->value(max);
    max_ctrl->bounds(min,max);
	max_ctrl->step(float(intrange)/100.0);
	max_ctrl->precision(2);
	max_ctrl->callback(slider_cb,this);

    brightness_ctrl = new Fl_Slider(xx+w2,yy+5+2*dy,ww-w2,15,"Brightn");
	brightness_ctrl->type(FL_HOR_SLIDER);
	brightness_ctrl->align(FL_ALIGN_LEFT);
    brightness_ctrl->value(float(intrange)/2.0);
    brightness_ctrl->bounds(min,max);
	brightness_ctrl->step(float(intrange)/100.0);
	brightness_ctrl->precision(2);
	brightness_ctrl->callback(slider_cb,this);

    contrast_ctrl = new Fl_Slider(xx+w2,yy+5+3*dy,ww-w2,15,"Contr");
	contrast_ctrl->type(FL_HOR_SLIDER);
	contrast_ctrl->align(FL_ALIGN_LEFT);
    contrast_ctrl->value(atan(255.0/float(intrange)));
    contrast_ctrl->bounds(0,PI/2.0);
	contrast_ctrl->step((PI/2.0)/100.0);
	contrast_ctrl->precision(2);
	contrast_ctrl->callback(slider_cb,this);

	frame->end();
//	this->update();
    frame->do_callback(); //redraw image that the transfer function is attached to ( 
    }

template <class ELEMTYPE >
void transfer_brightnesscontrast<ELEMTYPE >::get (const ELEMTYPE v, RGBvalue &p)
    {
//	float res = 127.5 + contrast*float(v-brightness_inv);
	float res = float(v-min)*255.0/float(max-min);
	if(res<0){res=0.0;}
	if(res>255){res=255.0;}
	p.set_mono(res);
    }

template <class ELEMTYPE >
void transfer_brightnesscontrast<ELEMTYPE >::update(string slider_label)
{
	if(slider_label=="Min"){
		min = this->min_ctrl->value();
		this->brightness_ctrl->value((this->source->get_max()-this->source->get_min())-float(min + max)/2.0);
		this->contrast_ctrl->value(atan(255.0/float(max-min)));

	}else if(slider_label=="Max"){
		max = this->max_ctrl->value();
		this->brightness_ctrl->value((this->source->get_max()-this->source->get_min())-float(min + max)/2.0);
		this->contrast_ctrl->value(atan(255.0/float(max-min)));

	}else if(slider_label=="Brightn"){
		cout<<"brightn."<<endl;
		float new_center = (this->source->get_max()-this->source->get_min()) - this->brightness_ctrl->value();
		float tmp = float(max-min)/2.0;
		float fmin = new_center - tmp;
		float fmax = new_center + tmp;
		if(fmin<this->source->get_min()){
			min = this->source->get_min();
		}else{
			min = fmin;
		}
		if(fmax>this->source->get_max()){
			max = this->source->get_max();
		}else{
			max = fmax;
		}

		this->min_ctrl->value(min);
		this->max_ctrl->value(max);
		this->contrast_ctrl->value(atan(255.0/float(max-min)));
//		cout<<"***brightn***"<<endl;
//		cout<<"new_center="<<new_center<<endl;
//		cout<<"min="<<min<<endl;
//		cout<<"max="<<max<<endl;

	}else if(slider_label=="Contr"){
		float center = float(max+min)/2.0;
		float new_intrange = 255.0/tan(float(this->contrast_ctrl->value()));
		float fmin = center - new_intrange/2.0;
		float fmax = center + new_intrange/2.0;
		if(fmin<this->source->get_min()){
			min = this->source->get_min();
		}else{
			min = fmin;
		}
		if(fmax>this->source->get_max()){
			max = this->source->get_max();
		}else{
			max = fmax;
		}
		this->min_ctrl->value(min);
		this->max_ctrl->value(max);
		this->brightness_ctrl->value((this->source->get_max()-this->source->get_min())-float(min + max)/2.0);
//		cout<<"***Contr***"<<endl;
//		cout<<"center="<<center<<endl;
//		cout<<"new_intrange="<<new_intrange<<endl;
//		cout<<"min="<<min<<endl;
//		cout<<"max="<<max<<endl;
	}
}

template <class ELEMTYPE >
void transfer_brightnesscontrast<ELEMTYPE >::slider_cb(Fl_Widget *o, void *v)
{
//	cout<<"slider_cb"<<endl;
	transfer_brightnesscontrast<ELEMTYPE>* tf = (transfer_brightnesscontrast<ELEMTYPE>*)v;
	tf->update(string(o->label()));
    tf->pane->do_callback(); //redraw transfer function source image...
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
void transfer_default<ELEMTYPE >::update()
    {
    std::string label = templ_to_string (this->source->get_min());
    black->copy_label(label.c_str());

    label = templ_to_string (this->source->get_max());
    white->copy_label(label.c_str());
    }

#endif