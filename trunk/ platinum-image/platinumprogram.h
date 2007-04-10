//////////////////////////////////////////////////////////////////////////
//
//  Helpers and intializations for programs using Platinum 
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

#include <FL/Fl.H>
//#include <FL/Fl_Double_Window.H>	//joel: Avoiding twinkles
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Image.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Tile.H>

#include "viewmanager.h"
#include "datamanager.h"
#include "rendermanager.h"

extern datamanager datamanagement;
extern rendermanager rendermanagement;
extern viewmanager viewmanagement;
extern userIOmanager userIOmanagement;

//Fl_Double_Window *w;	//joel: avoids twinkles

#include <FL/x.H>
#if !defined(WIN32) && !defined(__APPLE__)
#include "list_visuals.cxx"
#endif

void platinum_init ()

    {
    /*char aproject[100] = "";
    char avolume[100] = "";*/

    std::cout << "Initializing Platinum v. " << PLATINUM_MAJOR_VER << "." << PLATINUM_MINOR_VER << std::endl;

    /*const int MAXBUFFER=100;*/
    /*
    #define ever (;;)
    char c;
    for ever

    {
    char *optspec = "hp:v:"; // the options. x: = x with single argument
    if ((c = getopt(argc,argv,optspec)) == -1) break;
    if (strlen(optarg) >= MAXBUFFER) { cout << "Input overflow ERROR!\n"; exit(0); } switch (c)
    {
    case -99:// end of options list - break the while loop
    break;
    // options will follow
    case 'h':
    cout  << "Options: "  << &optspec  << endl;
    exit(1);
    case 'p':
    strcpy(aproject, optarg);
    break;
    case 'v':
    strcpy(avolume, optarg);
    break;
    }
    }
    */
    ////
    // Ladda in (eventuellt) projekt, volymsdata eller annat
    // INTE interaktivt - filnamn som inparametrar
    //
    //if (avolume[0] != 0) { datamanaging.load_volume(avolume); }
    //if (aproject[0] != 0) { datamanaging.load_project(aproject); }

    ////
    // Sätt igång hela tjosanhejsan
    //Main window
#if !defined(WIN32) && !defined(__APPLE__)
    //setup for Xwindows only
    int i = 1;
    cout << "Xwindows" << endl;
    Fl::args(argc,argv,i,arg);
    if (visid >= 0) {
        fl_open_display();
        XVisualInfo templt; int num;
        templt.visualid = visid;
        fl_visual = XGetVisualInfo(fl_display, VisualIDMask, &templt, &num);
        if (!fl_visual) {
            fprintf(stderr, "No visual with id %d, use one of:\n",visid);
            list_visuals();
            exit(1);
            }
    fl_colormap = XCreateColormap(fl_display, RootWindow(fl_display,fl_screen),
        fl_visual->visual, AllocNone);
    fl_xpixel(FL_BLACK); // make sure black is allocated in overlay visuals
        } else {
            Fl::visual(FL_RGB);
            }
#else
    //prevent dithering on legacy platforms
    Fl::visual(FL_RGB);
#endif

    //// Init FLTK

#ifdef __APPLE__
    //more OSX-like theme

    //"none", "plastic" or "gtk+"
    //Fl::scheme("plastic");
    //Fl::get_system_colors();

    Fl::background(250,250,250);
    /*Fl::foreground(0,0,0);
    Fl::background2(255,255,255);*/
#endif

    }

void platinum_setup (Fl_Window & window, int viewports_h = 2, int viewports_v = 2)
    {
    const int w_margin=15*2;
    int wwidth = window.w()-w_margin;
    int wheight = window.h()-w_margin;

    //set up the window
    window.resizable(&window);
    window.size (wwidth,wheight);

    //allows resizing the proportions of views & data/tools
    Fl_Tile * leftrighttile = new Fl_Tile(0,0,wwidth,wheight);

    //// skapa ett antal viewports
    //
    const int antalvp = viewports_h * viewports_v;
    for (int i=0;i < antalvp; i++) // create some viewports w/ renderers
        {
        viewmanagement.create_viewport();
        }

    //// Ge viewmanager och datamanager lämpliga FLTK-pekare och initiera
    //
    // viewmanagement.demo_setup_tiles(); // initialize tile layout; could as well load from file or ...
    viewmanagement.setup_regular_tiles (viewports_h, viewports_v);  //initialize regular tile layout
    viewmanagement.erase_all_connections(); // only do this once...
    viewmanagement.connect_views_viewports(); // kopplar ev. lediga VPs till behövande views

    //// Create all viewport widgets
    //
    int viewxsize = 550;
    int viewysize = wheight;
    viewmanagement.setup_views(0, viewxsize, viewysize); // 0...antal som används-1

    viewmanagement.listviewports();

    Fl_Tile * rightsidegroup = new Fl_Tile(viewxsize,0,wwidth - viewxsize,wheight);   //group containing datawidgets and feedback,
    //so they'll be resized properly
    datamanagement.datawidgets_setup();

    userIOmanagement.setup();

    rightsidegroup->box(FL_BORDER_BOX);
    rightsidegroup->resizable(rightsidegroup);
    rightsidegroup->end();

    leftrighttile->end();
    }

/*int platinum_run (int argc, char *argv[])
{
////
// ...finish the window creation
window.end();

////
//  start the main loop

window.show(argc, argv);

return Fl::run();
}*/

