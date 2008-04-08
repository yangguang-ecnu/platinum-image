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

#include <cmath>
#include <iostream>

void color_base::set_rgb(const IMGELEMCOMPTYPE r_, const IMGELEMCOMPTYPE g_, const IMGELEMCOMPTYPE b_)
    {
    r(r_);
    g(g_);
    b(b_);
    }

void color_base::rgb(IMGELEMCOMPTYPE &r_, IMGELEMCOMPTYPE &g_, IMGELEMCOMPTYPE &b_)
    {
    r_ = r();
    g_ = g();
    b_ = b();
    }

color_base::color_base(const IMGELEMCOMPTYPE i)
    {
    this->set_mono(i);
    }

const IMGELEMCOMPTYPE color_base::mono()
    {
    return (static_cast<IMGELEMCOMPTYPE>(0.3*r()+0.6*g()+0.1*b()));
    }
/*
RGBvalue::RGBvalue(IMGELEMCOMPTYPE r_,IMGELEMCOMPTYPE g_,IMGELEMCOMPTYPE b_)
	{
    r(r_);
    g(g_);
    b(b_);
	}
*/
RGBvalue::RGBvalue(const IMGELEMCOMPTYPE r_,const IMGELEMCOMPTYPE g_,const IMGELEMCOMPTYPE b_): color_base()
    {
    r(r_);
    g(g_);
    b(b_);
    }


RGBvalue::RGBvalue (const IMGELEMCOMPTYPE* p)
    {
    set_rgb (p);
    }

void RGBvalue::set_rgb (const IMGELEMCOMPTYPE * p)
    {
    memcpy (values,p,sizeof (IMGELEMCOMPTYPE)*3);
    }

void RGBvalue::set_rgb(const IMGELEMCOMPTYPE r_, const IMGELEMCOMPTYPE g_, const IMGELEMCOMPTYPE b_)
    {
    r(r_);
    g(g_);
    b(b_);
    }

void RGBAvalue::set_rgba (const IMGELEMCOMPTYPE * p)
    {
    memcpy (values,p,sizeof (IMGELEMCOMPTYPE)*4);
    }

void color_base::set_mono (const IMGELEMCOMPTYPE i)
    {
    r(i);
    g(i);
    b(i);
    }

RGBAvalue::RGBAvalue (const IMGELEMCOMPTYPE r_,const IMGELEMCOMPTYPE g_,const IMGELEMCOMPTYPE b_, IMGELEMCOMPTYPE a_): color_base()
    {
    r(r_);
    g(g_);
    b(b_);
    a(a_);
    }

RGBAvalue::RGBAvalue (const IMGELEMCOMPTYPE i, const IMGELEMCOMPTYPE a_): color_base(i)
    {
    a(a_);
    }





colornode::colornode():RGBvalue()
{
	position=0;
}

/*
//TODO: ööö: FAN: 2 overloads have no legal conversion for 'this' pointer...
colornode::colornode(colornode const &c)//:RGBvalue(c.r(),c.g(),c.b()) 
{
	const IMGELEMCOMPTYPE rr = c.r();
//	this->r( c.r() );
//	this->g( c.g() );
//	this->b( c.b() );
	this->position = c.position;
}
*/

colornode::colornode(float pos, IMGELEMCOMPTYPE r, IMGELEMCOMPTYPE g, IMGELEMCOMPTYPE b):RGBvalue(r,g,b)
{
	position = pos;
}