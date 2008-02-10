// $Id: bias_field.hxx $

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

#ifndef __bias_field_hxx__
#define __bias_field_hxx__


#include "bias_field.h"


template <int DIM>
bias_poly<DIM>::bias_poly(int ord)
{   
	order = ord;
	cout<<"bias_poly<"<<DIM<<">-constructor"<<endl;
	cout<<"DIM-value NOT recognized..."<<endl;
}

//-------------------- 2D case -----------------------
template <>
bias_poly<2>::bias_poly(int ord)
{   
	int DIM=2;
	order = ord;
	cout<<"bias_poly<2>-constructor"<<endl;
	int num_coeff = get_permutations(DIM+order,DIM);
	cout<<"num_coeff="<<num_coeff<<endl;

	if(order==2){
//		c[0]*x2 + c[1]y2 + c[2]xy + c[3]x + c[4]y  + c[5]
		c = vnl_vector<double>(num_coeff);
		c.fill(0.001);
		c[num_coeff-1]=1;
		exp = vnl_matrix<unsigned int>(num_coeff,DIM);
		exp[0][0] = 2; exp[0][1] = 0; 
		exp[1][0] = 0; exp[1][1] = 2; 
		exp[2][0] = 1; exp[2][1] = 1; 
		exp[3][0] = 1; exp[3][1] = 0; 
		exp[4][0] = 0; exp[4][1] = 1; 
		exp[5][0] = 0; exp[5][1] = 0; 
		poly = vnl_real_npolynomial(c,exp);
		cout<<"poly="<<endl<<poly<<endl;


	}else if(order==3){
		c = vnl_vector<double>(num_coeff);
		c.fill(0.001);
		c[num_coeff-1]=1;
		exp = vnl_matrix<unsigned int>(num_coeff,DIM);
		exp[0][0] = 3; exp[0][1] = 0; 
		exp[1][0] = 0; exp[1][1] = 3; 
		exp[2][0] = 2; exp[2][1] = 1; 
		exp[3][0] = 1; exp[3][1] = 2; 
		exp[4][0] = 2; exp[4][1] = 0; 
		exp[5][0] = 0; exp[5][1] = 2; 
		exp[6][0] = 1; exp[6][1] = 1; 
		exp[7][0] = 1; exp[7][1] = 0; 
		exp[8][0] = 0; exp[8][1] = 1; 
		exp[9][0] = 0; exp[9][1] = 0; 
		poly = vnl_real_npolynomial(c,exp);
		cout<<"poly="<<endl<<poly<<endl;

	}else{
		pt_error::error("bias_poly-constructor, unknown polynomial order",pt_error::serious);
	}
}

//-------------------- 3D case -----------------------
template <>
bias_poly<3>::bias_poly(int ord)
{   
	int DIM=3;
	order = ord;
	cout<<"bias_poly<"<<DIM<<">-constructor"<<endl;
	int num_coeff = get_permutations(DIM+order,DIM);
	cout<<"num_coeff="<<num_coeff<<endl;

	//set c and exp after (DIM and order...)
	//vnl_vector<double> c;			//polynomial coefficients
	//vnl_matrix<unsigned int> exp;	//polynomial exponents
	//poly = vnl_real_npolynomial();

	if(order==2){
		c = vnl_vector<double>(num_coeff);
		c.fill(0.001);
		c[num_coeff-1]=1;
		exp = vnl_matrix<unsigned int>(num_coeff,DIM);
		exp[0][0] = 2; exp[0][1] = 0; exp[0][2] = 0; 
		exp[1][0] = 0; exp[1][1] = 2; exp[1][2] = 0; 
		exp[2][0] = 0; exp[2][1] = 0; exp[2][2] = 2; 
		exp[3][0] = 1; exp[3][1] = 1; exp[3][2] = 0; 
		exp[4][0] = 1; exp[4][1] = 0; exp[4][2] = 1; 
		exp[5][0] = 0; exp[5][1] = 1; exp[5][2] = 1; 
		exp[6][0] = 1; exp[6][1] = 0; exp[6][2] = 0; 
		exp[7][0] = 0; exp[7][1] = 1; exp[7][2] = 0; 
		exp[8][0] = 0; exp[8][1] = 0; exp[8][2] = 1; 
		exp[9][0] = 0; exp[9][1] = 0; exp[9][2] = 0; 
		poly = vnl_real_npolynomial(c,exp);
		cout<<"poly="<<endl<<poly<<endl;

	}else if(order==3){

	}else{
		pt_error::error("bias_poly-constructor, unknown polynomial order",pt_error::serious);
	}
}


template <int DIM>
bias_poly<DIM>::~bias_poly()
{   
	cout<<"bias_poly()<"<<DIM<<">-destructor"<<endl;
}


/*
template <int DIM>
void bias_poly<DIM>::redraw() //VSINTERNAL COMPILER ERROR if "void" is left out!!!
{   
	cout<<"bias_poly()-redraw"<<endl;
}

template <>
void bias_poly<2>::redraw() //VSINTERNAL COMPILER ERROR if "void" is left out!!!
{   
	cout<<"bias_poly()-redraw -special case...2"<<endl;
}

*/

template <int DIM>
double bias_poly<DIM>::eval2D(double x, double y)
{
	cout<<"bias_poly<DIM>::eval2D cannot be called when DIM="<<DIM<<endl;
	return 0;
}

template <>
double bias_poly<2>::eval2D(double x, double y)
{
	vnl_vector<double> point = vnl_vector<double>(2);
	point[0]=x;
	point[1]=y;
	return poly.eval(point);
}

template <int DIM>
double bias_poly<DIM>::eval3D(double x, double y, double z)
{
	cout<<"bias_poly<DIM>::eval3D cannot be called when DIM="<<DIM<<endl;
	return 0;
}

template <>
double bias_poly<3>::eval3D(double x, double y, double z)
{
	vnl_vector<double> point = vnl_vector<double>(3);
	point[0]=x;
	point[1]=y;
	point[2]=z;
	return poly.eval(point);
}






#endif