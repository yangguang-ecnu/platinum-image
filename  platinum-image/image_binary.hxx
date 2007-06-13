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

template <int DIM>
image_binary<DIM>* binary_copycast (image_base* input) //! Converts might-be binary types into image_binary
    {
    image_binary<DIM > * output = NULL;

        {
        image_integer <unsigned char, DIM>* input_general =
            dynamic_cast<image_integer <unsigned char, DIM> *> (input) ; 

        if (input_general != NULL) //! If cast was successful, input had the tried type and input_general can be used in a call to new class' copy constructor
            {
            output = new image_binary<DIM> (input_general,true);
            }
        }

            {
        image_integer <unsigned short,DIM>* input_general =
            dynamic_cast<image_integer <unsigned short, DIM> *> (input) ;

        if (input_general != NULL) //! If cast was successful, input had the tried type and input_general can be used in a call to new class' copy constructor
            {
            output = new image_binary<DIM> (input_general,true);
            //delete input;
            } 
            }

    return output;
    }

template <int IMAGEDIM>
image_binary<IMAGEDIM>::image_binary(int w, int h, int d, IMGBINARYTYPE *ptr
                                    ):image_label<IMAGEDIM>(w, h, d, ptr)
    {}

/*template <int IMAGEDIM>
void image_binary<IMAGEDIM >:: transfer_function(transfer_base<IMGBINARYTYPE > * t)
    {
    if (this->tfunction != NULL)
        {delete this->tfunction;}

    if (t == NULL)
        this->tfunction = new transfer_mapcolor<IMGBINARYTYPE >(this);
    else
        this->tfunction = t;
    }*/

// *** Logical operations ***
    
template <int IMAGEDIM>
image_binary<IMAGEDIM> * image_binary<IMAGEDIM>::logical_or(image_binary<IMAGEDIM> *input, IMGBINARYTYPE object_value)
    {
    image_binary<IMAGEDIM> * output = new image_binary (this,false);
    
    image_storage<IMGBINARYTYPE >::iterator i1 = this->begin();
    image_storage<IMGBINARYTYPE >::iterator i2 = input->begin();
    image_storage<IMGBINARYTYPE >::iterator o = output->begin();
    while (i1 != this->end()) //images are same size and should necessarily end at the same time
        {
        if(*i1 == object_value || *i2 == object_value)
            {*o=object_value;}
        else
            {*o=!object_value;}
        ++i1; ++i2; ++o;
        }
	//output->image_has_changed();
    return output;
    }
    
template <int IMAGEDIM>
image_binary<IMAGEDIM> * image_binary<IMAGEDIM>::logical_and(image_binary<IMAGEDIM> *input, IMGBINARYTYPE object_value)
    {
    image_binary<IMAGEDIM> * output = new image_binary (this,false);
    
    image_storage<IMGBINARYTYPE >::iterator i1 = this->begin();
    image_storage<IMGBINARYTYPE >::iterator i2 = input->begin();
    image_storage<IMGBINARYTYPE >::iterator o = output->begin();
    while (i1 != this->end()) //images are same size and should necessarily end at the same time
        {
        if(*i1 == object_value && *i2 == object_value)
            {*o=object_value;}
        else
            {*o=!object_value;}
        ++i1; ++i2; ++o;
        }
	//output->image_has_changed();
    return output;
    }
    
template <int IMAGEDIM>
image_binary<IMAGEDIM> * image_binary<IMAGEDIM>::logical_xor(image_binary<IMAGEDIM> *input, IMGBINARYTYPE object_value)
    {
    image_binary<IMAGEDIM> * output = new image_binary (this,false);
    
    image_storage<IMGBINARYTYPE >::iterator i1 = this->begin();
    image_storage<IMGBINARYTYPE >::iterator i2 = input->begin();
    image_storage<IMGBINARYTYPE >::iterator o = output->begin();
    while (i1 != this->end()) //images are same size and should necessarily end at the same time
        {
        if((*i1 == object_value && *i2 != object_value) || (*i1 != object_value && *i2 == object_value))
            {*o=object_value;}
        else
            {*o=!object_value;}
        ++i1; ++i2; ++o;
        }
	//output->image_has_changed();
    return output;
    }
template <int IMAGEDIM>
image_binary<IMAGEDIM> * image_binary<IMAGEDIM>::logical_or_not(image_binary<IMAGEDIM> *input, IMGBINARYTYPE object_value)
    {
    image_binary<IMAGEDIM> * output = new image_binary (this,false);
    
    image_storage<IMGBINARYTYPE >::iterator i1 = this->begin();
    image_storage<IMGBINARYTYPE >::iterator i2 = input->begin();
    image_storage<IMGBINARYTYPE >::iterator o = output->begin();
    while (i1 != this->end()) //images are same size and should necessarily end at the same time
        {
        if(*i1 == object_value || *i2 != object_value)
            {*o=object_value;}
        else
            {*o=!object_value;}
        ++i1; ++i2; ++o;
        }
	//output->image_has_changed();
    return output;
    }
    
template <int IMAGEDIM>
image_binary<IMAGEDIM> * image_binary<IMAGEDIM>::logical_and_not(image_binary<IMAGEDIM> *input, IMGBINARYTYPE object_value)
    {
    image_binary<IMAGEDIM> * output = new image_binary (this,false);
    
    image_storage<IMGBINARYTYPE >::iterator i1 = this->begin();
    image_storage<IMGBINARYTYPE >::iterator i2 = input->begin();
    image_storage<IMGBINARYTYPE >::iterator o = output->begin();
    while (i1 != this->end()) //images are same size and should necessarily end at the same time
        {
        if(*i1 == object_value && *i2 != object_value)
            {*o=object_value;}
        else
            {*o=!object_value;}
        ++i1; ++i2; ++o;
        }
	//output->image_has_changed();
    return output;
    }
    
template <int IMAGEDIM>
void image_binary<IMAGEDIM>::invert()
    {
    image_storage<IMGBINARYTYPE >::iterator i = this->begin();
    while (i != this->end())
        {
        *i = !(*i);
        ++i;
        }
	this->image_has_changed();
    } 