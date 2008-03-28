// $Id$

// This file is part of the Platinum library.
// Copyright (c) 2007 Uppsala University.
//
//    The Platinum library is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    The Platinum library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with the Platinum library; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#include "global.h"

#include <cstdlib> //for NULL

#include "FLTKutilities.h"

void init_fl_menu_item(Fl_Menu_Item & m)
    {
    m.label("");
    m.shortcut(0);
    m.callback((Fl_Callback *)NULL,0);
    m.user_data(NULL);
    m.flags= 0;
    m.labeltype(FL_NORMAL_LABEL);
    m.labelfont(0);
    m.labelsize(FLTK_LABEL_SIZE);
    m.labelcolor(FL_BLACK);
    }

int fl_menu_size (const Fl_Menu_Item * const m)
    {
    if (m != 0)
        {
        int item;
        for (item = 0; m[item].label() !=NULL;item++)
            {}
        return item;
        }

    return -1;
    }

void fl_menu_userdata_delete (const Fl_Menu_Item * m)
    {
    int size = fl_menu_size(m);

    for (int i = 0; i < size;i++)
        {
        void * ud = m[i].user_data();
        if (m != NULL)
            { delete ud; }
        }
    }

horizresizeablescroll::horizresizeablescroll (int x, int y, int w, int h, const char *label) : Fl_Scroll (x,y,w,h,label)
    {
    //children are deleted automatically with parents, so  
    interior=new Fl_Pack(x,y,w-FLTK_SCROLLBAR_SIZE,h);
    type(Fl_Scroll::VERTICAL_ALWAYS);
    interior->type(FL_VERTICAL);
    box(FL_FLAT_BOX);
    interior->box(FL_NO_BOX);
    }

void horizresizeablescroll::begin ()
    {
    interior->begin();
    }

void horizresizeablescroll::end ()
    {
    interior->end();
    Fl_Scroll::end();
    }

void horizresizeablescroll::resize (int newx, int newy, int neww, int newh)
    {
    int scroll_x=interior->x()-x();
    int scroll_y=interior->y()-y();

    Fl_Widget::resize(newx, newy, neww, newh);
    interior->resize(newx+scroll_x,newy+scroll_y,neww-FLTK_SCROLLBAR_SIZE,interior->h());
    }



// -------- FLTK_Editable_Slider ------------

void FLTK_Editable_Slider::Slider_CB2() {
        static int recurse = 0;
        if(recurse){ 
			std::cout<<"FLTK_Editable_Slider-Slider_CB2-recurse"<<std::endl;
			return;
		}else{
            recurse = 1;
			input->value( float2str(slider->value()).c_str() );    // pass slider's value to input
//			std::cout<<"label()="<<this->label()<<std::endl;
			if(this->callback()!=NULL){ //if connected to a callback!!!
				this->do_callback(); //calls callback function connected to this "FLTK_Editable_Slider"
			}
			recurse = 0;
        }
    }

void FLTK_Editable_Slider::Slider_CB(Fl_Widget *w, void *data) {

        ((FLTK_Editable_Slider*)data)->Slider_CB2();
    }


void FLTK_Editable_Slider::Input_CB2(){
        static int recurse = 0;
        if(recurse){
			std::cout<<"FLTK_Editable_Slider-Input_CB2-recurse"<<std::endl;
			return;
		}else{
            recurse = 1;
            slider->value(atof(input->value()));
//			std::cout<<"label()="<<this->label()<<std::endl;
			this->do_callback(); //calls callback function connected to this class
            recurse = 0;
        }
    }

void FLTK_Editable_Slider::Input_CB(Fl_Widget *w, void *data) {
        ((FLTK_Editable_Slider*)data)->Input_CB2();
    }

FLTK_Editable_Slider::FLTK_Editable_Slider(int x, int y, int w, int h, const char *l, int input_w) : Fl_Group(x,y,w,h,0) {
        input  = new Fl_Float_Input(x, y, input_w, h);
        input->callback(Input_CB, (void*)this);
        input->when(FL_WHEN_ENTER_KEY|FL_WHEN_NOT_CHANGED);
		input->label(l);
		input->labelsize(h);
		input->textsize(FLTK_LABEL_SIZE);

        slider = new Fl_Slider(x+input_w+5, y, w - (input_w+5), h);
        slider->type(FL_HOR_SLIDER);
        slider->callback(Slider_CB, (void*)this);

		end();			// close the group
    }

float FLTK_Editable_Slider::value() 
	{ return slider->value(); }

void FLTK_Editable_Slider::value(float val) 
	{ slider->value(val); Slider_CB2(); }

void FLTK_Editable_Slider::value_no_Fl_callback(float val)
	{ input->value(float2str(val).c_str()); slider->value(val);}    // pass slider's value to input

void FLTK_Editable_Slider::minimum(float val)
	{ slider->minimum(val); }

float FLTK_Editable_Slider::minimum()
	{ return slider->minimum(); }

void FLTK_Editable_Slider::maximum(float val) 
	{ slider->maximum(val); }

float FLTK_Editable_Slider::maximum() 
	{ return slider->maximum(); }

void FLTK_Editable_Slider::bounds(float low, float high) 
	{ slider->bounds(low, high); }

void FLTK_Editable_Slider::step(float val)
	{ slider->step(val); }

void FLTK_Editable_Slider::precision(float val)
	{ slider->precision(val); }

void FLTK_Editable_Slider::labelsize(unsigned char s)
	{ input->labelsize(s); }

void FLTK_Editable_Slider::textsize(unsigned char s)
	{ input->textsize(s); }

const char* FLTK_Editable_Slider::label()
	{ return input->label();};
