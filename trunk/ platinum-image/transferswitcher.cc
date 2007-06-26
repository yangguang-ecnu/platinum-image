// $Id: $

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

#include "transfer.h"

tcontrolpane::tcontrolpane(int X, int Y, int W, int H, const char *L)
  : Fl_Group(X, Y, W, H, L) {
  tcontrolpane *o = this;
{ Fl_Menu_Button* o = switchbtn = new Fl_Menu_Button(120, 0, 100, 15, "Default");
  o->labelsize(10);
}
end();
}

void tcontrolpane::clear() {
  remove(switchbtn);
Fl_Group::clear();
add(switchbtn);
}

void tcontrolpane::resize(int x,int y,int w,int h) {
  Fl_Group::resize(x,y,w,h);
//switchbtn new size & pos
}
