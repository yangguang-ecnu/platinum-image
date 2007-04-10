//
// viewport: Skoter in/utmatning, inklusive knappar, markorer, 
// koordinatgrafik for en 2D-yta (via FLTK). Bilddata renderas av 
// klassen "renderer"
//

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

#include "viewport.h"

#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Scroll.H>

#include <FL/Fl_Tile.H>
#include <FL/Fl_Box.H>
#include <FL/Enumerations.H>

#include "viewmanager.h"

extern datamanager datamanagement;
extern rendermanager rendermanagement;
extern viewmanager viewmanagement;

using namespace std;

int viewport::maxviewportID = 0;
bool viewport::renderermenu_built=false;

static Fl_Menu_Item renderermenu_global[NUM_RENDERER_TYPES_PLUS_END];   //this one should be protected too

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
    //for blend & volume callbacks
    int rend_index;

    //for blend callback
    blendmode mode;

    //for volume callback
    int vol_id;

    //for preset direction callback    
    viewport * vport;
    preset_direction direction;
    };

void viewport::clear_rgbpixmap()
    {
    //blacken viewport
    for (long p=0; p < rgbpixmapwidth*rgbpixmapheight*RGBpixmap_bytesperpixel; p +=RGBpixmap_bytesperpixel )
        {
        rgbpixmap[p] = rgbpixmap[p + 1] = rgbpixmap[p + 2] = 0;
        }
    }

void init_fl_menu_item(Fl_Menu_Item * m)
    {
    m->label("");
    m->shortcut(0);
    m->callback((Fl_Callback *)NULL,0);
    m->user_data(NULL);
    m->flags= 0;
    m->labeltype(FL_NORMAL_LABEL);
    m->labelfont(0);
    m->labelsize(FLTK_LABEL_SIZE);
    m->labelcolor(FL_BLACK);
    }

viewport::viewport()
    {
    ID = ++maxviewportID;
    cout << "viewport constructor" << ID << endl;

    viewport_widget=NULL;
    volumemenu_button=NULL;
    renderermenu_button=NULL;
    viewport_buttons=NULL;

    rendererID=NO_RENDERER_ID;
    rendererIndex=-1;

    rgbpixmap = new uchar [0];
    rgbpixmapwidth = 0;
    rgbpixmapheight = 0;

    if (!renderermenu_built)
        {
        //static renderer menu not setup yet, build it once now

        int m;
        for (m=0;m<NUM_RENDERER_TYPES;m++ )
            {
            init_fl_menu_item(&renderermenu_global[m]);
            renderermenu_global[m].label(renderer_labels[m].c_str());
            renderermenu_global[m].flags= FL_MENU_RADIO;

            //temporary, since choice does not do anything at the moment
            renderermenu_global[m].deactivate();
            }
        //terminate menu
        renderermenu_global[m].label(NULL);

        renderermenu_built=true;
        }
    }

void viewport::connect_renderer(int rID)
    {
    rendererID = rID;               //this is the unique renderer ID within the class, NOT the vector array index!!!

    viewport_widget->needs_re_rendering=true;
    refresh();
    }

void viewport::refresh_from_combination(int c)
    {
    //conditional refresh, only perform if this instance uses
    //the argument rendercombination

    if (rendererIndex >= 0 && rendermanagement.get_combination_id(rendererIndex) == c)
        {
        //this call means that the selection of rendered volumes has changed,
        //possibly into nothing
        
        clear_rgbpixmap();

        if (viewport_widget->thresholder != NULL)
            {
            viewport_widget->thresholder->expire();
            }
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
            if (viewport_widget->thresholder != NULL)
                {viewport_widget->thresholder->renderer_index(rendermanagement.find_renderer_index(rendererID));}

            update_volume_menu();
            rebuild_renderer_menu();
            rebuild_blendmode_menu();

            viewport_widget->needs_re_rendering=true;
            viewport_buttons->activate();
            }

        viewport_widget->damage(FL_DAMAGE_ALL);
        }
    }

void viewport::update_fbstring (FLTKviewport* f)
    {
    vector<float> values;
    ostringstream infostring;

    //Draw values picked from data at mouse position
    //in later revisions this event and clicks may be relayed to the actual application

    values=rendermanagement.get_values(rendererIndex,f->mouse_pos[0]-f->x(),f->mouse_pos[1]-f->y(),rgbpixmapwidth,rgbpixmapheight);

    for (unsigned int i=0; i < values.size(); i++)
        {
        if (i>0) 
            {
            infostring << endl;
            }
        infostring << values[i];
        }

    f->feedback_string=infostring.str();
    }

threshold_overlay * viewport::get_threshold_overlay (thresholdparvalue * threshold_par)
    {    
    if (rendererID != NO_RENDERER_ID)
        {
        int p=0;
        int rendered_vol_ID=rendermanagement.volume_at_priority (rendererIndex,p);

        while (rendered_vol_ID > 0)
            {
            for (int d=0;threshold_par->id[d] != NOT_FOUND_ID ;d++)
                {
                if (rendered_vol_ID==threshold_par->id[d])
                    {
                    if (viewport_widget->thresholder == NULL)
                        {viewport_widget->thresholder=new threshold_overlay(viewport_widget,rendererIndex);}
                    return viewport_widget->thresholder;
                    }
                }
            p++;
            rendered_vol_ID=rendermanagement.volume_at_priority (rendererIndex,p);
            }

        if (viewport_widget->thresholder != NULL)
            {
            viewport_widget->thresholder->expire();
            viewport_widget->redraw();
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

void viewport::viewport_callback(Fl_Widget *callingwidget){
    FLTKviewport* f = (FLTKviewport*)callingwidget;
    //f points to the same GUI toolkit-dependent widget instance as viewport_widget

    if (f->callback_action == CB_ACTION_RESIZE)
        {
        if ((f->resize_w != rgbpixmapwidth || f->resize_h != rgbpixmapheight))
            {
            //resize: just update view size, re-render but don't redraw...yet
            update_viewsize(f->resize_w ,f->resize_h);

            if (f->ROI != NULL)
                {f->ROI->resize (0,0,1,f);}

            f->needs_re_rendering=true;
            }
        }
    else
        {
        //some kind of interactive action

        //UI constants
        const float wheel_factor=0.02;
        const float zoom_factor=0.01;
        const float pan_factor=(float)1/(min(rgbpixmapwidth,rgbpixmapheight));

        switch (f->callback_action) {
            case CB_ACTION_DRAG_PASS:
                //this callback is for general click & drag in viewport
                //however only function available yet is the histogram ROI

                if (rendererID != NO_RENDERER_ID)
                    {
                    if (!f->ROI->dragging)
                        {
                        //to improve performance, the attached histograms are cached during drag
                        f->ROI->attach_histograms(rendererIndex);
                        }

                    if (f->ROI->histograms.size() >0 )  //only ROI yourself if there is a suitable histogram around
                        {
                        if(FLTK2Dregionofinterest::current_ROI != f->ROI)
                            {
                            viewmanagement.refresh_viewports(); //erase ROIs shown in other viewports
                            FLTK2Dregionofinterest::current_ROI = f->ROI;
                            }

                        f->ROI->drag(f->mouse_pos[0],f->mouse_pos[1],f->drag_dx,f->drag_dy,f);
                        }
                    }
                break;

            case CB_ACTION_DRAG_PAN:
                {
                float pan_x=0;
                float pan_y=0;

                pan_x-=f->drag_dx*pan_factor; 
                pan_y-=f->drag_dy*pan_factor;

                f->ROI->resize (f->drag_dx,f->drag_dy,1,f);
                rendermanagement.move(rendererIndex,pan_x,pan_y);

                f->needs_re_rendering=true;
                }
            break;

            /*case CB_ACTION_WHEEL_ZOOM:
            zoom*=1+f->wheel_y*wheel_factor;
            f->needs_re_rendering=true;
            break;*/

            case CB_ACTION_DRAG_ZOOM:
                f->ROI->resize (0,0,1+f->drag_dy*zoom_factor,f);
                rendermanagement.move(rendererIndex,0,0,0,1+f->drag_dy*zoom_factor);

                //zooming invalidates ROI
                FLTK2Dregionofinterest::current_ROI = NULL;

                f->needs_re_rendering=true;
                break;

            case CB_ACTION_HOVER:
                {
                update_fbstring(f);
                }
            break;

            case CB_ACTION_WHEEL_FLIP:
                rendermanagement.move(rendererIndex,0,0,f->wheel_y*wheel_factor);    //relative coordinates are designed so that
                //1 = one z voxel step for z pan

                f->ROI->attach_histograms(rendererIndex);

                f->needs_re_rendering=true;

                update_fbstring(f);
                break;
            }
        }

        if (FLTK2Dregionofinterest::current_ROI == f->ROI && (f->callback_action == CB_ACTION_DRAG_PASS ) ||f->callback_action==CB_ACTION_WHEEL_FLIP || f->callback_action==CB_ACTION_DRAG_FLIP )
            {
            //each drag iteration or when moving in view Z direction:
            //convert coordinates for region of interest and make widgets update

            vector<FLTKuserIOpar_histogram2D *>::iterator itr =f->ROI->histograms.begin();  
            while (itr != f->ROI->histograms.end())
                {
                int one_vol_ID= (*itr)->histogram_volume_ID(0);     //assumption: same voxel size, dimensions, orientation etc.
                // - voxel coordinates for one apply to the other as well

                regionofinterest reg;
                reg.start = rendermanagement.get_location (rendererIndex,one_vol_ID,f->ROI->region_start_x,f->ROI->region_start_y,f->w(),f->h());
                reg.size = rendermanagement.get_location (rendererIndex,one_vol_ID,f->ROI->region_end_x,f->ROI->region_end_y,f->w(),f->h())-reg.start;
                //remove sign from size
                for (int d=0; d < 3 ; d++)
                    {reg.size[d]=fabs(reg.size[d]);}

                //sista steget; skicka det nya området till histogrammet
                (*itr)->highlight_ROI (&reg);

                itr++;
                }
            }

        if (rendererIndex>=0 && f->needs_re_rendering)
            {
            rendermanagement.render(rendererIndex, rgbpixmap, rgbpixmapwidth, rgbpixmapheight);
            if (viewport_widget->thresholder !=NULL)
                {viewport_widget->thresholder->render();}
            f->needs_re_rendering = false;
            }


int actionValue = f->callback_action;
        switch (actionValue)
            {
            case CB_ACTION_CLICK_PASS:
            case CB_ACTION_HOVER:
            case CB_ACTION_RESIZE:
            
            case CB_ACTION_DRAG_FLIP:

                f->damage(FL_DAMAGE_ALL);
                break;

            case CB_ACTION_WHEEL_ZOOM:
            case CB_ACTION_DRAG_ZOOM:
            case CB_ACTION_DRAG_PAN:
            case CB_ACTION_WHEEL_FLIP:
            case CB_ACTION_DRAG_PASS:
            case CB_ACTION_DRAW:

                f->damage(FL_DAMAGE_ALL);
                f->draw(rgbpixmap);
                f->damage(0);
                break;
            }
    }

    int viewport::get_id()
        {
        return ID;
        }

    int viewport::get_renderer_id()
        {
        return rendererID;
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
                cout << "RGBpixmap set to NULL "  << endl; 
                }
            }

        if (des_width <=0 || des_height <=0)
            {
            rgbpixmapwidth = rgbpixmapheight = 0;
            }
        else
            {
            rgbpixmapwidth = des_width;
            rgbpixmapheight = des_height;
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

        //the containerwidget is the full viewport area: image + controls
        Fl_Group *containerwidget;
        containerwidget = new Fl_Group(xpos,ypos,width,height);

        ////buttons controlling view & rendering
        //
        viewport_buttons = new Fl_Pack(xpos,ypos,width,buttonheight,"");
        viewport_buttons->type(FL_HORIZONTAL);

        volumemenu_button = new Fl_Menu_Button(xpos+(buttonleft+=buttonwidth),ypos,buttonwidth,buttonheight,"Volumes");

        renderermenu_button = new Fl_Menu_Button(xpos+(buttonleft+=buttonwidth),ypos,buttonwidth,buttonheight,"Renderer");
        renderermenu_button->copy(renderermenu_global);
        renderermenu_button->user_data(NULL);

        //with only one renderer available, this menu is sort of superfluous
        renderermenu_button->hide();

        //direction menu is constant for each viewport
        Fl_Menu_Item dir_menu_items [6+1];

        int m;
        for (m=0;m<6;m++)
            {
            menu_callback_params * cbp=new menu_callback_params;
            cbp->direction=(preset_direction)m;
            cbp->vport=this;

            init_fl_menu_item(&dir_menu_items[m]);

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

            init_fl_menu_item(&blend_menu_items[m]);

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

        volumemenu_button->box(FL_THIN_UP_BOX);
        volumemenu_button->labelsize(FLTK_SMALL_LABEL);

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

    void viewport::update_volume_menu()
        {
        int m=0;//

        const Fl_Menu_Item * cur_menu, *base_menu;

        Fl_Menu_Item new_menu[datamanager::MAXVOLUMES+1];

        base_menu=datamanagement.FLTK_volume_menu_items();
        cur_menu=volumemenu_button->menu();

        if (cur_menu!=NULL)
            {
            //delete old callback data
            for(unsigned int i=0;cur_menu[i].label()!=NULL && i < datamanager::MAXVOLUMES;i++)
                {
                delete ((menu_callback_params*)cur_menu[i].user_data());
                }
            }

        if (rendererIndex >= 0)
            {
            do 
                {            
                if (new_menu[m].label()!=NULL)
                    {
                    //long v=base_menu[m].argument();
                    //attach menu_callback_params and
                    //set checkmarks according to displayed volumes

                    memcpy (&new_menu[m],&base_menu[m],sizeof(Fl_Menu_Item));

                    menu_callback_params * p= new menu_callback_params;
                    p->rend_index=rendererIndex;
                    p->vol_id=base_menu[m].argument();  //volume ID is stored in user data initially

                    new_menu[m].callback((Fl_Callback *)toggle_volume_callback);
                    new_menu[m].user_data(p);
                    new_menu[m].flags=FL_MENU_TOGGLE;

                    if (rendermanagement.volume_rendered(rendererIndex,p->vol_id) !=BLEND_NORENDER)
                        {
                        //checked
                        new_menu[m].set();
                        }
                    }
                } while (new_menu[m++].label() !=NULL && m < datamanager::MAXVOLUMES);

                volumemenu_button->copy(new_menu);
            }
        }

    void viewport::toggle_volume_callback(Fl_Widget *callingwidget, void * params )
        {
        menu_callback_params * widget_user_data=(menu_callback_params *)params;

        rendermanagement.toggle_volume(widget_user_data->rend_index,widget_user_data->vol_id);
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
            //Fl_Menu_Item *m = (Fl_Menu_Item*)&(menubar->menu()[t]);

            if (rendererIndex >= 0)
                {
                int this_renderer_type=rendermanagement.get_renderer_type(rendererIndex);

                renderermenu[this_renderer_type].setonly();
                }

            //choice doesn't do anything right now, so menu should be disabled
            for (int m=0; m < NUM_RENDERER_TYPES ; m++)
                {(renderermenu[m]).deactivate();}
            }
        }

    void viewport::rebuild_blendmode_menu ()//update checkmark for current blend mode
        {
        if (blendmenu_button != NULL)
            {
            Fl_Menu_Item * blendmodemenu=(Fl_Menu_Item *)blendmenu_button->menu();

            //blend modes are different - or not available - for other renderer types
            //basic check for this:
            int this_renderer_type=rendermanagement.get_renderer_type(rendererIndex);

            for (int m=0; m < NUM_BLEND_MODES ; m++)
                {
                ((menu_callback_params *)(blendmodemenu[m].argument()))->rend_index=rendererIndex;

                if (rendererIndex <0 || this_renderer_type != RENDERER_MPR)
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