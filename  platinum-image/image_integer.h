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

        image_integer (itk::SmartPointer< itk::OrientedImage<ELEMTYPE, IMAGEDIM > > &i):image_scalar<ELEMTYPE, IMAGEDIM>(i) {}

    template<class SOURCETYPE>
        image_integer(image_general<SOURCETYPE, IMAGEDIM> * old_image, bool copyData = true): image_scalar<ELEMTYPE, IMAGEDIM>(old_image, copyData)
        {} //copy constructor

        image_integer(ELEMTYPE * inData, unsigned long inDataNumElems, long width, long height, Vector3D voxelSize) : image_scalar<ELEMTYPE, IMAGEDIM>(inData,inDataNumElems, width, height, voxelSize) {}

        image_integer (std::vector<std::string> files, long width, long height, bool bigEndian = false, long headerSize = 0, Vector3D voxelSize = Vector3D (1,1,4), unsigned int startFile = 1,unsigned int increment = 1): image_scalar<ELEMTYPE, IMAGEDIM> (files, width, height, bigEndian, headerSize, voxelSize, startFile,increment) {}
	    image_integer(const string filepath, const string name=""):image_scalar<ELEMTYPE, IMAGEDIM>(filepath, name){}

        std::vector<HistoPair> get_distribution();

        // *** processing ***
		vector<ELEMTYPE> get_distinct_values_in_slice_2D(int slice, int direction=2, bool ignore_zeroes=false);
        ELEMTYPE gauss_fit2(); ///Compute optimal threshold value by fitting two gaussian distributions to the histogram.
        ELEMTYPE otsu(); ///Compute optimal threshold value by Otsu¥s method.
		std::vector<ELEMTYPE> k_means(int n_means);

        ELEMTYPE components_hist_3D(); ///Compute optimal threshold value by computing a number of components histogram.
        image_label<IMAGEDIM> * narrowest_passage_3D(image_binary<IMAGEDIM> * mask, IMGBINARYTYPE object_value=TRUE, bool regionGrow=true); ///Computing a narrowest passage transform followed by competetive region growing. Assumes the current image to be a distance image and the mask image contains seedpoints for the target.
        image_label<IMAGEDIM> * narrowest_passage_3D(image_binary<IMAGEDIM> * mask, int low_x, int high_x, int low_y, int high_y, int low_z, int high_z, IMGBINARYTYPE object_value=TRUE, bool regionGrow=true); ///Computing a narrowest passage transform followed by competetive region growing. Assumes the current image to be a distance image and the mask image contains seedpoints for the target.
		image_label<IMAGEDIM> * narrowest_passage_3D(image_binary<IMAGEDIM> * mask, int* no_internal_seeds, int y_direction, int z_direction,bool allow_before=true,IMGBINARYTYPE object_value=TRUE, bool regionGrow=true);

		image_integer<ELEMTYPE, IMAGEDIM>* get_subvolume_from_region_3D(Vector3Dint vox_pos, Vector3Dint vox_size);
		image_integer<ELEMTYPE, IMAGEDIM>* get_subvolume_from_region_3D(int x1, int y1, int z1, int x2, int y2, int z2);


	private:
		int findNode(int e, int* par_node); ///Support function to narrowest_passage_3D
		int mergeNodes(int e1, int e2, int* par_node); ///Support function to narrowest_passage_3D
		void markRecursive(int m, int* par_node, bool* marked); ///Support function to narrowest_passage_3D
		ELEMTYPE getSeedLevel(int m, int* par_node, bool* marked); ///Support function to narrowest_passage_3D
    };

#endif
