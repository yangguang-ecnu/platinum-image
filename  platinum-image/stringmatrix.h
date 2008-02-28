//////////////////////////////////////////////////////////////////////////
//
//   stringmatrix $Revision: $
///
///  Matrix class that handles a 2D array of strings, based on the stl::vector class

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

#ifndef __stringmatrix__
#define __stringmatrix__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <algorithm>		// STL sort
#include <fstream>
#include <algorithm>		// STL sort

using namespace std;

class col_sort
{
	int _pos, _reverse;
public:
	col_sort(int pos, int reverse);										//sets variables
	bool operator()(const vector<string> &a, const vector<string> &b);	//compares strings (when inside the column range)
};

class row_sort
{
	int _reverse;
public:
	row_sort(int reverse);										//sets variables
	bool operator()(const string &a, const string &b);	//compares strings (when inside the column range)
};



class stringmatrix
{
private:
	vector<vector<string> > data;

protected:

public: 
	stringmatrix();
	stringmatrix(int no_rows, int no_cols, string value="");
//	stringmatrix(const stringmatrix &s);
	~stringmatrix();

	string get(int r, int c);
	int getInt(int r, int c);
	long getLong(int r, int c);
	double getDouble(int r, int c);

	void set(int r, int c, string val);
	void set(int r, int c, int val);
	void set(int r, int c, float val);
	void set_where_first_col_contains(string key, int c, string val);

	void fill(string val);
	void clear();

//	void rows(int val);
//	void cols(int val);
	inline unsigned int rows();
	inline unsigned int cols();

	// --- row operations ---
	void add_row(vector<string> r, string space_holder="");
	void add_three_strings_row(string s1, string s2, string s3, string space_holder="");

	void remove_row(int pos);
	void insert_row(int pos, vector<string> r, string space_holder="");
	void replace_row(int pos, vector<string> r, string space_holder="");
	void swap_rows(int r1, int r2);
//	void sort_row(int pos, int reverse);		//NOTE: only sorts string in this row...


	// --- col operations ---
	void add_col(vector<string> c, string space_holder="");
	void remove_col(int pos);
	void insert_col(int pos, vector<string> c, string space_holder="");
	void replace_col(int pos, vector<string> c, string space_holder="");
	void swap_cols(int c1, int c2);
	void sort_table_using_col(int pos, int reverse);		//sorts the whole table using this column

	void print_all();
	void read_from_csvfile(string filepath, string sep=";");
	void write_to_csvfile(string filepath, string sep=";");
};

#endif