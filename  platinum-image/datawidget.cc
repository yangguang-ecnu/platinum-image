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

#include "datawidget.h"

#include "datamanager.h"
#include "rendermanager.h"
#include "transfer.h"

using namespace std;

extern datamanager datamanagement;
extern rendermanager rendermanagement;
extern uchar *animage; //defined in datamanager.cc

const int datawidget_base::thumbnail_size = 128;



#pragma mark transferfactory statics

transferfactory::tf_menu_params::tf_menu_params (const std::string t,image_base * i)
    {
    image = i;
    type = t;
    }

void transferfactory::tf_menu_params::switch_tf()
    {
    pt_error::error_if_null(image,"Trying to make tfunction menu item with image = NULL");
    image->transfer_function(type);
//	image->redraw(); //JK - to redraw the 
    }

#pragma mark datawidget_base

// *** begin FLUID ***

void datawidget_base::cb_filenamebutton_i(Fl_Input*, void*) {
  datamanagement.set_image_name(data_id,string(datanamebutton->value()));
}
void datawidget_base::cb_filenamebutton(Fl_Input* o, void* v) {
  ((datawidget_base*)(o->parent()->parent()))->cb_filenamebutton_i(o,v);
}

void datawidget_base::edit_geometry_callback(Fl_Widget *callingwidget, void *){
    datawidget_base * the_datawidget=(datawidget_base *)(callingwidget->user_data());
//	cout<<"the_datawidget->get_data_id()="<<the_datawidget->get_data_id()<<endl;
    the_datawidget->show_hide_edit_geometry();
}

const Fl_Menu_Item datawidget_base::menu_featuremenu_base[] = {
 {"Remove", 0,  (Fl_Callback*)datamanager::removedata_callback, (void*)(&datamanagement), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Save as VTK", 0,  (Fl_Callback*)datamanager::save_vtk_callback, (void*)(&datamanagement), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Save histogram", 0,  (Fl_Callback*)datamanager::save_hist_callback, (void*)(&datamanagement), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Duplicate", 0,  0, 0, 1, FL_NORMAL_LABEL, 0, 14, 0},
 {"Geometry Edit(Show/Hide)", 0, (Fl_Callback*)datawidget_base::edit_geometry_callback,0,0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0}
};

// *** custom constructor declared in FLUID: ***

datawidget_base::datawidget_base(data_base * d, std::string n):Fl_Pack(0,0,270,130,NULL) {
    data_id = d->get_id();
    thumbnail_image = new unsigned char [thumbnail_size*thumbnail_size];
    
    deactivate(); //activated when the image is added to datamanager

    // *** resume FLUID inits ***

    datawidget_base *o = this;
    o->box(FL_THIN_DOWN_FRAME);
    o->color(FL_BACKGROUND_COLOR);
    o->selection_color(FL_BACKGROUND_COLOR);
    o->labeltype(FL_NO_LABEL);
    o->labelfont(FL_HELVETICA_BOLD);
//    o->labelfont(0);//ööö
    o->labelsize(14);
    o->labelcolor(FL_FOREGROUND_COLOR);
    o->align(FL_ALIGN_TOP);
    o->when(FL_WHEN_RELEASE);
        { Fl_Pack* o = hpacker = new Fl_Pack(0, 0, 270, 25);
        o->type(1);
            { Fl_Input* o = datanamebutton = new Fl_Input(0, 0, 240, 25);
            o->color(FL_LIGHT1);
            o->callback((Fl_Callback*)cb_filenamebutton, (void*)(this));
            o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
            Fl_Group::current()->resizable(o);
            }
            { Fl_Menu_Button* o = featuremenu = new Fl_Menu_Button(240, 0, 30, 25);
            o->box(FL_THIN_UP_BOX);
            o->user_data((void*)(this));
            o->menu(NULL);
            }
        resizable(datanamebutton);
        o->end();
        }
        { Fl_Box* o = thumbnail = new Fl_Box(0, 25, 270, 65);
        o->box(FL_EMBOSSED_BOX);
        o->hide();
        image( new Fl_RGB_Image(thumbnail_image, thumbnail_size, thumbnail_size, 1));
        image( NULL);
        }
        { Fl_Pack* o = extras = new Fl_Pack(0, 90, 270, 40);
    o->end();
        }
    type(VERTICAL);
    resizable(extras);
    name(n);
    datamanagement.add_datawidget(this);
    end();
    }

// *** end FLUID ***

datawidget_base::~datawidget_base ()
    {
	cout<<"~datawidget_base ()"<<endl;
    datamanagement.remove_datawidget(this);

    delete image();
    image(NULL);

    if (thumbnail_image != NULL){
		delete [] thumbnail_image;
		cout<<"delete [] thumbnail_image"<<endl;
	}

	/*
	Fl_Pack *hpacker;
    Fl_Input *datanamebutton;
    void cb_filenamebutton_i(Fl_Input*, void*);
    static void cb_filenamebutton(Fl_Input*, void*);
	static void edit_geometry_callback(Fl_Widget *callingwidget, void *);
	Fl_Menu_Button *featuremenu;

    // *** thumbnail
    const static int thumbnail_size;
    uchar * thumbnail_image;
    Fl_Box *thumbnail;
    
    // *** menus       
    enum {remove_mi_num=0,save_mi_num, dup_mi_num};
    const static Fl_Menu_Item menu_featuremenu_base[];
    const static Fl_Menu_Item *remove_mi;
    const static Fl_Menu_Item *save_vtk_mi;
    const static Fl_Menu_Item *duplicate_mi;
    Fl_Pack *extras;

	FLTKgeom_base *geom_widget; //JK

	*/

    }

void datawidget_base::refresh_thumbnail ()
    {
    rendermanagement.render_thumbnail(thumbnail_image, thumbnail_size, thumbnail_size, data_id);
    }

int datawidget_base::get_data_id() const
    {
    return data_id;
    }

const string datawidget_base::name() const
    {
    return std::string(_name);
    }

void datawidget_base::name(std::string n)
    {
    _name.assign(n);

    //datanamebutton->value(NULL);
    //((Fl_Input *)datanamebutton)->value(n.c_str());
    datanamebutton->value(_name.c_str());

    //when interactively changed, redrawing widget is
    //done elsewhere (most notably in datamanagement.set_image_name( ... )
    }

void datawidget_base::from_file(bool f)
    {
    fromFile = f;
    }

bool datawidget_base::from_file() const
    {
    return fromFile;
    }

void datawidget_base::show_hide_edit_geometry()
{
	if(geom_widget==NULL){
		geom_widget = new FLTKgeom_image(data_id); //JK
		extras->add(geom_widget);
		geom_widget->hide();
		geom_widget->show();
	}else{
		if(geom_widget->visible()){
			geom_widget->hide();
		}else{
			geom_widget->show();
		}
	}
}


#pragma mark datawidget<image_base>

const Fl_Menu_Item datawidget<image_base>::tfunctionmenu = {"Transfer function", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0};
const Fl_Menu_Item datawidget<image_base>::transferfunction_mi = {"Show/hide", 0,  (Fl_Callback*)cb_show_hide_tfunction, 0, 128, FL_NORMAL_LABEL, 0, 14, 0};

datawidget<image_base>::datawidget(image_base* im, std::string n): datawidget_base (im,n)
{
    { Fl_Group* o = tfunction_ = new Fl_Group(0, 90, 270, 40);
        o->box(FL_EMBOSSED_FRAME);
        o->labelsize(11);
        o->align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
        o->hide();
        o->end();
    }
    extras->add(tfunction_);

	//JK Cannot get geometry data yet since data has not been added to datamanagement 
	geom_widget=NULL;			
//    extras->add(geom_widget);

   
    int fMenuSize = fl_menu_size (menu_featuremenu_base);
    menu_featuremenu_plustf = new Fl_Menu_Item[fMenuSize+1+1];

    for (int fmenuindex = 0; fmenuindex < fMenuSize ; fmenuindex++)
        {        
        menu_featuremenu_plustf [fmenuindex] = menu_featuremenu_base[fmenuindex];
        }
    
    menu_featuremenu_plustf[fMenuSize] = tfunctionmenu;
    //setup_transfer_menu (menu_featuremenu_plustf[fMenuSize],im);

    {
        Fl_Menu_Item showhide_item;
        const int TFSUBNUMHEADITEMS = 1;
        
        showhide_item.label("Show/hide");
        showhide_item.shortcut (0);
        showhide_item.callback(static_cast<Fl_Callback*>(cb_show_hide_tfunction));
        showhide_item.user_data(NULL);
        showhide_item.flags = 128;
        showhide_item.labeltype(FL_NORMAL_LABEL);
        showhide_item.labelfont(0);
        showhide_item.labelsize(14);
        showhide_item.labelcolor(0);
        
        Fl_Menu_Item * tfunctions = transfer_manufactured::factory.function_menu(static_cast<Fl_Callback*>(cb_transferswitch));
        
        int subMenuSize = TFSUBNUMHEADITEMS+fl_menu_size (tfunctions);
        
        tfunction_submenu = new Fl_Menu_Item [subMenuSize+1];
        
        tfunction_submenu[0] = showhide_item;
        
        for (int i = TFSUBNUMHEADITEMS; i < subMenuSize ; i++)
            {
            int fmenuindex = i-TFSUBNUMHEADITEMS;
            
            tfunction_submenu [i] = tfunctions[fmenuindex];
            
            tfunction_submenu [i].user_data(new transferfactory::tf_menu_params (transferfactory::tf_name(fmenuindex), im ));
            }
        tfunction_submenu[subMenuSize].label(NULL);
        tfunction_submenu[TFSUBNUMHEADITEMS].set();
        
        menu_featuremenu_plustf[fMenuSize].flags = FL_SUBMENU_POINTER;
        menu_featuremenu_plustf[fMenuSize].user_data (tfunction_submenu);
        
        delete [] tfunctions;        
    }
    
    menu_featuremenu_plustf[fMenuSize+1].label(NULL);
    
    featuremenu->menu(menu_featuremenu_plustf);


    }

datawidget<image_base>::~datawidget()
{
    fl_menu_userdata_delete(tfunction_submenu);
    delete [] menu_featuremenu_plustf;
    delete [] tfunction_submenu;
}

void datawidget<image_base>::setup_transfer_menu(Fl_Menu_Item* submenuitem, image_base * im) {
    Fl_Menu_Item showhide_item;
    const int TFSUBNUMHEADITEMS = 1;
    
    showhide_item.label("Show/hide");
    showhide_item.shortcut (0);
    showhide_item.callback(static_cast<Fl_Callback*>(cb_show_hide_tfunction));
    showhide_item.user_data(NULL);
    showhide_item.flags = 128;
    showhide_item.labeltype(FL_NORMAL_LABEL);
    showhide_item.labelfont(0);
    showhide_item.labelsize(14);
    showhide_item.labelcolor(0);
    
    Fl_Menu_Item * tfunctions = transfer_manufactured::factory.function_menu(static_cast<Fl_Callback*>(cb_transferswitch));
    
    int subMenuSize = TFSUBNUMHEADITEMS+fl_menu_size (tfunctions);
    
    tfunction_submenu = new Fl_Menu_Item [subMenuSize+1];
    
    tfunction_submenu[0] = showhide_item;
    
    for (int i = TFSUBNUMHEADITEMS; i < subMenuSize ; i++)
        {
        int fmenuindex = i-TFSUBNUMHEADITEMS;
        
        tfunction_submenu [i] = tfunctions[fmenuindex];
        
        tfunction_submenu [i].user_data(new transferfactory::tf_menu_params (transferfactory::tf_name(fmenuindex), im ));
        }
    tfunction_submenu[subMenuSize].label(NULL);
    tfunction_submenu[TFSUBNUMHEADITEMS].set();
    
    submenuitem->flags = FL_SUBMENU_POINTER;
    submenuitem->user_data (tfunction_submenu);
    
    delete [] tfunctions;
}

Fl_Group * datawidget<image_base>::reset_tf_controls()
{
    tfunction_->clear();
    tfunction_->begin();
    return tfunction_;
}

void datawidget<image_base>::cb_transferswitch(Fl_Widget* callingwidget, void* v) {
	cout<<"cb_transferswitch"<<endl;
	transferfactory::tf_menu_params * par = reinterpret_cast<transferfactory::tf_menu_params *>(v);
	par->switch_tf();

	//Each item has space for a callback function and an argument for that function. 
	//Due to back compatability, the Fl_Menu_Item itself is not passed to the callback, 
	//instead you have to get it by calling ((Fl_Menu_*)w)->mvalue() where w is the widget argument.
	const Fl_Menu_Item * item = reinterpret_cast<Fl_Menu_*>(callingwidget)->mvalue();
	const_cast<Fl_Menu_Item *>(item)->setonly();

	datawidget<image_base> * the_datawidget = dynamic_cast<datawidget<image_base> * >(reinterpret_cast<datawidget_base *>(callingwidget->user_data()));
	pt_error::error_if_null(the_datawidget,"cb_transferswitch called with datawidget type not being datawidget<image_base>",pt_error::fatal);

	//    the_datawidget->tfunction_->hide();
	//resize the the current "extras" Fl_Pack...
	the_datawidget->extras->size(the_datawidget->extras->w(),the_datawidget->extras->h()+the_datawidget->tfunction_->h());
	the_datawidget->tfunction_->show();	//Always show the GUI for a selected transfer function 
	the_datawidget->parent()->parent()->redraw();	
}


void datawidget<image_base>::cb_show_hide_tfunction(Fl_Widget* callingwidget, void*)
{
	cout<<"cb_show_hide_tfunction"<<endl;
    //datawidget_base * the_datawidget_base=reinterpret_cast<datawidget_base *>(callingwidget->user_data());
    datawidget<image_base> * the_datawidget = dynamic_cast<datawidget<image_base> * >(reinterpret_cast<datawidget_base *>(callingwidget->user_data()));
    
    pt_error::error_if_null(the_datawidget,"cb_show_hide_tfunction called with datawidget type not being datawidget<image_base>",pt_error::fatal);
    
    if (the_datawidget->tfunction_->visible())
        {
        the_datawidget->tfunction_->hide();
        }
    else
        { 
        the_datawidget->extras->size(the_datawidget->extras->w(),the_datawidget->extras->h()+the_datawidget->tfunction_->h());
        the_datawidget->tfunction_->show();
        }
    
    the_datawidget->parent()->parent()->redraw();
}



#pragma mark datawidget<point_collection>

datawidget<point_collection>::datawidget (point_collection* p, std::string n): datawidget_base (p,n)
{
    featuremenu->menu(menu_featuremenu_base);
    
    //TODO: disable Save as VTK
}




//-----------------------------
FLTKVector3D::FLTKVector3D(Vector3D v, int x, int y, int w, int h, const char *sx, const char *sy, const char *sz):Fl_Group(x,y,w,h)
{
	int dh = int(float(h)/3.0);
	data_x = new Fl_Value_Input(x+7,y		,w-7,dh-2,sx);
	data_y = new Fl_Value_Input(x+7,y+dh	,w-7,dh-2,sy);
	data_z = new Fl_Value_Input(x+7,y+2*dh	,w-7,dh-2,sz);

	data_x->callback(vector_cb);	data_x->when(FL_WHEN_RELEASE);
	data_y->callback(vector_cb);	data_y->when(FL_WHEN_RELEASE);
	data_z->callback(vector_cb);	data_z->when(FL_WHEN_RELEASE);

	value(v);

	end();
}	

void FLTKVector3D::value(Vector3D v)
{
	data_x->value(v[0]);
	data_y->value(v[1]);
	data_z->value(v[2]);
}

Vector3D FLTKVector3D::value()
{
	Vector3D v;
	v[0] = data_x->value();
	v[1] = data_y->value();
	v[2] = data_z->value();
	return v;
}

void FLTKVector3D::vector_cb(Fl_Widget *w, void*)
{
//	cout<<"vector_cb(Fl_Widget *w, void*)"<<endl;
	FLTKVector3D* v3D = (FLTKVector3D*)w->parent();
	v3D->do_callback(v3D);
}
//-----------------------------
FLTKMatrix3D::FLTKMatrix3D(Matrix3D m, int x, int y, int w, int h):Fl_Group(x,y,w,h)
{
	int dh = int(float(h)/3.0);
	int dw = int(float(w)/3.0);
	data_00 = new Fl_Value_Input(x+5	,y		,dw-5,dh-2);
	data_01 = new Fl_Value_Input(x+5	,y+dh	,dw-5,dh-2);
	data_02 = new Fl_Value_Input(x+5	,y+2*dh	,dw-5,dh-2);
	data_10 = new Fl_Value_Input(x+5+dw	,y		,dw-5,dh-2);
	data_11 = new Fl_Value_Input(x+5+dw	,y+dh	,dw-5,dh-2);
	data_12 = new Fl_Value_Input(x+5+dw	,y+2*dh	,dw-5,dh-2);
	data_20 = new Fl_Value_Input(x+5+2*dw,y		,dw-5,dh-2);
	data_21 = new Fl_Value_Input(x+5+2*dw,y+dh	,dw-5,dh-2);
	data_22 = new Fl_Value_Input(x+5+2*dw,y+2*dh,dw-5,dh-2);

	data_00->callback(matrix_cb);	data_00->when(FL_WHEN_RELEASE);
	data_01->callback(matrix_cb);	data_01->when(FL_WHEN_RELEASE);
	data_02->callback(matrix_cb);	data_02->when(FL_WHEN_RELEASE);
	data_10->callback(matrix_cb);	data_10->when(FL_WHEN_RELEASE);
	data_11->callback(matrix_cb);	data_11->when(FL_WHEN_RELEASE);
	data_12->callback(matrix_cb);	data_12->when(FL_WHEN_RELEASE);
	data_20->callback(matrix_cb);	data_20->when(FL_WHEN_RELEASE);
	data_21->callback(matrix_cb);	data_21->when(FL_WHEN_RELEASE);
	data_22->callback(matrix_cb);	data_22->when(FL_WHEN_RELEASE);

	value(m);

	end();
}	

void FLTKMatrix3D::value(Matrix3D m)
{
	data_00->value(m[0][0]);
	data_01->value(m[0][1]);
	data_02->value(m[0][2]);

	data_10->value(m[1][0]);
	data_11->value(m[1][1]);
	data_12->value(m[1][2]);

	data_20->value(m[2][0]);
	data_21->value(m[2][1]);
	data_22->value(m[2][2]);
}

Matrix3D FLTKMatrix3D::value()
{
	Matrix3D m;
	m[0][0] = data_00->value();
	m[0][1] = data_01->value();
	m[0][2] = data_02->value();

	m[1][0] = data_10->value();
	m[1][1] = data_11->value();
	m[1][2] = data_12->value();

	m[2][0] = data_20->value();
	m[2][1] = data_21->value();
	m[2][2] = data_22->value();
	return m;
}
void FLTKMatrix3D::matrix_cb(Fl_Widget *w, void*)
{
//	cout<<"matrix_cb(Fl_Widget *w, void*)"<<endl;
	FLTKMatrix3D* m3D = (FLTKMatrix3D*)w->parent();
	m3D->do_callback(m3D);
}


//--------------------------
#pragma mark FLTKgeom_base
FLTKgeom_base::FLTKgeom_base(int id, int x, int y, int w, int h):Fl_Group(x,y,w,h)
{
    data_id = id;
//	end();
}

//FLTKgeom_image::FLTKgeom_image(image_base *im):FLTKgeom_image(im->get_id()){}


FLTKgeom_image::FLTKgeom_image(int id, int x, int y, int w, int h):FLTKgeom_base(id,x,y,w,h)
{
//	cout<<"origin="<<datamanagement.get_image(data_id)->get_origin()<<endl;
	orig = new FLTKVector3D(datamanagement.get_image(data_id)->get_origin(),x,y,20,h,"x","y","z");
	size = new FLTKVector3D(datamanagement.get_image(data_id)->get_voxel_size(),x+20,y,20,h,"dx","dy","dz");
	orient = new FLTKMatrix3D(datamanagement.get_image(data_id)->get_orientation(),x+45,y,w-45,h);

	orig->callback(orig_update_cb);
	size->callback(size_update_cb);
	orient->callback(orient_update_cb);

	end();
}

void FLTKgeom_image::orig_update_cb(Fl_Widget *w, void*)
{
	FLTKVector3D *v = (FLTKVector3D*)w;
	FLTKgeom_image *g = (FLTKgeom_image*)v->parent();
	datamanagement.get_image(g->data_id)->set_origin(v->value());	
	datamanagement.data_has_changed(g->data_id);
}

void FLTKgeom_image::size_update_cb(Fl_Widget *w, void*)
{
	FLTKVector3D *v = (FLTKVector3D*)w;
	FLTKgeom_image *g = (FLTKgeom_image*)v->parent();
	datamanagement.get_image(g->data_id)->set_voxel_size(v->value());
	datamanagement.data_has_changed(g->data_id);
}

void FLTKgeom_image::orient_update_cb(Fl_Widget *w, void*)
{
	FLTKMatrix3D *m = (FLTKMatrix3D*)w;
	FLTKgeom_image *g = (FLTKgeom_image*)m->parent();
	datamanagement.get_image(g->data_id)->set_orientation(m->value());
	datamanagement.data_has_changed(g->data_id);
}


//	static void orient_update_cb(Fl_Widget *w, void*);