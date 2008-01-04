//////////////////////////////////////////////////////////////////////////
//
//   fcm - Fuzzy c-means $Revision: $
///
///  Class for FCM segmentation of multispectral data
///  Planned extensions are sFCM (where spatial information of the voxel neighbourhoods are included)
///  and inuFCM (Intensity Non Uniformity) where a smoothly varying model compensates for INU.
///
//   $LastChangedBy: joel.kullberg $

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

#ifndef __fcm__
#define __fcm__

#include "string.h"
#include <vector>
#include "global.h"
#include "image_integer.hxx"
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>


typedef vector<image_scalar<float,3>*> fcm_image_vector_type;

//#include <sstream>
using namespace std;


class fcm
{

protected:
	fcm_image_vector_type images;
	float m;						// FCM "fuzzyness" (the larger the more focused cluster weighting)
	float u_maxdiff_limit;			// Stop criterion for iterating solution (when u show less variation...)
	image_binary<3> *image_mask;	// A binary mask for limiting inclusion of multispectral data

	// data structures for vector-based FCM
	vnl_matrix<float> u;			//degree of membership	u(n_clust,n_pix)
	vnl_matrix<float> V;			//cluster center		V(n_clust, n_bands)
	vnl_matrix<float> X;			//pixel intensities		X(n_bands,n_pix)
	vnl_matrix<float> int_dist;		//distance in feature space... / int_dist(n_clust, n_pix)... for calc speedup

	// data structures for image-based FCM (simplifies inclusion of spatial info)
	fcm_image_vector_type u_images;	//degree of membership	(n_clust)
	//vnl_matrix<float> V;			//the same V matrix is used...
	//vnl_matrix<float> X;			//not needed...
	fcm_image_vector_type int_dist_images;	//distance in feature space... / int_dist(n_clust)... for calc speedup


	int nx();			//image size in x-directiuon
	int ny();			//image size in y-directiuon
	int nz();			//image size in z-directiuon
	int n_pix_masked;
	int n_pix();		//total number of pixels (=nx*ny*nz) alt. n_pix_masked...
	int n_bands();		//number bands (image contrasts) (e.g. T1w, T2w)
	int n_clust();		//number of clusters sought (e.g. background, grey matter, white matter)

	bool is_pixel_included(int i, int j, int k); //if image mask == NULL och pixel inside mask

	// ------ Vector-based version of help functions... --------
	// Following functions implement subrutines of the fcm iteration...
	// they are called with arguments (though not needed) to clarify involved data structures.
//	int get_num_pixels_in_mask(image_binary<3> *image_mask);
	void fill_X(vnl_matrix<float> &X, image_binary<3> *image_mask=NULL);
	void calc_int_dist_matrix_euclidean(vnl_matrix<float> &int_dist, const vnl_matrix<float> &X, const vnl_matrix<float> &V);
	void calc_memberships(vnl_matrix<float> &u, const vnl_matrix<float> &int_dist, const float m);
	void calc_cluster_centers(vnl_matrix<float> &V, const vnl_matrix<float> &u, const vnl_matrix<float> &X, float m);

	// ------ Image-based version of help functions... --------
	void calc_int_dist_images_euclidean(const vnl_matrix<float> &V);
	void calc_memberships(fcm_image_vector_type u_images, const fcm_image_vector_type &int_dist_images, const float m);
	void calc_cluster_centers(vnl_matrix<float> &V, const fcm_image_vector_type u_images, float m);

public:
	fcm(fcm_image_vector_type vec, vnl_matrix<float> V_init_clusters, float m_fuzzyness=2, float u_maxdiff_limit=0.05, image_binary<3> *mask=NULL);
	fcm(fcm_image_vector_type vec, float m_fuzzyness, vnl_matrix<float> V_init_clusters, float u_maxdiff_limit=0.05, image_binary<3> *mask=NULL);
	~fcm();

	void Update_vectorfcm(); //executes fcm algorithm ( in sweet ITK style... ;-)  )...
	void Update_imagefcm(); //executes fcm algorithm ( in sweet ITK style... ;-)  )...
	void save_membership_images_to_dcm(string file_path_base, float scale_factor=1000); 
	void save_membership_images_to_vtk(string file_path_base); 
	fcm_image_vector_type get_membership_images();

	fcm_image_vector_type get_image_vector_from_u_vector(); //note that geometrical info is not reconstructed...
};



#endif __fcm__