//////////////////////////////////////////////////////////////////////////
//
//   Dicom_importer $Revision: $
///
///  Window for importing single/multiple dicom images
///	 This file is adapted from "sortapp.cxx" example from The Fl_Table class (Version 3.12, 04/02/2006 )
///  created by Greg Ercolano (http://seriss.com/people/erco/fltk/).

//  sortapp.cxx -- Test the Fl_Table_Row class with an actual app
//  Click on the headers to sort that column.

//  singleinput.cxx -- Sample table with a single Fl_Input for editing cells
//  1.00 04/18/03 Mister Satan -- Initial implementation
//  1.10 05/17/03 Greg Ercolano -- Small mods to follow changes to Fl_Table

//   $LastChangedBy: $

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
#include "Fl_Table_Row.H"

//#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Value_Slider.H>

#include "itkGDCMImageIO.h"


//#ifdef MICROSOFT
#define popen _popen
//#endif /*MICROSOFT*/

using namespace std;

class DcmTagEntry
{
public:
	DcmTagEntry(); 
	DcmTagEntry(string t, string tgroup, string tname); 
	DcmTagEntry(const DcmTagEntry &d); 
	string get_tagkey();

	string title;
	string tag_group;
	string tag_name;
};


// A row of columns
class Row
{
public:
	vector<string> words;
};

// Sort class to handle sorting strings by column
class SortColumn
{
	int _col, _reverse;
public:
	SortColumn(int col, int reverse);				//sets variables
	bool operator()(const Row &a, const Row &b);	//compares strings (when inside the column range)
};


class DcmTable : public Fl_Table_Row
{
private:
	vector<Row> _rowdata;	// data in each row
	int _maxcols;				// max # columns in all rows
	int _sort_reverse;			// sort direction
	int _sort_lastcol;			// buffers the last "sorting column"

	static void event_callback(Fl_Widget*, void*);	// static callback for table events
	void event_callback2();							// callback for table events

protected:
	// table cell drawing 
	// Column headers are bold, an arrow displays the "column that holds the sorting"
	// Selected rows are given the "selection_color"
	void draw_cell(TableContext context, int R=0, int C=0, int X=0, int Y=0, int W=0, int H=0);
	void sort_column(int col, int reverse=0);			// sort table by a column - uses stl::sort

public:
	DcmTable(int x, int y, int w, int h, const char *l=0);
	~DcmTable();
	vector<DcmTagEntry> dcmtags;

	void load_testdata();				// load test data
	void print_all();
	void load_command(const char *cmd);	// load the output of a command into table - for example load_command("dir");
	void autowidth(int pad);			// automatically set column widths to data maximum width
};




class dcmimportwin : public Fl_Window
{
public:
	dcmimportwin(int xx, int yy, int ww, int hh, const char *ll=0);

private:
	DcmTable *table;
	Fl_Check_Button* incl_subfolder_check_button;
	Fl_Input* import_volume_input;

	static void button_cb(Fl_Button* b, void* bstring);
	void button_cb2(string s);
};


//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------

class settingswin : public Fl_Window
{
public:
	settingswin(int x, int y, int w, int h, const char *l=0);
};




const int MAX_COLS = 4;
const int MAX_ROWS = 50;

Fl_Callback input_cb;

void input_cb(Fl_Widget*, void* v);
void setcols_cb(Fl_Widget* w, void* v);// Change number of columns
void setrows_cb(Fl_Widget* w, void* v);// Change number of rows

class SingleInput : public Fl_Table
{
	Fl_Int_Input* input;
//	int values[MAX_ROWS][MAX_COLS];
	string values[MAX_ROWS][MAX_COLS];
//	string values[100][4];
	int row_edit, col_edit;

protected:
	void draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H); // Handle drawing all cells in table
	static void event_callback(Fl_Widget*, void*);// Callback whenever someone clicks on different parts of the table
	void event_callback2();

public:
	SingleInput(int x, int y, int w, int h, const char *l=0);
	~SingleInput();

	void set_value();
	void rows(int val);
	void cols(int val);
	inline int rows();
	inline int cols();
};


#endif