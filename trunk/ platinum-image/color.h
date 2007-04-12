//////////////////////////////////////////////////////////////////////////
//
//  Color
//
//  Classes storing/manipulating color values
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

#ifndef __color__
#define __color__

#include <vector>

#include "global.h"

#define IMGELEMCOMPTYPE unsigned char
#define IMGELEMCOMPMAX 255.0
#define RADDR 0
#define GADDR 1
#define BADDR 2
#define AADDR 3

// HSV: see http://en.wikipedia.org/wiki/HSV_color_space

class color_base
    {
    // *** get functions ***
    virtual void hsv(float &h, float &s, float &v) = 0;
    virtual void rgb(float &r_, float &g_, float &b_);
    
    virtual IMGELEMCOMPTYPE r() = 0;
    virtual IMGELEMCOMPTYPE g() = 0;
    virtual IMGELEMCOMPTYPE b() = 0;
    virtual float h() = 0;
    virtual float s() = 0;
    virtual float v() = 0;

    // *** set functions *** 
    virtual void set_hsv(int position, float r, float g, float b) = 0; // convert to HSV
    virtual void set_rgb(int position, float r_, float g_, float b_);
    virtual void r (IMGELEMCOMPTYPE) = 0;
    virtual void g (IMGELEMCOMPTYPE) = 0;
    virtual void b (IMGELEMCOMPTYPE) = 0;
    virtual void h (float) = 0;
    virtual void s (float) = 0;
    virtual void v (float) = 0;
    };

class HSVvalue:public color_base
    {
    protected:
        float h,s,v;
    };

class RGBvalue:public color_base
    {
    protected:
        IMGELEMCOMPTYPE values [3];
    public:
        // *** get functions ***
        IMGELEMCOMPTYPE r()
            {return values[RADDR];}
        IMGELEMCOMPTYPE g()
            {return values[GADDR];}
        IMGELEMCOMPTYPE b()
            {return values[BADDR];}
        void write (IMGELEMCOMPTYPE * addr)  //write value at address
            { memcpy (addr,values,sizeof (IMGELEMCOMPTYPE)*3);}    

        // *** set functions ***
        void r(IMGELEMCOMPTYPE r_)
            {values[RADDR] = r_;}
        void g(IMGELEMCOMPTYPE g_)
            {values[GADDR] = g_;}
        void b(IMGELEMCOMPTYPE b_)
            {values[BADDR] = b_;}
    };

class RGBAvalue:public RGBvalue
    {
    protected:
        IMGELEMCOMPTYPE values [4];
    public:
        IMGELEMCOMPTYPE a()
            {return values[BADDR];}
        void a(IMGELEMCOMPTYPE a_)
            {values[AADDR] = a_;}
        void write (IMGELEMCOMPTYPE * addr)
            {memcpy (addr,values,sizeof (IMGELEMCOMPTYPE)*4);}
    };

class colornode:public HSVvalue
    {
    protected:
        int position; // what voxel/pixel/... value this color corresponds to
    public:
        // convert from HSV - behövs EFTER interpolation - kunde vara en fristående funktion men passar här
        // dessa behöver "vector" så vi kan sortera arrayen
        // void operator=(const color &k) { }; // DEFAULT OPERATOR OK
        bool operator==(const colornode &k)
            { return position==k.position; }
        bool operator!=(const colornode &k)
            { return position!=k.position; }
        bool operator<(const colornode &k)
            { return position<k.position; }
        bool operator>(const colornode &k)
            { return position>k.position; }

        void set_rgb(int position, float h, float s, float v);
        void set_hsv(int position, float r, float g, float b); // convert to HSV
        void hsv(float &h, float &s, float &v);
        void rgb(float &r, float &g, float &b);
        void write (IMGELEMCOMPTYPE * addr);    //write value at address
    };

#endif
