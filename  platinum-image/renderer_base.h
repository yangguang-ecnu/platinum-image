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

#include "listedfactory.h"

class renderer_base
{
public:
    const static float display_scale;
protected:
    int identitet;
    
    int imagestorender_id;	// rendercombination id
    int wheretorender_id;	// rendergeometry id
    
    image_base *get_imagepointer(int imageindex);
    
    static int maxrendererID;
    
public:
	renderer_base();
    virtual ~renderer_base() {}
    
    static listedfactory<renderer_base> renderer_factory;
    
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
    
#pragma mark *** render parameters ***
    
    rendercombination * imagestorender;    //a list of images to render
                                           //public, because it is managed by viewport too
                                           //could also make renderer and viewport friends
    
    rendergeometry * wheretorender;                 //lookat and direction vectors for rendering
    void connect_geometry (rendergeometry *);       //attach a certain geometry to this renderer
    int combination_id();    
	void connect_combination (rendercombination *);
	
	int geometry_id() const;
	
#pragma mark *** rendering & data interaction ***
    
    virtual void render_position(unsigned char *rgb, int rgb_sx, int rgb_sy)
        {pt_error::error("Calling undefined render_position(...)",pt_error::warning);}
    
    virtual void render_threshold (unsigned char *rgba, int rgb_sx, int rgb_sy, thresholdparvalue * threshold) {pt_error::error("Calling undefined render_threshold(...)",pt_error::warning);}
    
    //get values from current view, pixel coordinates
    virtual std::map<std::string,float> get_values_view(int vx, int vy,int sx,int sy) const;    
    
    //get values from composite, unit image coordinates
    virtual std::map<std::string,float> get_values_world(Vector3D unitPos) const;             
    
    //convert view coordinates to voxels, virtual since the result depends on what's visible,
    //which in turn depends on how it's rendered
    virtual Vector3D view_to_voxel(int vx, int vy, int sx, int sy,int imageID = -1) const    
        {
		pt_error::error("Calling undefined view_to_voxel(...)",pt_error::warning);
		Vector3D tmp;
		return tmp;	//Corrects error in Visual C++ compilation... ("must return a value")
		}
    virtual Vector3D view_to_world(int vx, int vy,int sx,int sy) const 
        {
		pt_error::error("Calling undefined view_to_world(...)",pt_error::warning);
		Vector3D tmp;
		return tmp;	//Corrects error in Visual C++ compilation... ("must return a value")
		}
    
    //result is deterministic regardless of what's visible, no virtual:
    std::vector<int> world_to_view (int view_size_x,int view_size_y,const Vector3D wpos) const;
    static std::vector<int> world_to_view (rendergeometry * g,int sx,int sy,const Vector3D wpos);

    void look_at (float x, float y, float z,float zoom=0);
    
    //NOTE: none of the move commands update the rendered image,
    //that should be done once elsewhere
    
    virtual void move( float pan_x, float pan_y, float pan_z);
    //!move in world coordinates
    
    virtual void move_view (int vsize,int pan_x, int pan_y, int pan_z = 0, float zoom_d = 1);
    //!move in view coordinates
    
    virtual void move_voxels (int,int,int);
    //!move in voxels - which image's voxel is a question of definition  
    
    int get_id()
        { return identitet; }
    
    virtual std::string find_typekey() const = 0;
    
    virtual bool supports_mode (int m)
        {return false;}
    
    virtual void connect_image(int rID) = 0;

	virtual void refesh_overlay(int vp_offset_x, int vp_offset_y, int vp_w, int vp_h)=0;
};

#endif
