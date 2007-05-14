//////////////////////////////////////////////////////////////////////////
//
//   Renderer_base $Revision$
///
///  Renderers produce a 2D viewport pixmap out of images and point sets.
///  The base class has virtual functions for coordinate storage
///  conversion, common to different renderer types.
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

#ifndef __renderer_base__
#define __renderer_base__

#include <iostream>
#include <vector>

#include "rendercombination.h"
#include "datamanager.h"
#include "rendergeometry.h"
#include "threshold.h"

#include "global.h"

class image_base;

//renderer types
enum RENDERER_TYPES { RENDERER_MPR=0, NUM_RENDERER_TYPES, NUM_RENDERER_TYPES_PLUS_END};

//name strings for renderer types
const std::string renderer_labels[] = {"MPR renderer"};

class renderer_base
    {
    protected:
        int identitet;

        int imagestorender_id;
        int wheretorender_id;

        image_base *get_imagepointer(int imageindex);

        static int maxrendererID;

    public:
        renderer_base();
        virtual ~renderer_base() {}
        
        // renderer_base(const renderer_base &k) { *this=k; ::renderer_base(); }

        // virtual const renderer_base &operator=(const renderer_base &k) { return k; }
        bool virtual operator<<(const renderer_base &k)
            { return identitet==k.identitet; }
        bool virtual operator==(const renderer_base &k)
            { return identitet==k.identitet; }
        bool virtual operator==(const int &k) { return identitet==k; }
        bool virtual operator!=(const renderer_base &k)
            { return identitet!=k.identitet; }
        bool virtual operator<(const renderer_base &k)
            { return identitet<k.identitet; }
        bool virtual operator>(const renderer_base &k)
            { return identitet>k.identitet; }
        friend std::istream &operator>>(std::istream &in, renderer_base &k)
            { in >> k.identitet; return in; }
        friend std::ostream &operator<<(std::ostream &ut, const renderer_base &k)
            { ut << "[renderer_base. ID= " << k.identitet << "] "; return ut; }

        // *** render parameters ***

        rendercombination * imagestorender;    //a list of images to render
                                                //public, because it is managed by viewport too
                                                //could also make renderer and viewport friends

        rendergeometry * wheretorender;                 //lookat and direction vectors for rendering
        void connect_geometry (rendergeometry *);       //attach a certain geometry to this renderer
        int combination_id();
        void connect_combination (rendercombination *);

        // *** rendering & data interaction ***

        virtual void render_position(unsigned char *rgb, int rgb_sx, int rgb_sy) = 0;
        virtual void render_threshold (unsigned char *rgba, int rgb_sx, int rgb_sy, thresholdparvalue * threshold) = 0;

        //get values from current view, pixel coordinates
        virtual std::vector<float> get_values(int vx, int vy,int sx,int sy) = 0;    

        //get values from composite, unit image coordinates
        virtual std::vector<float> get_values(Vector3D unitPos);             

        //convert view coordinates to voxels, virtual since the result depends on what's visible,
        //which in turn depends on how it's rendered
        virtual Vector3D view_to_voxel(int imageID, int vx, int vy, int sx, int sy)    
            = 0;                                 

        void look_at (float x, float y, float z,float zoom=0);

        virtual void move( float pan_x, float pan_y, float pan_z=0, float zoom=1);

        int get_id()
            { return identitet; }

        virtual int renderer_type() = 0;

        virtual void connect_image(int rID)
            = 0;
    };

#endif
