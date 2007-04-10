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

void rendermanager::move(int rendererIndex, float panX, float panY, float panZ, float scale)
    {
    renderers[rendererIndex]->move( panX, panY, panZ, scale);
    }

void rendermanager::render(int rendererIndex, unsigned char *rgbimage, int rgbXsize, int rgbYsize)
    {
    renderers[rendererIndex]->render_position(rgbimage, rgbXsize, rgbYsize);
    }

void rendermanager::render_thumbnail(unsigned char *rgb, int rgb_sx, int rgb_sy, int volume_ID)
    {
    //decision: MPRs are nice for thumbnails

    rendererMPR::render_thumbnail(rgb, rgb_sx, rgb_sy, volume_ID);
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

vector<int> rendermanager::combinations_from_volume (int volumeID)
    {
    //when thumbnails are implemented, they should be re-rendered at this point

    vector<renderer_base*>::iterator itr = renderers.begin();
    vector<int> found_combinations;
    vector<int>::iterator citr;
    int index=0;

    while (itr != renderers.end())
        {
        if (volume_rendered(index, volumeID) != BLEND_NORENDER)
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
    return renderers[rendererIndex]->volumestorender->get_id();
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

void rendermanager::toggle_volume (int rendererIndex, int volumeID)
    {
    renderers[rendererIndex]->volumestorender->toggle_volume( volumeID);
    }

int rendermanager::volume_rendered(int rendererIndex, int volID)
    {
    return renderers[rendererIndex]->volumestorender->volume_rendered(volID);
    }

int rendermanager::renderer_empty (int rendererID)
    {
    int rendererIndex = find_renderer_index(rendererID);

    if (rendererIndex != -1)
        {return (renderers[rendererIndex]->volumestorender->volume_ID_by_priority(0) == 0 ? RENDERER_EMPTY : RENDERER_NOT_EMPTY);}

    return -1;
    }

int rendermanager::get_renderer_type(int rendererIndex)
    {
    return renderers[rendererIndex]->renderer_type();
    }

vector<float> rendermanager::get_values (int index, int px, int py,int sx, int sy)
    {
    if (index != -1)
        {
        return renderers[index]->get_values(px, py,sx,sy);
        }

    vector<float> v;

    v.push_back(-1);

    return v;  //if ID is invalid
    }

Vector3D rendermanager::get_location (int rendererIndex, int volumeID, int px, int py, int sx, int sy)
    {
    return renderers[rendererIndex]->view_to_voxel(volumeID, px, py, sx, sy);
    }

void rendermanager::connect_volume_renderer(int rendererID, int imageID)
    {
    int renderindex = find_renderer_index(rendererID);

    renderers[renderindex]->connect_image(imageID);
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
        renderers[v]->volumestorender->image_vector_has_changed();
        if (!renderers[v]->volumestorender->volume_remaining(0))
            {
            //renderer has no volumes, we might want to kill it - BUT
            //the renderer owns the rendercombination object and is thus
            //the nexus for viewing volumes, and to let users
            //leave viewports empty and then select volumes to view
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

int rendermanager::get_blend_mode (int rendererIndex)
    {
    return renderers[rendererIndex]->volumestorender->blend_mode();
    }

int rendermanager::volume_at_priority (int rendererIndex, int priority)
    {
    return renderers[rendererIndex]->volumestorender->volume_ID_by_priority(priority);
    }

void rendermanager::set_blendmode(int renderer_index,blendmode mode)
    {
    renderers[renderer_index]->volumestorender->blend_mode(mode);
    }