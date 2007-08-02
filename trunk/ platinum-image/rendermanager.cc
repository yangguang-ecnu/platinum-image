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
#include <vector>
#include <algorithm>

#include "rendermanager.h"
#include "rendererMPR.h"
#include "viewmanager.h"

rendermanager rendermanagement;
extern viewmanager viewmanagement;

using namespace std;

/*void rendermanager::move(int rendererIndex, float panX, float panY, float panZ, float scale)
    {
    renderers[rendererIndex]->move_view( undefinedVariable,panX, panY, panZ, scale);
    }*/

void rendermanager::render(int rendererIndex, unsigned char *rgbimage, int rgbXsize, int rgbYsize)
    {
    renderers[rendererIndex]->render_position(rgbimage, rgbXsize, rgbYsize);
    }

void rendermanager::render_thumbnail(unsigned char *rgb, int rgb_sx, int rgb_sy, int image_ID)
    {
    //decision: MPRs are nice for thumbnails

    rendererMPR::render_thumbnail(rgb, rgb_sx, rgb_sy, image_ID);
    }

void rendermanager::render_threshold (int rendererIndex, unsigned char *rgba, int rgb_sx, int rgb_sy, thresholdparvalue * threshold)
    {
    renderers[rendererIndex]->render_threshold(rgba, rgb_sx, rgb_sy,threshold);
    }

int rendermanager::find_renderer_index(int uniqueID)
    {
    for (unsigned int i=0; i < renderers.size(); i++)
        {
        if (*renderers[i]==uniqueID) { return i;
            }
        }
    return -1; // not found
    }

int rendermanager::find_renderer_id (int index)
{
    return renderers[index]->get_id();
}

renderer_base * rendermanager::get_renderer (int ID)
{
    /*std::vector<renderer_base *>::iterator renderer = find ( renderers.begin(), renderers.end(), ID);
    
    if (renderer != renderers.end())
        { return *renderer; }*/
    
    for ( vector<renderer_base*>::iterator itr = renderers.begin();itr != renderers.end();itr++)
        {
        if (**itr==ID)
            { return *itr;}
        }
    
    return NULL;
}

vector<int> rendermanager::combinations_from_data (int dataID)
    {
    //when thumbnails are implemented, they should be re-rendered at this point

    vector<renderer_base*>::iterator itr = renderers.begin();
    vector<int> found_combinations;
    vector<int>::iterator citr;
    int index=0;

    while (itr != renderers.end())
        {
        if (image_rendered(index, dataID) != BLEND_NORENDER)
            {
            int combination = get_combination_id(index);

            if (find(found_combinations.begin(),found_combinations.end(),combination) == found_combinations.end())
                //check for duplicate
                {found_combinations.push_back(combination);}
            }
        itr++;
        index++;
        }

    return found_combinations;
    }

int rendermanager::get_combination_id(int rendererIndex)
    {
    return renderers[rendererIndex]->imagestorender->get_id();
    }

int rendermanager::get_geometry_id(int rendererIndex)
    {
    return renderers[rendererIndex]->wheretorender->get_id();
    }

void rendermanager::listrenderers()
    {
    cout << "Antal renderare: " << renderers.size() << endl;
    for (unsigned int i=0; i < renderers.size(); i++) { std::cout << *renderers[i] << std::endl; } 
    }

int rendermanager::create_renderer(RENDERER_TYPES rendertype)
    {
    renderer_base *arenderer;

    switch (rendertype)
        {
        case RENDERER_MPR:
            arenderer = new rendererMPR();
            break;
        default:
            arenderer=NULL;
            cout << "Unknown renderer. Not initialized." << endl; break;
        }
    renderers.push_back(arenderer);

    geometries.push_back(new rendergeometry());
    arenderer->connect_geometry (geometries[geometries.size()-1]);

    combinations.push_back(new rendercombination());
    arenderer->connect_combination (combinations[combinations.size()-1]);

    viewmanagement.refresh_viewports();
    return arenderer->get_id();
    }

void rendermanager::remove_renderer (int ID)
    {
    int index;

    index=find_renderer_index(ID);

    if (index >=0)
        {
        delete renderers[index];
        renderers.erase(renderers.begin()+index);
        }

    viewmanagement.refresh_viewports();
    }

void rendermanager::toggle_image (int rendererIndex, int imageID)
    {
    renderers[rendererIndex]->imagestorender->toggle_image( imageID);
    }

int rendermanager::image_rendered(int rendererIndex, int volID)
    {
    return renderers[rendererIndex]->imagestorender->image_rendered(volID);
    }

int rendermanager::renderer_empty (int rendererID)
    {
    int rendererIndex = find_renderer_index(rendererID);

    if (rendererIndex != -1)
        {return (renderers[rendererIndex]->imagestorender->empty() ? RENDERER_EMPTY : RENDERER_NOT_EMPTY);}

    return -1;
    }

int rendermanager::get_renderer_type(int rendererIndex)
    {
    return renderers[rendererIndex]->renderer_type();
    }

std::map<std::string,float> rendermanager::get_values (int index, int px, int py,int sx, int sy)
    {
    if (index != -1)
        {
        return renderers[index]->get_values_view(px, py,sx,sy);
        }

    std::map<std::string,float> m;

    return m;  //if ID is invalid
    }

Vector3D rendermanager::get_location (int rendererIndex, int imageID, int px, int py, int sx, int sy)
    {
    return renderers[rendererIndex]->view_to_voxel(px, py, sx, sy,imageID);
    }

void rendermanager::connect_data_renderer(int rendererID, int dataID)
    {
    int renderindex = find_renderer_index(rendererID);

    renderers[renderindex]->connect_image(dataID);
    }

void rendermanager::data_has_changed(int ID)
    {
    vector<int> combos = rendermanager::combinations_from_data (ID);

    for (vector<int>::iterator c = combos.begin();c != combos.end(); c++)
        {
        rendermanagement.combination_update_callback(*c);
        }
    }

void rendermanager::combination_update_callback (int c_id)
    {
    viewmanagement.refresh_viewports_from_combination(c_id);
    }

void rendermanager::geometry_update_callback (int g_id)
    {
    viewmanagement.refresh_viewports_from_geometry(g_id);
    }

void rendermanager::image_vector_has_changed() 
    {
    for (unsigned int v=0;v < renderers.size();v++)
        {
        renderers[v]->imagestorender->image_vector_has_changed();
        if (renderers[v]->imagestorender->empty())
            {
            //renderer has no images, we might want to kill it - BUT
            //the renderer owns the rendercombination object and is thus
            //the nexus for viewing images, and to let users
            //leave viewports empty and then select images to view
            //we have to leave the renderer active

            //see also end of initialize_viewport

            //remove_renderer(renderers[v]->get_id());
            }

        }
    for (unsigned int c=0;c < combinations.size();c++)
        {
        combinations[c]->image_vector_has_changed();
        }
    }

void rendermanager::set_geometry(int renderer_index,Matrix3D * dir)
    {
    renderers[renderer_index]->wheretorender->dir=(*dir);
    renderers[renderer_index]->wheretorender->refresh_viewports();
    }

void rendermanager::set_geometry(int renderer_ID,Vector3D look_at,float zoom)
{
    int index = find_renderer_index(renderer_ID);
    
    if( zoom > 0)
        {renderers[index]->wheretorender->zoom = zoom;}
    
    renderers[index]->wheretorender->look_at = look_at;
    
    renderers[index]->wheretorender->refresh_viewports();
}

int rendermanager::get_blend_mode (int rendererIndex)
    {
    return renderers[rendererIndex]->imagestorender->blend_mode();
    }

rendercombination* rendermanager::get_combination (int ID)
{
    return get_renderer(ID)->imagestorender;
}

/*int rendermanager::image_at_priority (int rendererIndex, int priority)
    {
    return renderers[rendererIndex]->imagestorender->image_ID_by_priority(priority);
    }*/

void rendermanager::set_blendmode(int renderer_index,blendmode mode)
    {
    renderers[renderer_index]->imagestorender->blend_mode(mode);
    }