//$Id$

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

//This file exists only because transfer_manufactured and transferfactory are co-dependent and transferfactory and the subclasses of transfer_manufactured all use templates

template <class ELEMTYPE >
transfer_scalar_base<ELEMTYPE > * transferfactory::Create(factoryIdType unique_id, image_storage<ELEMTYPE >* s)
{
    int n = 0;
    
    //transfer function template constructors, same order as in tfunction_names[]:
    
    if (unique_id == tfunction_names [n++] )
        {return new transfer_default<ELEMTYPE>(s);}
    
    if (unique_id == tfunction_names [n++] )
        {return new transfer_brightnesscontrast<ELEMTYPE>(s);}

	if (unique_id == tfunction_names [n++] )
        {return new transfer_threshold_illustrator<ELEMTYPE>(s);}

	if (unique_id == tfunction_names [n++] )
        {return new transfer_scalar_to_RGB_linear<ELEMTYPE>(s);}
    
    if (unique_id == tfunction_names [n++] )
        {return new transfer_mapcolor<ELEMTYPE>(s);}
    
    if (unique_id == tfunction_names [n++] )
        {return new transfer_linear<ELEMTYPE>(s);}
    
    if (unique_id == tfunction_names [n++] )
        {return new transfer_spline<ELEMTYPE>(s);}
    
    return NULL;
}
