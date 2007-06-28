//////////////////////////////////////////////////////////////////////////
//
//  Viewporttool $Revision:$
//
/// The base class for implementing mouse behaviors/actions in a viewport
///
//  $LastChangedBy: $
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

#include "image_base.h"

class pt_event;

class viewporttool 
{
protected:
    image_base * image; //do dynamic_cast to whatever class that is needed
    
public:
    viewporttool(viewport *);
    virtual ~viewporttool;
    
    virtual attach(image_base *) = 0;

    static void grab (pt_event &event);
    bool handle(int event,enum {create, adjust} );
};