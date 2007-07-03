// $Id: $

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

DcmTagEntry::DcmTagEntry()
{
	title = "";
	tag_group = "";
	tag_name = "";
}

DcmTagEntry::DcmTagEntry(string t, string tgroup, string tname)
{
	title = t;
	tag_group = tgroup;
	tag_name = tname;
}

DcmTagEntry::DcmTagEntry(const DcmTagEntry &d)
{
	title = d.title;
	tag_group = d.tag_group;
	tag_name = d.tag_name;
}

string DcmTagEntry::get_tagkey()
{
	return tag_group+"|"+tag_name;
}



SortColumn::SortColumn(int col, int reverse)
{
	_col = col;
	_reverse = reverse;
}


bool SortColumn::operator()(const Row &a, const Row &b)
{
	const char *ap = ( _col < (int)a.words.size() ) ? a.words[_col].c_str() : ""; 
	const char *bp = ( _col < (int)b.words.size() ) ? b.words[_col].c_str() : "";
	if ( _reverse ) { return(strcmp(ap, bp) > 0); }
	else            { return(strcmp(ap, bp) < 0); }
}


DcmTable::DcmTable(int x, int y, int w, int h, const char *l) : Fl_Table_Row(x,y,w,h,l)
{
	_maxcols = 0;
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

	//Study Date (0008,0020)
	//Series Date (0008,0021)
	//Acquisition Date (0008,0022)
	//Image Date (0008,0023)
	//Modality (0008,0060)
	dcmtags.push_back( DcmTagEntry("Patient Name","0010","0010") );
	dcmtags.push_back( DcmTagEntry("Study Date","0008","0020") );
	dcmtags.push_back( DcmTagEntry("Modality","0008","0060") );

}

DcmTable::~DcmTable()
{ }

// Sort a column up or down
void DcmTable::sort_column(int col, int reverse)
{
	sort(_rowdata.begin(), _rowdata.end(), SortColumn(col, reverse));
	redraw();
}

// Handle drawing all cells in table
void DcmTable::draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H)
{
	char *s = "";

	if ( R < (int)_rowdata.size() && C < (int)_rowdata[R].words.size() )
		s = const_cast<char *>(_rowdata[R].words[C].c_str());
	//		s = _rowdata[R].words[C];

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
			static char *head[] = 
				//#ifdef MICROSOFT
			{ "Date", "Time", "Size", "Filename", "", "", "", "", "" };
			//#else //MICROSOFT
			//			{ "Perms", "#L", "Own", "Group", "Size", "Date", "", "", "Filename" };
			//#endif //MICROSOFT

			fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, color());
			if ( C < 9 )
			{
				fl_color(FL_BLACK);
				fl_draw(head[C], X+2, Y, W, H, FL_ALIGN_LEFT, 0, 0);	// +2=pad left

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
				fl_draw(s, X+2, Y, W, H, FL_ALIGN_LEFT);	// +2=pad left

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

	for ( int r=0; r<(int)_rowdata.size(); r++ )
	{
		int w, h;
		for ( int c=0; c<(int)_rowdata[r].words.size(); c++ )
		{

			//			fl_measure(_rowdata[r].words[c].c_str(), w, h, 0);	//let FLTK measure the text w/h

			//jk4 - width test
			w = fl_width(_rowdata[r].words[c].c_str(),_rowdata[r].words[c].size()); //will include "white characters"
			h = fl_height();

			if ( (w + pad) > col_width(c))
			{ col_width(c, w + pad); }					//update w if needed...
		}
	}
	table_resized();
	redraw();
}

void DcmTable::load_testdata()
{
	string word;

	for(int r=0; r<10; r++)
	{
		// new row
		Row newrow;
		_rowdata.push_back(newrow);

		for(int c=0; c<10; c++)
		{
			if(c<dcmtags.size())
			{
				_rowdata[r].words.push_back(dcmtags[c].title);		//adds data to "table"
			}else{
				_rowdata[r].words.push_back("hej");		//adds data to "table"
			}
		}

		// keep track of most # columns
		if ( (int)_rowdata[r].words.size() > _maxcols )
		{ 
			_maxcols = (int)_rowdata[r].words.size(); 
		}
	}

	// Initialize table rows/cols based on data loaded
	rows((int)_rowdata.size());
	cols(_maxcols);

	// Auto-calculate widths, with 20 pixel padding
	autowidth(20);
}

void DcmTable::print_all()
{
	for(int r=0; r<rows(); r++)
	{
		for(int c=0; c<cols(); c++)
		{
			cout<<_rowdata[r].words[c]<<",";
		}
		cout<<endl;
	}
}

// Load table with output of 'cmd' - for example load_command("dir");
void DcmTable::load_command(const char *cmd)
{
	char s[512];
	FILE *fp = popen(cmd, "r");

	_maxcols = 0;

	for ( int r=0; fgets(s, sizeof(s)-1, fp); r++ )
	{
		if ( strlen(s) > 1 )
			s[strlen(s)-1] = 0;

		// new row
		Row newrow;
		_rowdata.push_back(newrow);

		char *ss = s, *word = s;
		while ( *ss )
		{
			// Found white space? 
			if ( strchr(" \t", *ss) )
			{
				// Make previous data a word, add to column
				{
					char tmp = *ss;
					*ss = 0;
					_rowdata[r].words.push_back(strdup(word));		//adds data to "table"
					*ss = tmp;
				}

				// Skip over all white
				while ( strchr(" \t", *ss) )
				{ ++ss; }

				// Next word starts at first non-white
				word = ss;
			}
			ss++;
		}

		// last word
		_rowdata[r].words.push_back(strdup(word));			//adds data to "table"

		// keep track of most # columns
		if ( (int)_rowdata[r].words.size() > _maxcols )
		{ _maxcols = (int)_rowdata[r].words.size(); }
	}

	// Initialize table rows/cols based on data loaded
	rows((int)_rowdata.size());
	cols(_maxcols);

	// Auto-calculate widths, with 20 pixel padding
	autowidth(20);
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

				sort_column(C, _sort_reverse);
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
		cout<<"***"<<incl_subfolder_check_button->value()<<endl;

	}else if(s=="settings"){
		//table->dcmtags
		//open an new window (give tag vector as an argument) that handles/modifies the tag vector.
		new settingswin(100,100,300,300,"Settings");

	}else if(s=="import"){
		//if file/files are selected -->  import and give the name in the input field
		//geometry information will be difficult to import, as files can be chosen randomly...
		cout<<"*"<<import_volume_input->value()<<endl;

	}else if(s=="close"){

	}else{
		cout<<"*Warning*"<<endl;
	}
}


dcmimportwin::dcmimportwin(int xx, int yy, int ww, int hh, const char *ll):Fl_Window(xx,yy,ww,hh,ll)
{
//	make_current();	// *Warning* - If this is forgotten, The window/graphics might end up in 
//	parent(NULL);	// *Warning* - If this is forgotten, The window/graphics might end up in 
	// unintended locations...

	int wm = 10;	//widget margin
	int wh = 30;	//widget height

	table = new DcmTable(wm, 2*wm+wh, w()-2*wm, h()-4*wm-2*wh);
	table->selection_color(FL_YELLOW);
	table->col_header(1);
	table->col_resize(1);
	table->when(FL_WHEN_RELEASE);		// handle table events on release
	table->load_testdata();
	//	table->load_command("dir");			// load table with a directory listing
	table->print_all();
	table->row_height_all(18);			// height of all rows
	table->end();


	Fl_Button* o;
	o = new Fl_Button(wm, wm, 100, wh, "Load directory");
	o->callback((Fl_Callback*)button_cb, (void*)"load");
	//	o->callback((Fl_Callback*)load_button_cb, (void*)this);


	//	Fl_Check_Button* incl_subfolder_check_button = new Fl_Check_Button(100+2*wm, wm, 160, wh, "Include subdirectories");
	incl_subfolder_check_button = new Fl_Check_Button(100+2*wm, wm, 160, wh, "Include subdirectories");
	incl_subfolder_check_button->down_box(FL_DOWN_BOX);
	incl_subfolder_check_button->value(1);


	o = new Fl_Button(wm, h()-wm-wh, 140, wh, "Dicom Tag Settings");
	o->callback((Fl_Callback*)button_cb, (void*)"settings");
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
	o->callback((Fl_Callback*)button_cb, (void*)"import");
	//	o->callback((Fl_Callback*)import_button_cb, (void*)this);

	o = new Fl_Button(w()-wm-65, h()-wm-wh, 65, wh, "Close");
	o->callback((Fl_Callback*)button_cb, (void*)"close");
	//	o->callback((Fl_Callback*)close_button_cb, (void*)this);

	end();
	resizable(table);
	show();
}


//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------

settingswin::settingswin(int x, int y, int w, int h, const char *l):Fl_Window(x,y,w,h,l)
{
	//Fl_parent(NULL); //*Warning*

	Fl_Window win(600, 400);
	win.size_range(1,1);

	SingleInput* table = new SingleInput(20, 20, win.w()-80, win.h()-80);

	// ROWS
	table->row_header(1);
	table->row_header_width(70);
	table->row_resize(1);
	table->rows(10);
	table->row_height_all(25);

	// COLS
	table->col_header(1);
	table->col_header_height(25);
	table->col_resize(1);
	table->cols(4);
	table->col_width_all(70);

	// Add children to window
	win.begin();

	// ROW
	Fl_Value_Slider setrows(win.w()-40,20,20,win.h()-80, 0);
	setrows.type(FL_VERT_NICE_SLIDER);
	setrows.bounds(2,MAX_ROWS);
	setrows.step(1);
	setrows.value(table->rows()-1);
	setrows.callback(setrows_cb, (void*)table);
	setrows.when(FL_WHEN_CHANGED);
	setrows.clear_visible_focus();

	// COL
	Fl_Value_Slider setcols(20,win.h()-40,win.w()-80,20, 0);
	setcols.type(FL_HOR_NICE_SLIDER);
	setcols.bounds(2,MAX_COLS);
	setcols.step(1);
	setcols.value(table->cols()-1);
	setcols.callback(setcols_cb, (void*)table);
	setcols.when(FL_WHEN_CHANGED);
	setcols.clear_visible_focus();

	win.end();
	win.resizable(table);
	win.show();

	Fl::run();
	//return Fl::run();
}



SingleInput::SingleInput(int x, int y, int w, int h, const char *l) : Fl_Table(x,y,w,h,l)
{
	callback(&event_callback, (void*)this);
	input = new Fl_Int_Input(w/2,h/2,0,0);
	input->hide();
	input->callback(input_cb, (void*)this);
	input->when(FL_WHEN_ENTER_KEY_ALWAYS);
	input->maximum_size(5);

	(new Fl_Box(9999,9999,0,0))->hide();  // HACK: prevent flickering in Fl_Scroll
	end();
}

SingleInput::~SingleInput()
{
}



void SingleInput::set_value()
{
//	values[row_edit][col_edit] = atoi(input->value()); 
	values[row_edit][col_edit] = string(input->value()); 
	input->hide(); 
}


void SingleInput::rows(int val) {
	if (input->visible()) 
		input->do_callback(); 
	Fl_Table::rows(val); 
}
void SingleInput::cols(int val) {
	if (input->visible()) 
		input->do_callback(); 
	Fl_Table::cols(val); 
}
int SingleInput::rows() { 
	return Fl_Table::rows(); 
}
int SingleInput::cols() { 
	return Fl_Table::cols(); 
}

// Handle drawing all cells in table

void SingleInput::draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H)
{
	static char s[30];

	switch ( context )
	{
	case CONTEXT_COL_HEADER:
		fl_font(FL_HELVETICA | FL_BOLD, 14);
		fl_push_clip(X, Y, W, H);
		{
			fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, col_header_color());
			fl_color(FL_BLACK);
			if (C != cols()-1)
			{
				s[0] = 'A' + C;
				s[1] = '\0';
				fl_draw(s, X, Y, W, H, FL_ALIGN_CENTER);
			}
			else
			{ fl_draw("TOTAL", X, Y, W, H, FL_ALIGN_CENTER); }
		}
		fl_pop_clip();
		return;

	case CONTEXT_ROW_HEADER:
		fl_font(FL_HELVETICA | FL_BOLD, 14);
		fl_push_clip(X, Y, W, H);
		{
			fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, row_header_color());
			fl_color(FL_BLACK);
			if (R != rows()-1)
			{
				sprintf(s, "%d", R+1);
				fl_draw(s, X, Y, W, H, FL_ALIGN_CENTER);
			}
			else
			{ fl_draw("TOTAL", X, Y, W, H, FL_ALIGN_CENTER); }
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

				if (C != cols()-1 && R != rows()-1)
				{
					fl_font(FL_HELVETICA, 14);
//					sprintf(s, "%d", values[R][C]);
					sprintf(s, "%s", values[R][C].c_str());
					fl_draw(s, X+3, Y+3, W-6, H-6, FL_ALIGN_RIGHT);
				}
/*				else
				{
					int T = 0;
					fl_font(FL_HELVETICA | FL_BOLD, 14);

					if (C == cols()-1 && R == rows()-1)		// TOTAL
					{
						for (int c=0; c<cols()-1; ++c)
							for (int r=0; r<rows()-1; ++r)
								T += values[r][c];
					}
					else if (C == cols()-1)			// ROW SUBTOTAL
					{
						for (int c=0; c<cols()-1; ++c)
							T += values[R][c];
					}
					else if (R == rows()-1)			// COL SUBTOTAL
					{
						for (int r=0; r<rows()-1; ++r)
							T += values[r][C];
					}

					sprintf(s, "%d", T);
					fl_draw(s, X+3, Y+3, W-6, H-6, FL_ALIGN_RIGHT);
				}
*/
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


void SingleInput::event_callback(Fl_Widget*, void *data)
{
	SingleInput *o = (SingleInput*)data;
	o->event_callback2();
}

void SingleInput::event_callback2()
{
	int R = callback_row(),
		C = callback_col();
	TableContext context = callback_context();

	switch ( context )
	{
	case CONTEXT_CELL:
		{
			if (C == cols()-1 || R == rows()-1) return;
			if (input->visible()) input->do_callback();
			row_edit = R;
			col_edit = C;
			int XX,YY,WW,HH;
			find_cell(CONTEXT_CELL, R, C, XX, YY, WW, HH);
			input->resize(XX,YY,WW,HH);
			char s[30];
//			sprintf(s, "%d", values[R][C]);
			sprintf(s, "%s", values[R][C].c_str());
			input->value(s);
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
	((SingleInput*)v)->set_value(); 
}


// Change number of columns
void setcols_cb(Fl_Widget* w, void* v)
{
	SingleInput* table = (SingleInput*)v;
	Fl_Valuator *in = (Fl_Valuator*)w;
	int cols = int(in->value()) + 1;
	table->cols(cols);
}

// Change number of rows
void setrows_cb(Fl_Widget* w, void* v)
{
	SingleInput* table = (SingleInput*)v;
	Fl_Valuator *in = (Fl_Valuator*)w;
	int rows = int(in->value()) + 1;
	table->rows(rows);
}
