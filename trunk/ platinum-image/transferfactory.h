//////////////////////////////////////////////////////////////////////////
//
//   Transferfactory $Revision$
///
///  Object factory that makes transfer function objects and the
///  transfer function menu
///
//   $LastChangedBy$
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

#ifndef __transferfactory__
#define __transferfactory__

#include "listedfactory.h"

template <class ELEMTYPE >
class image_storage;

template <class ELEMTYPE >
class transfer_base;

class transferfactory //! transfer gets its own object factory type because constructors for templated classes cannot be stored
{
protected:
    int num_items;
    static const std::string tfunction_names [];
public:
        transferfactory ();
    
    ~transferfactory();
    
    class tf_menu_params {
        image_base * image;
        std::string type;
public:
        tf_menu_params (const std::string t,image_base * i);
        void switch_tf();
    };
    
    template <class ELEMTYPE >
        transfer_base<ELEMTYPE > * Create(factoryIdType unique_id, image_storage<ELEMTYPE >* s);
    
    Fl_Menu_Item * function_menu (Fl_Callback * cb);
    
    static const std::string tf_name(int);
};

#endif