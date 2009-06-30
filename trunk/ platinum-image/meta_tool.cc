// $Id:$

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

#include <iomanip>

#include "meta_tool.h"
#include "renderer_base.h"	
#include "viewmanager.h"
#include "userIOmanager.h"

#include "rendermanager.h"
#include "image_scalar.h"
#include "image_scalar.hxx"

extern datamanager datamanagement;
extern rendermanager rendermanagement;
extern viewmanager viewmanagement;
extern userIOmanager userIOmanagement;


meta_tool::meta_tool(viewport_event & event) : nav_tool(event)
{
	if (event.type() == pt_event::hover || event.type() == pt_event::key)
	{
        event.grab();
	}
}

meta_tool::~meta_tool()
{
}

const std::string meta_tool::name()
{
	return "Metastasis analysis tool";
}

void meta_tool::init()
{
}

void meta_tool::handle(viewport_event &event)
{
    const int * mouse2d = event.mouse_pos_local();
    FLTK_Event_pane *fp = event.get_FLTK_Event_pane();

//	cout<<"event(type,state)=("<<event.type()<<","<<event.state()<<")";
//	cout<<"(x,y,w,h) ("<<myPort->ROI_rectangle_x<<",";
//	cout<<myPort->ROI_rectangle_y<<",";
//	cout<<myPort->ROI_rectangle_w<<",";
//	cout<<myPort->ROI_rectangle_h<<") -->"<<myPort->ROI_rect_is_changing<<endl;

	if(event.state() == pt_event::end && myPort->ROI_rect_is_changing && event.type() != pt_event::hover)
       {
		//hover is excluded since this event.type is thown when mouse is first clicked
		cout << "************" << endl;
		int center_x = myPort->ROI_rectangle_x + myPort->ROI_rectangle_w/2;
		int center_y = myPort->ROI_rectangle_y + myPort->ROI_rectangle_h/2;
		Vector3D center_world = myRenderer->view_to_world(center_x,center_y, fp->w(), fp->h());

		Vector3D corner1_world = myRenderer->view_to_world(myPort->ROI_rectangle_x,							myPort->ROI_rectangle_y, fp->w(), fp->h());
		Vector3D corner2_world = myRenderer->view_to_world(myPort->ROI_rectangle_x+myPort->ROI_rectangle_w, myPort->ROI_rectangle_y, fp->w(), fp->h());
		Vector3D corner3_world = myRenderer->view_to_world(myPort->ROI_rectangle_x,							myPort->ROI_rectangle_y + myPort->ROI_rectangle_h, fp->w(), fp->h());
		Vector3D w_world = corner2_world - corner1_world;
		Vector3D h_world = corner2_world - corner1_world;
		float width = magnitude(w_world);
		float height = magnitude(h_world);

		//"zoom" and "ZOOM_CONSTANT" are defined so that "zoom 1 gives 50 mm"
		viewmanagement.zoom_specific_vp(2, center_world, ZOOM_CONSTANT/max(width, height) );
		viewmanagement.zoom_specific_vp(3, center_world, ZOOM_CONSTANT/max(width, height) );
		viewmanagement.zoom_specific_vp(4, center_world, ZOOM_CONSTANT/max(width, height) );

		cout << "center_x,center_y " << center_x << "," << center_y << " (world)(" << center_world << ")" << endl;

		//zoom in other vp...
		myPort->ROI_rectangle_x = -1;
		myPort->ROI_rectangle_y = -1;
		myPort->ROI_rectangle_w = -1;
		myPort->ROI_rectangle_h = -1;
		myPort->ROI_rect_is_changing = false;
		myPort->refresh_overlay();
        }


	switch (event.type())
	{
		case pt_event::key:  

			if (event.key_combo(pt_event::space_key)) 
				{
				cout << "< SPACE KEY PRESSED >" << endl;
				fl_cursor(FL_CURSOR_CROSS, FL_BLACK, FL_WHITE); 	
				event.grab();
				}	

			if (event.key_combo(pt_event::space_key + pt_event::shift_key)) //ok
				{
				event.grab();	
				}
		//break;

		  case pt_event::adjust:
		//case (pt_event::adjust && pt_event::space_key):

			last_local_x = mouse2d[0];
			last_local_y = mouse2d[1];
			//cout << endl << "last_local_x: " << last_local_x << ", last_local_y: " << last_local_y << endl;
					
			if(myPort->ROI_rectangle_x == -1)
				{
				myPort->ROI_rectangle_x = last_local_x;
				myPort->ROI_rectangle_y = last_local_y;
				myPort->ROI_rect_is_changing = true;
				}

			else{
				myPort->ROI_rectangle_w = last_local_x - myPort->ROI_rectangle_x;
				myPort->ROI_rectangle_h = last_local_y - myPort->ROI_rectangle_y; //bara visar rektangeln
				}
	
		break;
 
		
		break;

	}
		// NOTE: no break, update hovering also (scroll is ignored because there is no iterate event)
	nav_tool::handle(event);
	//}
}