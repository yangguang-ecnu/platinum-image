//////////////////////////////////////////////////////////////////////////
//
//  bias_field.h $Revision $
//
//
//  $LastChangedBy $
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

#ifndef __bias_field__
#define __bias_field__

#include "bias_base.h"
#include <vnl/vnl_real_npolynomial.h>


template<int DIM>
class bias_poly //: public bias_base
{

    private:
		int order;						//polynomial coefficients
		vnl_real_npolynomial poly;
		vnl_vector<double> c;			//polynomial coefficients
		vnl_matrix<unsigned int> exp;	//polynomial exponents

    protected:
//        void redraw();
 
    public:
        bias_poly(int order_2_or_3 = 2);
//        bias_poly(bias_poly* const b);
        ~bias_poly();
		double eval2D(double x, double y);
		double eval3D(double x, double y, double z);

};

#endif