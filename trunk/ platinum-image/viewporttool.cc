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
extern viewmanager viewmanagement;
#include "rendermanager.h"
extern rendermanager rendermanagement;

#include <FL/Fl_Window.h>
#include <FL/Fl_Button.h>

// *** base class ***

// static members
Fl_Window * viewporttool::toolbox = NULL;
Fl_Pack * viewporttool::statusArea = NULL;
std::string viewporttool::selected = "";
std::map<std::string, viewporttool::vpt_create_pointer> viewporttool::tools = std::map<std::string, viewporttool::vpt_create_pointer>  ();

template <class TOOL>
void viewporttool::Register (std::string k)
{
    tools[k]=&CreateObject<TOOL>;
}

template <class TOOL>
viewporttool * viewporttool::CreateObject(viewport_event &event)
{
    return new TOOL (event);
}

void viewporttool::init (Fl_Pack * s)
{
    statusArea = s;
    Fl_Group::current(statusArea);
    
    //register tool classes
    
    viewporttool::Register<nav_tool>("Navigation tool");
    viewporttool::Register<dummy_tool>("Dummy tool");
    viewporttool::Register<histogram_tool>("Histogram highlight");
    
    selected = "Navigation tool";
    
    //create toolbox widget
        
    toolbox = new Fl_Window (0,statusArea->y(),0,statusArea->h()); //toolfactory.buttons will set correct width
    const bool horizontal = true;
    int buttonSize  = horizontal? toolbox->h():toolbox->w();
    int x = 0; //toolbox is a subwindow, so it starts at 0
    int y = 0;
    
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
            { x += buttonSize; }
        else
            { y += buttonSize; }
        }
    
    toolbox->size(x,y);
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

    viewmanagement.refresh_viewports_after_toolswitch();    
}

viewporttool * viewporttool::taste(viewport_event & event,viewport * vp,renderer_base * r)
{
    /*viewporttool * result = NULL;
    
    for (std::map<std::string, taste_fcn_pointer>::iterator i = tools.begin();i != tools.end();i++)
        {
        result = i->second(event);
        }
	return result;*/
    
    
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
    
    select (*key);
}

// *** tool classes ***
//registered in viewporttool::init

// *** navigation tool ***

const float nav_tool::wheel_factor=0.02;
const float nav_tool::zoom_factor=0.01;

nav_tool::nav_tool (viewport_event & event):viewporttool(event)
    {
    //a tool constructor has to respond to the type of events it accepts by grabbing them,
    //or it won't be created
    if (event.type() == pt_event::hover || event.type() == pt_event::browse || event.type() == pt_event::adjust || event.type() == pt_event::scroll)
        {event.grab();}
    }

void nav_tool::handle(viewport_event &event)
{
    if ( event.state() == pt_event::begin)
        {
        event.grab();
        
        dragLast[0] = event.mouse_pos()[0];
        dragLast[1] = event.mouse_pos()[1];
        }
    
    if (event.state() == pt_event::iterate)
        {
        const int * pms = myPort->pixmap_size();
        const float pan_factor=(float)1/(std::min(pms[0],pms[1]));
        const int * mouse = event.mouse_pos();
        
        FLTKviewport * fvp = event.get_FLTK_viewport();
        switch (event.type())
            {
            case pt_event::browse:
                event.grab();
                
                myRenderer->move(-(mouse[0]-dragLast[0])*pan_factor,-(mouse[1]-dragLast[1])*pan_factor);
                
                fvp->needs_rerendering();
                
                break;
                
            case pt_event::adjust:
                {
                    event.grab();
                    
                    myRenderer->move(0,0,0,1+(mouse[1]-dragLast[1])*zoom_factor);
                    
                    fvp->needs_rerendering();
                }
                break;
                
                /*case hover:
                {
                    event.grab();
                    
                    update_fbstring(f);
                }
                break;*/
                
            case pt_event::scroll:
                event.grab();
                
                myRenderer->move(0,0,event.scroll_delta()*wheel_factor);
                
                fvp->needs_rerendering();
                break;
            }
        
        dragLast[0] = mouse[0];
        dragLast[1] = mouse[1];
        }
}

/*viewporttool * nav_tool::taste_ (viewport_event & event)
    {
    nav_tool * t = new nav_tool(event);
    if (event.type
        
        
    return t;
    }*/

// *** dummy tool ***

dummy_tool::dummy_tool (viewport_event &event):viewporttool(event)
    {

    }

void dummy_tool::handle(viewport_event &event)
{}

// *** histogram_tool ***

histogram_tool::histogram_tool(viewport_event &event,thresholdparvalue * v,viewport * vp, renderer_base * r):nav_tool(event) 
{
    myPort = vp;
    myRenderer = r;

    event.ungrab(); //leave judgment of whether event should be grabbed up to this class
    
    ROI = NULL;
    overlay = NULL;
    
    if (v != NULL && vp != NULL && r !=NULL)
        {
        event.grab();
        
        //const int * mouse = event.mouse_pos();
        
        overlay=new threshold_overlay(event.get_FLTK_viewport(),rendermanagement.find_renderer_index( myPort->get_renderer_id()));
        ROI = new FLTK2Dregionofinterest(event.get_FLTK_viewport());
        }
     /*   
    FLTKviewport * fvp = event.get_FLTK_viewport();

    if (myRenderer != NULL)
        {
        int p=0;
        int rendered_vol_ID=myRenderer->imagestorender->image_ID_by_priority(p);
        
        while (rendered_vol_ID > 0)
            {
            for (int d=0;v->id[d] != NOT_FOUND_ID ;d++)
                {
                if (rendered_vol_ID==v->id[d])
                    {
                    if (overlay == NULL)
                        {overlay=new threshold_overlay(fvp,rendermanagement.find_renderer_index( myPort->get_renderer_id()));}
                    }
                }
            p++;
            rendered_vol_ID=myRenderer->imagestorender->image_ID_by_priority(p);
            }
        
        if (overlay != NULL)
            {
            overlay->expire();
            fvp->redraw();
            }
        }
    
    if (event.type() == pt_event::adjust && event.state() == pt_event::iterate)
        {
        if (myPort->get_renderer_id() != NO_RENDERER_ID)
            {
            if (!ROI->dragging)
                {
                //to improve performance, the attached histograms are cached during drag
                ROI->attach_histograms((fvp,rendermanagement.find_renderer_index( myPort->get_renderer_id())));
                }
            
            if (ROI->histograms.size() >0 )  //only ROI yourself if there is a suitable histogram around
                {
                if(FLTK2Dregionofinterest::current_ROI != ROI)
                    {
                    viewmanagement.refresh_viewports(); //erase ROIs shown in other viewports
                    FLTK2Dregionofinterest::current_ROI = ROI;
                    }
                
                ROI->drag(mouse[0],mouse[1],mouse[0]-dragLast[0],mouse[1]-dragLast[1],fvp);
                }
            }
        }*/
}

void histogram_tool::handle(viewport_event &event)
{
    //forward zoom and flip events to nav_tool
    if (event.type() == pt_event::browse || event.type() == pt_event::scroll)
        {
        nav_tool::handle(event);
        }

    const int * mouse = event.mouse_pos();

    if (event.state() == pt_event::begin  ) 
        {
        dragLast[0] = mouse[0];
        dragLast[1] = mouse[1];
        }

    FLTKviewport * fvp = event.get_FLTK_viewport();

    switch (event.type()) {
            case pt_event::adjust:
                //this callback is for general click & drag in viewport
                //however only function available yet is the histogram ROI

                if (myPort->get_renderer_id() != NO_RENDERER_ID)
                    {
                    event.grab();
                    if (event.state() == pt_event::begin  ) {
                            if (!ROI->dragging)
                                {
                                //to improve performance, the attached histograms are cached during drag
                                ROI->attach_histograms(rendermanagement.find_renderer_index( myPort->get_renderer_id()));
                                }
                            }

                    if (event.state() == pt_event::begin || event.state() == pt_event::iterate )
                        {
                        if (ROI->histograms.size() >0 )  //only ROI yourself if there is a suitable histogram around
                            {
                            if(FLTK2Dregionofinterest::current_ROI != ROI)
                                {
                                viewmanagement.refresh_viewports(); //erase ROIs shown in other viewports
                                FLTK2Dregionofinterest::current_ROI = ROI;
                                }

                            ROI->drag(this->dragLast[0],this->dragLast[1],mouse[0]-this->dragLast[0],mouse[1]-this->dragLast[1],fvp);
                            }
                        fvp->damage(FL_DAMAGE_ALL);
                        }
                    }
            break;
            
        case pt_event::browse:
            {
                /*float pan_x=0;
                float pan_y=0;
                
                pan_x-=this->dragLast[0]*pan_factor; 
                pan_y-=this->dragLast[1]*pan_factor;*/
                
                event.grab();
                ROI->resize (mouse[0]-this->dragLast[0],mouse[1]-this->dragLast[1],1,fvp);
            }
            break;
            
        case pt_event::create:
            event.grab();
            
            ROI->resize (0,0,1+this->dragLast[1]*zoom_factor,fvp);
                        
            //zooming invalidates ROI
            FLTK2Dregionofinterest::current_ROI = NULL;
            
            //fvp->needs_rerendering();
            break;
            
        case pt_event::scroll:     
            event.grab();
            
            ROI->attach_histograms(rendermanagement.find_renderer_index( myPort->get_renderer_id()));
            
            fvp->needs_rerendering();
            break;
            
        case pt_event::draw:
            event.grab();
            
            overlay->render();
            overlay->FLTK_draw(); //overlay has fvp associated earlier, where drawing is done

            fl_color(fl_rgb_color(0, 255, 255));
            ROI->draw(fvp);
            break;
            
        case pt_event::resize:
            event.grab();
            
            overlay->resize();
            ROI->resize (0,0,1,fvp); //"clear" hack ROI

            break;
    }

    if (event.state() == pt_event::end && ROI->dragging)
        {
        event.grab();
        ROI->drag_end();
        fvp->damage(FL_DAMAGE_ALL);
        }
    
    if (FLTK2Dregionofinterest::current_ROI == ROI && (event.type() == pt_event::adjust ) ||event.type() ==pt_event::scroll )
        {
        event.grab();
        
        //each drag iteration or when moving in view Z direction:
        //convert coordinates for region of interest and make widgets update
        
        std::vector<FLTKuserIOpar_histogram2D *>::iterator itr =ROI->histograms.begin();  
        while (itr != ROI->histograms.end())
            {
            int one_vol_ID= (*itr)->histogram_image_ID(0);     //assumption: same voxel size, dimensions, orientation etc.
                                                               // - voxel coordinates for one apply to the other as well
            
            regionofinterest reg;
            reg.start = rendermanagement.get_location (rendermanagement.find_renderer_index( myPort->get_renderer_id()),one_vol_ID,ROI->region_start_x,ROI->region_start_y,fvp->w(),fvp->h());
            reg.size = rendermanagement.get_location (rendermanagement.find_renderer_index( myPort->get_renderer_id()),one_vol_ID,ROI->region_end_x,ROI->region_end_y,fvp->w(),fvp->h())-reg.start;
            //remove sign from size
            for (int d=0; d < 3 ; d++)
                {reg.size[d]=fabs(reg.size[d]);}
            
            //sista steget; skicka det nya området till histogrammet
            (*itr)->highlight_ROI (&reg);
            
            itr++;
            }
        }

    if (event.state() == pt_event::iterate)
        {
        dragLast[0] = mouse[0];
        dragLast[1] = mouse[1];
        }
}

void histogram_tool::attach (viewport * vp, renderer_base * r)
{
    //myWidget = fvp;
    myPort = vp;
    myRenderer = r;
    overlay->renderer_index(rendermanagement.find_renderer_index( myPort->get_renderer_id()));
}

threshold_overlay * histogram_tool::get_overlay ()
{
    return overlay;
    /*if (rendererID != NO_RENDERER_ID)
        {
        int p=0;
        int rendered_vol_ID=rendermanagement.image_at_priority (rendererIndex,p);
        
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
            rendered_vol_ID=rendermanagement.image_at_priority (rendererIndex,p);
            }
        
        if (viewport_widget->thresholder != NULL)
            {
            viewport_widget->thresholder->expire();
            viewport_widget->redraw();
            }
        }*/
}