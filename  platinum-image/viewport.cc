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

#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Tile.H>
#include <FL/Fl_Box.H>
#include <FL/Enumerations.H>

#include "histo2D_tool.h"
#include "histogram.h"
#include "FLTKuserIOparameters.h"
#include "viewmanager.h"
#include "rendermanager.h"
#include "datamanager.h"

extern datamanager datamanagement;
extern rendermanager rendermanagement;
extern viewmanager viewmanagement;

using namespace std;

int viewport::maxviewportID = 0;
bool viewport::renderermenu_built=false;

//static Fl_Menu_Item renderermenu_global[NUM_RENDERER_TYPES_PLUS_END];   //this one should be protected too

enum preset_direction {AXIAL, CORONAL, SAGITTAL,AXIAL_NEG, CORONAL_NEG, SAGITTAL_NEG};

const char * preset_direction_labels[] =
{
    "Axial",
    "Coronal",
    "Sagittal",
    "-Axial",
    "-Coronal",
    "-Sagittal"
};

const char * blend_mode_labels[] =
{
    "Overwrite",
    "Max",
    "Min",
    "Average",
    "Tint"
};

struct menu_callback_params
{
    //for blend & image callbacks
    int rend_index;
    
    //for blend callback
    blendmode mode;
    
    //for image callback
    int vol_id;
    
    //for preset direction callback    
    viewport * vport;
    preset_direction direction;
};

void viewport::clear_rgbpixmap()
{
    //blacken viewport
    for (long p=0; p < pixMapSize[0]*pixMapSize[1]*RGBpixmap_bytesperpixel; p +=RGBpixmap_bytesperpixel )
        {
        rgbpixmap[p] = rgbpixmap[p + 1] = rgbpixmap[p + 2] = 0;
        }
} 

viewport::viewport()
{
    ID = ++maxviewportID;
    
    viewport_widget=NULL;
    imagemenu_button=NULL;
    renderermenu_button=NULL;
    viewport_buttons=NULL;
    busyTool = NULL;
    
    rendererID=NO_RENDERER_ID;
    rendererIndex=-1;
    
    //rgbpixmap = new uchar [0];
    rgbpixmap = NULL;
    pixMapSize[0] = 0;
    pixMapSize[1] = 0;
}

viewport::~viewport()
{
    if (busyTool != NULL)
        {delete busyTool;}
    
    if (rgbpixmap != NULL)
        {delete[] rgbpixmap; }
    
    //if (renderermenu_button != NULL)
    //    { fl_menu_userdata_delete (renderermenu_button->menu()); }
}

void viewport::connect_renderer(int rID)
{
    rendererID = rID;               //this is the unique renderer ID within the class, NOT the vector array index!!!
    
    viewport_widget->needs_rerendering();
    refresh();
}

void viewport::refresh_from_combination(int c)
{
    //conditional refresh, only perform if this instance uses
    //the argument rendercombination
    
    if (rendererIndex >= 0 && rendermanagement.get_combination_id(rendererIndex) == c)
        {
        //this call means that the selection of rendered images has changed,
        //possibly into nothing
        
        clear_rgbpixmap();
        
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

void viewport::refresh_after_toolswitch()
{
    if (busyTool != NULL)
        {
        delete busyTool;
        busyTool = NULL;
        
        refresh();
        }
}

void viewport::refresh()
{
    //called when re-render and redraw may be needed
    
    // we cache the vector ID to speed things up - now it must be recomputed
    rendererIndex = rendermanagement.find_renderer_index(rendererID);
    
    if (viewport_widget != NULL)
        {
        if (rendererIndex == -1)
            {
            //our renderer has been deleted
            rendererID=NO_RENDERER_ID;
            clear_rgbpixmap();
            viewport_buttons->deactivate();
            }
        else
            {
            histo2D_tool * utool = dynamic_cast<histo2D_tool * > (busyTool);
            if (utool != NULL)
                {utool->attach (this,rendermanagement.get_renderer(rendererID));}
            
            update_objects_menu();
            rebuild_renderer_menu();
            rebuild_blendmode_menu();
            
            viewport_widget->needsReRendering=true;
            viewport_buttons->activate();
            }
        
        viewport_widget->damage(FL_DAMAGE_ALL);
        }
}

void viewport::update_fbstring (FLTKviewport* f)
{
    std::map<std::string,float> values;
    ostringstream infostring;
    
    //Draw values picked from data at mouse position
    //in later revisions this event and clicks may be relayed to the actual application
    
    values=rendermanagement.get_values(rendererIndex,f->mouse_pos[0]-f->x(),f->mouse_pos[1]-f->y(),pixMapSize[0],pixMapSize[1]);
    
    for (std::map<std::string,float>::iterator itr = values.begin(); itr != values.end();itr++)
        {
        if (itr != values.begin()) 
            {
            infostring << endl;
            }
        infostring << itr->first << ": " << itr->second;
        }
    
    f->feedback_string=infostring.str();
}

threshold_overlay * viewport::get_threshold_overlay (thresholdparvalue * threshold_par)
{    
    histo2D_tool * utool = NULL;
    
    //2D histogram should only allow this call when the uim tool is selected
    if (busyTool == NULL)
        {
        viewport_event e = viewport_event(0,viewport_widget);
        busyTool = utool = new histo2D_tool (e,threshold_par,this, rendermanagement.get_renderer(rendererID));
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

//// Callback wrapper
//
void viewport::viewport_callback(Fl_Widget *callingwidget, void *thisviewport)
{
    ((viewport*)thisviewport)->viewport_callback(callingwidget);
}

bool viewport::render_if_needed (FLTKviewport * f)
{
    if (rendererIndex>=0 && f->needsReRendering)
        {
        rendermanagement.render(rendererIndex, rgbpixmap, pixMapSize[0], pixMapSize[1]);
        
        f->needsReRendering = false;
        
        return true;
        }
    return false;
}

void viewport::viewport_callback(Fl_Widget *callingwidget){
    FLTKviewport* f = (FLTKviewport*)callingwidget;
    //f points to the same GUI toolkit-dependent widget instance as viewport_widget

    if (f->callback_event.type() == pt_event::draw)
        {
        f->callback_event.grab();
        render_if_needed(f);
        f->damage(FL_DAMAGE_ALL);
        f->draw(rgbpixmap);
        f->damage(0);
        }
    if (busyTool == NULL)
        { 
        busyTool = viewporttool::taste(f->callback_event,this,rendermanagement.get_renderer(rendererID));
        }
    
    if (busyTool != NULL) //might have been created earlier too
        {
        busyTool->handle(f->callback_event);
        if (render_if_needed(f))
            { f->redraw(); }
        }
    
    //handle events "otherwise"
    if (!f->callback_event.handled())
        {
        //"default" behavior for the viewport goes here
        
        //call render_if_needed(); if image may need re-rendering
        }
    
    //handle events regardless of whether a tool caught them
    switch (f->callback_event.type())
        {
        case pt_event::resize:
            if ((f->resize_w != pixMapSize[0] || f->resize_h != pixMapSize[1]))
                {
                //resize: just update view size, re-render but don't redraw...yet
                
                const int * r = f->callback_event.get_resize();
                update_viewsize(r[0] ,r[1]);
                
                f->needs_rerendering();
                }
            break;
        }
}

int viewport::get_id () const
{
    return ID;
}

int viewport::get_renderer_id () const
{
    return rendererID;
}

const int * viewport::pixmap_size () const
{
    return pixMapSize;
}

void viewport::cb_renderer_select (Fl_Widget * o, void * v)
{
    listedfactory<renderer_base>::lf_menu_params * par = reinterpret_cast<listedfactory<renderer_base>::lf_menu_params *>(v);
    const Fl_Menu_Item * item = reinterpret_cast<Fl_Menu_*>(o)->mvalue();
    
    /*par->receiver; //the viewport
        par->Create(); //the new renderer*/
    
    const_cast<Fl_Menu_Item *>(item)->setonly();
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
        pixMapSize[0] = pixMapSize[1] = 0;
        }
    else
        {
        pixMapSize[0] = des_width;
        pixMapSize[1] = des_height;
        }
    
    clear_rgbpixmap();
}

void viewport::initialize_viewport(int xpos, int ypos, int width, int height)
{
    const int buttonheight=20;
    const int buttonwidth=70;
    int buttonleft=0;
    
    ////
    // Init FLTK-related data if needed to be updated
    
    int drawheight = height - buttonheight;
    
    update_viewsize(width, drawheight);
    
    containerwidget = new Fl_Group(xpos,ypos,width,height);
    
    ////buttons controlling view & rendering
    //
    viewport_buttons = new Fl_Pack(xpos,ypos,width,buttonheight,"");
    viewport_buttons->type(FL_HORIZONTAL);
    
    imagemenu_button = new Fl_Menu_Button(xpos+(buttonleft+=buttonwidth),ypos,buttonwidth,buttonheight,"Objects");
    
    renderermenu_button = new Fl_Menu_Button(xpos+(buttonleft+=buttonwidth),ypos,buttonwidth,buttonheight,"Renderer");
    renderermenu_button->copy(renderer_base::renderer_factory.menu(cb_renderer_select,(void*)this));
    renderermenu_button->user_data(NULL);
    
    //direction menu is constant for each viewport
    Fl_Menu_Item dir_menu_items [6+1];
    
    int m;
    for (m=0;m<6;m++)
        {
        menu_callback_params * cbp=new menu_callback_params;
        cbp->direction=(preset_direction)m;
        cbp->vport=this;
        
        init_fl_menu_item(dir_menu_items[m]);
        
        dir_menu_items[m].label(preset_direction_labels[m]);
        dir_menu_items[m].callback(&set_direction_callback);
        dir_menu_items[m].user_data(cbp);
        dir_menu_items[m].flags= FL_MENU_RADIO;
        
        //backwards slices do not work in the orthogonal renderer yet
        //so we'll disable those choices
        if (m > SAGITTAL)
            {dir_menu_items[m].deactivate();}
        }
    //terminate menu
    dir_menu_items[m].label(NULL);
    
    //Axial is pre-set, set checkmark accordingly
    dir_menu_items[AXIAL].setonly();
    
    directionmenu_button = new Fl_Menu_Button(xpos+(buttonleft+=buttonwidth),ypos,buttonwidth,buttonheight,preset_direction_labels[AXIAL]);
    directionmenu_button->copy(dir_menu_items);
    
    Fl_Menu_Item blend_menu_items [NUM_BLEND_MODES+1];
    
    for (m=0;m<NUM_BLEND_MODES;m++ )
        {
        menu_callback_params * cbp=new menu_callback_params;
        cbp->mode=(blendmode)m;
        cbp->rend_index=rendererIndex;
        
        init_fl_menu_item(blend_menu_items[m]);
        
        blend_menu_items[m].label(blend_mode_labels[m]);
        blend_menu_items[m].callback(&set_blendmode_callback);
        blend_menu_items[m].user_data(cbp);
        blend_menu_items[m].flags= FL_MENU_RADIO;
        
        if (rendererIndex < 0)
            {blend_menu_items[m].deactivate();}
        }
    //terminate menu
    blend_menu_items[m].label(NULL);
    
    blendmenu_button = new Fl_Menu_Button(xpos+(buttonleft+=buttonwidth),ypos,buttonwidth,buttonheight);
    blendmenu_button->copy(blend_menu_items);
    
    //styling
    blendmenu_button->box(FL_THIN_UP_BOX);
    blendmenu_button->labelsize(FLTK_SMALL_LABEL);
    
    renderermenu_button->box(FL_THIN_UP_BOX);
    renderermenu_button->labelsize(FLTK_SMALL_LABEL);
    
    directionmenu_button->box(FL_THIN_UP_BOX);
    directionmenu_button->labelsize(FLTK_SMALL_LABEL);
    
    imagemenu_button->box(FL_THIN_UP_BOX);
    imagemenu_button->labelsize(FLTK_SMALL_LABEL);
    
    viewport_buttons->end();
    
    //// the image frame
    //
    viewport_widget = new FLTKviewport(xpos,ypos+buttonheight,width,height-buttonheight);
    viewport_widget->callback(viewport_callback, this); //viewport (_not_ FLTKviewport)
                                                        //handles the callbacks
    containerwidget->resizable(viewport_widget);
    
    containerwidget->end();
    containerwidget->box(FL_BORDER_BOX);
    containerwidget->align(FL_ALIGN_CLIP);
    
    //attach MPR renderer - so that all viewports can be populated for additional views
    viewmanagement.connect_renderer_to_viewport(ID,rendermanagement.create_renderer(RENDERER_MPR));
}


void viewport::initialize_GL ()
{
    Fl_Group::current(containerwidget);
    
    GL_widget = new Fl_Gl_Window(viewport_widget->x(),viewport_widget->y(),viewport_widget->w(),viewport_widget->h());
    viewport_widget->hide();
    
    containerwidget->end();
}


void viewport::hide_GL ()
{
    if (pt_error::error_if_null(GL_widget,"Attempting to hide GL without having initialized first",pt_error::warning) != NULL)
        { 
        GL_widget->hide();
        viewport_widget->show();
        }
}

void viewport::update_objects_menu()
{
    unsigned int m=0;
    
    const Fl_Menu_Item * cur_menu, *base_menu;
    
    base_menu=datamanagement.FLTK_objects_menu();
    cur_menu=imagemenu_button->menu();
    
    int baseMenuSize = fl_menu_size(base_menu);
    
    //Fl_Menu_Item new_menu[baseMenuSize+1];
    Fl_Menu_Item * new_menu = new Fl_Menu_Item[baseMenuSize+1];
    
    if (cur_menu!=NULL)
        {
        //delete old callback data (menu is deleted by fl_menu::copy)
        
        fl_menu_userdata_delete(cur_menu);
        }
    
    if (base_menu != NULL && rendererIndex >= 0)
        {
        do 
            {       
                const char * dummy = base_menu[m].label();
                memcpy (&new_menu[m],&base_menu[m],sizeof(Fl_Menu_Item));
                
                if (new_menu[m].label()!=NULL)
                    {
                    //long v=base_menu[m].argument();
                    //attach menu_callback_params and
                    //set checkmarks according to displayed images
                    
                    menu_callback_params * p= new menu_callback_params;
                    p->rend_index=rendererIndex;
                    p->vol_id=base_menu[m].argument();  //image ID is stored in user data initially
                    
                    new_menu[m].callback((Fl_Callback *)toggle_image_callback);
                    new_menu[m].user_data(p);
                    new_menu[m].flags=FL_MENU_TOGGLE;
                    
                    if (rendermanagement.image_rendered(rendererIndex,p->vol_id) !=BLEND_NORENDER)
                        {
                        //checked
                        new_menu[m].set();
                        }
                    }
            } while (new_menu[m++].label() !=NULL && m <= baseMenuSize);
        
        imagemenu_button->copy(new_menu);
        }
    else
        { imagemenu_button->menu(NULL); }
}

void viewport::toggle_image_callback(Fl_Widget *callingwidget, void * params )
{
    menu_callback_params * widget_user_data=(menu_callback_params *)params;
    
    rendermanagement.toggle_image(widget_user_data->rend_index,widget_user_data->vol_id);
}

void viewport::set_direction_callback(Fl_Widget *callingwidget, void * p )
{
    enum {x=0,y,z};
    menu_callback_params * params=(menu_callback_params *)p;
    
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
    
    switch (params->direction) {
        case AXIAL:
            dir[x][0]=1; //voxel direction of view x
            dir[y][1]=1; //voxel direction of view y
            dir[z][2]=1; //voxel direction of slicing
            break;
            
        case CORONAL:
            dir[x][0]=1;
            dir[z][1]=-1;
            dir[y][2]=1;
            break;
            
        case SAGITTAL:
            dir[y][0]=1;
            dir[z][1]=-1;
            dir[x][2]=1;
            break;
            
        case AXIAL_NEG:
            dir[x][0]=1;
            dir[y][1]=-1;
            dir[z][2]=-1;
            break;
            
        case CORONAL_NEG:
            dir[x][0]=-1;
            dir[z][1]=1;
            dir[y][2]=-1;
            break;
            
        case SAGITTAL_NEG:
            dir[y][0]=-1;
            dir[z][1]=-1;
            dir[x][2]=1;
            break;
    }
    
    Matrix3D * dir_p=new Matrix3D(dir);
    rendermanagement.set_geometry(params->vport->rendererIndex,dir_p);
    
    //callingwidget=directionmenu_button
    callingwidget->label(preset_direction_labels[params->direction]);
    
    delete dir_p;
}

void viewport::set_blendmode_callback(Fl_Widget *callingwidget, void * p )
{
    menu_callback_params * params=(menu_callback_params *)p;
    
    rendermanagement.set_blendmode(params->rend_index,params->mode);
}

void viewport::rebuild_renderer_menu ()
{
    //since available renderer types do not change at runtime,
    //the menu isn't really rebuilt - only the radio checkmark is reassigned
    
    if (renderermenu_button != NULL)
        {
        Fl_Menu_Item * renderermenu=(Fl_Menu_Item *)renderermenu_button->menu();
        int numItems = fl_menu_size (renderermenu);
        
        if (rendererIndex >= 0)
            {
            std::string this_renderer_type=rendermanagement.get_renderer_type(rendererIndex);
            
            for (int m = 0; m < numItems;m++)
                {
                listedfactory<renderer_base>::lf_menu_params * p = reinterpret_cast<listedfactory<renderer_base>::lf_menu_params * > (renderermenu[m].user_data());
                if (p->type == this_renderer_type)
                    { renderermenu[m].setonly(); }
                }
            }   
        if (numItems <= 1) //no real choice, disable
            {
            renderermenu[0].deactivate();
            }
        
        if (numItems == 0) //no choice at all, disable even more!
            {
            renderermenu_button->deactivate();
            }
        }
}

void viewport::rebuild_blendmode_menu ()//update checkmark for current blend mode
{
    if (blendmenu_button != NULL)
        {
        Fl_Menu_Item * blendmodemenu=(Fl_Menu_Item *)blendmenu_button->menu();
        
        //blend modes are different - or not available - for other renderer types
        //basic check for this:
        //int this_renderer_type=rendermanagement.get_renderer_type(rendererIndex);
        
        for (int m=0; m < NUM_BLEND_MODES ; m++)
            {
            ((menu_callback_params *)(blendmodemenu[m].argument()))->rend_index=rendererIndex;
            
            if (rendererIndex <0 || rendermanagement.renderer_supports_mode(rendererIndex,m))
                {blendmodemenu[m].deactivate();}
            else
                {blendmodemenu[m].activate();}
            }
        
        if (rendererIndex >= 0)
            {
            int this_blend_mode=rendermanagement.get_blend_mode(rendererIndex);
            
            blendmodemenu[this_blend_mode].setonly();
            
            blendmenu_button->label(blend_mode_labels[this_blend_mode]);
            }
        }
}