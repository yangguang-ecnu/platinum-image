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

#include <iostream>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Tile.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_File_Chooser.H>

#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_Widget.H>

#include <FL/fl_draw.H>

#include <FL/Fl_Scroll.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Light_Button.H>

#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Group.H>

#include "global.h"

#include "viewmanager.h"
#include "rendermanager.h"
#include "userIOmanager.h"

extern rendermanager rendermanagement;
extern userIOmanager userIOmanagement;
viewmanager viewmanagement;

void viewmanager::listviewports()
{
std::cout << "# of viewports: " << viewports.size() << std::endl;
for (unsigned int i=0; i < viewports.size(); i++) { std::cout << viewports[i] << std::endl; } 
}

int viewmanager::create_viewport()
    {
    //viewport *aviewport;
    //aviewport = new viewport;
    viewports.push_back(viewport());

    return viewports.back().get_id(); //return ID of the viewport just created
    }

void create_viewport(char namn[])
    {
    //... int identifierare ... ger unik identifierare Ât varje objekt?

    //viewports.assign(...); 
    }

void delete_viewport(int viewportID) 
    {
    //iterate through viewportmapping. all ID equal to param are set to  -1
    //viewports.delete(ID);
    }

/*void set_view_at_viewport(int virtualview, int horizontal, int vertical, int viewportID)
{}*/

//// this contains the setup information - should be loaded/stored, changed by interacting w. widgets, ...
//

void viewmanager::setup_demo_tiles()
    {
    irregular_tiles = true;
    
    const int demonoofhorizontaltiles[MAXVIRTUALVIEWS] = { 2,0,0 };
    for (int vv=0; vv < MAXVIRTUALVIEWS; vv++) { noofhorizontaltiles[vv] = demonoofhorizontaltiles[vv]; }

const int demonoofverticaltiles[MAXVIRTUALVIEWS][MAXHORIZONTALGRID] = { { 2,2,0,0  } };
for (int vv=0; vv < MAXVIRTUALVIEWS; vv++)
for (int h=0; h < MAXHORIZONTALGRID ; h++)
    { noofverticaltiles[vv][h] = demonoofverticaltiles[vv][h];  }

const float demotilewidthpercent[MAXVIRTUALVIEWS][MAXHORIZONTALGRID] = { { 0.5, 0.5, 0.0, 0.0 } }; //last will be the remainder; i.e. not used
for (int vv=0; vv < MAXVIRTUALVIEWS; vv++)
for (int h=0; h < MAXHORIZONTALGRID ; h++)
    { tilewidthpercent[vv][h] =  demotilewidthpercent[vv][h]; }

const float demotileheightpercent[MAXVIRTUALVIEWS][MAXHORIZONTALGRID][MAXVERTICALGRID] = { { { 0.5, 0.0, 0.0, 0.0 }, { 0.5, 0.0, 0.0, 0.0 }, { 0.5, 0.0, 0.0, 0.0 }, { 0.3, 0.1, 0.3, 0.1 }, { 0.2, 0.5, 0.1, 0.1 } } }; // last will be the remainder; i.e. not used
for (int vv=0; vv < MAXVIRTUALVIEWS; vv++)
for (int h=0; h < MAXHORIZONTALGRID ; h++)
for (int v=0; v < MAXVERTICALGRID; v++)
    { tileheightpercent[vv][h][v] = demotileheightpercent[vv][h][v]; }
    }

void viewmanager::setup_regular_tiles(int t_h, int t_v, int t_vv)
    {
    //basic tile setup with even grid spacing
    
    //range checking
    if (t_h > MAXHORIZONTALGRID)
        {
#ifdef _DEBUG
        std::cout << "Error in setup_regular_tiles - requested # of horizontal tiles (" << t_h << ") larger than MAXHORIZONTALGRID (" << MAXHORIZONTALGRID << ")"  << std::endl;
#endif
        t_h = 1;
        }
    
    if (t_v > MAXVERTICALGRID)
        {
#ifdef _DEBUG
        std::cout << "Error in setup_regular_tiles - requested # of vertical tiles (" << t_v << ") larger than MAXVERTICALGRID (" << MAXVERTICALGRID << ")"  << std::endl;
#endif
        t_v = 1;
        }
    
    if (t_vv > MAXVIRTUALVIEWS)
        {
#ifdef _DEBUG
        std::cout << "Error in setup_regular_tiles - requested # of virtual views (" << t_vv << ") larger than MAXVIRTUALVIEWS (" << MAXVIRTUALVIEWS << ")"  << std::endl;
#endif
        t_vv = 1;
        }

    
    irregular_tiles = false;
    
    for (int vv= 0; vv < t_vv; vv++)
        {
        noofhorizontaltiles [vv] = t_h;
        
        for (int h = 0; h < t_h; h++ )
            {
            noofverticaltiles [vv][h] = t_v;
            tilewidthpercent[vv][h] = 1.0/t_h;
            
            for (int v = 0; v < t_v; v++ )
                {
                tileheightpercent[vv][h][v] = 1.0/t_v; 
                }
            }
        }
    }


// Vilken av de N vyerna ska vi visa (vi kan bara titta pÂ en Ât gÂngen)?
//
void viewmanager::setup_views(int virtualview, int areawidth, int areaheight) // 0...antal som anv‰nds-1
    {
    // storage for all tiles et.c. generated in the loop (not really needed)
    Fl_Widget *boxes[MAXHORIZONTALGRID*MAXVERTICALGRID];
    int vtilesused = 0;
    int boxesused = 0;

    ////
    // this sets up a full virtual view in the CURRENT window. We prepared ...
    // Fl_Window window(areawidth,areaheight) ...[you are here]... window.end(); window.show(argc,argv);
    // ...  outside this call
    //Fl_Tile *htile = new Fl_Tile(0,0,areawidth,areaheight);
    
    Fl_Tile *verticaltiles[MAXHORIZONTALGRID];

    float tilewidthpercentsum = 0;
    for (int h=0; h < noofhorizontaltiles[virtualview]; h++)
        {
        int tilewidth = int(tilewidthpercent[virtualview][h]*areawidth);
        if (h==noofhorizontaltiles[virtualview]-1)
            { tilewidth=areawidth-int(tilewidthpercentsum*areawidth); } // avoid rounding errors
        if (irregular_tiles)
            {verticaltiles[vtilesused] = new Fl_Tile(int(tilewidthpercentsum*areawidth), 0,tilewidth ,areaheight);}
        float tileheightpercentsum = 0;
        for (int v=0; v < noofverticaltiles[virtualview][h]; v++)
            {
            int tileheight = int(tileheightpercent[virtualview][h][v]*areaheight);
            if (v==noofverticaltiles[virtualview][h]-1)
                { tileheight = areaheight - int(tileheightpercentsum*areaheight); }
            if (viewportmapping[v][h][virtualview] >= 0)
                {
                int VPind = find_viewport_index( viewportmapping[v][h][virtualview] );
                //Create viewport widget and set position
                viewports[ VPind ].initialize_viewport(int(tilewidthpercentsum*areawidth),int(tileheightpercentsum*areaheight),tilewidth,tileheight);
                }
            else
                {
                //placeholder
                boxes[boxesused] = new Fl_Button(int(tilewidthpercentsum*areawidth),int(tileheightpercentsum*areaheight),tilewidth,tileheight,"placeholder");
                boxes[boxesused]->box(FL_DOWN_BOX);
                boxes[boxesused]->align(FL_ALIGN_CLIP);
                boxes[boxesused]++;
                }
            tileheightpercentsum += tileheightpercent[virtualview][h][v];
            }
                if (irregular_tiles)
                    {verticaltiles[vtilesused]->end();}
        vtilesused++;
        tilewidthpercentsum += tilewidthpercent[virtualview][h];
        }

    //htile->box(FL_BORDER_FRAME);
    //htile->end();

    //make htile (outer group for viewports) the resizable portion of the window
    //htile->parent()->resizable(htile);
    }

void viewmanager::erase_all_connections()
    {
    //// s‰tt upp kopplingsarrayen mellan de virtuella vyerna (varav en ‰r synlig) och de viewport:s som finns
    //
    for (int vv=0; vv < MAXVIRTUALVIEWS; vv++) // samma mˆnster fˆr alla virtuella vyer - distribuera viewports
        {
        for (int v=0; v < MAXVERTICALGRID; v++)
            for (int h=0; h < MAXHORIZONTALGRID; h++)
                {
                viewportmapping[v][h][vv] = -1;
                }
        }
    }

int viewmanager::find_viewport_index(int id) // Goes from ID to INDEX returns NOT_FOUND_ID if none found
    {
    for (unsigned int i=0; i < viewports.size(); i++) 
        {
        if (viewports[i] == id) { return i; break; } // should break x3 but too lazy now...
        } 
    return NOT_FOUND_ID;
    }

//// v h vv in the viewarray -> viewport index
//
int viewmanager::get_viewport_id(int v, int h, int virtualview)
    {
    return viewportmapping[v][h][virtualview];
    }

//// return unique ID
//
int viewmanager::get_renderer_id(int viewportid)
    {
    int vpindex = find_viewport_index(viewportid);
    return viewports[vpindex].get_renderer_id();
    }

int viewmanager::find_viewport_not_in_view()
    {
    int unused = NOT_FOUND_ID;
    for (unsigned int i=0; i < viewports.size(); i++) 
        {
        int alreadyinuse = 0;
        for (int vv=0; vv < MAXVIRTUALVIEWS; vv++) // samma mˆnster fˆr alla virtuella vyer - distribuera viewports
            {
            for (int v=0; v < MAXVERTICALGRID; v++)
                for (int h=0; h < MAXHORIZONTALGRID; h++)
                    {
                    if (viewports[i] == viewportmapping[v][h][vv]) { alreadyinuse = 1; break; }
                    // should break x3 but too lazy now...
                    }
            }
        if (alreadyinuse == 0)
            {
            unused = viewports[i].get_id();
            //    std::cout << "Found viewport " << unused << std::endl; 
            break;
            } // found one
        } 
    return unused;
    }

int viewmanager::find_viewport_no_renderer()
    {
    // returns -1 if none found

    int free_vp_id = NOT_FOUND_ID;
    for (unsigned int i=0; i < viewports.size(); i++) 
        {
        if (viewports[i].get_renderer_id()==NO_RENDERER_ID)
            {
            //found viewport without renderer
            free_vp_id = viewports[i].get_id();
            }
        }
    return free_vp_id;
    }

int viewmanager::find_viewport_no_images () 
    {
    // returns -1 if none found

    std::vector<viewport>::iterator itr=viewports.begin();
    int empty_vp_id = NOT_FOUND_ID;

    while (itr != viewports.end())
        {
        if (rendermanagement.renderer_empty((*itr).get_renderer_id()) == RENDERER_EMPTY) //null if threshold parameter does not match any rendered image
            {return (*itr).get_id();}
        itr++;
        }
    return empty_vp_id;
    }


void viewmanager::connect_views_viewports()
    {
    //int antalvp = viewports.size();

    //set up connection between virtual views and viewports
    
    for (int vv=0; vv < MAXVIRTUALVIEWS; vv++) // same pattern for each virtual view
        {
        for (int h=0; h < noofhorizontaltiles[vv] ; h++) // no use to waste views outside the visible space
            {
            for (int v=0; v < noofverticaltiles[vv][h]; v++) // no use to waste views outside the visible space
                {
                int nyvp = find_viewport_not_in_view();
                // std::cout << nyvp << std::endl;
                if (nyvp != NOT_FOUND_ID) { viewportmapping[v][h][vv] = nyvp; }
                else  { viewportmapping[v][h][vv] = NOT_FOUND_ID; }
                }
            }
        }
    }

void viewmanager::connect_renderer_to_viewport(int viewportID, int rendererID)
    {
    int vpindex = find_viewport_index(viewportID);
    viewports[vpindex].connect_renderer(rendererID);
    //new renderer, refresh viewport!
    }

void viewmanager::list_connections()
    {
    //int antalvp = viewports.size();

    std::cout << "Listing of viewport to view connections:" << std::endl;
    for (int vv=0; vv < MAXVIRTUALVIEWS; vv++) // samma mˆnster fˆr alla virtuella vyer - distribuera viewports
        {
        std::cout << "Virtual view " << vv << std::endl;
        for (int v=0; v < MAXVERTICALGRID; v++)
            {
            for (int h=0; h < MAXHORIZONTALGRID; h++)
                {
                int VPind = find_viewport_index( viewportmapping[v][h][vv] );
                std::cout << viewportmapping[v][h][vv] << "(index " << VPind << ") ";
                }
            std::cout << std::endl;
            }
        }
    }

void viewmanager::refresh_viewports_after_toolswitch()
    {
    for (std::vector<viewport>::iterator itr=viewports.begin(); itr != viewports.end(); itr++) 
        {
        (*itr).refresh_after_toolswitch();
        }
    }

void viewmanager::refresh_viewports_from_combination(int c)
    {
    for (unsigned int i=0; i < viewports.size(); i++) 
        {
        viewports[i].refresh_from_combination(c);
        }
    }

void viewmanager::refresh_viewports_from_geometry(int g)
    {
    for (unsigned int i=0; i < viewports.size(); i++) 
        {
        viewports[i].refresh_from_geometry(g);
        }
    }

void viewmanager::refresh_viewports()
    {
    for (unsigned int i=0; i < viewports.size(); i++) 
        {
        viewports[i].refresh();
        }
    }

std::vector<threshold_overlay *> viewmanager::get_overlays (thresholdparvalue * threshold)
    {
    std::vector<viewport>::iterator itr=viewports.begin();
    std::vector<threshold_overlay *> result;

    userIOmanagement.select_tool ("Histogram highlight");
    
    while (itr != viewports.end())
        {
        threshold_overlay * olay;

        olay=(*itr).get_threshold_overlay (threshold);
        if (olay != NULL) //null if threshold parameter does not match any rendered image
            {result.push_back(olay);}
        itr++;
        }
    return result;
    }
	


