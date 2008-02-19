// $Id: filters.hxx $

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

#ifndef __filters_hxx__
#define __filters_hxx__

#include "filters.h"


filter_median::filter_median(int nx, int ny, int nz, int xcenter, int ycenter, int zcenter) 
{
	if (xcenter<-1000) {xcenter=nx/2;}
	if (ycenter<-1000) {ycenter=ny/2;}
	if (zcenter<-1000) {zcenter=nz/2;}
	data = vnl_matrix<float>(nx*ny*nz,3);
	int element=0;
	for (int z=0; z<nz ; z++) {
		for (int y=0; y<ny ; y++) {
			for (int x=0; x<nx; x++) {	
				data.put(element, 0, x-xcenter);
				data.put(element, 1, y-ycenter);
				data.put(element, 2, z-zcenter);
				element++;
			}
		}
	}
}

float filter_median::apply(float* neighbourhood)
{
	int size=this->data.rows();
	sort(neighbourhood,neighbourhood+size);
	int undefined=0;
	for (int i=0; i<size; i++) {
		if (neighbourhood[i]<std::numeric_limits<float>::max() ) {undefined++;}
	}	
	cout << "Median: " << neighbourhood[(size-undefined)/ 2] << endl;
	return neighbourhood[(size-undefined)/ 2];
}

void filter_linear::set_data_from_floats(float* w, int nx, int ny, int nz, int xcenter, int ycenter, int zcenter) 
{
	data = vnl_matrix<float>(nx*ny*nz,4);
	int element=0;
	for (int z=0; z<nz ; z++) {
		for (int y=0; y<ny ; y++) {
			for (int x=0; x<nx; x++) {
				data.put(element, 0, x-xcenter);
				data.put(element, 1, y-ycenter);
				data.put(element, 2, z-zcenter);
				data.put(element, 3, w[x+nx*y+nx*ny*z]);
				element++;
			}
		}		
	}
}

void filter_linear::set_data_from_ints(int *w, int nx, int ny, int nz, int xcenter, int ycenter, int zcenter)
{
	int num_elements=0;
	for (int i=0; i<nx*ny*nz; i++) {
		if (w[i]!=0) {num_elements++;}
	}
	data = vnl_matrix<float>(num_elements,4);
	int element=0;
	for (int z=0; z<nz ; z++) {
		for (int y=0; y<ny ; y++) {
			for (int x=0; x<nx; x++) {
				if (w[x+nx*y+nx*ny*z]!=0) {
					data.put(element, 0, x-xcenter);
					data.put(element, 1, y-ycenter);
					data.put(element, 2, z-zcenter);
					data.put(element, 3, w[x+nx*y+nx*ny*z]);
					element++;
				}
			}
		}		
	}
}

float filter_linear::apply(float* neighbourhood) {
	float res=0;
	for (int i=0; i<this->data.rows(); i++) {
		if (neighbourhood[i]<std::numeric_limits<float>::max() ) {
			res+=this->data.get(i,3)*neighbourhood[i];
		}
	}
	return res;
}

void filter_linear::print() {
	cout<<"filter_linear::print()"<<endl;

	for (int i=0; i<data.rows(); i++) {
		for (int j=0; j<data.cols(); j++) {
			cout << data.get(i,j) << "  ";
		}
		cout << endl;
	}
}

filter_sobel_2d::filter_sobel_2d(int dir) {
	// dir=direction; 0=east, 1=southeast, 2=south...7=northeast
	if (dir<0) {dir=0;}
	int nx=3; int ny=3; int nz=1;
	int *w = new int[nx*ny*nz];
	w[0]=abs((dir+2)%8-3)-2; w[1]= abs((dir+1)%8-3)-2; w[2]= abs((dir+0)%8-3)-2;
	w[3]=abs((dir+3)%8-3)-2; w[4]= 0;                  w[5]= abs((dir+7)%8-3)-2;
	w[6]=abs((dir+4)%8-3)-2; w[7]= abs((dir+5)%8-3)-2; w[8]= abs((dir+6)%8-3)-2;
	this->set_data_from_ints(w,nx,ny,nz,1,1,0);
	delete w;
	cout << "Created 2d sobel filter, direction: " << dir%8 << endl;
}

filter_gaussian::filter_gaussian(int nx, int ny, int nz, float x_std_dev, float y_std_dev, float z_std_dev, int xcenter, int ycenter, int zcenter)
{
	if (x_std_dev<0) {x_std_dev=(float)nx/5;}
	if (y_std_dev<0) {y_std_dev=(float)ny/5;}
	if (z_std_dev<0) {z_std_dev=(float)nz/5;}
	if (xcenter<-1000) {xcenter=nx/2;}
	if (ycenter<-1000) {ycenter=ny/2;}
	if (zcenter<-1000) {zcenter=nz/2;}

	float *w = new float[nx*ny*nz];
	float sum=0;
	for (int z=0; z<nz; z++) {
		for (int y=0; y<ny; y++) {
			for (int x=0; x<nx; x++) {
				w[x+nx*y+nx*ny*z]=exp( -pow(x-xcenter, 2)/pow(x_std_dev,2) - pow(y-ycenter, 2)/pow(y_std_dev,2) - pow(z-zcenter, 2)/pow(z_std_dev,2) );
				sum+=w[x+nx*y+nx*ny*z];
			}
		}
	}
	for (int z=0; z<nz; z++) {
		for (int y=0; y<ny; y++) {
			for (int x=0; x<nx; x++) {
				w[x+nx*y+nx*ny*z]=w[x+nx*y+nx*ny*z]/sum; // normalize
			}
		}
	}
	this->set_data_from_floats(w,nx,ny,nz,xcenter,ycenter,zcenter);
	delete w;
	cout << "Created gaussian " << nx << "x" << ny << "x" << nz << " filter." << endl;
}

filter_mean::filter_mean(int nx, int ny, int nz, int xcenter, int ycenter, int zcenter)
{
	if (xcenter<-1000) {xcenter=nx/2;}
	if (ycenter<-1000) {ycenter=ny/2;}
	if (zcenter<-1000) {zcenter=nz/2;}
	int num=nx*ny*nz;
	float *w = new float[num];
	for (int i=0; i<num; i++) {
		w[i]=(float)1/num;
	}
	set_data_from_floats(w,nx,ny,nz,xcenter,ycenter,zcenter);
	delete w;
	cout << "Created mean " << nx << "x" << ny << "x" << nz << " filter." << endl;
}

#endif