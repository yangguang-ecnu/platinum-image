//AF

#include "landmark_tool.h"
#include "renderer_base.h"	
#include "viewmanager.h"
#include "userIOmanager.h"

#include "rendermanager.h"

extern datamanager datamanagement;

extern rendermanager rendermanagement;
//extern viewmanager viewmanagement;
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

std::string landmark_tool::vector3d_to_string(Vector3D v)
{

	// returns the largest integer not greater than v[i]
	for (int i=0; i<3; i++)
	{
		v[i] = floor(v[i]);
	}

	std::ostringstream v_str;
	v_str << v;
	
	return v_str.str();
}


void landmark_tool::handle(viewport_event &event)
{
	std::vector<int> mouse = event.mouse_pos_local();
	
    FLTKviewport * fvp = event.get_FLTK_viewport();
	Vector3D mouse3d = myRenderer->view_to_world(mouse[0], mouse[1], fvp->w(), fvp->h());
		
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
				
				if (index_of_active == 0)	// 0 means that no line in the Fl_Hold_Browser i chosen (the index of the first row in Fl_Hold_Browser is 1)
				{
					std::cout << "No landmark is active" << std::endl;
					return;
				}
								
//					Vector3D & v = points->get_point_at(index_of_active - 1);	// the first index of the Fl_Hold_Browser is 1
//					v = mouse3d;												// update the data in datamanagement
				//points->set_point(index_of_active - 1, mouse3d);	// the first index of the Fl_Hold_Browser is 1

				points->add_pair(index_of_active, mouse3d);	// the first index of the Fl_Hold_Browser is 1
				
				userIOmanagement.set_landmark(userIO_ID, index_of_active, mouse3d);

			}
		break;	
		
		case pt_event::hover:
			event.grab();	
			//Vector3D pos = myRenderer->view_to_voxel(mouse[0], mouse[1], fvp->w(), fvp->h());			
			Vector3D pos = myRenderer->view_to_world(mouse[0], mouse[1], fvp->w(), fvp->h());			
//			if (pos[0]<0)	// negative coordinates signify outside of bounds
//			{
//				userIOmanagement.interactive_message();
//			}
//			else
//			{			
				//userIOmanagement.interactive_message("Voxel " + vector3d_to_string(pos));
				// OBS!! FIXA SNYGGARE UTSKRIFT AV KOORDINATER!! mha printf EJ cout
				
				std::ostringstream oss;
				oss << pos;

				userIOmanagement.interactive_message("World " + oss.str());
				
				
				
//			}
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








