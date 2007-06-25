//////////////////////////////////////////////////////////////////////////
//
//  Image_integer $Revision$
//
//  Image type with integer values, in itself and as base for more
//  task-specific subclasses
//
//  $LastChangedBy$
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

#ifndef __image_integer__
#define __image_integer__

#include "image_scalar.h"

template<class ELEMTYPE, int IMAGEDIM = 3>
class image_integer : public image_scalar <ELEMTYPE, IMAGEDIM>
    {
    public:
        image_integer ():image_scalar<ELEMTYPE, IMAGEDIM>()
            {}
        image_integer(int w, int h, int d, ELEMTYPE *ptr = NULL):image_scalar<ELEMTYPE, IMAGEDIM>(w, h, d, ptr) {};

		image_integer (itk::SmartPointer< itk::Image<ELEMTYPE, IMAGEDIM > > &i):image_scalar<ELEMTYPE, IMAGEDIM>(i) {}

		template<class SOURCETYPE>
		image_integer(image_general<SOURCETYPE, IMAGEDIM> * old_image, bool copyData = true): image_scalar<ELEMTYPE, IMAGEDIM>(old_image, copyData)
			{} //copy constructor

        image_integer(ELEMTYPE * inData, unsigned long inDataNumElems, long width, long height, Vector3D voxelSize) : image_scalar<ELEMTYPE, IMAGEDIM>(inData,inDataNumElems, width, height, voxelSize) {}

		image_integer (std::vector<std::string> files, long width, long height, bool bigEndian = false, long headerSize = 0, Vector3D voxelSize = Vector3D (1,1,4), unsigned int startFile = 1,unsigned int increment = 1): image_scalar<ELEMTYPE, IMAGEDIM> (files, width, height, bigEndian, headerSize, voxelSize, startFile,increment) {}
	
		void draw_line_2D(int x0, int y0, int x1, int y1, int z, ELEMTYPE value, int direction=2); ///Draw a line between (x0,y0) and (x1,y1) in plane z using color described by value. The coordinates are given on the plane orthogonal to the axis given by direction.
		std::vector<HistoPair> get_distribution();
		std::vector<double> get_slice_sum(int direction=2);

        // *** processing ***
        image_binary<IMAGEDIM> * threshold(ELEMTYPE low, ELEMTYPE high, IMGBINARYTYPE true_inside_threshold=true); ///Return a image_binary where all voxels with values between low and high gets the value true_inside_threshold.
		ELEMTYPE gauss_fit2(); ///Compute optimal threshold value by fitting two gaussian distributions to the histogram.
        
        ELEMTYPE components_hist_3D(); ///Compute optimal threshold value by computing a number of components histogram.
        image_label<IMAGEDIM> * narrowest_passage_3D(image_binary<IMAGEDIM> * mask, IMGBINARYTYPE object_value=TRUE); ///Computing a narrowest passage transform followed by competetive region growing. Assumes the current image to be a distance image and the mask image contains seedpoints for the target.
        image_label<IMAGEDIM> * narrowest_passage_3D(image_binary<IMAGEDIM> * mask, int low_x, int high_x, int low_y, int high_y, int low_z, int high_z, IMGBINARYTYPE object_value=TRUE); ///Computing a narrowest passage transform followed by competetive region growing. Assumes the current image to be a distance image and the mask image contains seedpoints for the target.
		void mask_out(image_binary<IMAGEDIM> *mask, IMGBINARYTYPE object_value=TRUE, ELEMTYPE blank=0); ///All voxels in the current image where the corresponding mask voxels != object_value are set to blank.
		void mask_out(int low_x, int high_x, int low_y, int high_y, int low_z, int high_z, ELEMTYPE blank=0); ///All voxels within the given sub-volume are set to blank.
		void copy(image_integer<ELEMTYPE, IMAGEDIM> *source, int low_x, int high_x, int low_y, int high_y, int low_z, int high_z, int direction=2); //Copy subvolume

	private:
		int findNode(int e, int* par_node); ///Support function to narrowest_passage_3D
		int mergeNodes(int e1, int e2, int* par_node); ///Support function to narrowest_passage_3D
		void markRecursive(int m, int* par_node, bool* marked); ///Support function to narrowest_passage_3D
		ELEMTYPE getSeedLevel(int m, int* par_node, bool* marked); ///Support function to narrowest_passage_3D
    };

//with C++ templates, declaration and definition go together
#include "image_integer.hxx"

#endif
