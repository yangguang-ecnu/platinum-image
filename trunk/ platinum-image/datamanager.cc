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

#include "datamanager.h"
#include "viewmanager.h"
#include "rendermanager.h"
#include "rawimporter.h"
#include "image_integer.h"

datamanager datamanagement;
extern rendermanager rendermanagement;
extern viewmanager viewmanagement;
extern userIOmanager userIOmanagement;

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Group.H>

using namespace std;

uchar *animage;

struct IDENTIFIER
    {
    char name[100];
    int identtype; // +-1 =char-array +-2 =short +4 =float -4 =int  0 =size-based
    int noofparams; // (only valid for identtype != 0; all params must be of same type
    };

IDENTIFIER IDENTIFIERS[] = {
    {"Version",2,1},
    {"Volumesize",2,3} // 3 params of type unsigned short
    };

int NOOFIDENTIFIERS = 2;

//char prioriteter[10][2]; //possible priorities: 0...99
//int prioindex = 0;
//int prioritet = -1;

datamanager::datamanager()
{
    for (unsigned int m=0; m <datamanager::MAXVOLUMES;m++)
        {raw_volume_menu[m].label (NULL); }
    closing = false;
}

datamanager::~datamanager()
    {
    //since volumes and vectors are pointer arrays we have to collect garbage ourselves

    closing = true;

    for (unsigned int i=0; i < volumes.size(); i++)
        { delete volumes[i]; }
    volumes.clear();

    for (unsigned int i=0; i < vectors.size(); i++)
        { delete vectors[i]; }

    vectors.clear();
    }

void datamanager::removedata_callback(Fl_Widget *callingwidget, void *thisdatamanager)
    {
    //callback for "Load volume" widget

    datawidget * the_datawidget=(datawidget *)(callingwidget->user_data());

    //here we want to check with the datawidget whether this is volume
    //or vector (or other) data
    //for now, the assumption is volume data

    ((datamanager*)thisdatamanager)->remove_volume( the_datawidget->get_volume_id() );
    }

void datamanager::save_vtk_volume_callback(Fl_Widget *callingwidget, void * thisdatamanager)
    {
    datawidget * the_datawidget=(datawidget *)(callingwidget->user_data());
#ifdef _DEBUG
    cout << "Save VTK volume ID=" << the_datawidget->get_volume_id() << endl;
#endif

    int volume_index=((datamanager*)thisdatamanager)->find_image_index(the_datawidget->get_volume_id());

    Fl_File_Chooser chooser(".","Visualization Toolkit volume (*.vtk)",Fl_File_Chooser::CREATE,"Save VTK volume");

    chooser.ok_label("Save") ;
    chooser.preview(false); 

    chooser.show();

    while(chooser.shown())
        { Fl::wait(); }

    if ( chooser.value() == NULL )
    {
        fprintf(stderr, "(User hit 'Cancel')\n");
        return;
    }

    ((datamanager*)thisdatamanager)->volumes[volume_index]->save_image_to_VTK_file(chooser.value(1));
    }

#define LISTHEADERHEIGHT 25

void datamanager::datawidgets_setup()
    {
    const int margin=10;

    const unsigned int xpos=Fl_Group::current()->x();
    const unsigned int width=Fl_Group::current()->w();
    const unsigned int ypos=0;

    Fl_Group* outergroup; //group containing data list and load control
    Fl_Widget *load_button;

    outergroup=new Fl_Group (xpos,ypos,width,DATALISTINITHEIGHT);

    Fl_Box * datalabel = new Fl_Box(xpos,ypos,width,LISTHEADERHEIGHT,"Data");
    datalabel->labelfont(FL_HELVETICA_BOLD );

    //we should create a bitmap here (i.e. the "animage"), and fill it with
    //color gradient or solid color depending on volume type

    data_widget_box = new horizresizeablescroll(xpos,ypos+LISTHEADERHEIGHT,width,DATALISTINITHEIGHT-BUTTONHEIGHT-margin*2-LISTHEADERHEIGHT*2);

    data_widget_box->end();

    load_button = new Fl_Button(xpos,data_widget_box->y()+data_widget_box->h()+margin,120,BUTTONHEIGHT, "Load volume...");
    load_button->callback(loadvolume_callback,this);    //joel

    Fl_Box * toolslabel = new Fl_Box(xpos,load_button->y()+load_button->h()+margin,width,LISTHEADERHEIGHT,"Tools");
    toolslabel->labelfont(FL_HELVETICA_BOLD );

    outergroup->resizable(data_widget_box);
    outergroup->box(FL_FLAT_BOX);
    outergroup->end();
    }

void datamanager::add(image_base * v)
    {
    if (volumes.size() < MAXVOLUMES)
        {
        int the_volume_id= v->get_id();

        if (find_image_index(the_volume_id) == -1)
            {
            volumes.push_back(v);

            int freeViewportID=viewmanagement.find_viewport_no_volumes();

            if (freeViewportID != NOT_FOUND_ID)
                {
                rendermanagement.connect_volume_renderer(viewmanagement.get_renderer_id(freeViewportID),the_volume_id);
                }

            image_vector_has_changed();
            image_has_changed(the_volume_id);
            }
        else
            {
#ifdef _DEBUG
            cout << "Trying to re-add volume ID " << the_volume_id << endl;
#endif
            }
        }
    else
        {
#ifdef _DEBUG
        //This error condition should really never happen, if it does there is
        //reason to rethink the dependency on a fixed volume capacity the way 
        //MAXVOLUMES works

        cout << "Error when adding volume: number of volumes in datamanager exceeds MAXVOLUMES" << endl;
#endif
        }
    }

void datamanager::remove_volume (int id)
    {
    int index;

    index=find_image_index(id);

    if (index >=0)
        {
        delete volumes[index];

        volumes.erase(volumes.begin()+index);
        }
#ifdef _DEBUG
    if (index >=0)
        {
        cout << "Deleted volume with ID=" << id << ", index=" << index << endl;
        cout << "There are now " << volumes.size() << " volumes" << endl;
        }
    else
        {
        cout << "Danger danger: volume with ID " << id << " not found" << endl;
        }
#endif

    image_vector_has_changed();
    }

int datamanager::first_volume()
    {
    return volumes[0]->get_id();
    }

int datamanager::next_volume(int id)
{
    int index=find_image_index(id);

    if (index != -1) {
        if ((index + 1) < (signed int)volumes.size()) {
            //volume exists and is not last
            return volumes[index+1]->get_id();
        }

        else {
            //last volume
            return 0;
        }
    }

    //error: id not found
    return -1; 
}

string datamanager::get_volume_name(int ID)
{
    int index=find_image_index(ID);
    if (index >=0) {
        return volumes[index]->name();
    }

    string error_string="(volume_name error)";
    return error_string;
}

void datamanager::set_volume_name(int ID,string n)
{
    int index=find_image_index(ID);
    if (index >=0)
    {
        volumes[index]->name(n);
        image_vector_has_changed();
    }
}

void datamanager::loadvolume_callback(Fl_Widget *callingwidget, void *thisdatamanager)
// argument must tell us which instance, if multiple
    {
    ((datamanager*)thisdatamanager)->loadvolumes();	//joel
    }

void datamanager::loadvolumes() // argument must tell us which instance, if multiple
    {
    Fl_File_Chooser chooser(".","Any file - raw (*)\tVisualization Toolkit volume (*.vtk)\tTyped DICOM file (*.dcm)",Fl_File_Chooser::MULTI,"Load VTK/DICOM volume");

    chooser.show();

    while(chooser.shown())
        { Fl::wait(); }

    if ( chooser.value() == NULL )
        {
#ifdef _DEBUG
        cout << "Cancel" << endl;
#endif
        return;
        }
    //Load user's choice

#ifdef _DEBUG
    // Print what the user picked
    fprintf(stdout, "--------------------\n");
    fprintf(stdout, "DIRECTORY: '%s'\n", chooser.directory());
    fprintf(stdout, "    COUNT: %d files selected\n", chooser.count());
#endif

    vector<string> files;

    for ( int t=1; t<=chooser.count(); t++ )
        {
#ifdef _DEBUG
        std::cout <<  chooser.value(t) << endl;
#endif
        files.push_back(chooser.value(t));
        }

    if (files.size() > 0)
        {  
        image_base::load(files);
        }
    }

int datamanager::create_empty_volume(int x, int y, int z, int unit) // argument must tell us which instance, if multiple
    {
    image_base *avolume = NULL;
    
    switch (unit)
        {
        case VOLDATA_CHAR:      avolume=new image_integer<char>(); break;
        case VOLDATA_UCHAR:     avolume=new image_integer<unsigned char>(); break;
        case VOLDATA_SHORT:     avolume=new image_integer<short>(); break;
        case VOLDATA_USHORT:    avolume=new image_integer<unsigned short>(); break;
        case VOLDATA_DOUBLE:    avolume=new image_scalar<double>(); break;
        default: cout << "Unsupported data type\n" << endl;
        }

    avolume->initialize_dataset(x,y,z);

    add(avolume);

    return avolume->get_id();
    }

int datamanager::create_empty_volume(image_base * blueprint, imageDataType unit)
    {
    image_base *avolume = NULL;

    avolume = blueprint->alike(unit);

    //no initialize_dataset, relevant information (except volume) is taken
    //from blueprint

    add(avolume);

    return avolume->get_id();
    }


void datamanager::listvolumes()
    {
    cout << "Antal volymer: " << volumes.size() << endl;

    for (unsigned int i=0; i < volumes.size(); i++) { cout << *volumes[i] << endl; } 
    }

int datamanager::find_image_index(int uniqueID)
    {
    for (unsigned int i=0; i < volumes.size(); i++)
        {
        if (*volumes[i]==uniqueID)
            { return i; }
        }
    return -1; // not found
    }

image_base * datamanager::get_image (int ID)
    {
    vector<image_base*>::iterator itr=volumes.begin();

    while (itr != volumes.end())
        {
        if (**itr == ID)
            { return *itr; }
        itr++;
        }

    return NULL;
    }

bool datamanager::FLTK_running ()
    {
    return !closing;
    }

void datamanager::add_datawidget(datawidget * the_widget)
    {
    //add FLTK widget belonging to datawidget object to the list
    Fl_Widget * data_widget=the_widget->get_widget();

    data_widget_box->interior->add(data_widget);
    data_widget->resize (data_widget_box->interior->x(),data_widget_box->interior->y(),data_widget_box->interior->w(),data_widget->h());

    refresh_datawidgets();
    }

void datamanager::refresh_datawidgets()
    {
    data_widget_box->redraw();
    }

void datamanager::remove_datawidget(datawidget * the_widget)
    {
    //remove FLTK widget belonging to datawidget object from list,
    //and delete it eventually

    if (FLTK_running ())   //see comment on closing in header
        {
        Fl_Widget* the_fl_widget=the_widget->get_widget();

        data_widget_box->interior->remove(the_fl_widget);
        Fl::delete_widget(the_fl_widget);
        }
    }

void datamanager::image_has_changed (int volume_ID, bool recalibrate)
    {
    //when thumbnails are implemented, they should be re-rendered at this point
    get_image(volume_ID)->image_has_changed(recalibrate);

    vector<int> changed_combinations=rendermanagement.combinations_from_volume(volume_ID);
    vector<int>::iterator citr=changed_combinations.begin();

    citr=changed_combinations.begin();
    while (citr != changed_combinations.end())
        {
        viewmanagement.refresh_viewports_from_combination (*citr);
        citr++;
        }
    }

void datamanager::image_vector_has_changed()
    {
    rebuild_volume_menu();
    rendermanagement.image_vector_has_changed();
    userIOmanagement.image_vector_has_changed();
    }

void datamanager::rebuild_volume_menu()
{
    int v=first_volume();//volume ID for iteration
    int m=0;
    
    //delete old labels
    for(int i=0;raw_volume_menu[i].label()!=NULL;i++)
        {delete raw_volume_menu[i].label();}
    
    while (v > 0)
    {
        if (raw_volume_menu[m].label()!=NULL)
            {raw_volume_menu[m].label(NULL);}
        string labelstring=datamanagement.get_volume_name(v);
        char * menulabel=strdup(labelstring.c_str());
        
        raw_volume_menu[m].shortcut(0);
        raw_volume_menu[m].callback((Fl_Callback *)NULL,0);
        raw_volume_menu[m].argument((long)v);
        raw_volume_menu[m].flags= 0;
        raw_volume_menu[m].labeltype(FL_NORMAL_LABEL);
        raw_volume_menu[m].labelfont(0);
        raw_volume_menu[m].labelsize(FLTK_LABEL_SIZE);
        raw_volume_menu[m].labelcolor(FL_BLACK);
        
        raw_volume_menu[m].label(menulabel);
        
        m++;
        v=next_volume(v);
    }
    
    //terminate menu
    raw_volume_menu[m].label(NULL);
}

const Fl_Menu_Item * datamanager::FLTK_volume_menu_items()
{
    return raw_volume_menu;
}

// *** planned custom file format ***

// An identifier might look like:
// Version: 5
// ...or (the parameter gives the amount of bytes to read)...
// Volumedata: 15748
// ...or...
// Volumedata: "kalle.via"

// removes comments (# or // -lines), reads identifier and identifies data type (either size or variable type)

    void datamanager::parse_identifier(ifstream &in, int &identifierindex) // index follows the IDENTIFIERS array
        {
        char aline[1000];

        do
            { in.getline(aline,1000); }
        while (aline[0]=='#' || (aline[0]=='/' && aline[1]=='/'));


        identifierindex = -1;

        for (int i=0; i < NOOFIDENTIFIERS; i++)
            { 
            if (strncmp(aline, IDENTIFIERS[i].name, strlen(IDENTIFIERS[i].name)) == 0) // we found the target
                {
                identifierindex = i; return;
                }
            }
        return;
        }


////////
// loads spatial info and volumedata
//

void datamanager::parse_and_load_file(char filename[])
    {
    ifstream in(filename);

    int identindex;

    while (!in.eof())
        {
        parse_identifier(in, identindex);

        for (int param=0; param < IDENTIFIERS[identindex].noofparams; param++) // how many params to read
            {

            }
        }
    }