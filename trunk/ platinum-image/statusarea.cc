// $Id: $

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

#include "statusarea.h"

#include "FLTKviewport.h" //for VPT_TEST
#include "viewporttool.h"

#include <FL/Enumerations.H>
#include <FL/Fl_Window.H>
#include <stdio.h>

/*statusarea::statusarea(int X, int Y, int W, int H, const char *L = 0):Fl_Group (x,y,w,h,label)
{
    const int progress_w = 40;
    
    box(FL_NO_BOX);

    message_group = add_pane<Fl_Group> ("message");
    
    //Fl_Group::current (this);
    status_message = new Fl_Output (X,Y,W,H);
    status_message->box(FL_ENGRAVED_BOX);
    status_message->value("Platinum $Revision: 305 $");
    status_message->color(FL_BACKGROUND_COLOR);
    
    progress_ = new Fl_Progress (this->w()-progress_w,this->y(),progress_w,this->h());
    
    resizable(this);
    end();
     
}*/

statusarea::statusarea(int X, int Y, int W, int H, const char *L)
: Fl_Group(0, 0, W, H, L) {
    const int progress_w = 100;
    
    statusarea *o = this;
    o->box(FL_FLAT_BOX);
    o->color(FL_BACKGROUND_COLOR);
    o->selection_color(FL_BACKGROUND_COLOR);
    o->labeltype(FL_NO_LABEL);
    o->labelfont(0);
    o->labelsize(14);
    o->labelcolor(FL_FOREGROUND_COLOR);
    o->align(FL_ALIGN_BOTTOM_RIGHT);
    o->when(FL_WHEN_RELEASE);
    
    //o->color(FL_BLUE);
    
    { Fl_Output* o = messageText = new Fl_Output(0, 0, W-progress_w, 24);
        o->box(FL_THIN_DOWN_BOX);
        o->labeltype(FL_NO_LABEL);
        o->align(FL_ALIGN_CENTER);
        o->color(FL_BACKGROUND_COLOR);
        resizable(o);
    }            
    { Fl_Progress* o = progress_ = new Fl_Progress(W-progress_w, 0, progress_w, 24);
        o->box(FL_NO_BOX);
        o->color((Fl_Color)55);
        o->selection_color((Fl_Color)3);
        o->labeltype(FL_NO_LABEL);
        o->minimum(0);
        o->maximum(0);
        
        /*progress_->box(FL_FLAT_BOX);
        progress_->color(FL_BLUE);*/
    }

position(X, Y);
end();

message("Platinum $Revision: 305 $");
}

void statusarea::message (const std::string m)
{
    messageString = m;
    messageText->value(messageString.c_str());
    
}

void statusarea::progress (int step, std::string m, int num_steps)
{
    message (m);
    
    if (num_steps > 0)
        {
        progress_->maximum (num_steps);
        }
    
    if (pt_error::error_if_true(step > progress_->maximum(),"Setting progress step higher than current # of steps",pt_error::warning))
        {
        progress_->maximum (step+1);
        }
    
    progress_->value(step);
    
    Fl::check(); //update interface
}

void statusarea::interactive_message (const std::string m)
{
    if (m == "")
        {  
        //restore persistent message
        messageText->value(messageString.c_str());
        }
    else
        {
        messageText->value(m.c_str());
        }
}

void statusarea::switch_pane (std::string key)
{
    int X = x();
    int W = w();

    for (std::map<std::string, Fl_Group *>::iterator itr = panes.begin();itr != panes.end();itr++)
        {
        if (itr->first == key)
            {
            Fl_Group * gr = itr->second;
            
            gr->position(x(),y());
            //messageText->resize(gr->x()+gr->w(),y(),w()-gr->w(),w());
            X = gr->x()+gr->w();
            W = w()-gr->w();
            
            gr->show();
            }
        else
            {itr->second->hide();}
        }
    
    messageText->resize(X,y(),W,h());
    
    init_sizes();
    damage (FL_DAMAGE_CHILD);
}