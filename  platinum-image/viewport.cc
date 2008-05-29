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


viewport::viewport()
{
    ID = ++maxviewportID;
  
    rendererID=NO_RENDERER_ID;
    rendererIndex=-1;
	needs_re_rendering=true;

    busyTool = NULL;

    rgbpixmap = NULL;
    rgbpixmap_size[0] = 0;
    rgbpixmap_size[1] = 0;
}

viewport::~viewport()
{
    if (busyTool != NULL)
        {delete busyTool;}
    
    if (rgbpixmap != NULL)
        {delete[] rgbpixmap; }
}


void viewport::initialize_viewport(int xpos, int ypos, int width, int height)
{
    const int buttonheight=20; //JK2
//    const int buttonheight=0; //JK2
    const int buttonwidth=70;
   
    update_viewsize(width, height - buttonheight);
    
	the_widget = new FLTKviewport(xpos,ypos,width,height,this,buttonheight,buttonwidth); //JK2

    //attach MPR renderer - so that all viewports can be populated for additional views
    viewmanagement.connect_renderer_to_viewport(ID,rendermanagement.create_renderer(RENDERER_MPR)); //JK2
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
    //viewport_widget->needs_rerendering();
	needs_re_rendering = true;
    refresh();
}

void viewport::needs_rerendering()
{
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
	set_renderer_direction( direction ); //JK2 - changed in connection major make_over
//	the_widget->set_direction_button_label(direction); 

//  if ( directionmenu_button != NULL )
//	{
//      Fl_Menu_Item * directionmenu = (Fl_Menu_Item *) directionmenu_button->menu();
//		directionmenu[direction].setonly();
//		set_renderer_direction( direction );
//	}
}

bool viewport::render_if_needed (FLTK_draw_viewport * f)
{
//    if (rendererIndex>=0 && f->needsReRendering)
    if (rendererIndex>=0 && needs_re_rendering)
        {
        rendermanagement.render(rendererIndex, rgbpixmap, rgbpixmap_size[0], rgbpixmap_size[1]);
        
//        f->needsReRendering = false;
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
            needs_re_rendering=true;

			the_widget->viewport_widget->damage(FL_DAMAGE_ALL); //JK2-check if needed...
            }
        
        //viewport_widget->damage(FL_DAMAGE_ALL);
		//the_widget->damage(FL_DAMAGE_ALL); //JK2-check if needed...
//		if(the_widget->viewport_widget !=NULL){
//			the_widget->viewport_widget->damage(FL_DAMAGE_ALL); //JK2-check if needed...
//		}
    }
}

void viewport::refresh_overlay()
{
	the_widget->refresh_overlay(); //JK2 -TODO - make work...//jk2 pass this on..
}

void viewport::paint_overlay()
{
	rendermanagement.get_renderer(this->rendererID)->paint_overlay(0, 0, w(), h());
}


threshold_overlay * viewport::get_threshold_overlay (thresholdparvalue * threshold_par)
{    
    histo2D_tool * utool = NULL;
    
    //2D histogram should only allow this call when the uim tool is selected
    if (busyTool == NULL)
        {
//        viewport_event e = viewport_event(0,viewport_widget);															//JK-ööö
//        busyTool = utool = new histo2D_tool (e,threshold_par,this, rendermanagement.get_renderer(rendererID));		//JK-ööö
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
