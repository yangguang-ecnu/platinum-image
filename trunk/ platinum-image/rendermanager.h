//////////////////////////////////////////////////////////////////////////
//
//  Rendermanager $Revision$
///
/// Keeps a list of renderer objects and handles communication between
/// viewport and renderer.
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

#ifndef __rendermanager__
#define __rendermanager__

#include <vector>

#include "ptmath.h"

#include "rendercombination.h"
#include "rendergeometry.h"
#include "renderer_base.h"

#include "global.h"

//-1 is used as empty status value (when the renderer could not be found),
//so these values are chosen to be separate from that
#define RENDERER_EMPTY 1
#define RENDERER_NOT_EMPTY 0

class rendermanager
    {
    private:
        std::vector<rendergeometry *> geometries;
        std::vector<rendercombination *> combinations;
        std::vector<renderer_base*> renderers;
    public:
        void listrenderers();
        int create_renderer(RENDERER_TYPES rendertype);
        void remove_renderer (int ID);
        int find_renderer_index(int uniqueID);
        int find_renderer_id (int index);
        renderer_base * get_renderer (int ID);
        void connect_image_renderer(int rendererID, int imageID);
        void image_vector_has_changed();
        void image_has_changed (int ID);

        void move(int rendererIndex, float panX, float panY, float panZ=0, float scale=1);//alter rendergeometry (pan,zoom)
        void render(int rendererIndex, unsigned char *rgbimage, int rgbXsize, int rgbYsize);
        void render_thumbnail(unsigned char *rgb, int rgb_sx, int rgb_sy, int image_ID);
        void render_threshold(int rendererIndex, unsigned char *rgba, int rgb_sx, int rgb_sy, thresholdparvalue * threshold);
       
        void toggle_image (int rendererIndex, int imageID);   //turn image on or off in a combination,
                                                                //triggered by menu selection

        int get_renderer_type(int rendererIndex);
        int get_blend_mode (int rendererIndex);
        int get_combination_id(int rendererIndex);              //get id of combination object associated with
                                                                //renderer rendererIndex

        rendercombination* get_combination (int ID);

        int get_geometry_id(int rendererIndex);                 //get id of geometry object associated with
                                                                //renderer rendererIndex
        void combination_update_callback (int c_id);
        std::vector<int> combinations_from_image (int imageID);    //return any combinations containing the image with imageID
        void geometry_update_callback (int g_id);

        int image_rendered(int rendererIndex, int volID);  //pass-through for checking if
                                                            //a certain image (id) is part of a combination
                                                            //for renderer rendererIndex

        int image_at_priority (int rendererIndex, int priority);   //pass through for getting visible
                                                                    //image ID at certain priority (for iterating)
                                                                    //returns NOT_FOUND_ID past end

        int renderer_empty (int rendererID);       //returns whether renderer has no images to render or not
        std::vector<float> get_values (int rendererIndex, int px, int py,int sx, int sy);
        Vector3D get_location (int rendererIndex, int imageID, int px, int py, int sx, int sy);

        void set_geometry(int renderer_index,Vector3D * dir,Vector3D * look_at,int zoom);
        void set_geometry(int renderer_index,Matrix3D * dir);

        void set_blendmode(int rend_index,blendmode mode);   //sets combination-wide blend mode, if blendmode is later defined
                                                             //for each image in a combination, this should set all of them
    };

#endif
