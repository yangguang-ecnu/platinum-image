/////////////////////////////////////////////////////////////////////////////////
//
//  Minimal example application using Platinum
//
//

//   Copyright (c) 2007, Uppsala university
//   All rights reserved.
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions are met:
//       * Redistributions of source code must retain the above copyright
//         notice, this list of conditions and the following disclaimer.
//       * Redistributions in binary form must reproduce the above copyright
//         notice, this list of conditions and the following disclaimer in the
//         documentation and/or other materials provided with the distribution.
//       * Neither the name of the Uppsala university nor the
//         names of its contributors may be used to endorse or promote products
//         derived from this software without specific prior written permission.
//
//   THIS SOFTWARE IS PROVIDED BY UPPSALA UNIVERSITY ``AS IS'' AND ANY
//   EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//   DISCLAIMED. IN NO EVENT SHALL UPPSALA UNIVERSITY BE LIABLE FOR ANY
//   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "platinumprogram.h"

#include <cstdio>
#include <iostream>

#include "global.h"

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

// *** begin userIO demo functions ***

//string streams for parameter demo function
#include <sstream>
using namespace std;

void demo_function (int userIO_ID,int par_num)
    {
    //check par_num if "OK" button has been pressed
    if (par_num == USERIO_CB_OK)
        {
        ostringstream demostream;

        //get value for parameter 0
        float demovalue = userIOmanagement.get_parameter<float>(userIO_ID,0);

        //format to a string
        demostream << "Value: " << demovalue;

        //display a userIO message block
        userIOmanagement.show_message("Just a demo",demostream.str());
        }
    }

void add_demo_volume (int userIO_ID,int par_num)
    {
    if (par_num == USERIO_CB_OK)
        {
        int volID = datamanagement.create_empty_volume(20,20,20,1);
        datamanagement.get_image(volID)->testpattern();
        datamanagement.volume_has_changed(volID);
        }
    }

// *** end of userIO demo functions ***

int main(int argc, char *argv[])
    {
    //start up Platinum
    platinum_init();

    //set up main window

    const int w_margin=15*2;
    int windowwidth = 800-w_margin;
    int windowheight = 600-w_margin;

    Fl_Window window(800,600);

    //prepare Platinum for userIO creation
    platinum_setup(window);

    // *** begin userIO control definitions ***

    int control_demo_ID=userIOmanagement.add_userIO("Control demo",demo_function,"Show");
    userIOmanagement.add_par_float(control_demo_ID,"Demo value");
    userIOmanagement.finish_userIO(control_demo_ID);

    int create_vol_demo_ID=userIOmanagement.add_userIO("Add demo volume",add_demo_volume,"Add");
    userIOmanagement.finish_userIO(create_vol_demo_ID);

    // *** end userIO control definitions ***

    //finish the window creation
    window.end();

    window.show(argc, argv);

    return Fl::run();
    }
