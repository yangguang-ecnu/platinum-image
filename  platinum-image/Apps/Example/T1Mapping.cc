/////////////////////////////////////////////////////////////////////////////////
//
//  T1 Mapping application using Platinum
//	See description in: Kullberg2006 - J Magn Reson Imaging. 2006 Aug;24(2):394-401.
//	Whole-body T1 mapping improves the definition of adipose tissue: consequences for automated image analysis.
//

//   Copyright (c) 2007, Uppsala university
//   All rights reserved.
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions are met:
//       * Redistributions of source code must retain the above copyright
//         notice, this list of conditions and the following disclaimer.
//       * Redistributions in binary form must reproduce the above copyright
//         notice, this list of conditions and the following disclaimer in the
//         documentation and/or other materials provided with the distribution.
//       * Neither the name of the Uppsala university nor the
//         names of its contributors may be used to endorse or promote products
//         derived from this software without specific prior written permission.
//
//   THIS SOFTWARE IS PROVIDED BY UPPSALA UNIVERSITY ``AS IS'' AND ANY
//   EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//   DISCLAIMED. IN NO EVENT SHALL UPPSALA UNIVERSITY BE LIABLE FOR ANY
//   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "../../platinumprogram.h"

#include <cstdio>
#include <iostream>

#include "string.h"
#include "../../global.h"
#include "../../image_integer.hxx"

//retrieve command-line arguments
int visid = -1;
int arg(int argc, char **argv, int &i) {
	if (argv[i][1] == 'v') {
		if (i+1 >= argc) return 0;
		visid = atoi(argv[i+1]);
		i += 2;
		return 2;
	}
	return 0;
}

// *** begin userIO demo functions ***

//string streams for parameter demo function
#include <sstream>
using namespace std;



void t1_mapping_function (int userIO_ID,int par_num)
{
	if (par_num == USERIO_CB_OK)
	{
//		metadata m = metadata();
//		m.read_metadata_from_dcm_file("c:\\Joel\\TMP\\1.dcm");
//		m.print_all();
//		m.set_data_float(DCM_FLIP,7.7777777777777777);
//		m.print_all();


		image_integer<unsigned short,3> *large = dynamic_cast<image_integer<unsigned short,3>*>(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,0)));
		float tr = userIOmanagement.get_parameter<float>(userIO_ID,1);
		float te = userIOmanagement.get_parameter<float>(userIO_ID,2);
		long int large_flip = userIOmanagement.get_parameter<long int>(userIO_ID,3);
		std::string large_dcm = userIOmanagement.get_parameter<std::string>(userIO_ID,4);

		image_integer<unsigned short,3> *small = dynamic_cast<image_integer<unsigned short,3>*>(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,5)));
		long int small_flip = userIOmanagement.get_parameter<long int>(userIO_ID,6);
		std::string small_dcm = userIOmanagement.get_parameter<std::string>(userIO_ID,7);

		if(large != NULL && small != NULL && small != large){
		cout<<"Files successfully loaded..."<<endl;

		large->meta.read_metadata_from_dcm_file(large_dcm);
		if(tr>0){large->meta.set_data_float(DCM_TR, float(tr));}
		if(te>0){large->meta.set_data_float(DCM_TE, float(te));}
		large->meta.set_data_float(DCM_FLIP, float(large_flip));

		small->meta.read_metadata_from_dcm_file(small_dcm);
		small->meta.set_data_float(DCM_FLIP, float(small_flip));

		image_scalar<unsigned short,3> *t1map = large->calculate_T1Map_from_two_flip_angle_MRvolumes_3D(small,30,0,5000);
		t1map->name(large->name()+" - T1Map");
		datamanagement.add(t1map);

		}else{
		cout<<"Files NOT successfully loaded..."<<endl;
		}

	}
}

void reslice_function(int userIO_ID,int par_num)
{
	if (par_num == USERIO_CB_OK)
	{
		image_integer<unsigned short,3> *im = dynamic_cast<image_integer<unsigned short,3>*>(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,0)));
		long int dir = userIOmanagement.get_parameter<long int>(userIO_ID,1);
		long int start = userIOmanagement.get_parameter<long int>(userIO_ID,2);
		long int every = userIOmanagement.get_parameter<long int>(userIO_ID,3);
		std::string file = userIOmanagement.get_parameter<std::string>(userIO_ID,4);

		cout<<"dir="<<dir<<endl;
		cout<<"start="<<start<<endl;
		cout<<"every="<<every<<endl;
		cout<<"file="<<file<<endl;

		if(im != NULL && file != ""){
			image_general<unsigned short,3> *im2 = im->get_subvolume_from_slices_3D(int(start),int(every),int(dir));
			im2->name("im2..");
			datamanagement.add(im2);
			im2->save_to_VTK_file(file);
		}
	}
}


// *** end of userIO functions ***

int main(int argc, char *argv[])
{
	//start up Platinum
	platinum_init();

	//set up main window

	const int w_margin=15*2;
	int windowwidth = 1100-w_margin;
	int windowheight = 800-w_margin;

	Fl_Window window(1100,800);

	//prepare Platinum for userIO creation
	platinum_setup(window);


	// *** begin userIO control definitions ***

	// --------------------------
	int t1_mapping_ID=userIOmanagement.add_userIO("T1-Mapping calculator",t1_mapping_function,"Map T1");
	userIOmanagement.add_par_image(t1_mapping_ID,"Large flip volume");
	userIOmanagement.add_par_float_box(t1_mapping_ID,"TR (Overrides dcm info, if not 0)",5000,0);
	userIOmanagement.add_par_float_box(t1_mapping_ID,"TE (Overrides dcm info, if not 0)",5000,0);
	userIOmanagement.add_par_longint_box(t1_mapping_ID,"Flip angle (0-180)",180,0);
	userIOmanagement.add_par_string(t1_mapping_ID,"Large - Dcm file (leave empty if not needed)","C:\\Joel\\flip80.dcm");

	userIOmanagement.add_par_image(t1_mapping_ID,"Small flip volume");
	userIOmanagement.add_par_longint_box(t1_mapping_ID,"Flip angle (0-180)",180,0);
	userIOmanagement.add_par_string(t1_mapping_ID,"Small - Dcm file (leave empty if not needed)","C:\\Joel\\flip30.dcm");
	userIOmanagement.finish_userIO(t1_mapping_ID);

	/*
	int reslice_ID=userIOmanagement.add_userIO("SubVolume Reslice Tool...",reslice_function,"Reslice");
	userIOmanagement.add_par_image(reslice_ID,"Volume...");
	userIOmanagement.add_par_longint_box(reslice_ID,"Slice direction (0,1,2)",2,0);
	userIOmanagement.add_par_longint_box(reslice_ID,"Start slice",1000,0);
	userIOmanagement.add_par_longint_box(reslice_ID,"Every 'X' slice",1000,1);
	userIOmanagement.add_par_string(reslice_ID,"Save path_and_file","Test_save.vtk");
	userIOmanagement.finish_userIO(reslice_ID);
	*/
	// *** end userIO control definitions ***

	//finish the window creation
	window.end();

	window.show(argc, argv);

	return Fl::run();
}
