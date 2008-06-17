//////////////////////////////////////////////////////////////////////////
//
//  image_storage_iterator.hxx
//
//  Iterators for image_storage and derived classes
//
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

template<class ELEMTYPE >
image_storage<ELEMTYPE >::iterator::iterator(ELEMTYPE* i)
{
    ptr = i;
}

template<class ELEMTYPE >
image_storage<ELEMTYPE >::iterator::~iterator()
{}

template<class ELEMTYPE >
typename image_storage<ELEMTYPE >::iterator& image_storage<ELEMTYPE >::iterator::operator=(const typename image_storage<ELEMTYPE >::iterator& other)
{
    ptr = other.ptr;
    return(*this);
}

template<class ELEMTYPE >
bool image_storage<ELEMTYPE >::iterator::operator==(const typename image_storage<ELEMTYPE >::iterator& other)
{
    return(ptr == other.ptr );
}

template<class ELEMTYPE >
bool image_storage<ELEMTYPE >::iterator::operator!=(const typename image_storage<ELEMTYPE >::iterator& other)
{
    return(ptr != other.ptr );
}

template<class ELEMTYPE >
typename image_storage<ELEMTYPE >::iterator& image_storage<ELEMTYPE >::iterator::operator++()
{
    ptr++;
    return *this;
}

template<class ELEMTYPE >
typename image_storage<ELEMTYPE >::iterator& image_storage<ELEMTYPE >::iterator::operator++(int)
{
    iterator tmp(*this);
    ++(*this);
    return(tmp);
}

template<class ELEMTYPE >
ELEMTYPE& image_storage<ELEMTYPE >::iterator::operator*()
{
    return *ptr;
}

template<class ELEMTYPE >
ELEMTYPE* image_storage<ELEMTYPE >::iterator::operator->()
{
    return(&*(image_storage<ELEMTYPE >::iterator)*this);
}

template<class ELEMTYPE >
ELEMTYPE* image_storage<ELEMTYPE >::iterator::pointer()
{
    return ptr;
}

template<class ELEMTYPE >
typename image_storage<ELEMTYPE >::iterator image_storage<ELEMTYPE >::begin() {
    return(iterator(imagepointer()));
}

template<class ELEMTYPE >
typename image_storage<ELEMTYPE >::iterator image_storage<ELEMTYPE >::end() {
    return(iterator(imagepointer() + num_elements));
}

template<class ELEMTYPE >
typename image_storage<ELEMTYPE >::iterator& image_storage<ELEMTYPE >::iterator::operator--()
{
    ptr--;
    return *this;
}

template<class ELEMTYPE >
typename image_storage<ELEMTYPE >::iterator image_storage<ELEMTYPE >::iterator::operator+(unsigned long n) {
    return(iterator(ptr + n));
}