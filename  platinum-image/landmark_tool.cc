//AF
#include <iomanip>

#include "landmark_tool.h"
#include "renderer_base.h"	
#include "viewmanager.h"
#include "userIOmanager.h"

#include "rendermanager.h"

extern datamanager datamanagement;

extern rendermanager rendermanagement;
extern viewmanager viewmanagement;
extern userIOmanager userIOmanagement;

int landmark_tool::userIO_ID = -1;
int landmark_tool::point_collection_ID = -1;

landmark_tool::landmark_tool(viewport_event & event) : nav_tool(event) //viewporttool(event)
{
	if (event.type() == pt_event::create || event.type() == pt_event::hover)
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
	
    FLTKviewport * fvp = event.get_FLTK_viewport();
	Vector3D mouse3d = myRenderer->view_to_world(mouse2d[0], mouse2d[1], fvp->w(), fvp->h());

// ---- BEGIN REMOVE
Vector3D pos;
std::ostringstream oss;
// ---- BEGIN REMOVE

			pos = myRenderer->view_to_world(mouse2d[0], mouse2d[1], fvp->w(), fvp->h());			

			oss.setf ( ios::fixed );

			oss << "World " << setprecision(1) << pos;

			
			
			
			
			rendergeometry * g = rendermanagement.get_geometry ( myRenderer->geometry_id() );
			oss << " --- View [" << mouse2d[0] << "," << mouse2d[1] << "] --- look_at " << setprecision(1) << g->look_at;



			userIOmanagement.interactive_message( oss.str() );
			



// --- END REMOVE


// --- END REMOVE

		
	switch (event.type())
	{	
		case pt_event::create:
		
		
		
		
			if (point_collection_ID == -1)
			{
				std::cout << "point_collection_ID is not registered in landmark_tool" << std::endl;
				return;
			}
			
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
				std::cout << "No image in current viewport" << std::endl;
				return; 
			}
			
			if ( userIOmanagement.get_parameter<imageIDtype>(userIO_ID, 0) !=  top->get_id() )
			{
				std::cout << "The id of the selected image and the top image are not the same" << std::endl;
				return;
			}

			event.grab();
			
			rendermanagement.connect_data_renderer(myPort->get_renderer_id(), point_collection_ID);			
			
			if (event.state() == pt_event::begin)
			{				
				if ( datamanagement.find_data_index(point_collection_ID) == -1 )
				{	// not found
					std::cout << "The point_collection is not found in the datamanager" << std::endl;
					return;
				}
				
				point_collection * points = dynamic_cast<point_collection *>(datamanagement.get_data(point_collection_ID));
						
				int index_of_active = userIOmanagement.get_parameter<landmarksIDtype>(userIO_ID, 1);				
				points->set_active(index_of_active);
				
				if (index_of_active <= 0)	// -1 means active is not set
				{							// 0 means no line in the Fl_Hold_Browser i chosen (the index of the first row in Fl_Hold_Browser is 1)
					std::cout << "No landmark is active" << std::endl;		
					return;
				}
								
				points->add_pair(index_of_active, mouse3d);
				
				userIOmanagement.set_landmark(userIO_ID, index_of_active, mouse3d);
				

				viewmanagement.show_point(mouse3d, point_collection_ID);

				
			}
		break;	
		
		// This event is handled exactly the same as in nav_tool but the code has to be duplicated here because there is no
		// break after the pt_event::scroll in nav_tool and therefore the pt_event::hover is handled there if this event is
		// not taken care of here.
		case pt_event::scroll:
			if ( event.state() == pt_event::iterate)
			{
				const int * pms = myPort->pixmap_size();
				int viewSize = std::min(pms[0],pms[1]);

				event.grab();
				
				myRenderer->move_view(viewSize,0,0,event.scroll_delta()*wheel_factor);
				
				fvp->needs_rerendering();
			}
			//NOTE: no break, update hovering also

		case pt_event::hover:
			event.grab();	

			/*Vector3D*/ pos = myRenderer->view_to_world(mouse2d[0], mouse2d[1], fvp->w(), fvp->h());			

			//std::ostringstream oss;
			oss.setf ( ios::fixed );

			oss << "World " << setprecision(1) << pos;

			
			
			
			
			/*rendergeometry *   */g = rendermanagement.get_geometry ( myRenderer->geometry_id() );
			oss << " --- View [" << mouse2d[0] << "," << mouse2d[1] << "] --- look_at " << setprecision(1) << g->look_at;



			userIOmanagement.interactive_message( oss.str() );
			
			
		break;
	}


	
	nav_tool::handle(event);
}

void landmark_tool::register_userIO_ID(int id)
{
	userIO_ID = id;
}

void landmark_tool::register_point_collection_ID (int id)
{
	point_collection_ID = id;
}








