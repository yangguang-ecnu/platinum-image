// $Id$

///  Window for importing single/multiple dicom images
///	 This file is adapted from "sortapp.cxx" example from The Fl_Table class (Version 3.12, 04/02/2006 )
///  created by Greg Ercolano (http://seriss.com/people/erco/fltk/).

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

#define __dicom_importer_cc__

#include "dicom_importer.h"
//#include <sstream>
#include <iostream>


DcmTable::DcmTable(int x, int y, int w, int h, const char *l) : Fl_Table_Row(x,y,w,h,l)
{
	_sort_reverse = 0;
	_sort_lastcol = -1;
	end();
	callback(event_callback, (void*)this);



	string dcm_file = "C:\\Joel\\TMP\\1.dcm";
	itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();
	if (dicomIO->CanReadFile(dcm_file.c_str()))
	{
		dicomIO->SetFileName(dcm_file.c_str());
		dicomIO->ReadImageInformation();		//get basic DICOM header
	}

	dcmtags.load_from_csvfile("C:\\Joel\\TMP\\joel_dcm_tags.csv");
	// The loaded file has this content....
	//;0010;0010;
	//Study Date;0008;0020;
	//Series Date;0008;0021;
	//Acquisition Date;0008;0022;
	//Image Date;0008;0023;
	//Modality;0008;0060;

	data = stringmatrix(4,dcmtags.rows(),"*data*");
	data.set(0,0,"1");
	data.set(1,0,"6");
	data.set(2,0,"5");
	data.set(3,0,"2");
	data.set(4,0,"8");

	rows((int)data.rows());
	cols((int)data.cols());

	// Auto-calculate widths, with 20 pixel padding
	autowidth(20);

	print_all();
}

DcmTable::~DcmTable()
{ }


// Handle drawing all cells in table
void DcmTable::draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H)
{
	char *s = "";

	switch ( context )
	{
	case CONTEXT_STARTPAGE:
		fl_font(FL_COURIER, 16);
		return;

	case CONTEXT_ROW_HEADER:
		fl_color(FL_RED);
		fl_rectf(X, Y, W, H);
		return;

	case CONTEXT_COL_HEADER:
		fl_push_clip(X, Y, W, H);
		{

			fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, color());
			if ( C < dcmtags.rows() )
			{
				fl_color(FL_BLACK);
				fl_draw(dcmtags.get(C,0).c_str(), X+2, Y, W, H, FL_ALIGN_LEFT, 0, 0);	// +2=pad left

				// DRAW SORT ARROW
				if ( C == _sort_lastcol )
				{
					int xlft = X+(W-6)-8,
						xctr = X+(W-6)-4,
						xrit = X+(W-6)-0,
						ytop = Y+(H/2)-4,
						ybot = Y+(H/2)+4;

					if ( _sort_reverse )
					{
						// ENGRAVED DOWN ARROW
						fl_color(FL_WHITE);
						fl_line(xrit, ytop, xctr, ybot);
						fl_color(41);	// dark gray
						fl_line(xlft, ytop, xrit, ytop);
						fl_line(xlft, ytop, xctr, ybot);
					}
					else
					{
						// ENGRAVED UP ARROW
						fl_color(FL_WHITE);
						fl_line(xrit, ybot, xctr, ytop);
						fl_line(xrit, ybot, xlft, ybot);
						fl_color(41);	// dark gray
						fl_line(xlft, ybot, xctr, ytop);
					}
				}
			}
		}
		fl_pop_clip();
		return;

	case CONTEXT_CELL:
		{
			fl_push_clip(X, Y, W, H);
			{
				// BG COLOR
				fl_color( row_selected(R) ? selection_color() : FL_WHITE);
				fl_rectf(X, Y, W, H);

				fl_color(FL_BLACK);
				
				fl_draw(data.get(R,C).c_str(), X+2, Y, W, H, FL_ALIGN_LEFT);	// +2=pad left

				// BORDER
				fl_color(FL_LIGHT2); 
				fl_rect(X, Y, W, H);
			}
			fl_pop_clip();
			return;
		}

	case CONTEXT_ENDPAGE:
	case CONTEXT_RC_RESIZE:
	case CONTEXT_NONE:
	case CONTEXT_TABLE:
		return;
	}
}

// Automatically set column widths to widest data in each column
void DcmTable::autowidth(int pad)
{
	fl_font(FL_COURIER, 16);

	// Initialize all column widths to lowest value
	for ( int c=0; c<cols(); c++ )
	{ col_width(c, pad); }

	for ( int r=0; r<(int)data.rows(); r++ )
	{
		int w1, w2, h;
		for ( int c=0; c<(int)data.cols(); c++ )
		{

			//			fl_measure(_rowdata[r].words[c].c_str(), w, h, 0);	//let FLTK measure the text w/h

			//jk4 - width test
//			w = fl_width(_rowdata[r].words[c].c_str(),_rowdata[r].words[c].size()); //will include "white characters"
			w1 = fl_width(dcmtags.get(c,0).c_str(),dcmtags.get(c,0).size()); //will include "white characters"
			w2 = fl_width(data.get(r,c).c_str(),data.get(r,c).size()); //will include "white characters"
			w1 = max(w1,w2);
			h = fl_height();

			if ( (w1 + pad) > col_width(c))
			{ col_width(c, w1 + pad); }					//update w if needed...
		}
	}
	table_resized();
	redraw();
}

void DcmTable::print_all()
{
	cout<<"***DATA***"<<endl;
	data.print_all();
	cout<<endl<<"***DCMTAGS***"<<endl;
	dcmtags.print_all();
}


// Callback whenever someone clicks on different parts of the table
void DcmTable::event_callback(Fl_Widget*, void *data)
{
	DcmTable *o = (DcmTable*)data;
	o->event_callback2();
}

void DcmTable::event_callback2()
{
	//	int R = callback_row();			// currently unused
	int C = callback_col();

	TableContext context = callback_context();
	switch ( context )
	{
	case CONTEXT_COL_HEADER:		// someone clicked on column header
		{
			if ( Fl::event() == FL_RELEASE && Fl::event_button() == 1 )
			{
				if ( _sort_lastcol == C )
				{ _sort_reverse ^= 1; }	// Click same column? Toggle sort
				else
				{ _sort_reverse = 0; } 	// Click diff column? Up sort 

//				sort_column(C, _sort_reverse);
				data.sort_table_using_col(C,_sort_reverse);

				cout<<"Sort...+redraw"<<endl;
				redraw();

				_sort_lastcol = C;
			}
			break;
		}

	default:
		return;
	}
}



void dcmimportwin::button_cb(Fl_Button* b, void* bstring)
{
	dcmimportwin *w = (dcmimportwin*)b->parent();
	w->button_cb2(string((const char*)bstring));
}

void dcmimportwin::button_cb2(string s)
{
	cout<<"("<<s<<")"<<endl;

	if(s=="load"){
		//open file/folder chooser... import selected files/folders (incl "subfolders" if checked...)
		//return as a vector of strings...
		//for each file... get the dcm info specified in the DcmTable-"dcmtags" object...
		//fill the DcmTable-"data" with the data....
		cout<<"***"<<incl_subfolder_check_button->value()<<endl;

	}else if(s=="settings"){
		//open an new window (give tag vector as an argument) that handles/modifies the DcmTable-"dcmtags".
		//the "dcmtags" object might be passed on as an argument (pointer of copy constr...)

		settingswin::create(100,100,300,300,"Settings");

	}else if(s=="import"){
		//if file/files are selected -->  import and give the name in the input field
		//geometry information will be difficult to import, as files can be chosen randomly...
		//just set the geometry info to "default" (i.e no rotation, origin (0,0,0), scaling (1,1,1)
		cout<<"*"<<import_volume_input->value()<<endl;

	}else if(s=="close"){

	}else{
		cout<<"*Warning*"<<endl;
	}
}

dcmimportwin* dcmimportwin::create(int xx, int yy, int ww, int hh, const char *ll)
{
	Fl_Group::current(NULL);// *Warning* - If this is forgotten, The window/graphics might end up in 
	return new dcmimportwin(xx,yy,ww,hh,ll);
}


dcmimportwin::dcmimportwin(int xx, int yy, int ww, int hh, const char *ll):Fl_Window(xx,yy,ww,hh,ll)
{

	int wm = 10;	//widget margin
	int wh = 30;	//widget height

	table = new DcmTable(wm, 2*wm+wh, w()-2*wm, h()-4*wm-2*wh);
	table->selection_color(FL_YELLOW);
	table->col_header(1);
	table->col_resize(1);
	table->when(FL_WHEN_RELEASE);		// handle table events on release
	table->print_all();
	table->row_height_all(18);			// height of all rows
	table->end();


	Fl_Button* o;
	o = new Fl_Button(wm, wm, 100, wh, "Load directory");
	o->callback((Fl_Callback*)button_cb, "load");
	//	o->callback((Fl_Callback*)load_button_cb, (void*)this);


	//	Fl_Check_Button* incl_subfolder_check_button = new Fl_Check_Button(100+2*wm, wm, 160, wh, "Include subdirectories");
	incl_subfolder_check_button = new Fl_Check_Button(100+2*wm, wm, 160, wh, "Include subdirectories");
	incl_subfolder_check_button->down_box(FL_DOWN_BOX);
	incl_subfolder_check_button->value(1);


	o = new Fl_Button(wm, h()-wm-wh, 140, wh, "Dicom Tag Settings");
	o->callback((Fl_Callback*)button_cb, "settings");
	//	o->callback((Fl_Callback*)settings_button_cb, (void*)this);

	//	Fl_Input* import_volume_input = new Fl_Input(w()-3*wm-2*65-170, h()-wm-wh, 170, wh, "Import Volume Name");
	import_volume_input = new Fl_Input(w()-3*wm-2*65-170, h()-wm-wh, 170, wh, "Import Volume Name");
	import_volume_input->box(FL_DOWN_BOX);
	import_volume_input->color(FL_BACKGROUND2_COLOR);
	import_volume_input->selection_color(FL_SELECTION_COLOR);
	import_volume_input->labeltype(FL_NORMAL_LABEL);
	import_volume_input->labelfont(0);
	import_volume_input->labelsize(14);
	import_volume_input->labelcolor(FL_FOREGROUND_COLOR);
	//	import_volume_input->callback((Fl_Callback*)volume_name_field_cb, (void*)this);
	import_volume_input->align(FL_ALIGN_LEFT);
	import_volume_input->when(FL_WHEN_RELEASE);

	import_volume_input->value("Dicom import volume");


	o = new Fl_Button(w()-2*wm-2*65, h()-wm-wh, 65, wh, "Import");
	o->callback((Fl_Callback*)button_cb, "import");
	//	o->callback((Fl_Callback*)import_button_cb, (void*)this);

	o = new Fl_Button(w()-wm-65, h()-wm-wh, 65, wh, "Close");
	o->callback((Fl_Callback*)button_cb, "close");
	//	o->callback((Fl_Callback*)close_button_cb, (void*)this);

	end();
	resizable(table);
	show();
}





//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------


settingswin* settingswin::create(int xx, int yy, int ww, int hh, const char *ll)
{
	Fl_Group::current(NULL);// *Warning* - If this is forgotten, The window/graphics might end up in 
	return new settingswin(xx,yy,ww,hh,ll);
}

settingswin::settingswin(int x, int y, int w, int h, const char *l):Fl_Window(x,y,w,h,l)
{
	string_edit_table* table = new string_edit_table(20, 20, w-80, h-80,"",5,3);

	// ROWS
	table->row_header(1);
	table->row_header_width(70);
	table->row_resize(1);
	table->row_height_all(25);

	// COLS
	table->col_header(1);
	table->col_header_height(25);
	table->col_resize(1);
	table->col_width_all(70);

	resizable(table);
	show();
	Fl::run();
}



string_edit_table::string_edit_table(int x, int y, int w, int h, const char *l, int r_nr, int c_nr) : Fl_Table(x,y,w,h,l)
{
	//Just as an example...
	dcmtags.load_from_csvfile("C:\\Joel\\TMP\\joel_dcm_tags.csv");

	callback(&event_callback, (void*)this);
	input = new Fl_Input(w/2,h/2,0,0);
	input->hide();
	input->callback(input_cb, (void*)this);
	input->when(FL_WHEN_ENTER_KEY_ALWAYS);
	input->maximum_size(100);

	(new Fl_Box(9999,9999,0,0))->hide();  // HACK: prevent flickering in Fl_Scroll
	end();

	cols(dcmtags.cols());
	rows(dcmtags.rows());


}

string_edit_table::~string_edit_table()
{}

/*
string string_edit_table::get(int r, int c)
{
	return dcmtags.get(r,c);
}

void string_edit_table::set(int r, int c, string val)
{
	if(r>=0 && c>=0 && r<rows() && c<cols())
	{
		vector<string> row = values[r];
		row[c] = val;
		values[r] = row;
	}
}

void string_edit_table::print_all()
{
	for(int r=0;r<rows();r++){
		for(int c=0;c<cols();c++){
			cout<<get(r,c)<<" ";
		}
		cout<<endl;
	}
}
*/


void string_edit_table::set_value()
{
	//	values[row_edit][col_edit] = atoi(input->value()); 
	dcmtags.set( row_edit,col_edit,string(input->value()) ); 
	input->hide(); 
	cout<<"string(input->value())="<<string(input->value())<<endl;
	dcmtags.print_all();
}


// Handle drawing all cells in table
void string_edit_table::draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H)
{
	static char s[100];

	switch ( context )
	{
	case CONTEXT_COL_HEADER:
		fl_font(FL_HELVETICA | FL_BOLD, 14);
		fl_push_clip(X, Y, W, H);
		{
			fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, col_header_color());
			fl_color(FL_BLACK);

			if(C==0){fl_draw("Title:", X, Y, W, H, FL_ALIGN_CENTER);}
			else if(C==1){fl_draw("Tag Group:", X, Y, W, H, FL_ALIGN_CENTER);}
			else if(C==2){fl_draw("Tag Name:", X, Y, W, H, FL_ALIGN_CENTER);}

		}
		fl_pop_clip();
		return;

	case CONTEXT_ROW_HEADER:
		fl_font(FL_HELVETICA | FL_BOLD, 14);
		fl_push_clip(X, Y, W, H);
		{
			fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, row_header_color());
			fl_color(FL_BLACK);

			sprintf(s, "%d", R);
			fl_draw(s, X, Y, W, H, FL_ALIGN_CENTER);

		}
		fl_pop_clip();
		return;

	case CONTEXT_CELL:
		{
			if (R == row_edit && C == col_edit && input->visible())
			{ return; }

			// BACKGROUND
			fl_push_clip(X, Y, W, H);
			fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, FL_WHITE);
			fl_pop_clip();

			// TEXT
			fl_push_clip(X+3, Y+3, W-6, H-6);
			{
				fl_color(FL_BLACK);

				if (C <dcmtags.cols() && R <dcmtags.rows())
				{
					fl_font(FL_HELVETICA, 14);
					sprintf(s, "%s", dcmtags.get(R,C).c_str());
					fl_draw(s, X+3, Y+3, W-6, H-6, FL_ALIGN_LEFT);
				}
			}
			fl_pop_clip();

			return;
		}

	case CONTEXT_RC_RESIZE:
		{
			if (!input->visible()) return;
			find_cell(CONTEXT_TABLE, row_edit, col_edit, X, Y, W, H);
			if (X==input->x() && Y==input->y() && W==input->w() && H==input->h()) return;
			input->resize(X,Y,W,H);
			return;
		}

	default:
		return;
	}
}


void string_edit_table::event_callback(Fl_Widget*, void *data)
{
	string_edit_table *o = (string_edit_table*)data;
	o->event_callback2();
}

void string_edit_table::event_callback2()
{
	int R = callback_row(),
		C = callback_col();
	TableContext context = callback_context();

	switch ( context )
	{
	case CONTEXT_CELL:
		{
			//			if (C == cols()-1 || R == rows()-1) return;
			if (input->visible()) 
				input->do_callback();
			row_edit = R;
			col_edit = C;

			int XX,YY,WW,HH;
			find_cell(CONTEXT_CELL, R, C, XX, YY, WW, HH);
			input->resize(XX,YY,WW,HH);
			input->value(dcmtags.get(R,C).c_str());
			input->show();
			input->take_focus();
			return;
		}
	default:
		return;
	}
}



void input_cb(Fl_Widget*, void* v)
{ 
	((string_edit_table*)v)->set_value(); 
}
