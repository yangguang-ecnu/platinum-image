// $Id$

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

//#include "viewporttool.h"

#include "viewmanager.h"
#include "datamanager.h"
#include "rendermanager.h"
#include "userIOmanager.h"
#include "landmark_tool.h"


extern viewmanager viewmanagement;
extern rendermanager rendermanagement;
extern datamanager datamanagement;
extern userIOmanager userIOmanagement;

#include "statusarea.h"

#include <FL/Fl_Window.h>
#include <FL/Fl_Button.h>

#pragma mark *** base class ***

// static members
Fl_Pack * viewporttool::toolbox = NULL;
statusarea * viewporttool::statusArea = NULL;
std::string viewporttool::selected = "";
std::map<std::string, viewporttool::vpt_create_pointer> viewporttool::tools = std::map<std::string, viewporttool::vpt_create_pointer>  ();

template <class TOOL>
void viewporttool::Register ()
{
    tools[TOOL::name()]=&CreateObject<TOOL>;
    TOOL::init();
}

template <class TOOL>
viewporttool * viewporttool::CreateObject(viewport_event &event)
{
    return new TOOL (event);
}

void viewporttool::init (int posX, int posY,statusarea * s)
{
    statusArea = s;
    
    //register tool classes
    
    viewporttool::Register<nav_tool>();
    //viewporttool::Register<dummy_tool>();
    viewporttool::Register<cursor_tool>();
    viewporttool::Register<histo2D_tool>();
	viewporttool::Register<landmark_tool>();
    
    selected = "Navigation";
    
    //create toolbox widget
    const bool horizontal = true;

    toolbox = new Fl_Pack (posX,posY,1,statusArea->h());
    
    if (horizontal)
        { toolbox->type(FL_HORIZONTAL);}
    else
        {toolbox->type(FL_VERTICAL);};
        
    int buttonSize  = horizontal? toolbox->h():toolbox->w();
    int x = toolbox->x();
    int y = toolbox->y();
    
    toolbox->begin();
    
    for (std::map<std::string, vpt_create_pointer>::iterator i = tools.begin();i != tools.end();i++)
        {
        std::string name = i->first;
        Fl_Button * button = new Fl_Button (x,y,buttonSize,buttonSize);
        button->label(strdup(name.substr(0,1).c_str()));
        button->tooltip(strdup(name.c_str()));
        
        button->box(FL_UP_BOX);
        button->down_box(FL_DOWN_BOX);
        button->type(FL_RADIO_BUTTON);
        button->callback(cb_toolbutton,(void*)&(i->first));

        if (name == selected)
            { button->set(); }
        
        if (horizontal)
            { 
            x += buttonSize; 
            }
        else
            { 
            y += buttonSize;
            }
        }
    
    if (horizontal)
        { 
        y = buttonSize;
        }
    else
        { 
        x = buttonSize;
        }
    
    toolbox->resize(posX,posY,x,y);
    toolbox->end();
    }

viewporttool::viewporttool(viewport_event &)
{}

viewporttool::~viewporttool()
{}


/*void viewporttool::Register (std::string k,vpt_ctor_pointer f)
{
    tools[k]=f;
}*/

void viewporttool::select (const std::string key)
{
    selected = key;

    int nc = toolbox->children();
    for (int c= 0; c < nc;c++)
        {
        Fl_Button * b = dynamic_cast< Fl_Button * >(toolbox->child(c));

        if (*(reinterpret_cast<const std::string *>(b->user_data())) == key)
            {
            b->setonly();
            }
        }  
}

void viewporttool::select_only( const std::string key )
{
    selected = key;

	int nc = toolbox->children();
	for ( int c = 0; c < nc; c++ )
	{
        Fl_Button * b = dynamic_cast< Fl_Button * >(toolbox->child(c));

		if ( *(reinterpret_cast<const std::string *>(b->user_data())) == key )
		{
            b->setonly();
		}
		else
		{
			b->deactivate();
		}
	}  
}

viewporttool * viewporttool::taste(viewport_event & event,viewport * vp,renderer_base * r)
{
    viewporttool * result = tools[selected](event);
    
    if (!event.handled())
        {
        //the selected tool class did not want the event
        delete result;
        result = NULL;
        }
    else
        {
        result->myPort = vp;
        result->myRenderer = r;
        }
    
	return result;
}

void viewporttool::cb_toolbutton (Fl_Widget * button,void * key_ptr)
{
    std::string * key = reinterpret_cast<std::string *>(key_ptr);
    
    userIOmanagement.select_tool (*key);
}

// *** tool classes ***
//registered in viewporttool::init

#pragma mark *** navigation tool ***

const float nav_tool::wheel_factor=renderer_base::display_scale/10;
const float nav_tool::zoom_factor=0.005;

nav_tool::nav_tool (viewport_event & event):viewporttool(event)
    {    
    //a tool constructor has to respond to the type of events it accepts by grabbing them,
    //or it won't be created
    if (event.type() == pt_event::hover || event.type() == pt_event::browse || event.type() == pt_event::adjust || event.type() == pt_event::scroll || event.type() == pt_event::key || event.type() == pt_event::focus_viewports)
        {event.grab();}

	zoom_start_pos = std::vector<int>(2);
    }

const std::string nav_tool::name()
{
    return "Navigation";
}

void nav_tool::init()
{}

void nav_tool::handle(viewport_event &event)
{

	FLTKviewport * fvp = event.get_FLTK_viewport();

    if ( event.state() == pt_event::begin )
	{
        cout<<"nav_tool.. begin..."<<endl;
//		cout<<"(pt_event::begin)"<<endl;
        event.grab();
		
        dragLast[0] = event.mouse_pos_global()[0]; //used in various functions
        dragLast[1] = event.mouse_pos_global()[1];

		//Used in zoom function
		//Note: it is important to use local mouse coordinates when using world<-->view transformations...
		global_zoom_start_pos = myRenderer->view_to_world(event.mouse_pos_local()[0], event.mouse_pos_local()[1], fvp->w(), fvp->h());
		zoom_start_pos[0] = event.mouse_pos_local()[0];
		zoom_start_pos[1] = event.mouse_pos_local()[1];
    }

	if ( event.type() == pt_event::focus_viewports )
	{	// double click
		if ( rendermanagement.get_combination(myRenderer->combination_id())->top_image() != NULL )
		{	// there is an image in current viewport
		
			// The coordinate of the mouse pointer in the current viewport is shown in the other viewports
			// (if there is at least one image in the viewport)
			// TODO: implement a drop-down menu for each viewport where the user can set which viewports it should connect with.

			std::vector<int> mouse2d = event.mouse_pos_local();
			Vector3D mouse3d = myRenderer->view_to_world(mouse2d[0], mouse2d[1], fvp->w(), fvp->h());
			viewmanagement.show_point_by_combination ( mouse3d, myRenderer->combination_id() ); //jk-ööö
		}
	}
	
	if ( event.type() == pt_event::focus_viewports_center )
	{	// double click and shift
		std::cout << "Focus and center" << std::endl;
		std::vector<int> mouse2d = event.mouse_pos_local();
		Vector3D mouse3d = myRenderer->view_to_world(mouse2d[0], mouse2d[1], fvp->w(), fvp->h());
		viewmanagement.show_point_by_combination(mouse3d, myRenderer->combination_id(), -1);		
	}

    if (!event.handled())
	{
		
        const int * pms = myPort->pixmap_size();
        int viewSize = std::min(pms[0],pms[1]);
        //const float pan_factor=renderer_base::display_scale/(std::min(pms[0],pms[1]));
        const int * mouse = event.mouse_pos_global();
                
        switch (event.type())
		{
			
            case pt_event::browse:	// pan
                if ( event.state() == pt_event::iterate)
				{
                    event.grab();
                    
                    myRenderer->move_view(viewSize,-(mouse[0]-dragLast[0]),-(mouse[1]-dragLast[1]));
                    
                    fvp->needs_rerendering();
				}
			break;
                
            case pt_event::adjust:	// zoom
                if ( event.state() == pt_event::iterate)
				{
                    cout<<"nav_tool_zoom..."<<endl;
                    event.grab();
//                    cout<<"("<<mouse[1]<<","<<dragLast[1]<<") "<<1+(mouse[1]-dragLast[1])*zoom_factor<<endl;

					float z = abs(float(1.0+(mouse[1]-dragLast[1])*zoom_factor)); //the absolute value is needed since negative values inverts the image...
					
					//zoom...
					myRenderer->move_view(viewSize,0,0,0,z); 

					//move....
					std::vector<int> new_pos = myRenderer->world_to_view(fvp->w(),fvp->h(),global_zoom_start_pos);
//					int dx = float(myPort->pixmap_size()[0]/2-zoom_start_pos[0])*(1.0-z);
//					int dy = float(myPort->pixmap_size()[1]/2-zoom_start_pos[1])*(1.0-z);
					int dx = new_pos[0]-zoom_start_pos[0];
					int dy = new_pos[1]-zoom_start_pos[1];
//                    cout<<"new_pos[0]="<<new_pos[0]<<endl;
  //                  cout<<"new_pos[1]="<<new_pos[1]<<endl;
    //                cout<<"dx="<<dx<<endl;
      //              cout<<"dy="<<dy<<endl;
					myRenderer->move_view(viewSize,dx,dy);
                    
                    fvp->needs_rerendering();
				}					
			break;
                
			case pt_event::rotate:
				if ( event.state() == pt_event::iterate )
				{
					//TODO... correct slice pos illustrations
					event.grab();
					cout<<"***pt_event::rotate - temporary work (ctrl + shift + mouse drag)***"<<endl; 
					float dx = (mouse[0]-dragLast[0]);
					float dy = (mouse[1]-dragLast[1]);
//					cout<<"dx="<<dx<<endl;
//					cout<<"dy="<<dy<<endl;
					Matrix3D dir = myRenderer->wheretorender->dir;
//					cout<<"dir="<<endl<<dir<<endl;
//					Matrix3D m = mg.get_rot_matrix_3D( 0.0, float(-dx*PI/180.0), float(dy*PI/180.0));
					Matrix3D m = create_rot_matrix_3D(dy*PI/180.0, -dx*PI/180.0, 0.0);
//					cout<<"m="<<endl<<m<<endl;
					dir = dir*m;
//					cout<<"dir2="<<endl<<dir<<endl;

					myRenderer->wheretorender->dir = dir;

					fvp->needs_rerendering();
//					refresh_by_image_and_direction(); //redraws slice locators in other viewports //TODO: use fl_overlays
//					this->refresh_overlay_by_image_and_direction();
					viewmanagement.update_overlays();


				/*	
					image_base * top;
					if ( top = rendermanagement.get_combination(myRenderer->combination_id())->top_image() )
					{	// there is an image in current viewport
					
						Matrix3D m = datamanagement.get_image(top->get_id())->get_orientation();

						Vector3D angle;
//						angle[0] = -(mouse[0]-dragLast[0]);
//						angle[1] = -(mouse[1]-dragLast[1]);
//						angle[2] = 0.0;
						angle[0] = 0.0;
						angle[1] = 0.0;
						angle[2] = (mouse[0]-dragLast[0]);
						
						// convert degrees to radians
						angle *= ( PI / 180.0 );	
						//matrix_generator mg; //depricated use create_rot... instead...
						//m = mg.get_rot_matrix_3D ( angle[2], angle[1], angle[0] ) * m;
			
						datamanagement.get_image(top->get_id())->set_orientation(m); //TODO: use rotate_orientation instead...
						datamanagement.data_has_changed(top->get_id());
						
						// skriv en metod som roterarar en bild (volym) kring dess centrum (ändrar orientation och origin.
						// origin måste ändras för att rotationen ska bli "korrekt")
						// och en metod som roterar bild kring dess origin (ändrar endast orientation)
						// rendermanagement.center3d_and_fit ( top->get_id() );
					}
*/
				}
			break;	// end of pt_event::rotate
				
            case pt_event::scroll:	// pan in z-direction
                if ( event.state() == pt_event::iterate)
				{
                    event.grab();
                    
                    myRenderer->move_view(viewSize,0,0,event.scroll_delta()*wheel_factor);
                    
                    fvp->needs_rerendering();

//					refresh_by_image_and_direction(); //redraws slice locators in other viewports //TODO: use fl_overlays
//					this->refresh_overlay_by_image_and_direction();
					viewmanagement.update_overlays();

				}
			//NOTE: no break, update hovering also
				
            case pt_event::hover:
                //display values
                switch (event.state() )
				{
                    case pt_event::begin:
                    case pt_event::iterate:
					{
						event.grab();
						
						std::vector<int> lmouse = event.mouse_pos_local();
						
						numbers.str("");
						
						//get values and update statusfield
						
						const std::map<std::string, float> values = myRenderer->get_values_view( lmouse[0], lmouse[1], fvp->w(), fvp->h() );

						if (values.empty())
						{
							userIOmanagement.interactive_message();
						}
						else
						{
							for (std::map<std::string,float>::const_iterator itr = values.begin(); itr != values.end();itr++)
							{
								if (itr != values.begin())
									{ numbers << "; "; }
								else
									{ numbers << "Value(s) "; }
								numbers << itr->first << ": " << itr->second;
							}
							userIOmanagement.interactive_message(numbers.str());
						}
					}
					break;
					
                    case pt_event::end:	// leaving a viewport
                        event.grab();
                        userIOmanagement.interactive_message();
					break;
                        
				}
			break;

            case pt_event::key:				
                if (event.key_combo(pt_event::pageup_key))
				{
                    event.grab();
					move_voxels( 0, 0, -1 );
				}
				
                if (event.key_combo(pt_event::pagedown_key))
				{
                    event.grab();
					move_voxels( 0, 0, 1 );
				}
                
                if (event.handled())
				{
                    fvp->needs_rerendering();
				}
				
				if ( event.key_combo( pt_event::space_key + pt_event::shift_key ) )
				{
					event.grab();
					center3d_and_fit();
				}
				
				if ( event.key_combo( pt_event::space_key ) )
				{			
					event.grab();
					center2d();
				}

			break;
		}
        
        if (event.state() == pt_event::end)
		{

		}
        
        dragLast[0] = mouse[0];
        dragLast[1] = mouse[1];
	}
}

void nav_tool::refresh_by_image_and_direction()
{
	// get geometries that holds at least one of the images in the input combination and have a different
	// direction than the input geometry (i.e. not the same nor the opposite direction)					
	std::vector<int> geometryIDs = rendermanagement.geometryIDs_by_image_and_direction( myRenderer->combination_id() );

	for ( std::vector<int>::const_iterator itr = geometryIDs.begin(); itr != geometryIDs.end(); itr++ )
	{
		viewmanagement.refresh_viewports_from_geometry( *itr ) ;
	}
}

/*
void nav_tool::refresh_overlay_by_image_and_direction()
{
	cout<<"refresh_overlay_by_image_and_direction..."<<endl;
	std::vector<int> geometryIDs = rendermanagement.geometryIDs_by_image_and_direction( myRenderer->combination_id() );
	cout<<"geometryIDs.size()="<<geometryIDs.size()<<endl;

	for ( std::vector<int>::const_iterator itr = geometryIDs.begin(); itr != geometryIDs.end(); itr++ )
	{
		cout<<"*itr="<<*itr<<endl;
		viewmanagement.refresh_overlays_from_geometry( *itr ) ;
	}
}
*/

void nav_tool::move_voxels( int x, int y, int z )
{
	if ( rendermanagement.renderer_empty(myRenderer->get_id()) == RENDERER_NOT_EMPTY )
	{ 
		myRenderer->move_voxels( x, y, z);
		refresh_by_image_and_direction(); //TODO - use fl_overlays for speedup
	
		//JK ööö window()->make_current() and then fl_overlay_rect()
//		fl_overlay_rect(10,20,30,60); //overlay_window is needed.
	}
}

void nav_tool::center3d_and_fit()
{
	image_base * top;
	if ( top = rendermanagement.get_combination(myRenderer->combination_id())->top_image() )
	{	// there is an image in current viewport			
		rendermanagement.center3d_and_fit(myRenderer->get_id(), top->get_id());
		refresh_by_image_and_direction();
	}
}

void nav_tool::center2d()
{
	image_base * top;
	if ( top = rendermanagement.get_combination(myRenderer->combination_id())->top_image() )
	{	// there is an image in current viewport			
		rendermanagement.center2d(myRenderer->get_id(), top->get_id());
		refresh_by_image_and_direction();
	}
}

#pragma mark *** dummy tool ***

dummy_tool::dummy_tool (viewport_event &event):viewporttool(event)
    {

    }

const std::string dummy_tool::name()
{
    return "Dummy";
}

void dummy_tool::handle(viewport_event &event)
{}

void dummy_tool::init()
{}

threshold_overlay * histo2D_tool::get_overlay ()
{
    return overlay;
}

#pragma mark *** cursor_tool ***

Fl_Output * cursor_tool::coord_display = NULL;
Fl_Button * cursor_tool::make_button = NULL;
point * cursor_tool::selection = NULL;
cursor_tool * cursor_tool::selectionOwner = NULL;

cursor_tool::cursor_tool(viewport_event &event):nav_tool(event)
{
    if (event.type() == pt_event::create || event.type() == pt_event::hover)
        {
        event.grab();
        }
}

cursor_tool::~cursor_tool()
{
    if (selection != NULL && selectionOwner == this)
        { 
        selectionOwner = NULL;
        
        delete selection;
        selection = NULL;
        }
}
Vector3D cursor_tool::get_global_selection_coords()
{
	if(selection==NULL){
		cout<<"selection==NULL"<<endl;
	}
	Vector3D coord = selection->get_point(0);
	cout<<"cursor_tool::get_global_selection_coords()="<<coord<<endl;
	return coord;
}

const std::string cursor_tool::name()
{
    return "Cursor";
}

void cursor_tool::cb_make(Fl_Widget*, void* p)
{    
    if (selection != NULL)
        {
        int rendererID = selectionOwner->myPort->get_renderer_id();
        datamanagement.add(selection);

        rendermanagement.connect_data_renderer(rendererID,selection->get_id());
        }
    
    selection = NULL;
    
    selectionOwner = NULL;
}

void cursor_tool::init()
{
    const int controls_w = 90;
    
    Fl_Group * controls = userIOmanagement.status_area->add_pane<Fl_Group>(name());
    controls->size(controls_w+5,controls->h());
    //controls->type(FL_HORIZONTAL);
    //coord_display = new Fl_Output (controls->x(),controls->y(),60,controls->h(),"Location");
    make_button = new Fl_Button (controls->x(),controls->y(),controls_w,controls->h(),"Make point");
    make_button->callback(cb_make,(void*)NULL);
    controls->box(FL_FLAT_BOX);
    controls->color(FL_RED);
    
    controls->end();
}

void cursor_tool::handle(viewport_event &event)
{
    std::vector<int> mouse = event.mouse_pos_local();
    
    FLTKviewport * fvp = event.get_FLTK_viewport();

    if (event.type() == pt_event::create)
        {
        event.grab();
        Vector3D mouse3D = myRenderer->view_to_world(mouse[0],mouse[1],fvp->w(),fvp->h());
        
        switch (event.state())
            {
            case pt_event::begin:
                if (selection == NULL)
                    {
                    selection = new point(mouse3D);
                    selectionOwner = this;
                    }
                else
                    {
                    viewmanagement.refresh_viewports();
                    }
            case pt_event::iterate:
                selectionOwner = this;
                make_button->user_data(this);

                selection->set_origin (mouse3D);
                
                fvp->damage(FL_DAMAGE_ALL);
                
                break;
            }
        }
    if (event.type() == pt_event::draw)
        {
        if (selection != NULL && selectionOwner == this)
            {
            event.grab();
            
            const int chsize = 6;
            const int chmarg = chsize + 2;
            const int chlen = 8;
            std::vector<int> drawC = myRenderer->world_to_view (fvp->w(),fvp->h(),selection->get_origin());

            drawC[0] += fvp->x();
            drawC[1] += fvp->y();
                   
            fl_push_clip(fvp->x(),fvp->y(),fvp->w(),fvp->h());
            
            fl_color(FL_GRAY);
            fl_line(drawC[0], drawC[1]-chmarg, drawC[0], drawC[1]-chmarg-chlen); //above 
            fl_line(drawC[0]+chmarg,drawC[1],drawC[0]+chmarg+chlen,drawC[1]); //right 
            fl_line(drawC[0], drawC[1]+chmarg, drawC[0], drawC[1]+chmarg+chlen); //below 
            fl_line(drawC[0]-chmarg,drawC[1],drawC[0]-chmarg-chlen,drawC[1]); //left
            
            fl_color(FL_WHITE);
            fl_circle(drawC[0],drawC[1],chsize/2);
            
            fl_pop_clip();
            }
        }    
    
    if (event.type() == pt_event::resize)
        {
        event.grab();
    
        //event.resize_point(selection[0],selection[1]);
        }
    
    if (event.type() == pt_event::hover ) //|| event.type() == pt_event::scroll)
        {
        event.grab();

        switch (event.state())
            {
            case pt_event::begin:
                //get pointer to renderer
                //renderer = rendermanagement.get_renderer( myPort->get_renderer_id());
            case pt_event::iterate:
                {
                    numbers.str("");
                    //get coords and update statusfield
                    Vector3D pos;
                    pos = myRenderer->view_to_voxel(mouse[0], mouse[1],fvp->w(),fvp->h());
                    if (pos[0]<0) //negative coordinates signify outside of
                                  //(positive and negative) bounds
                        {
                        userIOmanagement.interactive_message();
                        }
                    else
                        {
                        for (int d = 0;d < 3;d++)
                            {pos[d] = floor(pos[d]);}
                        numbers << "Voxel " << pos;
                        userIOmanagement.interactive_message(numbers.str());
                        }
                }
                break;
            case pt_event::end:
                //renderer = NULL;
                userIOmanagement.interactive_message();
                break;
            }
        }
    
    nav_tool::handle(event);
}
