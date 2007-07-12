//////////////////////////////////////////////////////////////////////////
//
//  Histo2D_tool $Revision: $
//
/// The base class for implementing mouse behaviors/actions in a viewport
///
//  $LastChangedBy: $
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

#ifndef __histo2D_tool__
#define __histo2D_tool__

#include "viewporttool.h"

#include "global.h"

#include <vector>

class FLTKuserIOpar_histogram2D;
class threshold_overlay;

class FLTK2Dregionofinterest
{
protected:
    ROImode mode;
    int canvas_size_x,canvas_size_y;
public:
    FLTK2Dregionofinterest (Fl_Widget *);
    
    void draw (Fl_Widget * canvas);
    void drag (int x_last, int y_last, int dx, int dy,Fl_Widget * frame);
    void drag_end ();
    void resize (int dx, int dy, float zoom,Fl_Widget * frame);
    void resize (viewport_event &e);
    
    //this variable fills the same purpose as a region-of-interest manager,
    //lets all ROIs know/change which one is active and is drawn
    static FLTK2Dregionofinterest * current_ROI;
    
    int region_start_x, region_start_y,region_end_x, region_end_y;  //current ROI in screen coordinates
    
    std::vector<FLTKuserIOpar_histogram2D *> histograms;   //histograms to be updated
    void attach_histograms (int rendererIndex);            //find corresponding histograms
    
    //cached data during drags
    
    bool dragging;                                              //whether dragging is underway
    int drag_root_x;                                            //start of box/circle
    int drag_root_y;                                            //to handle negative dimensions
};

class histo2D_tool : public nav_tool //tool for userIO click & drag (only in Histo2D at this time)
{
    //NOTE: this tool was ported from an earlier architecture and
    //a bad example of viewporttool implementation. For a good example, see cursor_tool
private:
    FLTK2Dregionofinterest * ROI;
    threshold_overlay * overlay;
    
public:
    histo2D_tool (viewport_event &event,thresholdparvalue * v = NULL,viewport * vp = NULL, renderer_base * r = NULL);
    
    static const std::string name ();
    static void init (); 
    virtual void handle(viewport_event &);
    
    void attach (viewport * vp, renderer_base * r);
    threshold_overlay * get_overlay();
};

#endif //__histo2D_tool__