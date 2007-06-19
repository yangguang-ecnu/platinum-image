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

#define IMGLABELTYPE unsigned char
#define IMGBINARYTYPE IMGLABELTYPE

class image_base;

#include "image_base.h"

#include "histogram.h"
#include "transfer.h"

template<class ELEMTYPE>
class image_storage : public image_base
    {
    friend class transfer_base<ELEMTYPE>;

    private:
        void set_parameters ();
        ELEMTYPE * dataptr;

    protected:
        image_storage();
        template<class SOURCETYPE>
        image_storage(image_storage<SOURCETYPE> * s);

        transfer_base<ELEMTYPE> * tfunction;
        histogram_1D<ELEMTYPE> * stats;

        virtual void transfer_function(transfer_base<ELEMTYPE> * t = NULL);
        //allows subclasses to set a different default transfer function, and
        //to reject unsuitable choices

        // *** Image data pointer ***

        ELEMTYPE * imagepointer()
            {
            if (dataptr == NULL) 
                { /*throw exception*/ }
            
            return (dataptr); 
            }

        void imagepointer(ELEMTYPE * new_value)
            {
            dataptr = new_value;
            }
        
        void deallocate ()
            {
            delete dataptr;
            }

        unsigned long num_elements;        //image size in # pixels/voxels

        ELEMTYPE maxvalue;
        ELEMTYPE minvalue;

    public:
        virtual ~image_storage();

        float get_max_float();
        float get_min_float();
        ELEMTYPE get_max();
        ELEMTYPE get_min();
        ELEMTYPE get_num_values()
            { return stats->num_values(); }
        const histogram_1D<ELEMTYPE> * get_histogram()
            {return stats;}
        void stats_refresh();
        void min_max_refresh();     //! lighter function that _only_ recalculates max/min values,
                                    //! for use inside processing functions

        void erase();
		void fill(ELEMTYPE value);
        void scale(ELEMTYPE new_min=0, ELEMTYPE new_max=255);

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
                ELEMTYPE* pointer();
				iterator& operator--();             
				iterator operator+(unsigned long n);
            };

        iterator begin();        
        iterator end();   
    };

//template implementation files
#include "histogram.hxx"
#include "transfer.hxx"
#include "transfer_interpolated.hxx"

//with C++ templates, declaration and definition go together
#include "image_storage_iterator.hxx"
#include "image_storage.hxx"

#endif