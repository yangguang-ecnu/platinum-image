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

#ifndef __color__
#define __color__

#include <vector>

#include "global.h"

// HSV: see http://en.wikipedia.org/wiki/HSV_color_space

class color
    {
    private:
        int position; // what voxel/pixel/... value this color corresponds to
        float H, S, V;
    public:
        void set_color(int position, float h, float s, float v);
        void set_rgbcolor(int position, float r, float g, float b); // convert to HSV
        void get_color(float &h, float &s, float &v);
        void get_rgbcolor(float &r, float &g, float &b); // convert from HSV - behövs EFTER interpolation - kunde vara en fristående funktion men passar här
        // dessa behöver "vector" så vi kan sortera arrayen
        // void operator=(const color &k) { }; // DEFAULT OPERATOR OK
        bool operator==(const color &k)
            { return position==k.position; }
        bool operator!=(const color &k)
            { return position!=k.position; }
        bool operator<(const color &k)
            { return position<k.position; }
        bool operator>(const color &k)
            { return position>k.position; }
    };

#endif
