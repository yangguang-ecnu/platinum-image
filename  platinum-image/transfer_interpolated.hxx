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

// *** transfer_interpolated (base class) ***
template <class ELEMTYPE>
transfer_interpolated<ELEMTYPE >::transfer_interpolated(image_storage <ELEMTYPE > * s):transfer_base<ELEMTYPE >(s)
{}

// *** transfer_linear ***

template <class ELEMTYPE >
transfer_linear<ELEMTYPE >::transfer_linear(image_storage <ELEMTYPE > * s):transfer_interpolated<ELEMTYPE >(s)
{}

// *** transfer_spline ***

template <class ELEMTYPE>
transfer_spline<ELEMTYPE >::transfer_spline(image_storage <ELEMTYPE > * s):transfer_interpolated<ELEMTYPE >(s)
{}