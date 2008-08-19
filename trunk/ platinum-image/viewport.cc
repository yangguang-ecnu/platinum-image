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

#include "histo2D_tool.h"
//#include "histogram.h"
#include "FLTKuserIOparameters.h"
#include "datamanager.h"
#include "rendermanager.h"
#include "viewmanager.h"

extern datamanager datamanagement;
extern rendermanager rendermanagement;
extern viewmanager viewmanagement;

using namespace std;

int viewport::maxviewportID = 0;
//bool viewport::renderermenu_built=false;
panefactory viewport::pfactory = panefactory();


viewport::viewport(VIEWPORT_TYPE vpt)
{
    ID = ++maxviewportID;
    VIEWPORT_TYPE vp_type = vpt;

  
    rendererID=NO_RENDERER_ID;
    rendererIndex=-1;
	needs_re_rendering = true;

    busyTool = NULL;

    rgbpixmap = NULL;
    rgbpixmap_size[0] = 0;
    rgbpixmap_size[1] = 0;

	ROI_rectangle_x = -1;
	ROI_rectangle_y = -1;
	ROI_rectangle_w = -1;
	ROI_rectangle_h = -1;

	ROI_rect_is_changing = false;
}

viewport::~viewport()
{
    if (busyTool != NULL)
        {delete busyTool;}
    
    if (rgbpixmap != NULL)
        {delete[] rgbpixmap; }
}


void viewport::initialize_viewport(int xpos, int ypos, int width, int height, VIEWPORT_TYPE vpt)
{
	vp_type = vpt;
    const int buttonheight=20;
    const int buttonwidth=70;
   
    update_viewsize(width, height - buttonheight);
    
	the_widget = new FLTKviewport(xpos,ypos,width,height,this,buttonheight,buttonwidth); //JK2 -  specify vp_type...

    //attach MPR renderer - so that all viewports can be populated for additional views
    viewmanagement.connect_renderer_to_viewport(ID,rendermanagement.create_renderer(RENDERER_MPR)); //JK2- do this in vp_class
}

int viewport::x(){
	return the_widget->x();
}
int viewport::y(){
	return the_widget->y();
}
int viewport::w(){
	return the_widget->w();
}
int viewport::h(){
	return the_widget->h();
}

int viewport::h_pane(){
	return the_widget->h_pane();
}


void viewport::clear_rgbpixmap()
{
    //blacken viewport
    for (long p=0; p < rgbpixmap_size[0]*rgbpixmap_size[1]*RGBpixmap_bytesperpixel; p +=RGBpixmap_bytesperpixel )
        {
        rgbpixmap[p] = rgbpixmap[p + 1] = rgbpixmap[p + 2] = 0;
        }
} 

Matrix3D viewport::get_renderer_direction()
{
	return rendermanagement.get_direction(rendererIndex);
}


void viewport::set_renderer_direction( const Matrix3D & dir )
{
    Matrix3D * dir_p = new Matrix3D(dir);
	
    rendermanagement.set_geometry( rendererIndex, dir_p );
    
    delete dir_p;
	
}

void viewport::set_renderer_direction( preset_direction direction ) 
{
    enum { x, y, z };
    
    Matrix3D dir;
    dir.Fill(0);
    
    /**dir[0][0]=1; //voxel direction of view x
        *dir[2][1]=1; //voxel direction of view y
    *dir[1][2]=1; //voxel direction of slicing*/
    
    //remember,
    // A sagittal plane divides the body into left and right portions.
    //The midsagittal plane is in the midline, i.e. it would pass through midline structures such as the navel or spine, and all other sagittal planes are parallel to it.
    // A coronal plane divides the body into dorsal and ventral portions.
    // A transverse plane divides the body into cranial (cephalic) and caudal portions.
    
	//enum preset_direction {Z_DIR, Y_DIR, X_DIR, Z_DIR_NEG, Y_DIR_NEG, X_DIR_NEG};

    switch ( direction )
	{
        case Z_DIR:
        //case AXIAL:
            dir[x][0]=1;	// the x direction of the viewport ("0") lies in the positive ("+1") x direction ("x") of the world coordinate system
            dir[y][1]=1;	// the y direction of the viewport ("1") lies in the positive ("+1") y direction ("y") of the world coordinete system
            dir[z][2]=1;	// the z direction of the viewport ("2") lies in the positive ("+1") z direction ("z") of the world coordinate system
            break;
            
        case Y_DIR:
//        case CORONAL:
            dir[x][0]=1;
            dir[z][1]=-1;
            dir[y][2]=1;
            break;
            
        case X_DIR:
//        case SAGITTAL:
            dir[y][0]=-1;
            dir[z][1]=-1;
            dir[x][2]=1;
            break;
            
        case Z_DIR_NEG:
//        case AXIAL_NEG:
            dir[x][0]=-1;
            dir[y][1]=1;
            dir[z][2]=-1;
            break;
            
        case Y_DIR_NEG:
//        case CORONAL_NEG:
            dir[x][0]=-1;
            dir[z][1]=-1;
            dir[y][2]=-1;
            break;
            
        case X_DIR_NEG:
//        case SAGITTAL_NEG:
            dir[y][0]=1;
            dir[z][1]=-1;
            dir[x][2]=-1;
            break;
    }
	
	set_renderer_direction( dir );

//	directionmenu_button->label( preset_direction_labels[direction] );
	the_widget->set_direction_button_label(direction);
}

int viewport::get_id () const
{
    return ID;
}

int viewport::get_renderer_id () const
{
    return rendererID;
}

void viewport::connect_renderer(int rID)
{
    rendererID = rID;               //this is the unique renderer ID within the class, NOT the vector array index!!!
    //pane_widget->needs_rerendering();
	needs_re_rendering = true;
    refresh();
}

void viewport::needs_rerendering()
{
//	cout<<"viewport::needs_rerendering()... (id="<<ID<<")"<<endl;
	needs_re_rendering = true;
}

const int * viewport::pixmap_size () const
{
    return rgbpixmap_size;
}

void viewport::update_viewsize(int des_width, int des_height)
{
    const float grow_factor=1.5;  //margin area added when re-allocating of rgbpixmap
    
    if (abs(RGBpixmap_bytesperpixel*des_width*des_height)  > sizeof (rgbpixmap))
        {
        long rgbpixmap_datasize=(long)RGBpixmap_bytesperpixel*des_width*des_height*grow_factor;
        
        if (rgbpixmap != NULL)
            {
            delete []rgbpixmap;
            }
        
        if (rgbpixmap_datasize > 0 )
            {
            rgbpixmap= new uchar[rgbpixmap_datasize];
            }
        else
            {
            rgbpixmap=NULL;
            }
        }
    
    if (des_width <=0 || des_height <=0)
        {
        rgbpixmap_size[0] = rgbpixmap_size[1] = 0;
        }
    else
        {
        rgbpixmap_size[0] = des_width;
        rgbpixmap_size[1] = des_height;
        }
    
    clear_rgbpixmap();
}

void viewport::enable_and_set_direction( preset_direction direction )
{
	set_renderer_direction( direction );
}

bool viewport::render_if_needed()
{
    if (rendererIndex>=0 && needs_re_rendering)
        {
//		cout<<"viewport::render_if_needed --> viewport_ID="<<ID<<" rendererIndex="<<rendererIndex<<endl;
        rendermanagement.render(rendererIndex, rgbpixmap, rgbpixmap_size[0], rgbpixmap_size[1]);
        needs_re_rendering = false;
        return true;
        }
    return false;
}


void viewport::refresh_after_toolswitch()
{
    if (busyTool != NULL)
        {
        delete busyTool;
        busyTool = NULL;
        
        refresh();
        }
}

void viewport::refresh_from_geometry(int g)
{
    if (rendererIndex >= 0 && rendermanagement.get_geometry_id(rendererIndex) == g)
        {
        refresh();
        }
}

void viewport::refresh_from_combination(int c)
{
    //conditional refresh, only perform if this instance uses the argument rendercombination
    
    if (rendererIndex >= 0 && rendermanagement.get_combination_id(rendererIndex) == c)
        {
        //this call means that the selection of rendered images has changed, possibly into nothing
        clear_rgbpixmap();
        refresh();
        }
}


void viewport::refresh()
{
    //called when re-render and redraw may be needed
    
    // we cache the vector ID to speed things up - now it must be recomputed
    rendererIndex = rendermanagement.find_renderer_index(rendererID);
    
    if (the_widget != NULL)
        {
        if (rendererIndex == -1)
            {
            //our renderer has been deleted
            rendererID=NO_RENDERER_ID;
            clear_rgbpixmap();
            //viewport_buttons->deactivate();
            }
        else
            {
            histo2D_tool * utool = dynamic_cast<histo2D_tool * > (busyTool);
            if (utool != NULL)
                {
				utool->attach(this,rendermanagement.get_renderer(rendererID));
				}
            
			the_widget->refresh_menus();
            needs_re_rendering = true;

			the_widget->pane_widget->damage(FL_DAMAGE_ALL); //JK2-check if needed...
            }
        
        //pane_widget->damage(FL_DAMAGE_ALL);
		//the_widget->damage(FL_DAMAGE_ALL); 
//		if(the_widget->pane_widget !=NULL){
//			the_widget->pane_widget->damage(FL_DAMAGE_ALL); 
//		}
    }
}

void viewport::refresh_overlay()
{
	the_widget->refresh_overlay(); //JK2 -TODO - make work...//jk2 pass this on..
}

void viewport::paint_overlay()
{
//	rendermanagement.get_renderer(this->rendererID)->paint_overlay(w(), h());
//	cout<<"***h()="<<h_pane()<<endl;

	//here the height of the pane needs to be used since the height of the "viewport" includes the buttons...
	rendermanagement.get_renderer(this->rendererID)->paint_overlay(w(), h_pane()); //h_pane is needed to compensate for button height... 

	fl_color(FL_GRAY);
	fl_rect(0, 0, w(), h()); //SO - framing the viewports 888
	

	if(ROI_rectangle_x >-1 ){
		fl_color(FL_RED);
		//fl_line_style(FL_DOT, 2, 0); //fl_line_style(int style, int width=0, char* dashes=0)
		fl_rect(ROI_rectangle_x,ROI_rectangle_y,ROI_rectangle_w,ROI_rectangle_h);
		//fl_line_style(0); //reset line style
	
	

	}
}


threshold_overlay * viewport::get_threshold_overlay (thresholdparvalue * threshold_par)
{    
    histo2D_tool * utool = NULL;
    
    //2D histogram should only allow this call when the uim tool is selected
    if (busyTool == NULL)
        {
//        viewport_event e = viewport_event(0,pane_widget);															//JK-ööö
//        busyTool = utool = new histo2D_tool (e,threshold_par,this, rendermanagement.get_renderer(rendererID));		//JK-ööö
		viewport_event e = viewport_event(0, (FLTK_Pt_pane*)the_widget->pane_widget);															//JK-ööö
        busyTool = utool = new histo2D_tool (e,threshold_par,this, rendermanagement.get_renderer(rendererID));		//JK-ööö
        }
    
    if (busyTool != NULL) //might have been created earlier too
        {        
        utool = dynamic_cast<histo2D_tool *>(busyTool);
        
        if (utool != NULL)
            {
            return utool->get_overlay();
            }
        }
    
    return NULL;
}


void viewport::set_look_at_and_zoom(Vector3D pos, float zoom)
{
	rendermanagement.get_renderer(rendererID)->look_at(pos[0],pos[1],pos[2],zoom);
}
