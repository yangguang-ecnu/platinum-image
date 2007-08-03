/////////////////////////////////////////////////////////////////////////////////
//
//  Rendercombination $Revision$
///
/// Stores list of images and blend mode(s) for renderers
///
//  $LastChangedBy$

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

#ifndef __imagerenderinfo__
#define __imagerenderinfo__
#include "colormap.h"
#include "datamanager.h"
#include "global.h"
//const int MAXRENDERVOLUMES = 10; //maximum layers for a single renderer

//blending modes
enum blendmode {BLEND_ENDOFLIST=-2,
BLEND_NORENDER,
BLEND_OVERWRITE=0,
BLEND_MAX,
BLEND_MIN,
BLEND_AVG,
BLEND_TINT, NUM_BLEND_MODES ,
RENDER_THRESHOLD};  //RENDER_THRESHOLD is not user-selectable, used internally 
                    //for rendering threshold overlays

//blend_mode_labels defined in viewport.cc
class rendercombination
    {
public:
        class renderpair {
public:
            int ID;
            data_base * pointer;
            blendmode mode;
            
            renderpair();
            renderpair(const int, data_base*,const blendmode);
        };        
        
private:
        //int renderimages [MAXRENDERVOLUMES];
        std::list<renderpair> renderdata;
        //colormap colortable;
        int id; //id to identify this combination in callbacks
        static int new_rc_ID;   //unique id to assign newly created combinations
        blendmode blend_mode_;
public:
            rendercombination();
        typedef std::list<renderpair>::const_iterator iterator;
        rendercombination(int ID);  //constructor that populates
                                    //the renderdata array 
                                    //from the beginning
        void data_vector_has_changed ();   //image has been added or removed - update renderlist
        iterator begin() const;
        iterator end() const;
        bool empty() const;
        image_base* top_image ()const; //topmost image
        image_base* get_imagepointer(int ID);
        void add_data(int ID);
        void toggle_data(int imageID);
        void remove_image(int ID);
        int image_rendered(int ID);//for updating various widgets: returns nonzero if
                                   //the image ID is included in this combination
                                   //may return value indicating per-image blending mode for this combination
            int get_id();
            blendmode blend_mode();          //get blendmoide
            void blend_mode (blendmode b);   //set blendmode
    };

#endif

