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

#define __userIOmanager_cc__

#include "userIOmanager.h"

userIOmanager userIOmanagement;

#include <FL/Fl.H>

userIOmanager::userIOmanager()
    {
    }

userIOmanager::~userIOmanager()
    {
    //userIOs are FLTK classes, deleted automatically with their parent
    }

void userIOmanager::setup()
    {
    const unsigned int xpos=Fl_Group::current()->x();
    const unsigned int ypos=DATALISTINITHEIGHT;
    const unsigned int height=Fl_Group::current()->h()-DATALISTINITHEIGHT; //claim the remaining vertical space below datawidget list
    const unsigned int width=Fl_Group::current()->w();

    widgets_scroll = new horizresizeablescroll(xpos,ypos,width,height);

    widgets_scroll->end();
    }

void userIOmanager::show_message (std::string name, std::string message)
    {
    int uioid = add_userIO (name,NULL,"");

    add_par_message(uioid,"",message);
    finish_userIO(uioid);
    }

int userIOmanager::add_userIO (std::string name, userIO_callback* cback,std::string ok_label)
    {
    widgets_scroll->begin();
    
    unsigned int new_userIO_y_pos=widgets_scroll->interior->y(); 
    
    if (IOblocks.size() >0)
    {new_userIO_y_pos=IOblocks.back()->y()+IOblocks.back()->h();}
    IOblocks.push_back(new userIO(widgets_scroll->interior->x(),new_userIO_y_pos,widgets_scroll->interior->w(),block_header_height,name,cback,ok_label));
    
    widgets_scroll->end();

    return IOblocks.back()->id();
    }

void userIOmanager::finish_userIO(int userIO_ID)
    {
    //no action for the specific userIO object, that may be
    //needed for other toolkits however
    
    widgets_scroll->redraw();
    }

void userIOmanager::delete_userIO (int userIO_ID)
    {
    userIO * tbd = block_from_ID (userIO_ID);

    //remove from userIO blocks vector
    IOblocks.remove(tbd);

    //remove from widget list
    tbd->hide();
    widgets_scroll->remove(tbd);

    //this is how deletion should be done theoretically,
    //the close button is removed and "safe deleted" first,
    //then delete the whole group (everything but
    //the button)
    //causes subsequent crash for some reason
   
    //delete tbd;
    }

userIO * userIOmanager::block_from_ID(int IOblockID)
    {
    std::list<userIO *>::iterator itr=IOblocks.begin();

    while (itr != IOblocks.end())
        {
        if (**itr == IOblockID)
            { return *itr; }
        itr++;
        }

    return (NULL);
    }

int userIOmanager::add_par_float(int userIO_ID, std::string new_param_name,float max,float min)
    {
    FLTKuserIOparameter_base * par=new FLTKuserIOpar_float (new_param_name,max,min);

    return block_from_ID(userIO_ID)->add_par(par);
    }

int userIOmanager::add_par_int(int userIO_ID, std::string new_param_name,long max,long min)
    {
    FLTKuserIOparameter_base * par=new FLTKuserIOpar_int (new_param_name,max,min);

    return block_from_ID(userIO_ID)->add_par(par);
    }

int userIOmanager::add_par_int_box(int userIO_ID, std::string new_param_name,long max,long min)
    {
    FLTKuserIOparameter_base * par=new FLTKuserIOpar_int_box (new_param_name,max,min);

    return block_from_ID(userIO_ID)->add_par(par);
    }

int userIOmanager::add_par_bool (int userIO_ID, std::string new_param_name,bool init_status)
    {
    FLTKuserIOparameter_base * par=new FLTKuserIOpar_bool (new_param_name,init_status);

    return block_from_ID(userIO_ID)->add_par(par);
    }

int userIOmanager::add_par_volume(int userIO_ID, std::string new_param_name)
    {
    FLTKuserIOparameter_base * par=new FLTKuserIOpar_volume (new_param_name);

    return block_from_ID(userIO_ID)->add_par(par);
    }

int userIOmanager::add_par_histogram_2D(int userIO_ID, std::string new_param_name)
{
    FLTKuserIOparameter_base * par=new FLTKuserIOpar_histogram2D (new_param_name);
    
    return block_from_ID(userIO_ID)->add_par(par);
}

int userIOmanager::add_par_message(int userIO_ID, std::string new_param_name, std::string message)
{
    FLTKuserIOparameter_base * par=new FLTKuserIOpar_message (new_param_name, message);
    
    return block_from_ID(userIO_ID)->add_par(par);
}

void userIOmanager::image_vector_has_changed()
    {
    std::list<userIO *>::iterator itr=IOblocks.begin();

    while (itr != IOblocks.end())
        {
        (*itr)->image_vector_has_changed();
        itr++;
        }
    }

std::vector<FLTKuserIOpar_histogram2D *> userIOmanager::get_histogram_for_volume (int volumeID)
    {
    std::vector<FLTKuserIOpar_histogram2D *> result;

    std::list<userIO *>::iterator itr=IOblocks.begin();

    while (itr != IOblocks.end())
        {
        std::vector<FLTKuserIOpar_histogram2D *> found;

        found=(*itr)->get_histogram_for_volume(volumeID);

        std::vector<FLTKuserIOpar_histogram2D *>::iterator ritr =found.begin();

            while (ritr != found.end())
                {
                result.push_back((*ritr));
                ritr++;
                }
        itr++;
        }

    return result;
    }

// resizable scroll group widget
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