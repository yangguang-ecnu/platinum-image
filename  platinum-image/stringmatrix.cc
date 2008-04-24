// $Id: $


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

#define __stringmatrix_cc__

#include "stringmatrix.h"
#include <iostream>

//------------------ SORTING --------------------------
col_sort::col_sort(int pos, int reverse)
{
	_pos = pos;
	_reverse = reverse;
}


bool col_sort::operator()(const vector<string> &a, const vector<string> &b)
{
	const char *ap = ( _pos < (int)a.size() ) ? a[_pos].c_str() : ""; 
	const char *bp = ( _pos < (int)b.size() ) ? b[_pos].c_str() : "";
	if ( _reverse ) { return(strcmp(ap, bp) > 0); }
	else            { return(strcmp(ap, bp) < 0); }
}

row_sort::row_sort(int reverse)
{
	_reverse = reverse;
}


bool row_sort::operator()(const string &a, const string &b)
{
	const char *ap = a.c_str(); 
	const char *bp = b.c_str(); 
	if ( _reverse ) { return(strcmp(ap, bp) > 0); }
	else            { return(strcmp(ap, bp) < 0); }
}


stringmatrix::stringmatrix()
{}

stringmatrix::stringmatrix(int no_rows, int no_cols, string value)
{
	cout<<"stringmatrix::stringmatrix(no_rows="<<no_rows<<", no_cols="<<no_cols<<", value="<<value.c_str()<<")"<<endl;

	vector<string> row;
	for(int c=0; c<no_cols; c++){row.push_back(value);}
	for(int r=0; r<no_rows; r++){data.push_back(row);}
}

/*
stringmatrix::stringmatrix(const stringmatrix &s)
{
if( (rows()==s.rows()) && (cols() == s.cols()) )
{
for(int r=0; r<rows(); r++)
{
for(int c=0; c<cols(); c++)
{
set(r,c,s.get(r,c));
}
}
}
}
*/

stringmatrix::~stringmatrix()
{}




// --------------------- GET/SET operations ------------------------

string stringmatrix::get(int r, int c) const
{
	string ret="";
	if(r>=0 && c>=0 && (unsigned int)(r)<rows() && (unsigned int)(c)<cols())
	{
		ret = data[r][c];
	}
	return ret;
}

int stringmatrix::getInt(int r, int c)
{
	return atoi(get(r,c).c_str());
}

long stringmatrix::getLong(int r, int c)
{
	return atol(get(r,c).c_str());
}

double stringmatrix::getDouble(int r, int c)
{
	return atof(get(r,c).c_str());
}



void stringmatrix::set(int r, int c, string val)
{
	if(r>=0 && c>=0 && r<rows() && c<cols())
	{
		data[r][c] = val;
	}
}

void stringmatrix::set(int r, int c, int val)
{
	if(r>=0 && c>=0 && r<rows() && c<cols())
	{
		char buffer[100];
		sprintf( buffer, "%i", val );              
		data[r][c] = string(buffer);
	}
}

void stringmatrix::set(int r, int c, float val)
{
	if(r>=0 && c>=0 && r<rows() && c<cols())
	{
		char buffer[100];
		sprintf( buffer, "%f", val );              
		data[r][c] = string(buffer);
	}
}

void stringmatrix::set_where_first_col_contains(string key, int c, string val)
{
//	cout<<"stringmatrix::set_where_first_col_contains...("<<key.c_str()<<")"<<endl;
	bool found = false;
	for(int r=0; r<rows(); r++)
	{
		if( (get(r,0).find(key)==0) && (get(r,0).size()==key.size()) ){ // spooky... compiler does not accept (string1 == string2) !!!
//			cout<<"found --> r="<<r<<endl;
			found = true;
			data[r][c] = val;
		}
	}
	if(!found){
		vector<string> row = vector<string>(cols());
		row[0]=key;
		row[c]=val;
		add_row(row);
	}
}


void stringmatrix::fill(string val)
{
	for(int r=0; r<rows(); r++)
	{
		for(int c=0; c<cols(); c++)
		{
			set(r,c,val);
		}
	}

}

void stringmatrix::clear()
{
	data.clear();
}


unsigned int stringmatrix::rows() const
{
	return data.size();
}

unsigned int stringmatrix::cols() const
{
	return (rows()>0)?data[0].size():0;
}


// --------------------- ROW operations ------------------------

void stringmatrix::add_row(vector<string> r, string space_holder)
{
	if( r.size()<=cols() || rows()==0 )
	{
		while(r.size()<cols()){	r.push_back(space_holder);}
		data.push_back(r);
	}
}

void stringmatrix::add_three_strings_row(string s1, string s2, string s3, string space_holder)
{
	if( 3<=cols() || rows()==0 )
	{
		vector<string> r;
		r.push_back(s1);
		r.push_back(s2);
		r.push_back(s3);
		while(r.size()<cols()){	r.push_back(space_holder);}
		data.push_back(r);
	}
}



void stringmatrix::remove_row(int pos)
{
	cout<<"remove_row(int pos)"<<endl;
	if( pos >=0 && pos < rows() )
	{
		vector<vector<string> >::iterator it;
		it = data.begin();
		it += pos;
		data.erase(it);
	}
}


void stringmatrix::insert_row(int pos, vector<string> r, string space_holder)
{
	cout<<"insert_row(...)"<<endl;
	if( pos >=0 && pos < rows() && r.size() <= cols() )
	{
		while(r.size()<cols())
		{
			r.push_back(space_holder);
		}
		vector<vector<string> >::iterator it;
		it = data.begin();
		it += pos;
		data.insert(it,r);
	}
}

void stringmatrix::replace_row(int pos, vector<string> r, string space_holder)
{
	cout<<"replace_row(...)"<<endl;
	if( pos >=0 && pos < rows() && r.size() <= cols() )
	{
		while(r.size()<cols())
		{
			r.push_back(space_holder);
		}
		remove_row(pos);
		insert_row(pos,r,space_holder);
	}
}

void stringmatrix::swap_rows(int r1, int r2)
{
	cout<<"swap_rows(...)"<<endl;
	if( r1 >=0 && r2 >=0 && r1 <rows() && r2 <rows() && r1 != r2 )
	{
		vector<string> tmp = data[r1];
		for(int c=0; c<cols(); c++)
		{
			data[r1][c]=get(r2,c);
			data[r2][c]=tmp[c];		
		}
	}
}

/*
void stringmatrix::sort_row(int pos, int reverse)
{
	if(pos>=0 && pos<rows()){
		sort(data[pos].begin(), data[pos].end(), row_sort(reverse));
	}
}
*/



// --------------------- COL operations ------------------------

void stringmatrix::add_col(vector<string> c, string space_holder)
{
	cout<<"add_col(...)"<<endl;
	if( c.size() <= rows() )
	{
		while(c.size()<rows()){	c.push_back(space_holder);}
	}
	for(int r=0; r<rows(); r++)
	{
		data[r].push_back(c[r]);
	}
}

void stringmatrix::remove_col(int pos)
{
	cout<<"remove_col(...)"<<endl;
	if( pos>=0 && pos < cols() )
	{
		vector<string>::iterator it;
		for(int r=0; r<rows(); r++)
		{
			it = data[r].begin();
			it += pos;
			data[r].erase(it);
		}

	}
}

void stringmatrix::insert_col(int pos, vector<string> c, string space_holder)
{
//	cout<<"insert_col(...)"<<endl;
	if( pos>=0 && pos < cols() && c.size() <= rows())
	{
		while(c.size()<rows()){	c.push_back(space_holder);}

		vector<string>::iterator it;
		for(int r=0; r<rows(); r++)
		{
			it = data[r].begin();
			it += pos;
			data[r].insert(it,c[r]);
		}
	}
}

void stringmatrix::replace_col(int pos, vector<string> c, string space_holder)
{
//	cout<<"replace_col(...)"<<endl;
	if( pos>=0 && pos < cols() && c.size() <= rows())
	{
		while(c.size()<rows()){	c.push_back(space_holder);}

		for(int r=0; r<rows(); r++)
		{
			set(r,pos,c[r]);
		}
	}
}

void stringmatrix::swap_cols(int c1, int c2)
{
//	cout<<"swap_cols(...)"<<endl;
	if( c1 >=0 && c2 >=0 && c1 <cols() && c2 <cols() && c1 != c2 )
	{
		string tmp;
		for(unsigned int r=0; r<rows(); r++)
		{
			tmp = data[r][c1];
			data[r][c1] = data[r][c2];
			data[r][c2] = tmp;
		}
	}
}

void stringmatrix::sort_table_using_col(int pos, int reverse)
{
	sort(data.begin(), data.end(), col_sort(pos, reverse));
}




// --------------------- INPUT/OUTPUT ------------------------

void stringmatrix::print_all()
{
	string s;
	cout<<"print_all... (r,c) ("<<rows()<<","<<cols()<<")"<<endl;
	for(int r=0; r<rows(); r++)
	{
		for(int c=0; c<cols(); c++)
		{
			cout<<get(r,c).c_str()<<",";
		}
		cout<<endl;
	}

}



void stringmatrix::read_from_csvfile(string filepath, string sep)
{
//	cout<<"read_from_csvfile... "<<endl;

	ifstream myfile (filepath.c_str());
	int ind1=0;
	int ind2=0;
	if (myfile.is_open())
	{
		char buffer[10000];
		string s;
		string word;
		data.clear();
		vector<string> row;
		while (! myfile.eof() )
		{
			myfile.getline(buffer,10000);
//			cout<<buffer<<endl;
			s=string(buffer);
			ind1=0;
			ind2=0;
			row = vector<string>();
			while(ind2>=0)
			{
				ind2 = s.find_first_of(sep,ind1);
				word = s.substr(ind1,ind2-ind1);
//				cout<<ind1<<" "<<ind2<<"  "<<word.c_str()<<"  "<<s.c_str()<<endl;
				ind1 = ind2+1;
				if(ind2>=0)
				{
					row.push_back(word);
				}
			}
			if(row.size()>0)
			{
				data.push_back(row);
			}
		}
		myfile.close();
	}
}

void stringmatrix::write_to_csvfile(string filepath, string sep)
{
	cout<<"write_to_csvfile... "<<endl;
    ofstream myfile;
	myfile.open(filepath.c_str());
	for(int r=0; r<rows(); r++){
		for(int c=0; c<cols(); c++){
			myfile<<get(r,c).c_str()<<sep.c_str();
		}
		myfile<<"\n";
	}
	myfile.close();
}

/*
void stringmatrix::write_to_csvfile(string filepath, string sep, bool merge_if_already_exists)
{
	cout<<"write_to_csvfile... "<<endl;
    ofstream myfile;
	if(merge_if_already_exists && file_exists(filepath)){
		cout<<"merge... "<<endl;
		stringmatrix tmp;
		tmp.read_from_csvfile(filepath);
		for(int r=0; r<rows(); r++){
			for(int c=0; c<cols(); c++){
				if(get(r,c)!="" && get(r,c)!=" "){
					set(r,c,tmp.get(r,c));
				}
			}
		}
		write_to_csvfile(filepath, sep, false); // now write the file...

	}else{
		myfile.open(filepath.c_str());
		for(int r=0; r<rows(); r++){
			for(int c=0; c<cols(); c++){
				myfile<<get(r,c).c_str()<<sep.c_str();
			}
			myfile<<"\n";
		}
	}
	myfile.close();
}
*/