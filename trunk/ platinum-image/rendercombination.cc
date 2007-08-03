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
        renderdata.push_front(renderpair(volID,datamanagement.get_data(volID),BLEND_OVERWRITE));
        }
    }

rendercombination::iterator rendercombination::begin() const
{
    return renderdata.begin();
}

rendercombination::iterator rendercombination::end() const
{
    return renderdata.end();
}

bool rendercombination::empty() const
{
    return renderdata.empty();    
}

image_base* rendercombination::top_image ()const
{
    for (std::list<renderpair>::const_iterator itr = renderdata.begin();itr != renderdata.end();itr++)
        {
        image_base* value = dynamic_cast<image_base* >(itr->pointer);
        
        if (value != NULL)
            { return value;}
        }
    
    return NULL;
}

void rendercombination::add_data(int dataID)
    {
    renderdata.push_back(renderpair(dataID,datamanagement.get_data(dataID),BLEND_OVERWRITE));
    
    rendermanagement.combination_update_callback(this->id);
}

void rendercombination::toggle_data(int dataID)
{
    bool removed=false;

    for (std::list<renderpair>::iterator itr = renderdata.begin();itr != renderdata.end() && removed == false;itr++)
        {
        
        if (itr->ID==dataID)
            {
            remove_image(dataID);
            removed=true;
            }
        }
    
    if (!removed)
        {add_data(dataID);  }
   }

void rendercombination::remove_image(int ID)
    {
    bool removed=false;
    
    for (std::list<renderpair>::iterator itr = renderdata.begin();itr != renderdata.end();itr++)
        {
        if (itr->ID == ID)
            {
            renderdata.erase(itr);
            removed = true;
            }
        }
    if (removed)
        {rendermanagement.combination_update_callback(this->id);}
  
    }

int rendercombination::image_rendered(int ID)
    {
    for (std::list<renderpair>::iterator itr = renderdata.begin();itr != renderdata.end();itr++)
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

void rendercombination::data_vector_has_changed()
    {
    //images may have been deleted too, we need to update both image ID and image pointer

    for (std::list<renderpair>::iterator itr = renderdata.begin();itr != renderdata.end();itr++)
        {
        itr->pointer=datamanagement.get_data(itr->ID);
        
        if ( itr->pointer == NULL)
            {
            //image at p does not exist
            
            renderdata.erase(itr);
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