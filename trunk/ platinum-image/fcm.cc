// $Id: fcm.cc  $

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

#include "fcm.h"

fcm::fcm(vector< image_scalar<float,3>* > vec, vnl_matrix<float> V_init_clusters, float m_fuzzyness, float u_diff_limit, image_binary<3> *mask)
{
	this->image_vector = vec;
	this->V = V_init_clusters;
	this->m = m_fuzzyness;
	u_maxdiff_limit = u_diff_limit;
	image_mask = mask;
	
	n_pix_masked=0;
	if(image_mask !=NULL){
		n_pix_masked = get_num_pixels_in_mask(image_mask);
	}

	//initialize the size of the rest of the objects...
	u = vnl_matrix<float>(n_clust(), n_pix());
	X = vnl_matrix<float>(n_bands(), n_pix());
	int_dist = vnl_matrix<float>(n_clust(), n_pix());
}



int fcm::nx()
{return image_vector[0]->get_size_by_dim(0);}

int fcm::ny()
{return image_vector[0]->get_size_by_dim(1);}

int fcm::nz()
{return image_vector[0]->get_size_by_dim(2);}

int fcm::n_pix()
{
	if(image_mask==NULL){
		return nx()*ny()*nz(); 
	}
	return n_pix_masked;
}

int fcm::n_bands()
{return image_vector.size();}

int fcm::n_clust()
{return V.rows();}


int fcm::get_num_pixels_in_mask(image_binary<3> *image_mask)
{
	int tmp=0;
	for(int k=0;k<nz();k++){
		for(int j=0;j<ny();j++){
			for(int i=0;i<nx();i++){
				if(image_mask->get_voxel(i,j,k)>0){
					tmp++;
				}
			}
		}
	}
	return tmp;
}

void fcm::fill_X(vnl_matrix<float> &X, image_binary<3> *image_mask_local)
{
	cout<<"fill_X..."<<endl;
	cout<<"X.rows()="<<X.rows()<<endl;
	cout<<"X.cols()="<<X.cols()<<endl;
	cout<<"n_pix()="<<n_pix()<<endl;

	int tmp=-1;
	if(image_mask_local == NULL){
		for(int k=0;k<nz();k++){
			for(int j=0;j<ny();j++){
				for(int i=0;i<nx();i++){
					tmp = k*ny()*nx() + j*nx() + i;
					//cout<<tmp<<" ";
					for(int band=0;band<n_bands();band++){
						X(band, tmp) = image_vector[band]->get_voxel(i,j,k);
					}
				}
			}
		}
	}else{
		for(int k=0;k<nz();k++){
			for(int j=0;j<ny();j++){
				for(int i=0;i<nx();i++){
					if(image_mask_local->get_voxel(i,j,k)>0){
						tmp++;
	//					cout<<tmp<<" ";
						for(int band=0;band<n_bands();band++){
							X(band, tmp) = image_vector[band]->get_voxel(i,j,k);
						}
					}
				}
			}
		}
	}
}



void fcm::calc_int_dist_matrix_euclidean(vnl_matrix<float> &int_dist, const vnl_matrix<float> &X, const vnl_matrix<float> &V)
{
	cout<<"calc_int_dist_matrix_euclidean..."<<endl;

	//int_dist(n_clust, n_pix) --> (i,j) 
	//vnl_matrix<float> X(n_bands, n_pix);		//pixel intensities		X(band,pixel)
	//vnl_matrix<float> V(n_clust, n_bands);		//cluster center		V(clust, band)

	int_dist.fill(0);
	for(int i=0;i<n_clust();i++){
		for(int j=0;j<n_pix();j++){
			for(int band=0;band<n_bands();band++){
				int_dist(i,j) += pow( float(X(band,j)-V(i,band)), float(2.0) );
			}
			int_dist(i,j) = sqrt(int_dist(i,j));
		}
	}
}


void fcm::calc_memberships(vnl_matrix<float> &u, const vnl_matrix<float> &int_dist, const float m)
{
	cout<<"calc_memberships..."<<endl;
	//matrix(rows, columns)
	//vnl_matrix<float> u(n_clust, n_pix);		//degree of membership	u(band,pixel)
	//vnl_matrix<float> int_dist(n_clust, n_pix);//distance in feature space... for calc speedup
	//float m = 2.0;			//FCM "fuzzyness" (the larger the more focused cluster weighting)

	u.fill(0);
	float denom=0;
	float factor = 2.0/(m-1);
	cout<<"n_clust()="<<n_clust()<<endl;
	cout<<"n_pix()="<<n_pix()<<endl;
	cout<<"u.rows()="<<u.rows()<<endl;
	cout<<"u.cols()="<<u.cols()<<endl;
	cout<<"int_dist.rows()="<<int_dist.rows()<<endl;
	cout<<"int_dist.cols()="<<int_dist.cols()<<endl;

	for(int i=0;i<n_clust();i++){
		cout<<"i="<<i<<endl;
		for(int j=0;j<n_pix();j++){
//			cout<<" "<<j;
			denom=0;
			for(int ii=0;ii<n_clust();ii++){
				if(int_dist(ii,j)>0){ 
					denom += pow( float(int_dist(i,j)/int_dist(ii,j)), factor );
				}
				//if dist==0 it is simply no included...
			}
			u(i,j) = 1.0/denom;
		}
	}
}


void fcm::calc_cluster_centers(vnl_matrix<float> &V, const vnl_matrix<float> &u, const vnl_matrix<float> &X, float m)
{
	cout<<"calc_cluster_centers..."<<endl;
	//matrix(rows, columns)
	//	vnl_matrix<float> u(n_clust, n_pix);		//degree of membership	u(band,pixel)
	//	vnl_matrix<float> X(n_bands, n_pix);		//pixel intensities		X(band,pixel)
	//	vnl_matrix<float> V(n_clust, n_bands);		//cluster center		V(clust, band)

	V.fill(0);
	float denom;
	float u_tmp;

	for(int i=0;i<n_clust();i++){	//cluster
		denom=0;
		for(int j=0;j<n_pix();j++){	//pix
			u_tmp = pow(u(i,j),m);
			for(int band=0;band<n_bands();band++){	//band
				V(i,band) += u_tmp*X(band,j);
			}
			denom += u_tmp;
		}
		for(int band=0;band<n_bands();band++){	//band
			V(i,band) = V(i,band)/denom;
		}
	}
}


//vector< image_scalar<float,3>* > fcm::get_image_vector_from(vnl_matrix<float> &u, int nx, int ny, int nz)
vector< image_scalar<float,3>* > fcm::get_image_vector_from_u()

{
	// vnl_matrix<float> u(n_clust, n_pix);		//degree of membership	u(band,pixel)
	vector< image_scalar<float,3>* > vec;
	image_scalar<float,3>* im;
	int tmp=-1;

	for(int c=0;c<n_clust();c++){
		tmp=-1;
		im = new image_scalar<float,3>(nx(),ny(),nz());

		if(image_mask==NULL){
			for(int i=0;i<nx();i++){
				for(int j=0;j<ny();j++){
					for(int k=0;k<nz();k++){
						tmp = i + j*nx() + k*nx()*ny();
						im->set_voxel(i,j,k,u(c,tmp));
					}
				}
			}
		}else{
			for(int k=0;k<nz();k++){
				for(int j=0;j<ny();j++){
					for(int i=0;i<nx();i++){
						if(image_mask->get_voxel(i,j,k)>0){
							tmp++;
							//cout<<tmp<<" ";
							im->set_voxel(i,j,k,u(c,tmp));
						}else{
							im->set_voxel(i,j,k,0);
						}
					}
				}
			}
		}

		im->name("FCM_u" + int2str(c));
		vec.push_back(im);
	}

	return vec;
}


void fcm::update()
{
	cout<<"fcm::update()..."<<endl;
	//-----------------------
	// Fill X-matrix... One might keep the images and iterate over the... needed when spatial is used...
	//-----------------------
	cout<<"fill_X..."<<endl;
	fill_X(X, image_mask);  //from the "image_vector"

	//-----------------------
	// Normalize X-matrix intensities... (row-wise...) (and scale image intensities from 0...max --> 0...1 
	// (one might do more intelligent trimming of the image top intensity values)
	//	vnl_matrix<float> X; //pixel intensities		X(band,pixel)
	//-----------------------
	cout<<"scale..."<<endl;
	for(int band=0;band<n_bands();band++){
		X.scale_row(band,1.0/X.get_row(band).max_value());
	}

	//-----------------------
	// FCM-algorithm Loop...
	//-----------------------
	float u_change_max=1;
	int iter=0;
	float u_diff;
	vnl_matrix<float> u2(n_clust(), n_pix());	//Temp. degree of membership2	u2(clust,pixel)

	//do a first roud outside to allow calculation of "u_change_max"...
	calc_int_dist_matrix_euclidean(int_dist, X, V);
	calc_memberships(u2, int_dist, m);
	calc_cluster_centers(V,u2,X,m);
	u = u2;

	cout<<"loop..."<<endl;

	while(u_change_max > u_maxdiff_limit)
	{ 
		iter++;
		cout<<"fcm iteration = "<<iter<<endl;

		//-----------------------
		// Calc dist_functions... int_dist(n_clust, n_pix) 
		// i.e. distance from each pixel_intensity to each cluster... (for example euclidean...)
		//-----------------------
		calc_int_dist_matrix_euclidean(int_dist, X, V);
		//	cout<<endl<<"int_dist="<<int_dist<<endl;

		//-----------------------
		// Update membership values...
		//-----------------------
		calc_memberships(u2, int_dist, m);

		//-----------------------
		// Update cluster centers values...
		//-----------------------
		calc_cluster_centers(V,u2,X,m);
		cout<<"V="<<endl<<V<<endl;

		u_change_max=0;
		for(int i=0;i<u.rows();i++){
			for(int j=0;j<u.cols();j++){
				u_diff = abs(u2(i,j)-u(i,j));
				if(u_diff > u_change_max){
					u_change_max = u_diff;
				}
			}
		}
		cout<<"u_change_max="<<u_change_max<<endl;

		u = u2;
	}
	cout<<"FCM limit reached..."<<endl;
}