//////////////////////////////////////////////////////////////////////////
//
//   Image_storage $Revision$
///
///  Abstract base class storing image data as a stream
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

#ifndef __image_storage__
#define __image_storage__

#include "transfer.h"

class image_base;

#include "image_base.h"

template<class ELEMTYPE>
class image_storage : public image_base
    {
    friend class transfer_base<ELEMTYPE>;

    private:
        void set_parameters ();

    protected:
        image_storage();
        template<class SOURCETYPE>
        image_storage(image_storage<SOURCETYPE> * s);

        transfer_base<ELEMTYPE> * tfunction;

        virtual void transfer_function(transfer_base<ELEMTYPE> * t = NULL);
        //allows subclasses to set a different default transfer function, and
        //to reject unsuitable choices

        ELEMTYPE *imageptr;
        unsigned long num_elements;        //image size in # pixels/voxels

        ELEMTYPE maxvalue;
        ELEMTYPE minvalue;

    public:
        virtual ~image_storage();

        void erase();
        float get_max_float();
        float get_min_float();
        ELEMTYPE get_max();
        ELEMTYPE get_min();
		void set_value_to_all_voxels(ELEMTYPE value);

        // *** iterator ***        
        class iterator : public std::iterator<std::forward_iterator_tag, ELEMTYPE>
            {
            private:
                ELEMTYPE* ptr;
			public:
				iterator(ELEMTYPE* i);               
				~iterator() {}
				iterator& operator=(const iterator& other);                
				bool operator==(const iterator& other);                
				bool operator!=(const iterator& other);                
				iterator& operator++();                
				iterator& operator++(int);                
				ELEMTYPE& operator*();                
				ELEMTYPE* operator->();               
				iterator& operator--();             
				iterator operator+(unsigned long n);
            };

        iterator begin();        
        iterator end();   
    };


//with C++ templates, declaration and definition go together
#include "image_storage_iterator.hxx"
#include "image_storage.hxx"

#endif