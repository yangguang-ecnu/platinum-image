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

#include "rendercombination.h"

#include "rendermanager.h"

extern rendermanager rendermanagement;
extern datamanager datamanagement;

int rendercombination::new_rc_ID=1;

rendercombination::renderpair::renderpair()
{
    ID = NOT_FOUND_ID;
    pointer = NULL;
    mode = BLEND_NORENDER;
};        

rendercombination::renderpair::renderpair(const int i,data_base* d,const blendmode m) 
{
    ID = i;
    pointer = d;
    mode = m;
};        

rendercombination::rendercombination()
    {
    //TODO: change the undefined ID from 0 to NOT_FOUND_ID
    id=new_rc_ID++;
    blend_mode_=BLEND_MAX;
    }

rendercombination::rendercombination(int volID)
    {
    id=new_rc_ID++;
   
    blend_mode_=BLEND_MAX;
    if (volID > 0)
        {
        renderimages.push_front(renderpair(volID,datamanagement.get_data(volID),BLEND_OVERWRITE));
        }
    }

rendercombination::iterator rendercombination::begin() const
{
    return renderimages.begin();
}

rendercombination::iterator rendercombination::end() const
{
    return renderimages.end();
}

bool rendercombination::empty() const
{
    return renderimages.empty();    
}

/*bool rendercombination::image_remaining(int priority)
    {
    for (int i=0;i <= priority;i++)
        {
        if ( i>= MAXRENDERVOLUMES || renderimages [i]==0)
            {
            //got to end, no image left
            return false;
            }
        }

    return true;
    }

int rendercombination::image_ID_by_priority (int priority)
    {
    return renderimages[priority];
    }

image_base* rendercombination::get_imagepointer(int p)
    {
    return renderimage_pointers[p];
    }*/

image_base* rendercombination::top_image ()const
{
    for (std::list<renderpair>::const_iterator itr = renderimages.begin();itr != renderimages.end();itr++)
        {
        image_base* value = dynamic_cast<image_base* >(itr->pointer);
        
        if (value != NULL)
            { return value;}
        }
    
    return NULL;
}

void rendercombination::add_image(int dataID)
    {
    renderimages.push_back(renderpair(dataID,datamanagement.get_data(dataID),BLEND_OVERWRITE));

    /*
    //find end of combinations array
    for (int i=0;empty_spot == (-1) && i < MAXRENDERVOLUMES;i++)
        {
        if (renderimages [i]==0)
            {
            empty_spot=i;
            }
        }

    if (empty_spot >=0)
        {
        renderimages[empty_spot]=volID;
        renderimage_pointers[empty_spot]=datamanagement.get_image(volID);
        if (empty_spot< MAXRENDERVOLUMES -1)
            {
            renderimages[empty_spot+1]=0;
            }
        rendermanagement.combination_update_callback(this->id);
        }
    else
        {
        std::cout << "Attempted to add image ID " << volID << ", render list was full" << std::endl;
        }
    */
    }

void rendercombination::toggle_image(int imageID)
{
    bool removed=false;

    for (std::list<renderpair>::iterator itr = renderimages.begin();itr != renderimages.end();itr++)
        {
        
        if (itr->ID==imageID)
            {
            remove_image(imageID);
            removed=true;
            }
        }
    
    if (!removed)
        {add_image(imageID);  }
    
    /*bool removed=false;
    
    for (int i=0; i<= MAXRENDERVOLUMES && renderimages [i]!=0;i++)
    {
        if (renderimages [i]==imageID)
        {
            remove_image(imageID);
            removed=true;
        }
    }
    
    if (!removed)
    {
        add_image(imageID);
    }*/
}

void rendercombination::remove_image(int ID)
    {
    bool removed=false;
    
    for (std::list<renderpair>::iterator itr = renderimages.begin();itr != renderimages.end();itr++)
        {
        if (itr->ID == ID)
            {
            renderimages.erase(itr);
            removed = true;
            }
        }
    if (removed)
        {rendermanagement.combination_update_callback(this->id);}
    
    /*bool removed=false;
    for (int i=0; i<= MAXRENDERVOLUMES && renderimages [i]!=0;i++)
        {
        if (renderimages [i]==ID)
            {
            removed=true;
            }
        if (removed)
            {
            renderimages[i]=renderimages[i+1];
            renderimage_pointers[i]=renderimage_pointers[i+1];
            }
        }
    if (removed)
        {rendermanagement.combination_update_callback(this->id);}*/
    }

int rendercombination::image_rendered(int ID)
    {
    for (std::list<renderpair>::iterator itr = renderimages.begin();itr != renderimages.end();itr++)
        {
            if (itr->ID ==ID)
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
    //intermediate solution, later a list of images to be rendered will be stored in a separate object
    //(rendercombination) and that will be the logical place to keep a cache like this

    //images may have been deleted too, we need to update both image ID and image pointer

    /*for (int r=0;r < MAXRENDERVOLUMES && renderimages [r]!=0;r++)
        {
        int i=datamanagement.find_image_index(abs(renderimages[r]));

        if (i != (-1) )
            {
            //rebuild cached list of pointers to rendered images
            //and increment p counter

            renderimage_pointers[r]=datamanagement.images[i];
            }
        else
            {
            //image at p does not exist

            renderimages[r]=0;

            for (int v=r;renderimages [v+1]!=0;v++)
                {
                renderimages[v]=renderimages[v+1];
                }
            }
        };*/
    for (std::list<renderpair>::iterator itr = renderimages.begin();itr != renderimages.end();itr++)
        {
        itr->pointer=datamanagement.get_image(itr->ID);
        
        if ( itr->pointer == NULL)
            {
            //image at p does not exist
            
            renderimages.erase(itr);
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