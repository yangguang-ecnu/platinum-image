//$Id: $

// This file is part of the Platinum library.
// Copyright (c) 2007 Uppsala University.
//
//    The Platinum library is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    The Platinum library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with the Platinum library; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#include "rendererMIP.h"

rendererMIP::rendererMIP():renderer_image_base()
{
}

void rendererMIP::render_position(unsigned char *rgb, int rgb_sx, int rgb_sy)
{
	this->fill_rgbimage_with_value(rgb,rgb_sx,rgb_sy,100);
}

std::string rendererMIP::find_typekey() const
{
return "MPR renderer";
}

void rendererMIP::connect_data(int dataID)
    {
    the_rc->add_data(dataID);
	}

void rendererMIP::paint_overlay(int vp_w, int vp_h_pane, bool paint_rendergeometry)
{
//	cout<<"rendererMPR::paint_overlay..("<<vp_w<<" "<<vp_h_pane<<") rc_id="<<the_rg->get_id()<<endl;
//	paint_slice_locators_to_overlay(vp_w, vp_h_pane, the_rg, the_rc);
//	if(paint_rendergeometry){
//		paint_rendergeometry_to_overlay(vp_w, vp_h_pane, the_rg, the_rc);
//	}
}


/*
rendererMIP::rendererMIP():rendererVTK ()
{
    //create rendered objects from selected images/points
    //vtkRenderer *ren1 = vtkRenderer::New();
    //ren1‑>AddActor(actor);
    
}
*/
