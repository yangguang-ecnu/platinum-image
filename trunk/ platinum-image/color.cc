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

#include "color.h"

void color_base::set_rgb(int position, float r_, float g_, float b_)
    {
    r((IMGELEMCOMPTYPE)(r_*IMGELEMCOMPMAX));
    g((IMGELEMCOMPTYPE)(g_*IMGELEMCOMPMAX));
    b((IMGELEMCOMPTYPE)(b_*IMGELEMCOMPMAX));
    }

void color_base::rgb(float &r_, float &g_, float &b_)
    {
    r_=static_cast<float>(r()/IMGELEMCOMPMAX);
    g_=static_cast<float>(g()/IMGELEMCOMPMAX);
    b_=static_cast<float>(b()/IMGELEMCOMPMAX);
    }
    