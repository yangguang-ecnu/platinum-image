//////////////////////////////////////////////////////////////////////////
//
//   dicom_importer $Revision$

///  Window for importing single/multiple dicom images
///	 This file is adapted from "sortapp.cxx" example from The Fl_Table class (Version 3.12, 04/02/2006 )
///	 created by Greg Ercolano (http://seriss.com/people/erco/fltk/).

///  The "dcmimportwin" uses a "dcmtable" (Fl_Table) to handle dicom meta data.
///  The "dcmtable" uses a "stringmatrix" for the string storage	
///  The "settingswin" uses a "string_edit_table" (Fl_Table) to allow editable settings.
///
///  Window for importing single/multiple dicom images
///	 This file is adapted from "sortapp.cxx" example from The Fl_Table class (Version 3.12, 04/02/2006 )
///  created by Greg Ercolano (http://seriss.com/people/erco/fltk/).
///  sortapp.cxx -- Test the Fl_Table_Row class with an actual app
///  Click on the headers to sort that column.
///
///  singleinput.cxx -- Sample table with a single Fl_Input for editing cells
///  1.00 04/18/03 Mister Satan -- Initial implementation
///  1.10 05/17/03 Greg Ercolano -- Small mods to follow changes to Fl_Table

//   $LastChangedBy$

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

#ifndef __dicom_importer__
#define __dicom_importer__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <algorithm>		// STL sort

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
//#include <FL/Fl_Table_Row.H>
#include "Utilities/fltk/Fl_Table_Row.H"

#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Value_Slider.H>

#include <FL/Fl_File_Chooser.H>
#include <FL/filename.H>

#include "itkGDCMImageIO.h"
#include "stringmatrix.h"
#include "fileutils.h"
#include "error.h"
#include "image_base.h"		//to include class image_loader


//#ifdef MICROSOFT
#ifdef WIN32
#define popen _popen
#endif /*Micro-soft*/

using namespace std;

//----------------------------------------------------------
//----------------------------------------------------------

class dcmtable : public Fl_Table_Row
{
private:
	stringmatrix data;			// contains the loaded dicom header data...
//	int _maxcols;				// max # columns in all rows
	int _sort_reverse;			// sort direction
	int _sort_lastcol;			// buffers the last "sorting column"

	static void event_callback(Fl_Widget*, void*);	// static callback for table events
	void event_callback2();							// callback for table events

	vector<string> selected_filenames;

protected:
	// table cell drawing 
	// Column headers are bold, an arrow displays the "column that holds the sorting"
	// Selected rows are given the "selection_color"
	void draw_cell(TableContext context, int R=0, int C=0, int X=0, int Y=0, int W=0, int H=0);

public:
	dcmtable(int x, int y, int w, int h, const char *l=0);
	~dcmtable();

	stringmatrix dcmtags;		// contains dicom tag info, updated via the "settings window"
	//---- dcmtags specification ----
	//row 0: title			
	//row 1: tag_group
	//row 2: tag_name
//	void load_testdata();				// load test data
	void print_all();
	void fill_table(vector<string> dcm_files);
	void autowidth(int pad);			// automatically set column widths to data maximum width
	void update_tabledata();
	vector<string> get_selected_filenames();
};


//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------

class dcmimportwin : public Fl_Window
{
private:
	dcmimportwin(int xx, int yy, int ww, int hh, const char *ll=0);
	dcmtable *table;
	Fl_Check_Button* incl_subfolder_check_button;
	Fl_Input* import_vol_name_input;

	static void button_cb(Fl_Button* b, const void* bstring);
	void button_cb2(string s);
	
	vector<string> get_dcm_files_from_dir(const char *dir, vector<string> dcm_files, bool incl_sub_dirs);

public:
	//makes sure current(NULL) is not forgotten...
	static dcmimportwin* create(int xx, int yy, int ww, int hh, const char *ll=0);

};

//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
class string_edit_table : public Fl_Table
{
	Fl_Input* input;
	int row_edit;
	int col_edit;

protected:
	void draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H); // Handle drawing all cells in table

	static void event_callback(Fl_Widget*, void*);// Callback whenever someone clicks on different parts of the table
	void event_callback2();

	static void input_cb(Fl_Widget*, void* v);

public:
	string_edit_table(int x, int y, int w, int h, const char *l, int nr_r, int nr_c);
	~string_edit_table();

	stringmatrix dcmtags;
	void set_value();
	void read_from_csvfile(string file);
	void write_to_csvfile(string file);
	void update_tabledata();
};

//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------

class settingswin : public Fl_Window
{
private:
	settingswin(int xx, int yy, int ww, int hh, dcmtable *dt, const char *ll=0);
	string_edit_table* table;
	dcmtable* dcmtable_ptr;
	Fl_Input* ins_row_input;
	Fl_Input* del_row_input;

	static void button_cb(Fl_Button* b, void* bstring);
	void button_cb2(string s);
	void update_tabledata();

public:
	static settingswin* create(int xx, int yy, int ww, int hh, dcmtable *dt, const char *ll=0);
};

//Fl_Callback input_cb;
//void input_cb(Fl_Widget*, void* v);

#endif