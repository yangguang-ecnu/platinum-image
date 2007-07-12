// $Id$

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
#include "dicom_importer.h"
#include "userIOmanager.h"
#include "image_integer.hxx"

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

//#define TESTMODE	//JK4 test mode "dicom_import_button"

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
    for (unsigned int m=0; m <datamanager::IMAGEVECTORMAX;m++)
        {raw_image_menu[m].label (NULL); }
    closing = false;
}

datamanager::~datamanager()
    {
    closing = true;

    for (unsigned int i=0; i < dataItems.size(); i++)
        { delete dataItems[i]; }
    dataItems.clear();
    }

void datamanager::removedata_callback(Fl_Widget *callingwidget, void *thisdatamanager)
    {
    //callback for "Load image" button

    datawidget_base * the_datawidget=(datawidget_base *)(callingwidget->user_data());

    //here we want to check with the object whether this is image
    //or vector (or other) data
    //for now, the assumption is image data

    ((datamanager*)thisdatamanager)->remove_image( the_datawidget->get_data_id() );
    }

void datamanager::save_vtk_callback(Fl_Widget *callingwidget, void * thisdatamanager)
    {
    datawidget_base * the_datawidget=(datawidget_base *)(callingwidget->user_data());
#ifdef _DEBUG
    cout << "Save VTK image ID=" << the_datawidget->get_data_id() << endl;
#endif

    int image_index=((datamanager*)thisdatamanager)->find_image_index(the_datawidget->get_data_id());

    Fl_File_Chooser chooser(".","Visualization Toolkit image (*.vtk)",Fl_File_Chooser::CREATE,"Save VTK image");

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

    ((datamanager*)thisdatamanager)->dataItems[image_index]->save_to_VTK_file(chooser.value(1));
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
    //color gradient or solid color depending on image type

    data_widget_box = new horizresizeablescroll(xpos,ypos+LISTHEADERHEIGHT,width,DATALISTINITHEIGHT-BUTTONHEIGHT-margin*2-LISTHEADERHEIGHT*2);

    data_widget_box->end();

    Fl_Group* buttongroup = new Fl_Group(xpos,data_widget_box->y()+data_widget_box->h(),width,BUTTONHEIGHT+margin*2);
    buttongroup->box(FL_NO_BOX);

    load_button = new Fl_Button(xpos,data_widget_box->y()+data_widget_box->h()+margin,120,BUTTONHEIGHT, "Load image...");
    load_button->callback(loadimage_callback,this);

//JK4 - dicom_import testing....
#ifdef TESTMODE
		Fl_Widget *dcm_import_button = new Fl_Button(xpos+120+margin,data_widget_box->y()+data_widget_box->h()+margin,120,BUTTONHEIGHT, "Dicom Importer...");
		dcm_import_button->callback(dcm_import_callback,this);
#endif

    buttongroup->resizable(NULL);
    buttongroup->end();

    Fl_Box * toolslabel = new Fl_Box(xpos,load_button->y()+load_button->h()+margin,width,LISTHEADERHEIGHT,"Tools");
    toolslabel->labelfont(FL_HELVETICA_BOLD );

    outergroup->resizable(outergroup);
    outergroup->box(FL_FLAT_BOX);
    outergroup->end();
    }

void datamanager::add(image_base * v)
{
    if (v != NULL)
        {
        if (dataItems.size() < IMAGEVECTORMAX)
            {
            int the_image_id= v->get_id();
            
            if (find_image_index(the_image_id) == -1)
                {
                dataItems.push_back(v);
                v->activate();
                
                int freeViewportID=viewmanagement.find_viewport_no_images();
                
                if (freeViewportID != NOT_FOUND_ID)
                    {
                    rendermanagement.connect_image_renderer(viewmanagement.get_renderer_id(freeViewportID),the_image_id);
                    }
                
                image_vector_has_changed();
                image_has_changed(the_image_id);
                }
            else
                {
                pt_error::error("Trying to re-add image ID ",pt_error::warning);
                }
            }
        else
            {
            //This error condition should really never happen, if it does there is
            //reason to rethink the dependency on a fixed image capacity the way 
            //IMAGEVECTORMAX works
            
            pt_error::error("Error when adding image: number of data items in datamanager exceeds IMAGEVECTORMAX",pt_error::fatal);
            }
        }
}

void datamanager::add(point_collection * p)
{
    pt_error::error_if_false(dataItems.size() < IMAGEVECTORMAX,"Error when adding point: number of data items in datamanager exceeds IMAGEVECTORMAX",pt_error::fatal);
    
    if(pt_error::error_if_null(p,"Can't add point to datamanager, pointer is NULL",pt_error::serious) != NULL)
        {
        dataItems.push_back(p);
        p->activate();
        
        image_vector_has_changed();
        image_has_changed(p->get_id());
        }
}

void datamanager::remove_image (int id)
    {
    int index;

    index=find_image_index(id);

    if (index >=0)
        {
        delete dataItems[index];

        dataItems.erase(dataItems.begin()+index);
        }
#ifdef _DEBUG
    if (index >=0)
        {
        cout << "Deleted image with ID=" << id << ", index=" << index << endl;
        cout << "There are now " << dataItems.size() << " data items" << endl;
        }
    else
        {
        cout << "Danger danger: image with ID " << id << " not found" << endl;
        }
#endif

    image_vector_has_changed();
    }

int datamanager::first_image()
    {
    return dataItems[0]->get_id();
    }

int datamanager::last_image()
    {
	if(dataItems.size()==0)
		return 0;
    return dataItems.back()->get_id();
    }

int datamanager::next_image(int id)
{
    int index=find_image_index(id);

    if (index != -1) {
        if ((index + 1) < (signed int)dataItems.size()) {
            //image exists and is not last
            return dataItems[index+1]->get_id();
        }

        else {
            //last image
            return 0;
        }
    }

    //error: id not found
    return -1; 
}

string datamanager::get_image_name(int ID)
{
    int index=find_image_index(ID);
    if (index >=0) {
        return dataItems[index]->name();
    }

    string error_string="(image_name error)";
    return error_string;
}

void datamanager::set_image_name(int ID,string n)
{
    int index=find_image_index(ID);
    if (index >=0)
    {
        dataItems[index]->name(n);
        image_vector_has_changed();
    }
}

void datamanager::dcm_import_callback(Fl_Widget *callingwidget, void *thisdatamanager)
// argument must tell us which instance, if multiple
{
#ifdef TESTMODE
		new dcmimportwin(100,100,900,600,"Dicom File Import");
#endif
}

void datamanager::loadimage_callback(Fl_Widget *callingwidget, void *thisdatamanager)
// argument must tell us which instance, if multiple
    {
    ((datamanager*)thisdatamanager)->loadimages();	//joel
    }

void datamanager::loadimages() // argument must tell us which instance, if multiple
    {
    Fl_File_Chooser chooser(".","Any file - raw (*)\tVisualization Toolkit image (*.vtk)\tTyped DICOM file (*.dcm)\tAnalyze .hdr image (*.hdr)\tAnalyze .obj image (*.obj)",Fl_File_Chooser::MULTI,"Load VTK/DICOM image");

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
        files.push_back(std::string(chooser.value(t)));
        }

    if (!files.empty())
        {  
        image_base::load(files);
        }
    }

int datamanager::create_empty_image(int x, int y, int z, int unit) // argument must tell us which instance, if multiple
    {
    image_base *animage = NULL;
    
    switch (unit)
        {
        case VOLDATA_CHAR:      animage=new image_integer<char>(); break;
        case VOLDATA_UCHAR:     animage=new image_integer<unsigned char>(); break;
        case VOLDATA_SHORT:     animage=new image_integer<short>(); break;
        case VOLDATA_USHORT:    animage=new image_integer<unsigned short>(); break;
        case VOLDATA_DOUBLE:    animage=new image_scalar<double>(); break;
        default: cout << "Unsupported data type\n" << endl;
        }

    animage->initialize_dataset(x,y,z);

    add(animage);

    return animage->get_id();
    }

int datamanager::create_empty_image(image_base * blueprint, imageDataType unit)
    {
    image_base *animage = NULL;

    animage = blueprint->alike(unit);

    //no initialize_dataset, relevant information (except image) is taken
    //from blueprint

    add(animage);

    return animage->get_id();
    }


void datamanager::listimages()
    {
    for (unsigned int i=0; i < dataItems.size(); i++)
        { cout << *dataItems[i] << endl; } 
    }

int datamanager::find_image_index(int uniqueID)
    {
    for (unsigned int i=0; i < dataItems.size(); i++)
        {
        if (*dataItems[i]==uniqueID)
            { return i; }
        }
    return -1; // not found
    }

image_base * datamanager::get_image (int ID)
    {
    vector<data_base*>::iterator itr=dataItems.begin();

    while (itr != dataItems.end())
        {
        if (**itr == ID)
            {
            image_base * i = dynamic_cast<image_base *>(*itr);
            
            if (pt_error::error_if_null(i,"Trying to get_image when requested ID is not image type",pt_error::fatal) != NULL)
                { return i; }
            }
        itr++;
        }

    return NULL;
    }

bool datamanager::FLTK_running ()
    {
    return !closing;
    }

void datamanager::add_datawidget(datawidget_base * data_widget)
    {
    //add FLTK widget belonging to datawidget object to the list

    data_widget_box->interior->add(data_widget);
    data_widget->resize (data_widget_box->interior->x(),data_widget_box->interior->y(),data_widget_box->interior->w(),data_widget->h());

    refresh_datawidgets();
    }

void datamanager::refresh_datawidgets()
    {
    data_widget_box->redraw();
    }

void datamanager::remove_datawidget(datawidget_base * the_fl_widget)
    {
    //remove FLTK widget belonging to datawidget object from list
    //the datawidget itself is deleted by its owner data_base

    the_fl_widget->hide();

    data_widget_box->interior->remove(the_fl_widget);
    }

void datamanager::image_has_changed (int image_ID, bool recalibrate)
    {
    //when thumbnails are implemented, they should be re-rendered at this point
    get_image(image_ID)->image_has_changed(recalibrate);

    vector<int> changed_combinations=rendermanagement.combinations_from_image(image_ID);
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
    rebuild_image_menu();
    rendermanagement.image_vector_has_changed();
    userIOmanagement.image_vector_has_changed();
    }

void datamanager::rebuild_image_menu()
{
    int v=first_image();//image ID for iteration
    int m=0;
    
    //delete old labels
    for(int i=0;raw_image_menu[i].label()!=NULL;i++)
        {delete raw_image_menu[i].label();}
    
    while (v > 0)
    {
        if (raw_image_menu[m].label()!=NULL)
            {raw_image_menu[m].label(NULL);}
        string labelstring=datamanagement.get_image_name(v);
        char * menulabel=strdup(labelstring.c_str());
        
        raw_image_menu[m].shortcut(0);
        raw_image_menu[m].callback((Fl_Callback *)NULL,0);
        raw_image_menu[m].argument((long)v);
        raw_image_menu[m].flags= 0;
        raw_image_menu[m].labeltype(FL_NORMAL_LABEL);
        raw_image_menu[m].labelfont(0);
        raw_image_menu[m].labelsize(FLTK_LABEL_SIZE);
        raw_image_menu[m].labelcolor(FL_BLACK);
        
        raw_image_menu[m].label(menulabel);
        
        m++;
        v=next_image(v);
    }
    
    //terminate menu
    raw_image_menu[m].label(NULL);
}

const Fl_Menu_Item * datamanager::FLTK_image_menu_items()
{
    return raw_image_menu;
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
// loads spatial info and imagedata
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