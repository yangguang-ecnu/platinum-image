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

template<int IMAGEDIM >
image_label<IMAGEDIM >::image_label ():image_integer<IMGLABELTYPE, IMAGEDIM>()
    {
    }

template<int IMAGEDIM >
void image_label<IMAGEDIM >:: transfer_function(transfer_base<IMGLABELTYPE > * t)
    {
    if (this->tfunction != NULL)
        {delete this->tfunction;}

    if (t == NULL)
        this->tfunction = new transfer_mapcolor<IMGLABELTYPE >(this);
    else
        this->tfunction = t;
    }