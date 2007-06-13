// $Id$

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

#include "FLTK2Dregionofinterest.h"

#include <algorithm>

#include <FL/fl_draw.H>

//#include "histogram.h"

#include "rendermanager.h"

extern userIOmanager userIOmanagement;
extern rendermanager rendermanagement;

FLTK2Dregionofinterest * FLTK2Dregionofinterest::current_ROI=NULL;

FLTK2Dregionofinterest::FLTK2Dregionofinterest (Fl_Widget * frame)
    {
    dragging=false;

    mode=ROI_RECT;

    region_start_x=0;
    region_start_y=0;
    region_end_x=0;
    region_end_y=0;

    canvas_size_x=frame->w();
    canvas_size_y=frame->h();
    }

void FLTK2Dregionofinterest::draw(Fl_Widget * canvas)
    {
    //Fl_Group canvas = Fl_Group::current();

    if (current_ROI == this)
        {
        switch (mode)
            {
            case ROI_RECT:
                fl_rect(canvas->x()+region_start_x, canvas->y()+region_start_y, region_end_x-region_start_x, region_end_y-region_start_y);
                break;
            case ROI_CIRCLE:
                fl_arc(canvas->x()+region_start_x, canvas->y()+region_start_y, region_end_x-region_start_x, region_end_y-region_start_y, 360);
                break;
            }
        }
    }

void FLTK2Dregionofinterest::drag (int x_last, int y_last, int dx, int dy,Fl_Widget * frame)
     {
     //const int grab_margin= 2;

     //drag signals that this is the active ROI
     current_ROI=this;

     switch (mode)
         {
         case ROI_RECT:
             if (!dragging && x_last-frame->x() >= region_start_x && x_last-frame->x() <= region_end_x &&
                 y_last-frame->y() >= region_start_y && y_last-frame->y() <= region_end_y )
                 {
                 //inside region, drag around
                 region_start_x+=dx;
                 region_start_y+=dy;
                 region_end_x+=dx;
                 region_end_y+=dy;
                 }
             else
                 {
                 int point_x=x_last-frame->x();
                 int point_y=y_last-frame->y();

                 //outside, start dragging a new region
                 if (dragging==false)
                     {
                     drag_root_x=point_x;
                     drag_root_y=point_y;

                     dragging=true;
                     }

                 region_start_x=drag_root_x;
                 region_start_y=drag_root_y;
                 region_end_x=dx+point_x;
                 region_end_y=dy+point_y;

                 //ensure region has positive dimension
                 if (region_end_x < region_start_x)
                     {t_swap (region_end_x,region_start_x);}
                 if (region_end_y < region_start_y)
                     {t_swap (region_end_y,region_start_y);}
                 }
             break;

         case ROI_CIRCLE:
             /*
             int center_x=region_start_x+ (region_end_x-region_start_x)/2;
             int center_y=region_start_y+ (region_end_y-region_start_y)/2;

             if sqrt(pow (x_last-canvas_size_x - center_x,2)+pow (y_last-canvas_size_y - center_y,2))
                {}
             */
             break;
         }
     }

 void FLTK2Dregionofinterest::drag_end()
     {
     dragging=false;
     histograms.clear();
     }

 void FLTK2Dregionofinterest::attach_histograms (int rendererIndex)
     {
     //first drag iteration; a number of values have to be collected and stored
     histograms.clear();

     int p=0;
     int rendered_vol_ID=rendermanagement.image_at_priority (rendererIndex,p);

     while (rendered_vol_ID > 0)
         {
         std::vector<FLTKuserIOpar_histogram2D *>  found;
         found=userIOmanagement.get_histogram_for_image(rendered_vol_ID);
         std::vector<FLTKuserIOpar_histogram2D *>::iterator fitr =found.begin();

         while (fitr != found.end())
             {
             if (std::find(histograms.begin(),histograms.end(),(*fitr)) == histograms.end())
                 //check for duplicate
                 {histograms.push_back((*fitr));}
             fitr++;
             }

         p++;
         rendered_vol_ID=rendermanagement.image_at_priority (rendererIndex,p);
         }
     }

 void FLTK2Dregionofinterest::resize (int dx, int dy, float zoom,Fl_Widget * frame)
     {
     if (frame->w()!= canvas_size_x || frame->h()!= canvas_size_y || zoom != 1)
         {
         //zooming or resizing has to be done through rendergeometry, reset region
         region_start_x=region_start_y=region_end_x=region_end_y=0;
         }

     region_start_x+=dx;
     region_start_y+=dy;
     region_end_x+=dx;
     region_end_y+=dy;

     canvas_size_x=frame->w();
     canvas_size_y=frame->h();
     }