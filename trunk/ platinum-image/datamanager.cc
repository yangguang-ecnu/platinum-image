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
#include "stringmatrix.h"
//#include "renderer_base.h"

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

//#define TESTMODE	//JK4 test mode --> "dcm_import_button"

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
    image_menu = NULL;
    rebuild_image_menu();
    
    objects_menu = NULL;
    rebuild_objects_menu();
    
    point_menu = NULL;
    rebuild_point_menu();
    
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
    datawidget_base * the_datawidget=(datawidget_base *)(callingwidget->user_data());

    ((datamanager*)thisdatamanager)->delete_data( the_datawidget->get_data_id() );
    }

void datamanager::save_vtk_callback(Fl_Widget *callingwidget, void * thisdatamanager)
    {
    datawidget_base * the_datawidget=(datawidget_base *)(callingwidget->user_data());

    int image_index=((datamanager*)thisdatamanager)->find_data_index(the_datawidget->get_data_id());

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
            
            if (find_data_index(the_image_id) == -1)
                {
                dataItems.push_back(v);
                v->activate();
                
                int freeViewportID=viewmanagement.find_viewport_no_images();
                
                if (freeViewportID != NOT_FOUND_ID)
                    {
                    int rendererID = viewmanagement.get_renderer_id(freeViewportID);
                    
                    if (rendermanagement.renderer_empty(rendererID))
                        {
                        //get a zoom factor that will show the entire image
                        Vector3D size = v->get_size();
                        float maxsize = max_norm (size);
                        
                        rendermanagement.set_geometry(rendererID,v->get_origin(),renderer_base::display_scale/maxsize);
                        }
                    
                    rendermanagement.connect_data_renderer(rendererID,the_image_id);
                    }
                
                data_vector_has_changed();
                data_has_changed(the_image_id);
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
        
        data_vector_has_changed();
        data_has_changed(p->get_id());
        }
}

void datamanager::delete_data (data_base * d)
{
    for (vector<data_base*>::iterator itr=dataItems.begin();itr != dataItems.end();itr++)
        {
        if (*itr == d)
            {
            delete *itr; //the data_base destructor calls XXX to
                         //remove it from dataItems 
            break;
            }
        }
}

void datamanager::delete_data (int id)
{
	pt_error::error("datamanager::delete_data",pt_error::debug);
    for (vector<data_base*>::iterator itr=dataItems.begin();itr != dataItems.end();itr++)
        {
        if ((*itr)->get_id() == id)
            {
            delete *itr;
            break;
            }
        }
}

void datamanager::remove_data (int id)
{
    for (vector<data_base*>::iterator itr=dataItems.begin();itr != dataItems.end();itr++)
        {
        if ((*itr)->get_id() == id)
            {
            dataItems.erase( itr); 
            
            data_vector_has_changed();
            break;
            }
        }
}

void datamanager::remove_data (data_base * d)
{
    for (vector<data_base*>::iterator itr=dataItems.begin();itr != dataItems.end();itr++)
        {
        if (*itr == d)
            {
            dataItems.erase( itr); 
            
            data_vector_has_changed();
            break;
            }
        }
}

string datamanager::get_data_name(int ID)
{
    data_base * d = get_data(ID);
    
    if (pt_error::error_if_null(d,"Attempt to get name from NULL data object",pt_error::serious) != NULL)
        { return d->name();}
    
    string error_string="(get_data_name error)";
    return error_string;
}

void datamanager::set_image_name(int ID,string n)
{
    int index=find_data_index(ID);
    if (index >=0)
    {
        dataItems[index]->name(n);
        data_vector_has_changed();
    }
}

void datamanager::dcm_import_callback(Fl_Widget *callingwidget, void *thisdatamanager)
// argument must tell us which instance, if multiple
{
#ifdef TESTMODE
	dcmimportwin::create(50,50,1200,800,"Dicom File Import");
#endif
}

void datamanager::loadimage_callback(Fl_Widget *callingwidget, void *thisdatamanager)
// argument must tell us which instance, if multiple
    {
    ((datamanager*)thisdatamanager)->loadimages();	
    }

void datamanager::loadimages() // argument must tell us which instance, if multiple
    {
    Fl_File_Chooser chooser(".","Any file - raw (*)\tVisualization Toolkit image (*.vtk)\tTyped DICOM file (*.dcm)\tAnalyze .hdr image (*.hdr)\tAnalyze .obj image (*.obj)",Fl_File_Chooser::MULTI,"Load VTK/DICOM image");

    chooser.show();

    while(chooser.shown())
        { Fl::wait(); }

    if ( chooser.value() == NULL )
        {
        pt_error::error("Image load dialog cancel",pt_error::notice);

        return;
        }
    //Load user's choice

    vector<string> files;

    for ( int t=1; t<=chooser.count(); t++ )
        {
        files.push_back(std::string(chooser.value(t)));
        }

    if (!files.empty())
        {  
        image_base::load(files);
        }
    }


//JK geometry information will be difficult to import, as files can be chosen arbitraryly...
//Set the geometry info to "default" (i.e no rotation, origin (0,0,0), scaling (1,1,1) )

void datamanager::load_dcm_import_vector(std::vector<std::string> dcm_filenames, std::string import_vol_name)
{

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
        default: pt_error::error("Unsupported data type",pt_error::serious);
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

int datamanager::find_data_index(int uniqueID)
    {
    for (unsigned int i=0; i < dataItems.size(); i++)
        {
        if (*dataItems[i]==uniqueID)
            { return i; }
        }
    return -1; // not found
    }

data_base * datamanager::get_data (int ID)
{
    for (vector<data_base*>::iterator itr=dataItems.begin();itr != dataItems.end();itr++)
        {
        if (**itr == ID)
            {
            return *itr;
            }
        }
	return NULL;//Pretty important line...
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

int datamanager::last_image()
    {
	if(dataItems.size()==0)
		return 0;
    return dataItems.back()->get_id();
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

void datamanager::data_has_changed (int dataID, bool recalibrate)
    {
    //when thumbnails are implemented, they should be re-rendered at this point
    get_data(dataID)->data_has_changed(recalibrate);

    vector<int> changed_combinations=rendermanagement.combinations_from_data(dataID);
    vector<int>::iterator citr=changed_combinations.begin();

    citr=changed_combinations.begin();
    while (citr != changed_combinations.end())
        {
        viewmanagement.refresh_viewports_from_combination (*citr);
        citr++;
        }
    }

void datamanager::data_vector_has_changed()
    {
    rebuild_objects_menu();

    rendermanagement.data_vector_has_changed();

    userIOmanagement.data_vector_has_changed();
    }

template <class OCLASS>
Fl_Menu_Item * datamanager::object_menu ()
{    
    std::vector<OCLASS *> objects;
    
    std::vector<data_base *>::iterator itr = dataItems.begin();
    int m=0;
    
    while (itr != dataItems.end())
        {
        OCLASS * ptr = dynamic_cast<OCLASS *>(*itr);
        if (ptr != NULL)
            { objects.push_back(ptr); }
        
        itr++;
        }
    
    Fl_Menu_Item * newMenu = new Fl_Menu_Item[objects.size()+1];
    
    typename std::vector<OCLASS *>::iterator oitr = objects.begin();
    
    while (oitr != objects.end())
        {
        init_fl_menu_item(newMenu[m]);
        
        string labelstring=datamanagement.get_data_name((*oitr)->get_id());
        char * menulabel=strdup(labelstring.c_str());
        
        newMenu[m].callback((Fl_Callback *)NULL,0);
        newMenu[m].argument((long)(*oitr)->get_id());
        
        newMenu[m].label(menulabel);
        
        m++;
        oitr++;
        }
    
    //terminate menu
    init_fl_menu_item(newMenu[m]);
    newMenu[m].label(NULL);
    
    return newMenu;
}

void datamanager::rebuild_point_menu()
{
if (point_menu != NULL)
    { 
    delete point_menu; 
    point_menu = NULL;
    }
    point_menu = object_menu<point_collection>();
}

void datamanager::rebuild_objects_menu()
{
    if (objects_menu != NULL)
        { 
        delete objects_menu; 
        objects_menu = NULL;
        }
    objects_menu = object_menu<data_base>();
    
    rebuild_point_menu();
    rebuild_image_menu();
}

void datamanager::rebuild_image_menu()
{
    if (image_menu != NULL)
        { 
        delete image_menu; 
        image_menu = NULL;
        }
    image_menu = object_menu<image_base>();
}

const Fl_Menu_Item * datamanager::FLTK_image_menu() const
{
    return image_menu;
}

const Fl_Menu_Item * datamanager::FLTK_objects_menu() const
{
    return objects_menu;
}

const Fl_Menu_Item * datamanager::FLTK_point_menu() const
{
    return point_menu;
}