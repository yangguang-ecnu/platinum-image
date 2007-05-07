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


template <class ELEMTYPE >
image_storage<ELEMTYPE >::image_storage() : image_base ()
    {
    imageptr = NULL;
    tfunction = NULL;

    transfer_function();  //set default transfer function

    minvalue=std::numeric_limits<ELEMTYPE>::min();
    maxvalue=std::numeric_limits<ELEMTYPE>::max();
    }

template <class ELEMTYPE >
image_storage<ELEMTYPE >::~image_storage()
    {
    if (imageptr != NULL)
        {delete imageptr;}

    delete tfunction;

    minvalue=std::numeric_limits<ELEMTYPE>::min();
    maxvalue=std::numeric_limits<ELEMTYPE>::max();
    }

template <class ELEMTYPE >
void image_storage<ELEMTYPE >::transfer_function(transfer_base<ELEMTYPE> * t)
    {
   if (tfunction != NULL)
        {delete tfunction;}

    if (t == NULL)
        { tfunction = new transfer_default<ELEMTYPE >(this); }
    else
        { tfunction = t; }
    }

template <class ELEMTYPE >
float image_storage<ELEMTYPE >::get_min_float()
    {
    return minvalue;
    }

template <class ELEMTYPE >
ELEMTYPE image_storage<ELEMTYPE >::get_min()
    {
    return minvalue;
    }

template <class ELEMTYPE >
float image_storage<ELEMTYPE >::get_max_float()
    {
    return maxvalue;
    }

template <class ELEMTYPE >
ELEMTYPE image_storage<ELEMTYPE >::get_max()
    {
    return maxvalue;
    }

template <class ELEMTYPE >
void image_storage<ELEMTYPE >::erase ()
    {
    memset (imageptr, 0, sizeof(ELEMTYPE) * num_elements);
    }
