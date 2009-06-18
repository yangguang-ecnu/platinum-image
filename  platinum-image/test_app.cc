/////////////////////////////////////////////////////////////////////////////////
//
//  Test application for Platinum development
//
//  $LastChangedBy$
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

#include "../../platinumprogram.h"

#include <cstdio>
#include <iostream>

#include "../../global.h"
#include "../../image_binary.hxx"
#include "../../image_complex.hxx"
#include "../../curve.hxx"
#include "../../color.h"

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

void add_demo_image (int userIO_ID,int par_num)
{
    if (par_num == USERIO_CB_OK)
        {
        image_integer<unsigned char,3>* demo_image = new image_integer<unsigned char,3>(20,20,20,NULL);
        //demo_image->erase();
        demo_image->testpattern();
        
        datamanagement.add(demo_image);
        }
}

//Example function that creates a new image_binary, fills it with 1, and runs a function on it.

void expand_function(int userIO_ID,int par_num)
{
    if (par_num == USERIO_CB_OK)
        {
        image_binary<3>* bin = new image_binary<3>(20,20,20);
		bin->fill(1);
		image_binary<3>* bin2 = dynamic_cast<image_binary<3>*>( bin->expand_borders(2,2,2) );
        
		delete bin;
        datamanagement.add(bin2);
        }
}

int main(int argc, char *argv[])
{
    //start up Platinum
    platinum_init();
    
    //set up main window
    Fl_Window window(800,600);
    
    //prepare Platinum for userIO creation
    platinum_setup(window);
    
    // *** begin userIO control definitions ***
    int create_vol_demo_ID=userIOmanagement.add_userIO("Add demo image",add_demo_image,"Add");
//    userIOmanagement.add_par_image(create_vol_demo_ID,"Working image: ");
//	userIOmanagement.add_par_histogram_2D(create_vol_demo_ID,"histogram_2d");
	//userIOmanagement.add_par_voxelseed(create_vol_demo_ID,"voxelseed");
//	userIOmanagement.add_par_float_box(create_vol_demo_ID,"float_box",50,0);
    userIOmanagement.finish_userIO(create_vol_demo_ID);

    int expand_ID=userIOmanagement.add_userIO("expanding test",expand_function,"Run");
    userIOmanagement.finish_userIO(expand_ID);

    // *** end userIO control definitions ***
    
    //finish the window creation
    window.end();
    window.show(argc, argv);

	//------------------
	complex<float> c = complex<float>(3,4);
	complex<float> d = complex<float>(4,5);
	image_complex<float,3> *im = new image_complex<float,3>(10,10,10,&c);
	im->fill(d);
	datamanagement.add(im,"im");
	//------------------

	RGBvalue val = RGBvalue();
	val.set_rgb_from_complex(complex<float>(5.0,0.1),float(0.1), 5.0);
	cout<<val<<endl;

	//preset viewport configuration 
//	viewmanagement.set_vp_direction(1,X_DIR);
//	viewmanagement.set_vp_blend_mode(1,BLEND_AVG);
//	viewmanagement.set_vp_renderer(2,"VTK-MIP");
    
    return Fl::run();
}