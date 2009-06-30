//$Id: $

#include "histo2D_tool.h"

#include <algorithm>
#include <FL/fl_draw.H>
#include <FL/Fl_Widget.H>

#include "global.h"

#include "threshold.h"
#include "userIOmanager.h"
#include "viewmanager.h"
#include "rendermanager.h"

extern viewmanager viewmanagement;
extern userIOmanager userIOmanagement;
extern rendermanager rendermanagement;

// *** histo2D_tool ***

histo2D_tool::histo2D_tool(viewport_event &event,thresholdparvalue * v,viewport * vp, renderer_base * r):nav_tool(event) 
{
    myPort = vp;
    myRenderer = r;

    event.ungrab(); //leave judgment of whether event should be grabbed up to this class
    
    ROI = NULL;
    overlay = NULL;
    
    if (v != NULL && vp != NULL && r !=NULL)
        {
        event.grab();
        
        //const int * mouse = event.mouse_pos_global();
        
        overlay=new threshold_overlay(event.get_FLTK_Event_pane(),rendermanagement.find_renderer_index( myPort->get_renderer_id()));
        ROI = new FLTK2Dregionofinterest(event.get_FLTK_Event_pane());
        }
    }

const std::string histo2D_tool::name()
{
    return "Histogram highlight";
}

void histo2D_tool::init()
{}

void histo2D_tool::handle(viewport_event &event)
{
    //forward zoom and flip events to nav_tool
    if (event.type() == pt_event::browse || event.type() == pt_event::scroll)
        {
        nav_tool::handle(event);
        }

    const int * mouse = event.mouse_pos_global();

    if (event.state() == pt_event::begin  ) 
        {
        last_global_x = mouse[0];
        last_global_y = mouse[1];
        }

    FLTK_Event_pane *fp = event.get_FLTK_Event_pane();

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

                            ROI->drag(this->last_global_x,this->last_global_y,mouse[0]-this->last_global_x,mouse[1]-this->last_global_y,fp);
                            }
                        fp->damage(FL_DAMAGE_ALL);
                        }
                    }
            break;
            
        case pt_event::browse:
            {
                event.grab();
                ROI->resize (mouse[0]-this->last_global_x,mouse[1]-this->last_global_y,1,fp);
            }
            break;
            
        case pt_event::create:
            event.grab();
            
            ROI->resize (0,0,1+this->last_global_y*zoom_factor,fp);
                        
            //zooming invalidates ROI
            FLTK2Dregionofinterest::current_ROI = NULL;
            
            break;
            
        case pt_event::scroll:     
            event.grab();
            
            ROI->attach_histograms(rendermanagement.find_renderer_index( myPort->get_renderer_id()));
            
            fp->needs_rerendering();
            break;
            
        case pt_event::draw:
            event.grab();
            
            overlay->render();
            overlay->FLTK_draw(); //overlay has fp associated earlier, where drawing is done

            fl_color(fl_rgb_color(0, 255, 255));
            ROI->draw(fp);
            break;
            
        case pt_event::resize:
            event.grab();
            
            overlay->resize();
            //ROI->resize (0,0,1,fp); //"clear" hack ROI
            ROI->resize (event);

            break;
    }

    if (event.state() == pt_event::end && ROI->dragging)
        {
        event.grab();
        ROI->drag_end();
        fp->damage(FL_DAMAGE_ALL);
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
            reg.start = rendermanagement.get_location (rendermanagement.find_renderer_index( myPort->get_renderer_id()),one_vol_ID,ROI->region_start_x,ROI->region_start_y,fp->w(),fp->h());
            reg.size = rendermanagement.get_location (rendermanagement.find_renderer_index( myPort->get_renderer_id()),one_vol_ID,ROI->region_end_x,ROI->region_end_y,fp->w(),fp->h())-reg.start;
            //remove sign from size
            for (int d=0; d < 3 ; d++)
                {reg.size[d]=fabs(reg.size[d]);}
            
            //sista steget; skicka det nya omrÂdet till histogrammet
            (*itr)->highlight_ROI (&reg);
            
            itr++;
            }
        }

    if (event.state() == pt_event::iterate)
        {
        last_global_x = mouse[0];
        last_global_y = mouse[1];
        }
}

void histo2D_tool::attach (viewport *vp, renderer_base * r)
{
    //myWidget = fp;
    myPort = vp;
    myRenderer = r;
    overlay->renderer_index(rendermanagement.find_renderer_index( myPort->get_renderer_id()));
}

// *** FLTK2Dregionofinterest ***

FLTK2Dregionofinterest * FLTK2Dregionofinterest::current_ROI=NULL;

FLTK2Dregionofinterest::FLTK2Dregionofinterest (Fl_Widget * frame)
{
    dragging=false;
    
    mode=ROI_RECT;
    
    region_start_x=0;
    region_start_y=0;
    region_end_x=0;
    region_end_y=0;
    
    canvas_size_x=frame->w();
    canvas_size_y=frame->h();
}

void FLTK2Dregionofinterest::draw(Fl_Widget * canvas)
{
    //Fl_Group canvas = Fl_Group::current();
    
    if (current_ROI == this)
        {
        switch (mode)
            {
            case ROI_RECT:
                fl_rect(canvas->x()+region_start_x, canvas->y()+region_start_y, region_end_x-region_start_x, region_end_y-region_start_y);
                break;
            case ROI_CIRCLE:
                fl_arc(canvas->x()+region_start_x, canvas->y()+region_start_y, region_end_x-region_start_x, region_end_y-region_start_y, 360);
                break;
            }
        }
}

void FLTK2Dregionofinterest::drag (int x_last, int y_last, int dx, int dy,Fl_Widget * frame)
{
    //const int grab_margin= 2;
    
    //drag signals that this is the active ROI
    current_ROI=this;
    
    switch (mode)
        {
        case ROI_RECT:
            if (!dragging && x_last-frame->x() >= region_start_x && x_last-frame->x() <= region_end_x &&
                y_last-frame->y() >= region_start_y && y_last-frame->y() <= region_end_y )
                {
                //inside region, drag around
                region_start_x+=dx;
                region_start_y+=dy;
                region_end_x+=dx;
                region_end_y+=dy;
                }
            else
                {
                int point_x=x_last-frame->x();
                int point_y=y_last-frame->y();
                
                //outside, start dragging a new region
                if (dragging==false)
                    {
                    drag_root_x=point_x;
                    drag_root_y=point_y;
                    
                    dragging=true;
                    }
                
                region_start_x=drag_root_x;
                region_start_y=drag_root_y;
                region_end_x=dx+point_x;
                region_end_y=dy+point_y;
                
                //ensure region has positive dimension
                if (region_end_x < region_start_x)
                    {t_swap (region_end_x,region_start_x);}
                if (region_end_y < region_start_y)
                    {t_swap (region_end_y,region_start_y);}
                }
            break;
            
        case ROI_CIRCLE:
            //nothing here
            break;
        }
}

void FLTK2Dregionofinterest::drag_end()
{
    dragging=false;
    histograms.clear();
}

void FLTK2Dregionofinterest::attach_histograms (int rendererIndex)
{
    //first drag iteration; a number of values have to be collected and stored
    histograms.clear();
        
    rendercombination::iterator itr = (rendermanagement.get_combination( rendermanagement.find_renderer_id(rendererIndex)))->begin();
    rendercombination::iterator end = (rendermanagement.get_combination( rendermanagement.find_renderer_id(rendererIndex)))->end();
    
    while (itr != end)
        {
        std::vector<FLTKuserIOpar_histogram2D *>  found;
        found=userIOmanagement.get_histogram_for_image(itr->ID);
        std::vector<FLTKuserIOpar_histogram2D *>::iterator fitr =found.begin();
        
        while (fitr != found.end())
            {
            if (std::find(histograms.begin(),histograms.end(),(*fitr)) == histograms.end())
                //check for duplicate
                {histograms.push_back((*fitr));}
            fitr++;
            }
        
        itr++;
        //rendered_vol_ID=rendermanagement.image_at_priority (rendererIndex,p);
        }
}

void FLTK2Dregionofinterest::resize (int dx, int dy, float zoom,Fl_Widget * frame)
{
    if (frame->w()!= canvas_size_x || frame->h()!= canvas_size_y || zoom != 1)
        {
        //zooming or resizing has to be done through rendergeometry, reset region
        region_start_x=region_start_y=region_end_x=region_end_y=0;
        }
    
    region_start_x+=dx;
    region_start_y+=dy;
    region_end_x+=dx;
    region_end_y+=dy;
    
    canvas_size_x=frame->w();
    canvas_size_y=frame->h();
}

void FLTK2Dregionofinterest::resize (viewport_event &e)
{    
    e.resize_point (region_start_x,region_start_y);
    e.resize_point (region_end_x,region_end_y);
    
    canvas_size_x=e.get_FLTK_Event_pane()->w();
    canvas_size_y=e.get_FLTK_Event_pane()->h();
}
