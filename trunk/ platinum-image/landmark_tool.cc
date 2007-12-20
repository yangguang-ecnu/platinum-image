
#include <iomanip>

#include "landmark_tool.h"
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

int landmark_tool::userIO_ID = -1;
//int landmark_tool::point_collection_ID = -1;

landmark_tool::landmark_tool(viewport_event & event) : nav_tool(event)
{
	if (event.type() == pt_event::create || event.type() == pt_event::hover || event.type() == pt_event::hover || event.type() == pt_event::key )
	{
        event.grab();
	}
}

landmark_tool::~landmark_tool()
{
}

const std::string landmark_tool::name()
{
	return "Landmark";
}

void landmark_tool::init()
{
}

void landmark_tool::handle(viewport_event &event)
{
	std::vector<int> mouse2d = event.mouse_pos_local();

	int point_collection_ID;
	
    FLTKviewport * fvp = event.get_FLTK_viewport();
	Vector3D mouse3d = myRenderer->view_to_world(mouse2d[0], mouse2d[1], fvp->w(), fvp->h());

	switch (event.type())
	{	
		case pt_event::create:
		
//			if (point_collection_ID == -1)
//			{
//				std::cout << "point_collection_ID is not registered in landmark_tool" << std::endl;
//				return;
//			}
			
			if (userIO_ID == -1)
			{
				std::cout << "userIO_ID is not registered in landmark_tool" << std::endl;
				return;	
			}
			
			// the get_id() of the top image could be implemented as a get_top_image_id() in renderer_base
			// image_base * top = rendermanagement.get_combination(myRenderer->combination_id())->top_image();
			image_base * top;
			if ( !(top = rendermanagement.get_combination(myRenderer->combination_id())->top_image()) )
			{
				// TODO: use pt_error
				std::cout << "No image in current viewport" << std::endl;
				return; 
			}
			
			if ( userIOmanagement.get_parameter<imageIDtype>(userIO_ID, 0) !=  top->get_id() )
			{
				// TODO: use pt_error
				std::cout << "The id of the selected image and the top image are not the same" << std::endl;
				return;
			}

			event.grab();
			
			point_collection_ID = userIOmanagement.get_landmarksID(userIO_ID);
						
			if (event.state() == pt_event::begin)
			{				
				if ( datamanagement.find_data_index(point_collection_ID) == -1 )
				{	// not found
					// TODO: use pt_error
					std::cout << "The point_collection is not found in the datamanager" << std::endl;
					return;
				}
				
				point_collection * points = dynamic_cast<point_collection *>(datamanagement.get_data(point_collection_ID));
						
				int index_of_active = userIOmanagement.get_parameter<landmarksIDtype>(userIO_ID, 1);				
				points->set_active(index_of_active);
				
				if (index_of_active <= 0)	// -1 means active is not set
				{							//  0 means no line in the Fl_Hold_Browser i chosen (the index of the first row in Fl_Hold_Browser is 1)
					// TODO: use pt_error
					std::cout << "No landmark is active" << std::endl;		
					return;
				}



//				image_scalar<unsigned short, 3> * img = dynamic_cast< image_scalar<unsigned short, 3> * > ( top );
//
//				Vector3D radius;
////				radius.Fill(50);
//				radius[0] = 50;
//				radius[1] = 0;
//				radius[2] = 0;
//				
//				// TODO: använd myGeometry->dir för att rotera radius korrekt och bestäm sedan även vilken typ som ska
//				// användas. tex MAX_GRAD_MAG_X och MAX_GRAD_MAG_Y eller MAX_GRAD_MAG_Y och MAX_GRAD_MAG_Z osv
//				rendergeometry * myGeometry = myRenderer->wheretorender;
//				std::cout << "radius " << radius << std::endl;				
//				radius[0] = abs( radius[0] );
//				radius[1] = abs( radius[1] );
//				radius[2] = abs( radius[2] );
//				std::cout << "radius after " << radius << std::endl << std::endl;
//
//				// TODO: change this later to use get_pos_of_type_in_region_world() with world coord (mouse3d) and radius in millimeters
//				// and remove the voxel_to_world()
//
//				Vector3D voxel_pos = img->get_pos_of_type_in_region_voxel( img->world_to_voxel( mouse3d ), radius, MAX_GRAD_MAG_X );
//				Vector3D world_pos = img->voxel_to_world( voxel_pos );
//		
//				// REMOVE THIS ROW!!! ONLY TEMPORARY!!
//				mouse3d = world_pos;




				points->add_pair( index_of_active, mouse3d );
				userIOmanagement.set_landmark(userIO_ID, index_of_active, mouse3d);
				viewmanagement.show_point_by_data ( mouse3d, point_collection_ID );

			}
		break;	// end of pt_event::create
		
		case pt_event::key:
			if ( event.key_combo(pt_event::pageup_key) )
			{
				event.grab();
				move_voxels( 0, 0, -1 );				
			}

			if ( event.key_combo(pt_event::pagedown_key) )
			{
				event.grab();
				move_voxels( 0, 0, 1 );
			}
			
			if ( event.handled() )
			{
				fvp->needs_rerendering();
			}

			if ( event.key_combo( pt_event::space_key ) )
			{
				event.grab();
				center_and_fit();
			}
		// NOTE: no break, update hovering also (scroll is ignored because there is no iterate event)
		
		case pt_event::scroll:
			if ( event.state() == pt_event::iterate)
			{
				event.grab();

				const int * pms = myPort->pixmap_size();
				int viewSize = std::min(pms[0],pms[1]);
				
				myRenderer->move_view(viewSize,0,0,event.scroll_delta()*wheel_factor);
				
				fvp->needs_rerendering();
				
				refresh_by_image_and_direction();
			}
		//NOTE: no break, update hovering also

		case pt_event::hover:
			event.grab();	

			Vector3D vpos = myRenderer->view_to_voxel(mouse2d[0], mouse2d[1],fvp->w(),fvp->h());

			if ( vpos[0] < 0 ) // negative coordinates signify outside of (positive and negative) bounds
			{
				userIOmanagement.interactive_message();
			}
			else
			{	// inside a non-empty viewport
				Vector3D wpos = myRenderer->view_to_world(mouse2d[0], mouse2d[1], fvp->w(), fvp->h());
				
				std::ostringstream oss;
				oss.setf ( ios::fixed );

				oss << "World " << setprecision(1) << wpos;

				userIOmanagement.interactive_message( oss.str() );
			}
		break;
		
			
	}


	
	nav_tool::handle(event);
}

void landmark_tool::register_userIO_ID(int id)
{
	userIO_ID = id;
}

//void landmark_tool::register_point_collection_ID (int id)
//{
//	point_collection_ID = id;
//}








