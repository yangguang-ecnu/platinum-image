// $Id$

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

#include "FLTKviewport.h"

#include "datamanager.h"
#include "rendermanager.h"
#include "viewmanager.h"

extern datamanager datamanagement;
extern rendermanager rendermanagement;
extern viewmanager viewmanagement;

//#include <FL/Enumerations.H>

using namespace std;


const char * preset_direction_labels[] = {"Z","Y","X","-Z","-Y","-X"};
//const char * preset_direction_labels[] ={"Axial","Coronal","Sagittal","-Axial","-Coronal","-Sagittal"};
const char * blend_mode_labels[] = {"Overwrite","Max","Min","Average","Tint","Grey+Red"};

struct menu_callback_params
{
    int rend_index;				//for blend & image callbacks
    blendmode mode;				//for blend callback
    int vol_id;					//for image callback
    viewport *vport;			//for preset direction callback    
    preset_direction direction;	//for preset direction callback    
};

string eventnames[] =
    {
    //array allows event names to be printed to strings and whatnot
    "FL_NO_EVENT",		//0
    "FL_PUSH",			//1
    "FL_RELEASE",		//2
    "FL_ENTER",		    //3
    "FL_LEAVE",		    //4
    "FL_DRAG",			//5
    "FL_FOCUS",		    //6
    "FL_UNFOCUS",		//7
    "FL_KEYDOWN",		//8
    "FL_KEYUP",	    	//9
    "FL_CLOSE",	    	//10
    "FL_MOVE",			//11
    "FL_SHORTCUT",		//12
    "FL_DEACTIVATE",	//13
    "FL_ACTIVATE",		//14
    "FL_HIDE",			//15
    "FL_SHOW",			//16
    "FL_PASTE",		    //17
    "FL_SELECTIONCLEAR",//18
    "FL_MOUSEWHEEL",	//19
    "FL_DND_ENTER", 	//20
    "FL_DND_DRAG",		//21
    "FL_DND_LEAVE", 	//22
    "FL_DND_RELEASE",	//23
    };


FLTKpane::FLTKpane(int X,int Y,int W,int H, viewport *vp_parent) : Fl_Overlay_Window(X,Y,W,H)
{}


void FLTKpane::needs_rerendering()
{
	viewport_parent->needs_rerendering();
}


//FLTK_VTK_pane::FLTK_VTK_pane(int X,int Y,int W,int H, viewport *vp_parent) : Fl_Overlay_Window(X,Y,W,H)
FLTK_VTK_pane::FLTK_VTK_pane(int X,int Y,int W,int H, viewport *vp_parent) : FLTKpane(X,Y,W,H,vp_parent)
{
	viewport_parent = vp_parent;
		
	vtkFlRenderWindowInteractor *fl_vtk_window = NULL;
	fl_vtk_window = new vtkFlRenderWindowInteractor(0,0,W,H,"");
    
	this->end();
	this->resizable(fl_vtk_window);

	//--------------------------------------------------
  
	vtkRenderWindow *renWindow = vtkRenderWindow::New();
	vtkRenderer *ren = vtkRenderer::New();
	ren->SetBackground(0.1,0.1,0.1);
	renWindow->AddRenderer(ren);

	// uncomment the statement below if things aren't rendering 100% on your
	// configuration; the debug output could give you clues as to why
	//renWindow->DebugOn();
	   
	// Here we treat the vtkFlRenderWindowInteractor just like any other old vtkRenderWindowInteractor
	fl_vtk_window->SetRenderWindow(renWindow);

	// just like with any other vtkRenderWindowInteractor(), you HAVE to call
	// Initialize() before the interactor will function.  See the docs in vtkRenderWindowInteractor.h
	fl_vtk_window->Initialize();

	// create an actor and give it cone geometry
	vtkConeSource *cone = vtkConeSource::New();
	cone->SetResolution(8);
	vtkPolyDataMapper *coneMapper = vtkPolyDataMapper::New();
	coneMapper->SetInput(cone->GetOutput());
	vtkActor *coneActor = vtkActor::New();
	coneActor->SetMapper(coneMapper);
	coneActor->GetProperty()->SetColor(1.0, 0.0, 1.0);

	ren->AddActor(coneActor);

	// We can now delete all our references to the VTK pipeline (except for
	// our reference to the vtkFlRenderWindowInteractor) as the objects
	// themselves will stick around until we dereference fl_vtk_window
	ren->Delete();
	renWindow->Delete();
	cone->Delete();
	coneMapper->Delete();
	coneActor->Delete();
}

void FLTK_VTK_pane::needs_rerendering()
{
	viewport_parent->needs_rerendering();
}


FLTK_Event_pane::FLTK_Event_pane(int X,int Y,int W,int H) : Fl_Widget(X,Y,W,H)
{}

int FLTK_Event_pane::handle(int event){
//	cout<<"FLTK_Event_pane::handle("<<eventnames[event]<<") ";

	FLTK_Pt_pane *fp = (FLTK_Pt_pane*)this->parent();
	fp->callback_event = viewport_event(event,fp);
//	cout<<"fp->w/h "<<fp->w()<<"/"<<fp->h()<<endl;


    switch (event)
        {
        case FL_ENTER:
            Fl::focus(this);
            //allows keyboard events to be received once the mouse is inside
        case FL_FOCUS:
        case FL_UNFOCUS:
            //usually, one wants to show that a widget has focus
            //which is shown/hidden with these events
            //with mouse-over focus however, this can be annoying
            
            fp->callback_event.grab();
            break;
        }
    
   fp->do_callback();

/*
    if(callback_event.handled()){
		return 1; 
	}else{
		return 0; 
	}
    
    callback_event = viewport_event (pt_event::no_type,NULL);
    return 1;
*/

return 1;//	return Fl_Widget::handle(event);
}


void FLTK_Event_pane::draw()
{}




//FLTK_Pt_pane::FLTK_Pt_pane(int X,int Y,int W,int H, viewport *vp_parent) : Fl_Overlay_Window(X,Y,W,H)
FLTK_Pt_pane::FLTK_Pt_pane(int X,int Y,int W,int H, viewport *vp_parent) : FLTKpane(X,Y,W,H,vp_parent)
{
	//cout<<"FLTK_Pt_pane::FLTK_Pt_pane "<<X<<" "<<Y<<" "<<W<<" "<<H<<endl;
	viewport_parent = vp_parent;
	event_pane = new FLTK_Event_pane(0,0,W,H);

	this->resizable(event_pane);			//Make sure thes is resized too...
	viewport_parent->needs_rerendering();
	callback_action=CB_ACTION_NONE;
}

FLTK_Pt_pane::~FLTK_Pt_pane()
{}
	
void FLTK_Pt_pane::draw_overlay()
{
	this->viewport_parent->paint_overlay();
}


void FLTK_Pt_pane::draw()
{
    //The draw() virtual method is called when FLTK wants you to redraw your widget.
    //It will be called if and only if damage()  is non-zero, and damage() will be cleared to zero after it returns
	//cout<<"FLTK_Pt_pane::draw()..."<<endl;
    callback_event = viewport_event(pt_event::draw,this);
    //callback_event.FLTK_event::attach (this);
	//pane_widget->callback(viewport_callback, this); //viewport (_not_ FLTK_Pt_pane) handles the callbacks
	do_callback(CB_ACTION_DRAW); //JK2
}

void FLTK_Pt_pane::draw(unsigned char *rgbimage)
{
//	cout<<"FLTK_Pt_pane::draw(unsigned char *rgbimage)..."<<endl;
//	cout<<"("<<x()<<" "<<y()<<" "<<w()<<" "<<h()<<")"<<endl;
    const int D=RGBpixmap_bytesperpixel;

    //damage (FL_DAMAGE_ALL);

    if (w()>0 && h()>0)
    {
        // do not redraw zero-sized viewport, fl_draw_image will break down
		
		fl_draw_image(rgbimage,0,0,w(),h());

/*		This code is probably not needed anymore:

        #if defined(__APPLE__)
            const int LD=w(); //size of one pixmap line
			// fl_draw_image(rgbimage,x(),y(),w(),h(), D,LD) ;
            fl_draw_image(rgbimage,0,0,w(),h(), D,LD); //JK- Drawing is done relative to the window...
        #else
			// fl_draw_image(rgbimage,x(),y(),w(),h(), D) ;
            fl_draw_image(rgbimage,0,0,w(),h(), D); //JK- Drawing is done relative to the window...
        #endif
*/
    }

    //draw_feedback();
}


void FLTK_Pt_pane::resize(int new_in_x,int new_in_y, int new_in_w,int new_in_h){
//	cout<<"FLTK_Pt_pane::resize..."<<endl;

    //store new size so CB_ACTION_RESIZE will know about it (via callback)
    resize_w=new_in_w; resize_h=new_in_h;
    callback_event = viewport_event(pt_event::resize, this);
    callback_event.set_resize(resize_w,resize_h);
    
    do_callback();
    
    //do the actual resize - redraw will follow, eventually
	Fl_Overlay_Window::resize(new_in_x,new_in_y,new_in_w,new_in_h);
    
    //Update "new" size so that pixmap reevaluation won't be triggered until next resize
    resize_w=w(); resize_h=h();
}

void FLTK_Pt_pane::needs_rerendering()
{
	viewport_parent->needs_rerendering();
}

void FLTK_Pt_pane::do_callback (callbackAction action)
{
    callback_action=action;
    Fl_Widget::do_callback();
    callback_action=CB_ACTION_NONE;
}



//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------

FLTKviewport::FLTKviewport(int xpos,int ypos,int width,int height, viewport *vp_parent, int buttonheight, int buttonwidth):Fl_Window(xpos,ypos,width,height)
{
	viewport_parent = vp_parent;
    int buttonleft=0;
    
    viewport_buttons = new Fl_Pack(0,0,width,buttonheight,"");
    viewport_buttons->type(FL_HORIZONTAL);
    
    datamenu_button = new Fl_Menu_Button(0+(buttonleft+=buttonwidth),0,buttonwidth,buttonheight,"Data");
    
    renderermenu_button = new Fl_Menu_Button(0+(buttonleft+=buttonwidth),0,buttonwidth,buttonheight,"Renderer");
	//The factory below returnsconnects the  
//	renderermenu_button->copy(rendermanager::renderer_factory.menu(cb_renderer_select,(void*)this)); 
	renderermenu_button->copy(viewport_parent->pfactory.get_FLTK_menu(cb_renderer_select2)); //JK2
    renderermenu_button->user_data(NULL);
    
    //direction menu is constant for each viewport
    Fl_Menu_Item dir_menu_items [6+1];
    
    int m;
    for (m=0;m<6;m++)
        {
        menu_callback_params * cbp=new menu_callback_params;
        cbp->direction=(preset_direction)m;
        cbp->vport=vp_parent;
        
        init_fl_menu_item(dir_menu_items[m]);
        
        dir_menu_items[m].label(preset_direction_labels[m]);
        dir_menu_items[m].callback(&set_direction_callback);
        dir_menu_items[m].user_data(cbp);
        dir_menu_items[m].flags= FL_MENU_RADIO;
        
        //backwards slices do not work in the orthogonal renderer yet
        //so we'll disable those choices
//        if (m > SAGITTAL)
//            {dir_menu_items[m].deactivate();}
        }

    //terminate menu
    dir_menu_items[m].label(NULL);
    
    //Axial is pre-set, set checkmark accordingly
    dir_menu_items[Z_DIR].setonly();
//    dir_menu_items[AXIAL].setonly(); //AXIAL_NEG
    

    directionmenu_button = new Fl_Menu_Button(0+(buttonleft+=buttonwidth),0,buttonwidth,buttonheight,preset_direction_labels[Z_DIR]);
    directionmenu_button->copy(dir_menu_items);
    
    Fl_Menu_Item blend_menu_items [NUM_BLEND_MODES+1];
    
    for (m=0;m<NUM_BLEND_MODES;m++ )
        {
//		cout<<"m="<<m<<"/"<<NUM_BLEND_MODES<<endl;

        menu_callback_params * cbp=new menu_callback_params;
        cbp->mode=(blendmode)m;
        cbp->rend_index=vp_parent->rendererIndex;
        
        init_fl_menu_item(blend_menu_items[m]);
        
        blend_menu_items[m].label(blend_mode_labels[m]);
        blend_menu_items[m].callback(&set_blendmode_callback);
        blend_menu_items[m].user_data(cbp);
        blend_menu_items[m].flags= FL_MENU_RADIO;
        
//        if (rendererIndex < 0)
  //          {blend_menu_items[m].deactivate();}
      }
    //terminate menu
    blend_menu_items[m].label(NULL);
    
    blendmenu_button = new Fl_Menu_Button(0+(buttonleft+=buttonwidth),0,buttonwidth,buttonheight);
    blendmenu_button->copy(blend_menu_items);
    
	//------ styling --------------
    blendmenu_button->box(FL_THIN_UP_BOX);
    blendmenu_button->labelsize(FLTK_SMALL_LABEL);
    renderermenu_button->box(FL_THIN_UP_BOX);
    renderermenu_button->labelsize(FLTK_SMALL_LABEL);
    directionmenu_button->box(FL_THIN_UP_BOX);
    directionmenu_button->labelsize(FLTK_SMALL_LABEL);
    datamenu_button->box(FL_THIN_UP_BOX);
    datamenu_button->labelsize(FLTK_SMALL_LABEL);
	//------ styling --------------
    
    viewport_buttons->end();
    


//	Fl_Group::current(NULL); //JK-ööö
//  pane_widget = new FLTK_Pt_pane(xpos,ypos+buttonheight,width,height-buttonheight);

	if(viewport_parent->vp_type == PT_MPR){
		pane_widget = new FLTK_Pt_pane(0,0+buttonheight,width,height-buttonheight, vp_parent);
	}else{
		pane_widget = new FLTK_VTK_pane(0,0+buttonheight,width,height-buttonheight, vp_parent);
	}

	pane_widget->callback(viewport_callback, this); //viewport (_not_ FLTK_Pt_pane) handles the callbacks


	this->resizable(pane_widget);
    this->end();
    this->box(FL_BORDER_BOX);
    this->align(FL_ALIGN_CLIP);
	}


FLTKviewport::~FLTKviewport()
{
// Following two lines give an error when exiting the program... 
// A likely cause is that the FLTK object has already been deleted when we try to 
	if(datamanagement.FLTK_running())
	{
		if (datamenu_button != NULL) //objects menu has user data which was allocated in line 542
		{
			fl_menu_userdata_delete (renderermenu_button->menu()); 
		}
	}
}

int FLTKviewport::h_pane()
{
	return pane_widget->h();
}

void FLTKviewport::refresh_menus()
{
	update_data_menu();
    rebuild_renderer_menu();
    rebuild_blendmode_menu();
}

void FLTKviewport::refresh_overlay()
{
	pane_widget->redraw_overlay();
}

void FLTKviewport::rebuild_renderer_menu ()
{
    //since available renderer types do not change at runtime,
    //the menu isn't really rebuilt - only the radio checkmark is reassigned
    
    if ( renderermenu_button != NULL )
        {
        Fl_Menu_Item * renderermenu=(Fl_Menu_Item *)renderermenu_button->menu();
	
		int numItems = fl_menu_size (renderermenu);
		
		if (viewport_parent->rendererIndex >= 0)
			{
			std::string this_renderer_type=rendermanagement.get_renderer_type(viewport_parent->rendererIndex);
			
			for (int m = 0; m < numItems;m++)
				{
//JK2				listedfactory<renderer_base>::lf_menu_params * p = reinterpret_cast<listedfactory<renderer_base>::lf_menu_params * > (renderermenu[m].user_data());
//				if (p->type == this_renderer_type)
//					{ renderermenu[m].setonly(); }
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

void FLTKviewport::rebuild_blendmode_menu()//update checkmark for current blend mode
{
    if (blendmenu_button != NULL)
        {
        Fl_Menu_Item * blendmodemenu=(Fl_Menu_Item *)blendmenu_button->menu();

		//blend modes are different - or not available - for other renderer types (than MPR...)
		//basic check for this:
		//int this_renderer_type=rendermanagement.get_renderer_type(rendererIndex);
		
		for (int m=0; m < NUM_BLEND_MODES ; m++)
			{
			((menu_callback_params*)(blendmodemenu[m].argument()))->rend_index = viewport_parent->rendererIndex;
			
			if (viewport_parent->rendererIndex <0 || !rendermanagement.renderer_supports_mode(viewport_parent->rendererIndex,m))
				{
					blendmodemenu[m].deactivate();
				}
			else
				{
					blendmodemenu[m].activate();
				}
			}
		
		if (viewport_parent->rendererIndex >= 0)
			{
			int this_blend_mode=rendermanagement.get_blend_mode(viewport_parent->rendererIndex);
			
			blendmodemenu[this_blend_mode].setonly();
			
			blendmenu_button->label(blend_mode_labels[this_blend_mode]);
			}
        }
}
void FLTKviewport::update_data_menu()
{	
    unsigned int m=0;
    
    const Fl_Menu_Item * cur_menu, *base_menu;
    
    base_menu=datamanagement.FLTK_objects_menu();
    cur_menu=datamenu_button->menu();
    
    unsigned int baseMenuSize = fl_menu_size(base_menu);
    
    //Fl_Menu_Item new_menu[baseMenuSize+1];
    Fl_Menu_Item * new_menu = new Fl_Menu_Item[baseMenuSize+1];
    
    if (cur_menu!=NULL)
        {
        //delete old callback data (menu is deleted by fl_menu::copy)
        
        fl_menu_userdata_delete(cur_menu);
        }
    
    if (base_menu != NULL && viewport_parent->rendererIndex >= 0)
        {
        do 
            {       
                memcpy (&new_menu[m],&base_menu[m],sizeof(Fl_Menu_Item));
                
                if (new_menu[m].label()!=NULL)
                    {
                    //long v=base_menu[m].argument();
                    //attach menu_callback_params and
                    //set checkmarks according to displayed images
                    
                    menu_callback_params * p= new menu_callback_params;
                    p->rend_index=viewport_parent->rendererIndex;
                    p->vol_id=base_menu[m].argument();  //image ID is stored in user data initially
//					p->vport = this; //JK
                    
                    new_menu[m].callback((Fl_Callback *)toggle_image_callback);
                    new_menu[m].user_data(p);
                    new_menu[m].flags=FL_MENU_TOGGLE;
                    if (rendermanagement.image_rendered(viewport_parent->rendererIndex,p->vol_id) !=BLEND_NORENDER)
                        {
                        //checked
                        new_menu[m].set();
                        }
                    }
            } while (new_menu[m++].label() !=NULL && m <= baseMenuSize);
        
        datamenu_button->copy(new_menu);
        delete new_menu;
        }
    else
        { datamenu_button->menu(NULL); }
}


void FLTKviewport::cb_renderer_select (Fl_Widget * o, void * v)
{
    listedfactory<renderer_base>::lf_menu_params * par = reinterpret_cast<listedfactory<renderer_base>::lf_menu_params *>(v);
    const Fl_Menu_Item * item = reinterpret_cast<Fl_Menu_*>(o)->mvalue();
    
    //par->receiver; //the viewport
    //par->Create(); //the new renderer
    
    const_cast<Fl_Menu_Item *>(item)->setonly();
}

void FLTKviewport::cb_renderer_select2 (Fl_Widget * o, void * v)
{
//    listedfactory<renderer_base>::lf_menu_params * par = reinterpret_cast<listedfactory<renderer_base>::lf_menu_params *>(v);
//    const Fl_Menu_Item * item = reinterpret_cast<Fl_Menu_*>(o)->mvalue();
    
    //par->receiver; //the viewport
    //par->Create(); //the new renderer
    
//    const_cast<Fl_Menu_Item *>(item)->setonly();
}

void FLTKviewport::set_blendmode_callback(Fl_Widget *callingwidget, void * p )
{
    menu_callback_params * params=(menu_callback_params *)p;
    
    rendermanagement.set_blendmode(params->rend_index,params->mode);
}

void FLTKviewport::toggle_image_callback(Fl_Widget *callingwidget, void * params )
{
    menu_callback_params * widget_user_data=(menu_callback_params *)params;

    rendermanagement.toggle_image(widget_user_data->rend_index,widget_user_data->vol_id);

	// update all viewports that shows at least one of the images in the current viewport:
	int c_id = rendermanagement.get_combination_id(widget_user_data->rend_index);
	if(c_id>=0){
		viewmanagement.refresh_viewports_from_combination(c_id);
//		viewmanagement.refresh_overlays();

		//since we are now in a static function... we have to send the window of the widget...
		viewmanagement.update_overlays();

		//JK also refresh other overlays...
	}else{
		viewmanagement.refresh_viewports(); //complicated to remember old settings... slow but simple solution... 
	}

	// TODO: only refresh viewports that holds the addded/removed image
	// hur kan det aktuella bild id:et erhållas? undersök om det verkligen är en bild (dvs exkludera tex en point_collection)
	// möjlig lösning är att jämföra de bilder combination innehåller före och efter toggle_image() 

//	cout<<"vport_id= "<<widget_user_data->vport->get_id()<<endl;
//	viewmanagement.refresh_viewports(); //time consuming to update all....
//	widget_user_data->vport->refresh(); //occasionally results in an error...
}


void FLTKviewport::set_direction_callback(Fl_Widget *callingwidget, void * p )
{
    menu_callback_params * params = (menu_callback_params *) p;
	params->vport->set_renderer_direction( params->direction );

	params->vport->refresh();
//	viewmanagement.refresh_overlays();
	viewmanagement.update_overlays();

/*
	// update all viewports that shows at least one of the images in the current viewport (slice locators)
	int combinationID = rendermanagement.get_combination_id( params->rend_index );
	std::vector<int> geometryIDs = rendermanagement.geometries_by_image ( combinationID );		// return geometries that holds at least one of the images in the input combination

	for ( std::vector<int>::const_iterator itr = geometryIDs.begin(); itr != geometryIDs.end(); itr++ )
	{
		viewmanagement.refresh_viewports_from_geometry( *itr );
	}
*/
}


void FLTKviewport::set_direction_button_label(preset_direction direction)
{
	directionmenu_button->label( preset_direction_labels[direction] );
}

void FLTKviewport::viewport_callback(Fl_Widget *callingwidget, void *thisFLTKviewport)
{
    ((FLTKviewport*)thisFLTKviewport)->viewport_callback(callingwidget);
}

void FLTKviewport::viewport_callback(Fl_Widget *callingwidget){
    FLTK_Pt_pane *fp = (FLTK_Pt_pane*)callingwidget;
    //fp points to the same GUI toolkit-dependent widget instance as pane_widget

    if (fp->callback_event.type() == pt_event::draw)
        {
        fp->callback_event.grab();
		render_if_needed();

        fp->damage(FL_DAMAGE_ALL);
//		cout<<"viewport::viewport_callback...draw(pxmap)"<<endl;//JK-ööö
        fp->draw(viewport_parent->rgbpixmap); //JK2-ööö, do this in FLTK_draw_vp???
        fp->damage(0);
        }

    if (viewport_parent->busyTool == NULL)
        { 
        viewport_parent->busyTool = viewporttool::taste(fp->callback_event,viewport_parent,rendermanagement.get_renderer(viewport_parent->rendererID));
        }
    
		if (viewport_parent->busyTool != NULL) //might have been created earlier too
        {
        viewport_parent->busyTool->handle(fp->callback_event);

        if (render_if_needed())
            {
				fp->redraw(); 
			}
        }
    
    //handle events "otherwise"
    if (!fp->callback_event.handled())
        {
        //"default" behavior for the viewport goes here
        
        //call render_if_needed(); if image may need re-rendering
        }
    
    //handle events regardless of whether a tool caught them
    switch (fp->callback_event.type())
        {
        case pt_event::resize:
            if ((fp->resize_w != viewport_parent->rgbpixmap_size[0] || fp->resize_h != viewport_parent->rgbpixmap_size[1]))
                {
                //resize: just update view size, re-render but don't redraw...yet
                
                const int *r = fp->callback_event.get_resize();
                viewport_parent->update_viewsize(r[0] ,r[1]);
                
                fp->needs_rerendering();
                }
            break;
        }

}

bool FLTKviewport::render_if_needed()
{
	return viewport_parent->render_if_needed();
}
/*
void FLTKviewport::update_fbstring (FLTK_Pt_pane* fp)
{
    std::map<std::string,float> values;
    ostringstream infostring;
    
    //Draw values picked from data at mouse position
    //in later revisions this event and clicks may be relayed to the actual application
    
    values=rendermanagement.get_values(viewport_parent->rendererIndex,fp->mouse_pos[0]-fp->x(),fp->mouse_pos[1]-fp->y(),viewport_parent->rgbpixmap_size[0],viewport_parent->rgbpixmap_size[1]);
    
    for (std::map<std::string,float>::iterator itr = values.begin(); itr != values.end();itr++)
        {
        if (itr != values.begin()) 
            {
            infostring << endl;
            }
        infostring << itr->first << ": " << itr->second;
        }
    
    fp->feedback_string=infostring.str();
}
*/