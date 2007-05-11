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

template <int IMAGEDIM>
image_binary<IMAGEDIM>::image_binary():image_integer<bool, IMAGEDIM>() 
    {}

template <int IMAGEDIM>
image_binary<IMAGEDIM>::image_binary(int w, int h, int d, bool *ptr
                                    ):image_integer<bool, IMAGEDIM>(w, h, d, ptr)
    {}

template <int IMAGEDIM>
void image_binary<IMAGEDIM >:: transfer_function(transfer_base<bool > * t)
    {
    if (this->tfunction != NULL)
        {delete this->tfunction;}

    if (t == NULL)
        this->tfunction = new transfer_mapcolor<bool >(this);
    else
        this->tfunction = t;
    }

// *** Logical operations ***
    
template <int IMAGEDIM>
image_binary<IMAGEDIM> * image_binary<IMAGEDIM>::logical_or(image_binary<IMAGEDIM> *input, bool object_value)
    {
    image_binary<IMAGEDIM> * output = new image_binary (this,false);
    
    image_storage<bool>::iterator i1 = this->begin();
    image_storage<bool>::iterator i2 = input->begin();
    image_storage<bool>::iterator o = output->begin();
    while (i1 != this->end()) //images are same size and should necessarily end at the same time
        {
        if(*i1 == object_value || *i2 == object_value)
            {*o=object_value;}
        else
            {*o=!object_value;}
        ++i1; ++i1; ++o;
        }
    return output;
    }
    
template <int IMAGEDIM>
image_binary<IMAGEDIM> * image_binary<IMAGEDIM>::logical_and(image_binary<IMAGEDIM> *input, bool object_value)
    {
    image_binary<IMAGEDIM> * output = new image_binary (this,false);
    
    image_storage<bool>::iterator i1 = this->begin();
    image_storage<bool>::iterator i2 = input->begin();
    image_storage<bool>::iterator o = output->begin();
    while (i1 != this->end()) //images are same size and should necessarily end at the same time
        {
        if(*i1 == object_value && *i2 == object_value)
            {*o=object_value;}
        else
            {*o=!object_value;}
        ++i1; ++i1; ++o;
        }
    return output;
    }
    
template <int IMAGEDIM>
image_binary<IMAGEDIM> * image_binary<IMAGEDIM>::logical_xor(image_binary<IMAGEDIM> *input, bool object_value)
    {
    image_binary<IMAGEDIM> * output = new image_binary (this,false);
    
    image_storage<bool>::iterator i1 = this->begin();
    image_storage<bool>::iterator i2 = input->begin();
    image_storage<bool>::iterator o = output->begin();
    while (i1 != this->end()) //images are same size and should necessarily end at the same time
        {
        if((*i1 == object_value && *i2 != object_value) || (*i1 != object_value && *i2 == object_value))
            {*o=object_value;}
        else
            {*o=!object_value;}
        ++i1; ++i1; ++o;
        }
    return output;
    }  