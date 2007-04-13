//////////////////////////////////////////////////////////////////////////
//
//  Colormap
//
//  Interpolated color map class.
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

#ifndef __colormap__
#define __colormap__

#include "color.h"

#include <iostream>
#include <vector>

#include "global.h"

////
// lagrar en samling färgnoder. Värden däremellan interpoleras fram.
//
class colormap
    {
    private:
//        std::vector<colornode> colors;
    public:
        void set_color(int position, float r, float g, float b);    // glöm inte att sortera/stoppa in på rätt plats
        void get_color(int position, float &r, float &g, float &b); // returns RGB at the interpolated position
                                                                    // - no use to return an instance of color
        void remove_color(int position);
        void initialize_colortable();  // always set colors at -oo, oo  in addition to user input
    };
#endif
