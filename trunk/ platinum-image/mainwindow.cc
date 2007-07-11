//////////////////////////////////////////////////////////////////////////
//
//  Intialization of main Platinum window $Revision$
//
//  $LastChangedBy$
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

#include "platinumprogram.h"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Pack.H>
//#include <FL/Fl_Output.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Tile.H>

//Linux (?)
#include <FL/x.H>
#if !defined(WIN32) && !defined(__APPLE__)
#include "list_visuals.cxx"
#endif

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
    // S‰tt igÂng hela tjosanhejsan
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

void platinum_setup (Fl_Window & window, int num_viewports_h, int num_viewports_v,int list_area_w )
    {
    const int status_area_h = 24;
    int win_w = window.w();
    int win_h = window.h();
    int view_w = win_w - list_area_w;
    int view_h = win_h - status_area_h;

    //set up the window;
    window.size (win_w,win_h);
    window.resizable(&window);

    //allows resizing the proportions of views & data/tools
    Fl_Tile * viewsNlists = new Fl_Tile(0,0,win_w,win_h-status_area_h);

    //Fl_Tile * views = new Fl_Tile(0,0,view_w,view_h);

    // *** skapa ett antal views ***
    //
    const int antalvp = num_viewports_h * num_viewports_v;
    for (int i=0;i < antalvp; i++) // create some views w/ renderers
        {
        viewmanagement.create_viewport();
        }

    // *** Ge viewmanager och datamanager l‰mpliga FLTK-pekare och initiera ***

    // viewmanagement.demo_setup_tiles(); // initialize tile layout; could as well load from file or ...
    viewmanagement.setup_regular_tiles (num_viewports_h, num_viewports_v);  //initialize regular tile layout
    viewmanagement.erase_all_connections(); // only do this once...
    viewmanagement.connect_views_viewports(); // kopplar ev. lediga VPs till behˆvande views

    // *** Create viewport widgets ***

    viewmanagement.setup_views(0, view_w, view_h); // 0...antal som anv‰nds-1

    viewmanagement.listviewports();

    //views->resizable(views);
    //views->end();

    Fl_Tile * tool_area = new Fl_Tile(view_w,0,list_area_w,win_h-status_area_h);   //group containing datawidgets and feedback, so they'll be resized properly
    datamanagement.datawidgets_setup();

    userIOmanagement.setup();

    tool_area->box(FL_BORDER_BOX);
    tool_area->resizable(tool_area);
    tool_area->end();

    viewsNlists->resizable(viewsNlists);
    viewsNlists->end();

    Fl_Group::current (&window); //make statusarea a subwindow
    
    userIOmanagement.status_area = new statusarea (0,win_h-status_area_h,win_w,status_area_h);    
    
    Fl_Pack * toolsNstatus = new Fl_Pack (0,win_h-status_area_h,win_w,status_area_h);
    toolsNstatus->type(FL_HORIZONTAL);
    viewporttool::init(userIOmanagement.status_area); //!spawn toolbox and give viewporttool pointer to statusarea
    userIOmanagement.status_area->resize(viewporttool::toolbox->x()+viewporttool::toolbox->w(),toolsNstatus->y(),win_w-viewporttool::toolbox->w(),status_area_h);
    toolsNstatus->add(userIOmanagement.status_area);

    toolsNstatus->resizable(userIOmanagement.status_area);
    toolsNstatus->end();
        int dummyX = userIOmanagement.status_area->x();
    int dummyW = userIOmanagement.status_area->w();
    
    toolsNstatus->deactivate();
    
    window.resizable(viewsNlists);
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

