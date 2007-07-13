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
    
    progress = new Fl_Progress (this->w()-progress_w,this->y(),progress_w,this->h());
    
    resizable(this);
    end();
     
}*/

statusarea::statusarea(int X, int Y, int W, int H, const char *L)
: Fl_Group(0, 0, W, H, L) {
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
    { Fl_Pack* o = statusPack = new Fl_Pack(0, 0, 500, 25);
        o->type(1);
        { Fl_Pack* o = optionsEnclosure = new Fl_Pack(0, 0, 40, 25);
            o->type(1);
            o->end();
        }
        { Fl_Output* o = messageText = new Fl_Output(40, 0, 460, 24);
            o->box(FL_THIN_DOWN_FRAME);
            o->labeltype(FL_NO_LABEL);
            o->align(FL_ALIGN_CENTER);
            Fl_Group::current()->resizable(o);
            //o->value("Platinum $Revision: 305 $");
        }
        o->end();
        Fl_Group::current()->resizable(o);
    }
    { Fl_Progress* o = progress = new Fl_Progress(500, 0, 100, 25);
        o->box(FL_NO_BOX);
        o->color((Fl_Color)55);
        o->selection_color((Fl_Color)3);
        o->labeltype(FL_NO_LABEL);
    }
    position(X, Y);
    end();
}

void statusarea::message (std::string m)
{
    messageText->value(m.c_str());
}

void statusarea::realtime_message (const char* const m)
{
    messageText->static_value(m);
}

void statusarea::switch_pane (std::string key)
{
    for (std::map<std::string, Fl_Group *>::iterator itr = panes.begin();itr != panes.end();itr++)
        {
        if (itr->first == key)
            {
            itr->second->position(optionsEnclosure->x(),optionsEnclosure->y());
            itr->second->show();
            }
        else
            {itr->second->hide();}
        }
    
    init_sizes();
    damage (FL_DAMAGE_CHILD);
}