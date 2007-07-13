//////////////////////////////////////////////////////////////////////////
//
//  Statusarea $Revision: $
//
/// Fl_Group that displays output strings, progress bar
///
//  $LastChangedBy: arvid.r $
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

#ifndef __statusarea__
#define __statusarea__

#include <map>
#include <string>

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Progress.H>

class statusarea : public Fl_Group {
public:
    statusarea(int X, int Y, int W, int H, const char *L = 0);
private:
    std::map<std::string,Fl_Group *> panes;
    Fl_Progress* progress;    
    Fl_Output *messageText;
    
    // end of FLUID output
public:
    void message (std::string);
    template <class FLGROUPTYPE>
    FLGROUPTYPE * add_pane (std::string); //!get controls pane which is also added to the statusarea's map
    void switch_pane (std::string key);
    void realtime_message (const char* const); //display fast-update message in char array stored elsewhere
                                   //must be cleared with realtime_message (NULL) before array
                                   //is deleted
};

template <class FLGROUPTYPE>
FLGROUPTYPE * statusarea::add_pane (std::string key)
{
    Fl_Group::current(NULL);
    
    FLGROUPTYPE * group = new FLGROUPTYPE (x(),y(),w(),h());
    //panes[key]->type(FL_HORIZONTAL);
    group->box(FL_THIN_DOWN_BOX);
    group->resizable(NULL); //controls pane will maintain its shape with resizes
    add(group);
    group->hide();
    
    panes[key] = group;
    return group;
}

#endif //__statusarea__