//////////////////////////////////////////////////////////////////////////
//
//   threshold.h $Revision$
///
///  Objects for working with n-dimensional threshold values $Revision$
///
//   $LastChangedBy$

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

#ifndef __threshold__
#define __threshold__

#include <cstddef>
#include <FL/Fl_Image.H>

#include "global.h"

class FLTKviewport;
class viewport;

enum {THRESHOLD_2D_MODE_RECT,THRESHOLD_2D_MODE_OVAL};

#define THRESHOLDMAXCHANNELS 3

//N-dimensional threshold values with optional shape variations. Includes a function for creating a new image through thresholding
class thresholdparvalue
    {
    public:
        float low [THRESHOLDMAXCHANNELS];       //lo & hi thresholds
        float high [THRESHOLDMAXCHANNELS];

        int id [THRESHOLDMAXCHANNELS];          //images for which these were selected, where
                                                //applicable (NOT_FOUND_ID otherwise)

        int get_id (int axis);                  //return bounds-safe ID

        int mode;

        thresholdparvalue();
        int make_threshold_image ();           //makes a new image
                                                //(and adds it to datamanager)
                                                //from the threshold parameter
                                                //returns ID of the new image
    };

//Overlay to viewports that displays a segmentation preview for the 2D histogram segmentation project.
class threshold_overlay
{
protected:
    //Fl_RGB_Image * overlay_image;
    FLTKviewport * owner;
    int width, height;
    int rendererIndex;                  //viewport's renderer

    thresholdparvalue * threshold;
public:
    threshold_overlay(FLTKviewport * o, int renderer_index);
    ~threshold_overlay();

    unsigned char * overlay_image_data;
    void render (thresholdparvalue * t = NULL);           //render thresholded image, NULL means re-render (with new geometry)
    void expire();                                        //called when a new thresholding does not
                                                          //apply to this instance or when view is moved etc; stop displaying
    void resize ();
                                                //image but keep buffer
    void FLTK_draw();                           //called by associated viewport to draw within
                                                //display thresholding
                                                //in viewport
    void renderer_index(int index);
};

#endif

