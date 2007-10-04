// ConfigFile.h
// Class for reading named values from configuration files
// Richard J. Wagner  v2.1  24 May 2004  wagnerr@umich.edu

// Copyright (c) 2004 Richard J. Wagner
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

// Typical usage
// -------------
// 
// Given a configuration file "settings.inp":
//   atoms  = 25
//   length = 8.0  # nanometers
//   name = Reece Surcher
// 
// Named values are read in various ways, with or without default values:
//   ConfigFile config( "settings.inp" );
//   int atoms = config.read<int>( "atoms" );
//   double length = config.read( "length", 10.0 );
//   string author, title;
//   config.readInto( author, "name" );
//   config.readInto( title, "title", string("Untitled") );
// 
// See file example.cpp for more examples.

#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using std::string;

class ConfigFile {
// Data
protected:
	string myDelimiter;  // separator between key and value
	string myComment;    // separator between value and comments
	string mySentry;     // optional string to signal end of file
	std::map<string,string> myContents;  // extracted keys and values
	
	typedef std::map<string,string>::iterator mapi;
	typedef std::map<string,string>::const_iterator mapci;

// Methods
public:
	ConfigFile( string filename, string delimiter = "=",string comment = "#", string sentry = "EndConfigFile" );
	ConfigFile();
	
	// Search for key and read value or optional default value
	template<class T> T read( const string& key ) const;  // call as read<T>
	template<class T> T read( const string& key, const T& value ) const;
	template<class T> bool readInto( T& var, const string& key ) const;
	template<class T> bool readInto( T& var, const string& key, const T& value ) const;
	
	// Modify keys and values
	template<class T> void add( string key, const T& value );
	
	void remove( const string& key );
	
	// Check whether key exists in configuration
	bool keyExists( const string& key ) const;
	
	// Check or change configuration syntax
	string getDelimiter() const { return myDelimiter; }
	string getComment() const { return myComment; }
	string getSentry() const { return mySentry; }
	string setDelimiter( const string& s )	{ string old = myDelimiter;  myDelimiter = s;  return old; }  
	string setComment( const string& s )	{ string old = myComment;  myComment = s;  return old; }
	
	// Write or read configuration
	friend std::ostream& operator<<( std::ostream& os, const ConfigFile& cf );
	friend std::istream& operator>>( std::istream& is, ConfigFile& cf );

	void save_to_file(const string filename);	//saves only the string data in "map" to file...
	template<class T> void update_value_in_file(const string filename, const string& key, const T& value );	//updates this value in the saved config file...
	
protected:
	template<class T> static string T_as_string( const T& t );
	template<class T> static T string_as_T( const string& s );
	static void trim( string& s );


// Exception types
public:
	struct file_not_found {
		string filename;
		file_not_found( const string& filename_ = string() ):filename(filename_) {} 
	};
	struct key_not_found {  // thrown only by T read(key) variant of read()
		string key;
		key_not_found( const string& key_ = string() ):key(key_) {} 
	};
};


/* static */
template<class T>
string ConfigFile::T_as_string( const T& t )
{
	// Convert from a T to a string
	// Type T must support << operator
	std::ostringstream ost;
	ost << t;
	return ost.str();
}


/* static */
template<class T>
T ConfigFile::string_as_T( const string& s )
{
	// Convert from a string to a T
	// Type T must support >> operator
	T t;
	std::istringstream ist(s);
	ist >> t;
	return t;
}


/* static */
template<>
inline string ConfigFile::string_as_T<string>( const string& s )
{
	// Convert from a string to a string
	// In other words, do nothing
	return s;
}


/* static */
template<>
inline bool ConfigFile::string_as_T<bool>( const string& s )
{
	// Convert from a string to a bool
	// Interpret "false", "F", "no", "n", "0" as false
	// Interpret "true", "T", "yes", "y", "1", "-1", or anything else as true
	bool b = true;
	string sup = s;
	for( string::iterator p = sup.begin(); p != sup.end(); ++p )
		*p = toupper(*p);  // make string all caps
	if( sup==string("FALSE") || sup==string("F") ||
	    sup==string("NO") || sup==string("N") ||
	    sup==string("0") || sup==string("NONE") )
		b = false;
	return b;
}


template<class T>
T ConfigFile::read( const string& key ) const
{
	// Read the value corresponding to key
	mapci p = myContents.find(key);
	if( p == myContents.end() ) throw key_not_found(key);
	return string_as_T<T>( p->second );
}


template<class T>
T ConfigFile::read( const string& key, const T& value ) const
{
	// Return the value corresponding to key or given default value
	// if key is not found
	mapci p = myContents.find(key);
	if( p == myContents.end() ) return value;
	return string_as_T<T>( p->second );
}


template<class T>
bool ConfigFile::readInto( T& var, const string& key ) const
{
	// Get the value corresponding to key and store in var
	// Return true if key is found
	// Otherwise leave var untouched
	mapci p = myContents.find(key);
	bool found = ( p != myContents.end() );
	if( found ) var = string_as_T<T>( p->second );
	return found;
}


template<class T>
bool ConfigFile::readInto( T& var, const string& key, const T& value ) const
{
	// Get the value corresponding to key and store in var
	// Return true if key is found
	// Otherwise set var to given default
	mapci p = myContents.find(key);
	bool found = ( p != myContents.end() );
	if( found )
		var = string_as_T<T>( p->second );
	else
		var = value;
	return found;
}


template<class T>
void ConfigFile::add( string key, const T& value )
{
	// Add a key with given value
	string v = T_as_string( value );
	trim(key);
	trim(v);
	myContents[key] = v;
	return;
}

template<class T>
void ConfigFile::update_value_in_file(const string filename, const string& key, const T& value )
{
	//ˆˆˆ

	std::cout<<"update_value_in_file...."<< std::endl;
	string value_string = T_as_string(value);

	std::ifstream in( filename.c_str() );
	if( !in ) throw file_not_found( filename ); 
	std::vector<std::string> lines;
	while(!in.eof()){
		string line;
		std::getline( in, line );
		lines.push_back(line);
	}
	in.close();



	// Load a ConfigFile from is
	// Read in keys and values, keeping internal whitespace
	typedef string::size_type pos;
	const string& delim  = myDelimiter;  // separator
	const string& comm   = myComment;    // comment
	const string& sentry = mySentry;     // end of file sentry
	const pos skip = delim.length();        // length of separator
	
	for(int i=0;i<lines.size();i++){
		// Read an entire line at a time
		string whole_line = lines[i];
//		cout<<"-------------------"<<endl;
//		cout<<whole_line<<endl;

		string line;
		string comment="";
		
		pos comm_pos = whole_line.find(comm);
//		cout<<"("<<comm_pos<<") ";
		line = whole_line.substr( 0, comm_pos );
//		cout<<line;
		if(comm_pos<whole_line.size()){
//			cout<<"*** ";
			comment = whole_line.substr(comm_pos, whole_line.size()-comm_pos );
		}
//		cout<<"***"<<comment<<endl;
		
		// Parse the line if it contains a delimiter
		pos delimPos = line.find( delim );
		if( delimPos < string::npos ){
			// Extract the key
			string this_key = line.substr( 0, delimPos );
			ConfigFile::trim(this_key);

//			cout<<"this_key=("<<this_key<<")"<<endl;
//			cout<<"key=("<<key<<")"<<endl;
			if(this_key == key){
				lines[i] = key + " " + delim + " " + value_string + "\t" + comment;
			}
		}
/*
			// See if value continues on the next line
			// Stop at blank line, next line with a key, end of stream,
			// or end of file sentry
			
			// Store key and value
			ConfigFile::trim(key);
			ConfigFile::trim(line);
			cf.myContents[key] = line;  // overwrites if key is repeated
		}
		*/
	}

//	cout<<"-------------------"<<endl;
//	for(int i=0;i<lines.size();i++){
//		cout<<lines[i]<<endl;
//	}
//	cout<<"-------------------"<<endl;

	std::ofstream out( filename.c_str() );
//	if( !in ) throw file_not_found( filename ); 
	for(int i=0;i<lines.size();i++){
		out<<lines[i]<<std::endl;
	}
	out.close();

}



#endif  // CONFIGFILE_H

// Release notes:
// v1.0  21 May 1999
//   + First release
//   + Template read() access only through non-member readConfigFile()
//   + ConfigurationFileBool is only built-in helper class
// 
// v2.0  3 May 2002
//   + Shortened name from ConfigurationFile to ConfigFile
//   + Implemented template member functions
//   + Changed default comment separator from % to #
//   + Enabled reading of multiple-line values
// 
// v2.1  24 May 2004
//   + Made template specializations inline to avoid compiler-dependent linkage
//   + Allowed comments within multiple-line values
//   + Enabled blank line termination for multiple-line values
//   + Added optional sentry to detect end of configuration file
//   + Rewrote messy trimWhitespace() function as elegant trim()
