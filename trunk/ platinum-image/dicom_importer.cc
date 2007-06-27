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


MyTable::MyTable(int x, int y, int w, int h, const char *l) : Fl_Table_Row(x,y,w,h,l)
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

MyTable::~MyTable()
{ }

// Sort a column up or down
void MyTable::sort_column(int col, int reverse)
{
	sort(_rowdata.begin(), _rowdata.end(), SortColumn(col, reverse));
	redraw();
}

// Handle drawing all cells in table
void MyTable::draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H)
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
void MyTable::autowidth(int pad)
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

void MyTable::load_testdata()
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

void MyTable::print_all()
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
void MyTable::load_command(const char *cmd)
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
void MyTable::event_callback(Fl_Widget*, void *data)
{
	MyTable *o = (MyTable*)data;
	o->event_callback2();
}

void MyTable::event_callback2()
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



dicom_importer::dicom_importer(int xx, int yy, int ww, int hh, const char *ll):Fl_Window(xx,yy,ww,hh,ll)
{
	MyTable *table = new MyTable(20, 20, w()-40, h()-40);
	table->selection_color(FL_YELLOW);
	table->col_header(1);
	table->col_resize(1);
	table->when(FL_WHEN_RELEASE);		// handle table events on release
	table->load_testdata();
	//	table->load_command("dir");			// load table with a directory listing
	table->print_all();
	table->row_height_all(18);			// height of all rows
	table->end();

	end();
	resizable(table);
	show();




	/*
	//This is verified to work.....

	Fl_Window win(900,500);
	MyTable table(20, 20, win.w()-40, win.h()-40);
	table.selection_color(FL_YELLOW);
	table.col_header(1);
	table.col_resize(1);
	table.when(FL_WHEN_RELEASE);		// handle table events on release
	//#if defined(_WIN32)
	table.load_command("dir");			// load table with a directory listing
	//#else
	//	table.load_command("ls -la /var/tmp");	// load table with a directory listing
	// table.load_command("cat README");	// load table with a directory listing
	//#endif
	table.row_height_all(18);			// height of all rows
	win.end();
	win.resizable(table);
	win.show();
	Fl::run();
	*/


}
