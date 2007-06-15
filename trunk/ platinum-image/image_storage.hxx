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

template <class fromType, class toType>
void copy_data( image_storage<fromType > * in,image_storage<toType > * out) //!General data copying
    {
    typename image_storage<fromType >::iterator i = in->begin();
    typename image_storage<toType >::iterator   o = out->begin();

    while (i != in->end() && o != out->end())
        {
        *o = *i;

        ++i; ++o;
        }

    /*if (!(i == in->end() && o == out->end()) )
        {
        throw ("Image sizes didn't match when copying data");
        }*/
    }


//template <class fromType>
//void copy_data( image_storage<fromType > * in,image_storage<IMGBINARYTYPE > * out) //!Data copy specialized for copying *to* boolean
//    {
//    typename image_storage<fromType >::iterator i = in->begin();
//    typename image_storage<IMGBINARYTYPE >::iterator   o = out->begin();
//
//    while (i != in->end() && o != out->end())
//        {
//        *o = (*i == true);
//
//        ++i; ++o;
//        }
//
//    /*if (!(i == in->end() && o == out->end()) )
//    {
//    throw ("Image sizes didn't match when copying boolean data");
//    }*/
//    }

template <class ELEMTYPE >
void image_storage<ELEMTYPE >::set_parameters()
    {
    dataptr = NULL;
    
    tfunction = NULL;
    transfer_function();  //set default transfer function

    stats = NULL;

    minvalue=std::numeric_limits<ELEMTYPE>::min();
    maxvalue=std::numeric_limits<ELEMTYPE>::max();
    }

template <class ELEMTYPE >
image_storage<ELEMTYPE >::image_storage() : image_base ()
    {
    set_parameters();
    }

template <class ELEMTYPE >
template<class SOURCETYPE>
image_storage<ELEMTYPE >::image_storage(image_storage<SOURCETYPE> * s):image_base (s)
    {
    set_parameters();
    }

template <class ELEMTYPE >
image_storage<ELEMTYPE >::~image_storage()
    {
    if (imagepointer() != NULL)
        { deallocate(); }

    delete tfunction;

    if (stats != NULL)
        { delete stats; }

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

/*
template <> //JK2 image_complex testing
float image_storage<std::complex<float> >::get_min_float()
    {
    return abs(minvalue);
    }
*/

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
/*
template <> // image_complex testing
float image_storage<std::complex<float> >::get_max_float()
    {
    return abs(maxvalue);
    }
*/
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
void image_storage<ELEMTYPE >::erase()
    {
    memset (imagepointer(), 0, sizeof(ELEMTYPE) * num_elements);
    }

template <class ELEMTYPE >
void image_storage<ELEMTYPE >::fill(ELEMTYPE value)
	{
	typename image_storage<ELEMTYPE>::iterator i = this->begin();
	while (i != this->end())
		{
		*i = value;
		++i;
		}
	}


template <class ELEMTYPE >
void image_storage<ELEMTYPE >::scale(ELEMTYPE new_min, ELEMTYPE new_max)
	{
	this->min_max_refresh();

	if(get_min()==get_max())
		{
		fill(0);
		}
	else
		{
		typename image_storage<ELEMTYPE>::iterator i = this->begin();
		while (i != this->end())
			{
			*i = new_min + (ELEMTYPE) (((*i)-get_min()) * ((new_max-new_min)/(double)(get_max()-get_min())));
			++i;
			}
		}
	}

template <class ELEMTYPE >
void image_storage<ELEMTYPE >::min_max_refresh()
    {
    /*ELEMTYPE val;

    ELEMTYPE pre_max=std::numeric_limits<ELEMTYPE>::min();
    ELEMTYPE pre_min=std::numeric_limits<ELEMTYPE>::max();
    
    typename image_storage<ELEMTYPE>::iterator itr = this->begin();
    while (itr != this->end())
        {
        val=*itr;
        
        pre_max = max (val, pre_max);
        pre_min = min (val, pre_min);
        
        ++itr;
        }*/
    
    stats->calculate(); 
   
    //don't change if values don't make sense - 
    //that would be an empty/zero image
    if (stats->min() < stats->max())
        {
        this->maxvalue=stats->max();
        this->minvalue=stats->min();
        }
    }


