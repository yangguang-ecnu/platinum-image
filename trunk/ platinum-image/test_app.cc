/////////////////////////////////////////////////////////////////////////////////
//
//  Test application for Platinum development
//
//  $LastChangedBy$
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

#include "platinumprogram.h"

#include <cstdio>
#include <iostream>

#include "global.h"

#include "image_integer.hxx"

//retrieve command-line arguments
int visid = -1;
int arg(int argc, char **argv, int &i) {
    if (argv[i][1] == 'v') {
        if (i+1 >= argc) return 0;
        visid = atoi(argv[i+1]);
        i += 2;
        return 2;
        }
return 0;
    }

void add_demo_image (int userIO_ID,int par_num)
    {
    if (par_num == USERIO_CB_OK)
        {
        image_integer<unsigned char,3>* demo_image = new image_integer<unsigned char,3>(20,20,20,NULL);
        //demo_image->erase();
        demo_image->testpattern();

        datamanagement.add(demo_image);
        }
    }

int main(int argc, char *argv[])
    {
    //start up Platinum
    platinum_init();

    //set up main window

    //const int w_margin=15*2;
    //int windowwidth = 800-w_margin;
    //int windowheight = 600-w_margin;

    Fl_Window window(800,600);

    //prepare Platinum for userIO creation
    platinum_setup(window);

    // *** begin userIO control definitions ***

    int create_vol_demo_ID=userIOmanagement.add_userIO("Add demo image",add_demo_image,"Add");
    userIOmanagement.finish_userIO(create_vol_demo_ID);

    // *** end userIO control definitions ***

    //finish the window creation
    window.end();

    window.show(argc, argv);

    return Fl::run();
    }
