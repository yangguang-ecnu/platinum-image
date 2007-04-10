//////////////////////////////////////////////////////////////////////////
//
//  RendererMPR
//
//  MPR renderer subclass which also renders thumbnails via a static
//  function. Like other possible renderers it also converts between
//  local and global coordinate systems.
//
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

#ifndef __rendererMPR__
#define __rendererMPR__

#include "cursor3D.h"
#include "renderer_base.h"
#include "colormap.h"
#include "datamanager.h"

#include "global.h"

//extern datamanager datamanagement;

//Complimentary vector functions (TODO: unuse and remove)
Vector3D mult_elems( const Vector3D & one, const Vector3D & other );
Vector3D div_elems( const Vector3D & numer, const Vector3D & denom );

class rendererMPR : public renderer_base
    {
    private:
        void fill_rgbimage_with_value(unsigned char *rgb, int x, int y, int w, int h, int rgb_sx, int value);

        static void render_(uchar *pixels, int rgb_sx, int rgb_sy,rendergeometry * where,rendercombination * what,thresholdparvalue * threshold);

    public:

        void connect_image(int vHandlerID); //add image to rendering combination
        int renderer_type();                        //return RENDERER_MPR

        static void render_thumbnail (unsigned char *rgb, int rgb_sx, int rgb_sy, int volume_ID);
        void render_threshold (unsigned char *rgba, int rgb_sx, int rgb_sy, thresholdparvalue * threshold);
        void render_position(unsigned char *rgb, int rgb_sx, int rgb_sy);

        Vector3D view_to_voxel(int volumeID, int vx, int vy,int sx,int sy);
        Vector3D view_to_unit(int vx, int vy,int sx,int sy);
       
        std::vector<float> get_values(int vx, int vy,int sx,int sy);
    };

#endif