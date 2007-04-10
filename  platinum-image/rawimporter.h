//////////////////////////////////////////////////////////////////////////
//
//  Rawimporter
//
//  Window for importing image stacks in raw format, single and multi-file
//
//

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


#ifndef __rawimporter__
#define __rawimporter__

#include <string>
#include <vector>

#include "image_base.h"

#include "global.h"

enum volume_load_mode {
    undefined,
    multifile,
    single_file};

std::string path_end (std::string file_path);   //helper to get the end 
                                                //(file/dir name) from a path

std::string path_parent (std::string file_path); //get the parent's path 
                                                //(../ dir name) from a path


// *** begin FLUID includes ***

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Spinner.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Output.H>

// *** end FLUID includes ***

class rawimporter : public Fl_Window
    {
    public:
        rawimporter(std::vector<std::string>);

        template <class intype, class outtype>
            static void raw_convert (intype* &inpointer, outtype* &outpointer, long numVoxels, bool bigEndian);                                           //convert data type and endianness on arrays

    private:
        //Helpers
        void get_input ();                                  //copy FLTK inputs to the
                                                            //respective variables

        void update_fields (Fl_Widget*);       //recalculate dependent field
                                                            //values and widget states

        //Widgets
        Fl_Group *voxeltypegroup;
        Fl_Choice *voxeltypemenu;
        Fl_Button *guessvoxeltype;
        Fl_Check_Button *signedbtn;
        Fl_Round_Button *endianbigbtn;
        Fl_Round_Button *endianlittlebtn;
        Fl_Group *headersizegroup;
        Fl_Input *headersizefield;
        Fl_Button *guessheadersize;
        Fl_Value_Input *imagesizex;
        Fl_Value_Input *imagesizey;
        Fl_Group *series_group;
        Fl_Group *numzgroup;
        Fl_Button *guessnumz;
        Fl_Value_Input *imagenumz;
        Fl_Group *interleave_group;
        Fl_Spinner *startfield;
        Fl_Spinner *incrementfield;
        Fl_Input *voxsizex;
        Fl_Input *voxsizey;
        Fl_Input *voxsizez;
        Fl_Button *rawimportcancel;
        Fl_Return_Button *rawimportok;
        
        //Callbacks
        static void cb_rawimportok(Fl_Return_Button* o, void*);
        static void cb_rawimportcancel(Fl_Button* o, void*);
        static void rawimporter::cb_field_changed(Fl_Widget* o, void*);

        //Data
        std::vector<std::string>    files;
        unsigned long               imageSize [3];
        bool                        bigEndian;
        unsigned long               headerSize;
        imageDataType               voxeltype;
        signed int                  sliceStart, sliceIncrement;
        unsigned long               fileSize;
        volume_load_mode            mode;
    };

#endif