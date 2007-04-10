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

#include "rendercombination.h"

#include "rendermanager.h"

extern rendermanager rendermanagement;
extern datamanager datamanagement;

int rendercombination::new_rc_ID=1;

rendercombination::rendercombination()
    {
    //TODO: change the undefined ID from 0 to NOT_FOUND_ID
    id=new_rc_ID++;
    rendervolumes[0]=0;
    blend_mode_=BLEND_MAX;
    }

rendercombination::rendercombination(int volID)
    {
    id=new_rc_ID++;
    rendervolumes[0]=volID;
   
    blend_mode_=BLEND_MAX;
    if (volID !=0)
        {
        rendervolume_pointers[0]=datamanagement.get_image(volID);

        rendervolumes[1]=0;
        }
    }

bool rendercombination::volume_remaining(int priority)
    {
    for (int i=0;i <= priority;i++)
        {
        if ( i>= MAXRENDERVOLUMES || rendervolumes [i]==0)
            {
            //got to end, no volume left
            return false;
            }
        }

    return true;
    }

int rendercombination::volume_ID_by_priority (int priority)
    {
    return rendervolumes[priority];
    }

image_base* rendercombination::get_volumepointer(int p)
    {
    return rendervolume_pointers[p];
    }

void rendercombination::add_volume(int volID)
    {
    int empty_spot=-1;

    //find end of combinations array
    for (int i=0;empty_spot == (-1) && i < MAXRENDERVOLUMES;i++)
        {
        if (rendervolumes [i]==0)
            {
            empty_spot=i;
            }
        }

    if (empty_spot >=0)
        {
        rendervolumes[empty_spot]=volID;
        rendervolume_pointers[empty_spot]=datamanagement.get_image(volID);
        if (empty_spot< MAXRENDERVOLUMES -1)
            {
            rendervolumes[empty_spot+1]=0;
            }
        rendermanagement.combination_update_callback(this->id);
        }
    else
        {
        std::cout << "Attempted to add volume ID " << volID << ", render list was full" << std::endl;
        }
    }

void rendercombination::toggle_volume(int volumeID)
{
    bool removed=false;
    
    for (int i=0; i<= MAXRENDERVOLUMES && rendervolumes [i]!=0;i++)
    {
        if (rendervolumes [i]==volumeID)
        {
            remove_volume(volumeID);
            removed=true;
        }
    }
    
    if (!removed)
    {
        add_volume(volumeID);
    }
    
}

void rendercombination::remove_volume(int ID)
    {
    bool removed=false;
    for (int i=0; i<= MAXRENDERVOLUMES && rendervolumes [i]!=0;i++)
        {
        if (rendervolumes [i]==ID)
            {
            removed=true;
            }
        if (removed)
            {
            rendervolumes[i]=rendervolumes[i+1];
            rendervolume_pointers[i]=rendervolume_pointers[i+1];
            }
        }
    if (removed)
        {rendermanagement.combination_update_callback(this->id);}
    }

int rendercombination::volume_rendered(int ID)
    {
        for (int r=0; r < MAXRENDERVOLUMES && rendervolumes [r]!=0; r++)
        {
            if (rendervolumes[r]==ID)
                {return blend_mode();}
        }

        return BLEND_NORENDER;
    }

int rendercombination::get_id()
    {
    return id;
    }

void rendercombination::image_vector_has_changed()
    {
    // we cache the vector ID to speed things up - now it must be recomputed
    //intermediate solution, later a list of volumes to be rendered will be stored in a separate object
    //(rendercombination) and that will be the logical place to keep a cache like this

    //volumes may have been deleted too, we need to update both image ID and image pointer

    for (int r=0;r < MAXRENDERVOLUMES && rendervolumes [r]!=0;r++)
        {
        int i=datamanagement.find_image_index(abs(rendervolumes[r]));

        if (i != (-1) )
            {
            //rebuild cached list of pointers to rendered volumes
            //and increment p counter

            rendervolume_pointers[r]=datamanagement.volumes[i];
            }
        else
            {
            //image at p does not exist

            rendervolumes[r]=0;

            for (int v=r;rendervolumes [v+1]!=0;v++)
                {
                rendervolumes[v]=rendervolumes[v+1];
                }
            }
        };

    rendermanagement.combination_update_callback(id);
    }

void rendercombination::blend_mode(blendmode b)
    {
    blend_mode_=b;
    rendermanagement.combination_update_callback(id);
    }

blendmode rendercombination::blend_mode()
    {
    return blend_mode_;
    }