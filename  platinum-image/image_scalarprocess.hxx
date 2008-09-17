//////////////////////////////////////////////////////////////////////////
//
//  Scalar process
//
//  Task-specific processing of scalar images
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

template <class ELEMTYPE, int IMAGEDIM>
voxel_set<ELEMTYPE> image_scalar<ELEMTYPE, IMAGEDIM>::get_voxel_set_from_image_data_3D()
{	
	cout<<"get_voxel_set_from_image_data_3D..."<<endl;
	voxel_set<ELEMTYPE> vs;

	for(int z=0; z<this->datasize[2]; z++){		
		for(int y=0; y<this->datasize[1]; y++){
			for(int x=0; x<this->datasize[0]; x++){
				vs.insert_voxel(new voxel<ELEMTYPE>(create_Vector3Dint(x,y,z),this->get_voxel(x,y,z)));
			}
		}
	}

	return vs;
}

template <class ELEMTYPE, int IMAGEDIM>
voxel_set<ELEMTYPE> image_scalar<ELEMTYPE, IMAGEDIM>::get_voxel_set_from_image_data_3D(ELEMTYPE exclude_value)
{	
	cout<<"get_voxel_set_from_image_data_3D..."<<endl;
	voxel_set<ELEMTYPE> vs;
	ELEMTYPE val=0;

	for(int z=0; z<this->datasize[2]; z++){		
		for(int y=0; y<this->datasize[1]; y++){
			for(int x=0; x<this->datasize[0]; x++){
				val = this->get_voxel(x,y,z);
				if(val != exclude_value){
					vs.insert_voxel(new voxel<ELEMTYPE>(create_Vector3Dint(x,y,z),val));
				}
			}
		}
	}

	return vs;
}
	 
/*
template <class ELEMTYPE, int IMAGEDIM>
voxel image_scalar<ELEMTYPE, IMAGEDIM>::get_median_voxel_3D(ELEMTYPE *exclude_value)
{
	voxel_set vs = get_voxel_set_from_image_data_3D(
	vs.get_median_voxel();
}
*/

template <class ELEMTYPE, int IMAGEDIM>
voxel_set<ELEMTYPE> image_scalar<ELEMTYPE, IMAGEDIM>::set_val_to_voxel_that_has_no_neighbour_with_val_in_vox_radius(int radius, Vector3Dint pos, ELEMTYPE from_val, ELEMTYPE to_val, ELEMTYPE nb_val)
{
	int z_from = std::max(0,pos[2]-radius);
	int y_from = std::max(0,pos[1]-radius);
	int x_from = std::max(0,pos[0]-radius);
	int z_to = std::min(int(this->nz()-1),pos[2]+radius);
	int y_to = std::min(int(this->ny()-1),pos[1]+radius);
	int x_to = std::min(int(this->nx()-1),pos[0]+radius);

	ELEMTYPE val;
	voxel_set<ELEMTYPE> res;

	for(int c=z_from; c<=z_to; c++){
		for(int b=y_from; b<=y_to; b++){
			for(int a=x_from; a<=x_to; a++){

				val = this->get_voxel(a,b,c);
				if(val == from_val){
					if(this->get_number_of_voxels_with_value_in_26_nbh(create_Vector3Dint(a,b,c),nb_val)==0){
						this->set_voxel(a,b,c,to_val);
						res.insert_voxel(new voxel<ELEMTYPE>(create_Vector3Dint(a,b,c),val));
					}
				}

			}
		}
	}
	return res;
}

template <class ELEMTYPE, int IMAGEDIM>
voxel_set<ELEMTYPE> image_scalar<ELEMTYPE, IMAGEDIM>::set_val_to_voxel_that_has_some_neighbour_with_val_in_vox_radius(int radius, Vector3Dint pos, ELEMTYPE from_val, ELEMTYPE to_val, ELEMTYPE nb_val)
{
	int z_from = std::max(0,pos[2]-radius);
	int y_from = std::max(0,pos[1]-radius);
	int x_from = std::max(0,pos[0]-radius);
	int z_to = std::min(int(this->nz()-1),pos[2]+radius);
	int y_to = std::min(int(this->ny()-1),pos[1]+radius);
	int x_to = std::min(int(this->nx()-1),pos[0]+radius);

	ELEMTYPE val;
	voxel_set<ELEMTYPE> res;

	for(int c=z_from; c<=z_to; c++){
		for(int b=y_from; b<=y_to; b++){
			for(int a=x_from; a<=x_to; a++){

				val = this->get_voxel(a,b,c);
				if(val == from_val){
					if(this->get_number_of_voxels_with_value_in_26_nbh(create_Vector3Dint(a,b,c),nb_val) > 0){
						this->set_voxel(a,b,c,to_val);
						res.insert_voxel(new voxel<ELEMTYPE>(create_Vector3Dint(a,b,c),val));
					}
				}

			}
		}
	}
	return res;
}


template <class ELEMTYPE, int IMAGEDIM>
image_scalar<ELEMTYPE, IMAGEDIM>* image_scalar<ELEMTYPE, IMAGEDIM>::calculate_T1Map_from_two_flip_angle_MRvolumes_3D(image_scalar<ELEMTYPE, IMAGEDIM > *small_flip, float body_thres, float t1_min, float t1_max)
{
	float fa1 = this->meta.get_data_float(DCM_FLIP);
	float fa2 = small_flip->meta.get_data_float(DCM_FLIP);
	float tr = this->meta.get_data_float(DCM_TR);
	float te = this->meta.get_data_float(DCM_TE);
	image_scalar<ELEMTYPE, IMAGEDIM>* t1map = NULL;

	if(fa1 <= fa2 || tr<=0 || te<=0){
		pt_error::error("calculate_T1Map_from_two_flip_angle_MRvolumes - Wrong flip angles...",pt_error::debug);
	}else{

		t1map = new image_scalar<ELEMTYPE, IMAGEDIM>(this);

		float alpha_l = pt_PI*fa1/180.0;
		float alpha_s = pt_PI*fa2/180.0;
		float sin_ratio = sin(alpha_l)/sin(alpha_s);
		float A=0;
		float t1=0;
		float tmp=0;

		for (int z=0; z < this->datasize[2]; z++){
			for (int y=0; y < this->datasize[1]; y++){
				for (int x=0; x < this->datasize[0]; x++){
					tmp = this->get_voxel(x,y,z);
					if(tmp<=body_thres){
						t1=t1_min;
					}else{
						A = small_flip->get_voxel(x,y,z) / tmp * sin_ratio;
						t1 = tr/ log( (cos(alpha_l)-A*cos(alpha_s))/(1-A) );

						//limiting the resulting T1-Range...
						if(t1<t1_min){t1=t1_min;}
						if(t1>t1_max){t1=t1_max;}
					}
					t1map->set_voxel(x,y,z,t1);
				}
			}
		}
	}
	return t1map;
}

template <class ELEMTYPE, int IMAGEDIM>
float image_scalar<ELEMTYPE, IMAGEDIM>::grad_mag_voxel( int x, int y, int z, GRAD_MAG_TYPE type )
{
	if ( ! this->is_voxelpos_within_image_3D( x, y, z) )
	{	// the point is not within the image
		return -1;
	}

	float mag_x = this->get_voxel( x + 1, y, z ) - this->get_voxel( x - 1, y, z );
	float mag_y = this->get_voxel( x, y + 1, z ) - this->get_voxel( x, y - 1, z );
	float mag_z = this->get_voxel( x, y, z + 1 ) - this->get_voxel( x, y, z - 1 );

	switch ( type )
	{
		case X:
			return sqrt( mag_x * mag_x );	
		case Y:
			return sqrt( mag_y * mag_y );
		case Z:
			return sqrt( mag_z * mag_z );
		case XY:
			return sqrt( mag_x * mag_x + mag_y * mag_y );
		case XZ:
			return sqrt( mag_x * mag_x + mag_z * mag_z );
		case YZ:
			return sqrt( mag_y * mag_y + mag_z * mag_z );
		case XYZ:
			return sqrt( mag_x * mag_x + mag_y * mag_y + mag_z * mag_z );
		default:
			return -1;
	}
}

template <class ELEMTYPE, int IMAGEDIM>
float image_scalar<ELEMTYPE, IMAGEDIM>::grad_mag_voxel( Vector3D point, GRAD_MAG_TYPE type )
{
	return grad_mag_voxel( point[0], point[1], point[2], type );
}

template <class ELEMTYPE, int IMAGEDIM>
float image_scalar<ELEMTYPE, IMAGEDIM>::weight_of_type( Vector3D center, Vector3D current, WEIGHT_TYPE type )
{
	switch ( type )
	{
		case CHESSBOARD:
			int dist_x = abs( current[0] - center[0] );
			int dist_y = abs( current[1] - center[1] );
			int dist_z = abs( current[2] - center[2] );
			return max( max( dist_x, dist_y ), dist_z );
			
		default:
			return 0;
	}
}

template <class ELEMTYPE, int IMAGEDIM>
float image_scalar<ELEMTYPE, IMAGEDIM>::appl_wb_correct_inclination(image_scalar<ELEMTYPE, IMAGEDIM> *fat, image_scalar<ELEMTYPE, IMAGEDIM> *water)
{
	cout<<"appl_wb_correct_inclination..."<<endl;
	image_scalar<ELEMTYPE,IMAGEDIM> *sum = new image_scalar<ELEMTYPE,IMAGEDIM>(fat);
	sum->combine(water,COMB_ADD);
	sum->fill_region_3D(1,0,40,0);	//clear first 40 slices to reduce sensitivity to position of arms....
	vector<Vector3D> cg_points = sum->get_in_slice_center_of_gravities_in_dir(1,1);
	line3D cg_line;
	cg_line.least_square_fit_line_to_points_in_3D(cg_points,1);

	image_scalar<unsigned short,3> *f2  = fat->correct_inclined_object_slicewise_after_cg_line(0,cg_line);
	image_scalar<unsigned short,3> *w2  = water->correct_inclined_object_slicewise_after_cg_line(0,cg_line);

	copy_data(f2,fat);
	copy_data(w2,water);
	delete sum;
	delete f2;
	delete w2;

	return cg_line.get_direction()[0];
}

template <class ELEMTYPE, int IMAGEDIM>
image_binary<3>* image_scalar<ELEMTYPE, IMAGEDIM>::appl_wb_segment_body_from_sum_image(int initial_thres)
{
	cout<<"Treshold..."<<endl;
	image_binary<3> *b = this->threshold(initial_thres);
//	b->save_to_VTK_file(base+"__b01_Body.vtk");

//	cout<<"Erode..."<<endl;
	b->erode_3D(3);
//	b->save_to_VTK_file(base+"__b02_Body_er.vtk");

//	cout<<"Dilate..."<<endl;
	b->dilate_3D(3);
	b->dilate_3D(3);
//	b->save_to_VTK_file(base+"__b03_Body_dil.vtk");

//	cout<<"Region Grow Bg..."<<endl;
	image_binary<3> *b2 = b->region_grow_3D(create_Vector3D(0,0,0),0,0);
//	body->save_to_VTK_file(base+"__b04_Body_rg.vtk");

	cout<<"Invert..."<<endl;
	b2->invert();
//	b2->save_to_VTK_file(base+"__b05_Body_rg_inv.vtk");

	cout<<"Erode..."<<endl;
	b2->erode_3D(3);
//	b2->save_to_VTK_file(base+"__b06_Body_er.vtk");
	delete b;
	b2->largest_object_3D();
	return b2;
}

template <class ELEMTYPE, int IMAGEDIM>
image_binary<3>* image_scalar<ELEMTYPE, IMAGEDIM>::appl_wb_segment_rough_lung_from_sum_image(image_binary<3> *mask, float lung_volume_in_litres)
{
	histogram_1D<ELEMTYPE> *h = this->get_histogram_from_masked_region_3D(mask);
	int lung_tresh = h->get_intensity_at_included_num_pix_from_lower_int(0,this->get_num_voxels_per_dm3() * lung_volume_in_litres);	// volume in liters...
	cout<<"lung_tresh="<<lung_tresh<<endl;

	image_binary<3> *rough_lung = this->threshold(0,lung_tresh);
	rough_lung->name("rough_lung");
	rough_lung->fill_region_3D(1,0,50,0);
	rough_lung->fill_region_3D(1,95,rough_lung->get_size_by_dim(1)-1,0);
	rough_lung->mask_out(mask);
	cout<<"Lungvol_after_thres_and_mask="<<rough_lung->get_voxel_volume_in_dm3()*rough_lung->get_number_of_voxels_with_value(1)<<endl;

	return rough_lung;
}

template <class ELEMTYPE, int IMAGEDIM>
image_binary<3>* image_scalar<ELEMTYPE, IMAGEDIM>::appl_wb_segment_one_lung_from_sum_image(image_binary<3> *thorax_body_mask, float lung_volume_in_litres, int low_threshold)
{

	image_binary<3> *rough_lung = this->appl_wb_segment_rough_lung_from_sum_image(thorax_body_mask, lung_volume_in_litres);
	rough_lung->name("rough_lung");
	rough_lung->save_to_file("c:/Joel/TMP/combi_1_rough_lung.vtk");

	image_integer<short,3> *dist = rough_lung->distance_345_3D();
	dist->name("dist");
	dist->save_to_file("c:/Joel/TMP/combi_2_dist.vtk");

	image_binary<3> *seeds = dist->threshold(15);
//	image_binary<3> *seeds = new image_binary<3>(rough_lung);
//	seeds->name("seeds");
//	seeds->erode_3D_26Nbh();
//	seeds->erode_3D_26Nbh();
//	seeds->erode_3D_26Nbh();
	seeds->largest_object_3D();

	image_binary<3> *res = dist->region_grow_3D_if_lower_intensity(seeds);
	res->name("res");
	res->dilate_3D_26Nbh();
	res->dilate_3D_26Nbh();
	res->save_to_file("c:/Joel/TMP/combi_3_res.vtk");

	histogram_1D<ELEMTYPE> *h2 = this->get_histogram_from_masked_region_3D(res);
	h2->save_histogram_to_txt_file("c:/Joel/TMP/combi_4_hist.txt");

	float a;
	float c;
	float s;
	h2->fit_gaussian_to_intensity_range(a,c,s,1,low_threshold);
	cout<<"a="<<a<<endl;
	cout<<"c="<<c<<endl;
	cout<<"s="<<s<<endl;
	cout<<"-->thres="<<c+2*s<<endl;

//	gaussian *g = new gaussian(a,c,s);
//	h2->save_histogram_to_txt_file(base + "__c08_lung hist.txt",false,g);

	//thresholda p¬ c + 2*s...

	image_binary<3> *right_lung = this->threshold(0,c+2*s);
	right_lung->name("right_lung");
	right_lung->mask_out(res);
	right_lung->largest_object_3D();

//	datamanagement.add(rough_lung);
//	datamanagement.add(dist);
//	datamanagement.add(seeds);
//	datamanagement.add(res);
	delete rough_lung;
	delete dist;
	delete seeds;
	delete res;

	return right_lung;
}

template <class ELEMTYPE, int IMAGEDIM>
image_binary<3>* image_scalar<ELEMTYPE, IMAGEDIM>::appl_wb_segment_both_lungs_from_sum_image(image_binary<3> *body_mask, float lung_volume_in_litres, int low_threshold)
{
	// ---- Create Common thorax mask -----
	image_binary<3> *thorax_mask = new image_binary<3>(body_mask);
	thorax_mask->name("thorax_mask");
	thorax_mask->fill_region_3D(1,0,45,0);
	thorax_mask->fill_region_3D(1,100,thorax_mask->get_size_by_dim(1)-1,0);

	Vector3D cg = thorax_mask->get_center_of_gravity(1);
	cout<<"thorax_cg="<<cg<<endl;
//	cout<<"s="<<thorax_mask->get_size_by_dim(0)<<endl;

	thorax_mask->erode_3D_26Nbh();


	// ---- Segment RIGHT Lung -----
	cout<<"***Right Lung***"<<endl;
	image_binary<3> *right_mask = new image_binary<3>(thorax_mask);
	right_mask->fill_region_3D(0,cg[0]+2,body_mask->get_size_by_dim(0)-1,0);
	image_binary<3> *r_lung = this->appl_wb_segment_one_lung_from_sum_image(right_mask,lung_volume_in_litres, low_threshold);
	r_lung->name("r_lung");



	// ---- Segment LEFT Lung -----
	cout<<"***Left Lung***"<<endl;
	int x1,y1,z1,x2,y2,z2;
	r_lung->get_span_of_value_3D(1,x1,y1,z1,x2,y2,z2);

	image_binary<3> *left_mask = new image_binary<3>(thorax_mask);
	left_mask->fill_region_3D(0,0,cg[0]-2,0);
	left_mask->fill_region_3D(1,0,y1,0);
	left_mask->fill_region_3D(1,y2,left_mask->ny()-1,0);
	image_binary<3> *l_lung = this->appl_wb_segment_one_lung_from_sum_image(left_mask,lung_volume_in_litres, low_threshold);
	l_lung->name("l_lung");


	r_lung->combine(l_lung,COMB_MAX);
	r_lung->name("lungs");

	datamanagement.add(thorax_mask);
//	datamanagement.add(right_mask);
//	datamanagement.add(left_mask);
	delete right_mask;
	delete left_mask;
	datamanagement.add(r_lung);
//	datamanagement.add(l_lung);
	delete l_lung;

	return r_lung;
}


/*
template <class ELEMTYPE, int IMAGEDIM>
image_binary<3>* image_scalar<ELEMTYPE, IMAGEDIM>::appl_wb_segment_lungs_from_sum_image(image_binary<3> *body_mask, float lung_volume_in_litres)
{
	
	image_binary<3> *half_body_mask = binary_copycast<3>(body_mask);	// = image_binary<3>(body_mask);
	//clear a priori regions.... y = 0...40 and 120...
	half_body_mask->fill_region_3D(1,0,45,0);
	half_body_mask->fill_region_3D(1,100,body_mask->get_size_by_dim(1)-1,0);
	half_body_mask->erode_3D_26Nbh();

	histogram_1D<ELEMTYPE> *h = this->get_histogram_from_masked_region_3D(half_body_mask);
	h->save_histogram_to_txt_file("d:/Joel/TMP/hist.txt");
	int lung_tresh = h->get_intensity_at_included_num_pix_from_lower_int(0,this->get_num_voxels_per_dm3()*lung_volume_in_litres);
	cout<<"lung_tresh="<<lung_tresh<<endl;

//-------------------------------------
//New determination of lung threshold....
//-------------------------------------

//	float amp=0;
//	float center=0;
//	float sigma=0;
//	h->fit_gaussian_to_intensity_range(amp,center,sigma,50,this->get_max()-1,true);

//	cout<<endl;
//	cout<<"Result-amp="<<amp<<endl;
//	cout<<"Result-center="<<center<<endl;
//	cout<<"Result-sigma="<<sigma<<endl;

//	gaussian *g = new gaussian(amp,center,sigma);
//	h->save_histogram_to_txt_file(base+"__c04c_masked_Lung_histogram.txt",g,false); //for threshold determination

//	int lung_tresh = center - 3*sigma;
//	cout<<"my_lung_thres="<<lung_tresh<<endl;

//-------------------------------------
//-------------------------------------

//	image_binary<3> *lungs = this->threshold(0,initial_upper_thres);
	image_binary<3> *lungs = this->threshold(0,lung_tresh);

	//clear a priori regions.... y = 0...50 and 95...
	lungs->fill_region_3D(1,0,50,0);
	lungs->fill_region_3D(1,95,lungs->get_size_by_dim(1)-1,0);

//	lungs->mask_out(body_mask);
	lungs->mask_out(half_body_mask);

	cout<<"Lungvol_after_thres_and_mask="<<lungs->get_voxel_volume_in_dm3()*lungs->get_number_of_voxels_with_value(1)<<endl;

	image_binary<3> *rough_lung_mask = new image_binary<3>(lungs);
//	rough_lung_mask->erode_3D(7);
//	rough_lung_mask->dilate_3D(20);
	rough_lung_mask->erode_3D_26Nbh();

	image_binary<3> *lung1 = new image_binary<3>(rough_lung_mask);
	image_binary<3> *lung2 = new image_binary<3>(rough_lung_mask);
	lung1->largest_object_3D();
	lung2->mask_out(lung1,0);
	lung2->largest_object_3D();
	lung1->combine(lung2,COMB_MAX);
	lung1->dilate_3D_26Nbh();
	lung1->dilate_3D_26Nbh();
//	lung1->save_to_file("D:/Joel/TMP/roughLungs.vtk");


	lungs->mask_out(lung1);

	delete half_body_mask;

	delete rough_lung_mask;
	delete lung1;
	delete lung2;

	return lungs;
}
*/


template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::appl_wb_segment_find_crotch_pos_from_water_percent_image(int &pos_x, int &pos_y, int mip_thres)
{
	cout<<"appl_wb_segment_find_crotch_pos_from_water_percent_image..."<<endl;

//	cout<<"create MIP (z)..."<<endl;
	image_scalar<ELEMTYPE,IMAGEDIM> *tmip = this->create_projection_3D(2);
	tmip->name("_tmip");
//	tmip->save_to_VTK_file("D:/Joel/TMP/_wpMIP1.vtk");

	cout<<"threshold MIP..."<<endl;
	image_binary<3> *tbin = tmip->threshold(mip_thres);  //95% MIP - water content....
	tbin->name("_tbin");
//	tbin->save_to_VTK_file("D:/Joel/TMP/_wpMIP2_thres.vtk");


	cout<<"filter bin..."<<endl;
//	tbin->dilate_2D();
	tbin->erode_2D();
	tbin->dilate_2D();
//	tbin->save_to_VTK_file("D:/Joel/TMP/_wpMIP3_open2D.vtk");


	cout<<"find_crotch..."<<endl;
	int nx = tbin->get_size_by_dim(0);
	int ny = tbin->get_size_by_dim(1);
	int x_start = nx/2 - nx/20;    //search spans 10% of center region...
	int x_end = nx/2 + nx/20;

	pos_y=1000;
	pos_x=0;

	for(int x=x_start;x<x_end;x++){			//for each x in 10% range from center...
		for(int y=ny-1;y>=0;y--){			//see how faar "up" you can go between the legs...
			if(tbin->get_voxel(x,y)==0){
				if(y<pos_y){
					pos_y = y;
					pos_x = x;
				}
			}else{
				break;
			}
		}
	}

	cout<<"pos_x="<<pos_x<<endl;
	cout<<"pos_y(=crotch_level)="<<pos_y<<endl;

//	datamanagement.add(tmip);
//	datamanagement.add(tbin);
	delete tmip;
	delete tbin;
}

template <class ELEMTYPE, int IMAGEDIM>
image_binary<3>* image_scalar<ELEMTYPE, IMAGEDIM>::appl_wb_segment_VAT_mask_from_this_water_percent_abd_subvolume(image_binary<3> *bin_body, string base)
{
	cout<<"Erode body_mini..."<<endl;
	image_binary<> *body_mini = new image_binary<>(bin_body);
	body_mini->erode_3D_26Nbh();
	body_mini->erode_3D_26Nbh();
	body_mini->erode_3D_26Nbh();
	
	cout<<"Mask body mini,	Threshold, largest object, Convex Hull..."<<endl;
	image_scalar<ELEMTYPE, IMAGEDIM> *abd = new image_scalar<ELEMTYPE, IMAGEDIM>(this);
	abd->mask_out(body_mini);
	image_binary<> *vat_mask = abd->threshold(500);
	if(base!=""){
		vat_mask->save_to_VTK_file(base + "__g00b_convex_hull.vtk");
	}
	vat_mask->largest_object_3D();
	vat_mask->convex_hull_line_filling_3D(0);
	vat_mask->convex_hull_line_filling_3D(2);
	if(base!=""){
		vat_mask->save_to_VTK_file(base + "__g00b_convex_hull.vtk");
	}
	vat_mask->dilate_3D_26Nbh();
	vat_mask->erode_3D_26Nbh();
	if(base!=""){
		vat_mask->save_to_VTK_file(base + "__g00c_closed.vtk");
	}

	image_binary<> *vat_mask_mini = new image_binary<>(vat_mask);
	vat_mask_mini->erode_2D(6,1);


	image_binary<> *sat = abd->threshold(0,500);
	image_binary<> *sat_seed = new image_binary<>(sat);
	sat_seed->mask_out(vat_mask,0);
	sat_seed->mask_out(bin_body);
	int x1;
	int y1;
	int z1;
	int x2;
	int y2;
	int z2;
	vat_mask->get_span_of_values_larger_than_3D(0,x1,y1,z1,x2,y2,z2);
	sat_seed->fill_region_3D(1,0,y1-1,0);
	sat_seed->fill_region_3D(1,y2+1,sat_seed->ny()-1,0);
	if(base!=""){
		sat_seed->save_to_VTK_file(base + "__g00e_sat_seed.vtk");
	}

	sat->mask_out(vat_mask_mini,0);
	if(base!=""){
		sat->save_to_VTK_file(base + "__g00f_sat_limit.vtk");
	}

	sat->region_grow_3D(sat_seed,1);

	if(base!=""){
		sat->save_to_VTK_file(base + "__g00g_sat_grown.vtk");
	}

	if(base!=""){
		vat_mask->save_to_VTK_file(base + "__g00h_vat_before.vtk");
	}
	vat_mask->mask_out(sat,0);
	if(base!=""){
		vat_mask->save_to_VTK_file(base + "__g00i_vat_after.vtk");
	}


	delete body_mini;
	delete abd;
	delete sat;
	delete sat_seed;

	return vat_mask;
}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::appl_wb_normalize_features_slicewise_by_global_mean_on_this_float (image_scalar<float, 3>* second_feature, image_scalar<float, 3>* sum, image_binary<3>* body_lung_mask)
{
	if (sum==NULL) {
		sum = new image_scalar<float, 3>(this);
		sum->combine(second_feature, COMB_ADD);
	}
	if (body_lung_mask==NULL) {
		body_lung_mask=new image_binary<3>(sum->appl_wb_segment_body_from_sum_image());
		image_binary<3>* lungs=sum->appl_wb_segment_both_lungs_from_sum_image(body_lung_mask);
		body_lung_mask->combine(lungs, COMB_SUB);
		delete lungs;
	}
	int value=0;
	int	num_voxels=0;
	int total_num_voxels=0;
	float total_mean=0;
	vector<float> mean(sum->get_size_by_dim(1));

	for (int j=0; j<sum->get_size_by_dim(1); j++){
		num_voxels=0;
		mean[j] = sum->get_mean_from_slice_3d(1, j, body_lung_mask);
		for (int i=0; i < sum->get_size_by_dim(0); i++){
			for (int k=0; k < sum->get_size_by_dim(2); k++){
				if (body_lung_mask->get_voxel(i,j,k)) {num_voxels++;}
			}
		}
		if (num_voxels!=0) {
			total_mean += mean[j]*num_voxels;
			total_num_voxels += num_voxels;
		}
		else {mean[j]=0;}
	}
	if (total_num_voxels!=0) {total_mean=total_mean / total_num_voxels;}

	float factor;
	// Set new intensities
	for (int j=0; j<sum->get_size_by_dim(1); j++){
		if (mean[j]!=0) {factor = total_mean / mean[j];}
		else {factor=1;}
		this->scale_slice_by_factor_3d(1, factor, j);
		second_feature->scale_slice_by_factor_3d(1, factor, j);
	}
	this->data_has_changed();
	second_feature->data_has_changed();
}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::appl_wb_SIM_bias_correction_on_this_float(image_scalar<float, 3>* second_feature, int num_iterations, float iteration_strength, float map_x_smoothing_std_dev, float map_y_smoothing_std_dev, float map_z_smoothing_std_dev, float feat1_smoothing_std_dev, float feat2_smoothing_std_dev, image_binary<3>* body_lung_mask, int num_buckets_feat1, int num_buckets_feat2, bool save_corrected_images_each_iteration, bool save_histogram_each_iteration, bool save_field_each_iteration) 
{
	image_scalar<float,3> *feat1 = new image_scalar<float,3>(this);
	image_scalar<float,3> *feat2 = new image_scalar<float,3>(second_feature);
		
	if (body_lung_mask==NULL) {
		cout << "Calculating body-lung mask..." << endl;
		image_scalar<float,3> *sum = new image_scalar<float,3>(feat1);
		sum->combine(feat2, COMB_ADD);
		body_lung_mask=sum->appl_wb_segment_body_from_sum_image();
		image_binary<3>* lungs=sum->appl_wb_segment_both_lungs_from_sum_image(body_lung_mask);
		body_lung_mask->combine(lungs, COMB_SUB);
		delete sum; delete lungs;
	}

	// bias correction field:
	image_scalar<float,3> *field = new image_scalar<float,3>(feat1);
	field->fill(0);	field->data_has_changed();
				
	// corrected images:
	image_scalar<float,3> *feat1_corr = new image_scalar<float,3>(feat1);
	image_scalar<float,3> *feat2_corr = new image_scalar<float,3>(feat2);

	//Parameters:
	filter_gaussian* gauss_feat1=new filter_gaussian(num_buckets_feat1-num_buckets_feat1%2+1, 0, feat1_smoothing_std_dev); // odd size of kernel
	filter_gaussian* gauss_feat2=new filter_gaussian(num_buckets_feat2-num_buckets_feat2%2+1, 1, feat2_smoothing_std_dev);
	filter_sobel_2d* sobel_feat1=new filter_sobel_2d(0);
	filter_sobel_2d* sobel_feat2=new filter_sobel_2d(2);
	filter_gaussian* gauss_x=new filter_gaussian(map_x_smoothing_std_dev*2+1,0,map_x_smoothing_std_dev);
	filter_gaussian* gauss_y=new filter_gaussian(map_y_smoothing_std_dev*2+1,1,map_y_smoothing_std_dev);
	filter_gaussian* gauss_z=new filter_gaussian(map_z_smoothing_std_dev*2+1,2,map_z_smoothing_std_dev);
	float feat1_scale; float feat2_scale;
	int feat1_bucket; int feat2_bucket;
	float feat1_min; float feat2_min;
	int bodysize=body_lung_mask->get_number_of_voxels_with_value(1);
	int xsize=feat1->get_size_by_dim(0);
	int ysize=feat1->get_size_by_dim(1);
	int zsize=feat1->get_size_by_dim(2);
	string str;

	image_scalar<float,3>** fields = new image_scalar<float,3>*[num_iterations];
	image_scalar<float,3>** feat1_corred = new image_scalar<float,3>*[num_iterations];
	image_scalar<float,3>** feat2_corred = new image_scalar<float,3>*[num_iterations];
	image_scalar<unsigned short,3>** histograms = new image_scalar<unsigned short,3>*[num_iterations];

	// iteration starts
	for (int iter=0; iter<num_iterations; iter++)
	{
		cout << "-----Starting iteration " << iter+1 << " out of " << num_iterations << " -----" << endl;

		// Calculate feature space
		image_scalar<unsigned short,3> *temp3 = feat1_corr->create2Dhistogram_3D(feat2_corr, true, num_buckets_feat1, num_buckets_feat2, body_lung_mask);
		image_scalar<float,3> *hist = new image_scalar<float,3>(temp3); delete temp3;
	
		// Expand
		image_scalar<float,3> *hist_expanded = dynamic_cast<image_scalar<float,3 >*>(hist->expand_borders(1, 1, 0));
		delete hist;
			
		// Calculate force in each bucket: Smooth, normalize and logarithmate, then derivate in each direction with sobel filter
		hist_expanded->filter_3D(gauss_feat1, 0);
		hist_expanded->filter_3D(gauss_feat2, 0);
		hist_expanded->set_sum_of_voxels_to_value(1);
		hist_expanded->logarithm_3d(0);

		image_scalar<float,3> *feat1_force_expanded = new image_scalar<float,3>(hist_expanded);
		image_scalar<float,3> *feat2_force_expanded = new image_scalar<float,3>(hist_expanded);
		delete hist_expanded;
		feat1_force_expanded->filter_3D(sobel_feat1, 0);
		feat2_force_expanded->filter_3D(sobel_feat2, 0);
	
		// Contract
		image_scalar<float,3> *feat1_force = dynamic_cast<image_scalar<float,3 >*>(feat1_force_expanded->get_subvolume_from_region_3D(1, 1, 0, num_buckets_feat1, num_buckets_feat2, 0));
		image_scalar<float,3> *feat2_force = dynamic_cast<image_scalar<float,3 >*>(feat2_force_expanded->get_subvolume_from_region_3D(1, 1, 0, num_buckets_feat1, num_buckets_feat2, 0));
		delete feat1_force_expanded; delete feat2_force_expanded;
	
		// Map forces to inhomogeniety field map as Force = feat1_force * feat1_intensity + feat2_force * feat2_intensity
		image_scalar<float,3> *inh_map = new image_scalar<float,3>(field);
		feat1_min=feat1_corr->get_min(); feat2_min=feat2_corr->get_min();
		feat1_scale = float(feat1_corr->get_max()-feat1_min)*1.000001/float(num_buckets_feat1);
		feat2_scale = float(feat2_corr->get_max()-feat2_min)*1.000001/float(num_buckets_feat2);
		for (int x=0; x<xsize; x++) {
			for (int y=0; y<ysize; y++) {
				for (int z=0; z<zsize; z++) {
					if (body_lung_mask->get_voxel(x,y,z))	{
						feat1_bucket=(feat1_corr->get_voxel(x,y,z)-feat1_min)/feat1_scale;
						feat2_bucket=(feat2_corr->get_voxel(x,y,z)-feat2_min)/feat2_scale;
						inh_map->set_voxel(x,y,z, (feat1_force->get_voxel(feat1_bucket,feat2_bucket)*feat1_bucket+feat2_force->get_voxel(feat1_bucket,feat2_bucket)*feat2_bucket) );
					}
				}
			}
		}
		delete feat1_force; delete feat2_force;
			
		// Smooth in each direction
		cout << "Gauss smoothing in x-direction" << endl;
		inh_map->filter_3D(gauss_x, 1, body_lung_mask, 1);
		cout << "Gauss smoothing in y-direction" << endl;
		inh_map->filter_3D(gauss_y, 1, body_lung_mask, 1);
		cout << "Gauss smoothing in z-direction" << endl;
		inh_map->filter_3D(gauss_z, 1, body_lung_mask, 1);

		// field[i] = field[i-1] + iteration_strength*(force / mean(abs(force)))
		inh_map->mask_out(body_lung_mask);
		inh_map->scale_by_factor(iteration_strength/((inh_map->get_sum_of_voxels(true, body_lung_mask))/bodysize)); // oklart varfàr
		field->combine(inh_map, COMB_ADD);
		field->data_has_changed();
			
		if (save_field_each_iteration) {
			fields[iter] = new image_scalar<float,3>(field);
			str = "Field, iteration " + int2str(iter+1);
			fields[iter]->name(str);
			datamanagement.add(fields[iter]);
		}
		delete inh_map;
	
		//Berâkna ekv (7) i SIM-paper
		//feat1_corr->fill(0); feat2_corr->fill(0);
		feat1_corr->fill(1); feat2_corr->fill(1);
		feat1_corr->combine(field, COMB_ADD); feat2_corr->combine(field, COMB_ADD);
		//feat1_corr->add_value_to_all_voxels(1); feat2_corr->add_value_to_all_voxels(1);
		feat1_corr->combine(feat1, COMB_MULT); feat2_corr->combine(feat2, COMB_MULT);
		//Set negative values to zero
		for (int x=0; x<xsize; x++) {
			for (int y=0; y<ysize; y++) {
				for (int z=0; z<zsize; z++) {
					if (feat1_corr->get_voxel(x,y,z)<0) {feat1_corr->set_voxel(x,y,z,0);}
					if (feat2_corr->get_voxel(x,y,z)<0) {feat2_corr->set_voxel(x,y,z,0);}
				}
			}
		}
		feat1_corr->data_has_changed(); feat2_corr->data_has_changed();

		if (save_histogram_each_iteration) {	
			histograms[iter] = feat1_corr->create2Dhistogram_3D(feat2_corr, false, num_buckets_feat1, num_buckets_feat2, body_lung_mask);
			str = "Histogram, iteration " + int2str(iter+1);
			histograms[iter]->data_has_changed();
			histograms[iter]->name(str);
			datamanagement.add(histograms[iter]);
		}

		if (save_corrected_images_each_iteration) {
			feat1_corred[iter] = new image_scalar<float,3>(feat1_corr);
			str = "Corrected feature 1, iteration " + int2str(iter+1);
			feat1_corred[iter]->name(str);
			datamanagement.add(feat1_corred[iter]);
			feat2_corred[iter] = new image_scalar<float,3>(feat2_corr);
			str = "Corrected feature 2, iteration " + int2str(iter+1);
			feat2_corred[iter]->name(str);
			datamanagement.add(feat2_corred[iter]);
		}

	// end of iteration
	}
	// Delete stuff
	delete feat1; delete feat2; delete body_lung_mask;
	delete gauss_feat1; delete gauss_feat2;
	delete sobel_feat1; delete sobel_feat2;
	delete gauss_x; delete gauss_y; delete gauss_z;
	delete field;

	cout << "Saving to file corrected_feat1_backup.vtk" << endl;
	feat1_corr->save_to_VTK_file("corrected_feat1_backup.vtk");
	cout << "Saving to file corrected_feat2_backup.vtk" << endl;
	feat2_corr->save_to_VTK_file("corrected_feat2_backup.vtk");

	feat1_corr->name("Corrected feature 1"); datamanagement.add(feat1_corr);
	feat2_corr->name("Corrected feature 2"); datamanagement.add(feat2_corr);
}


template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::appl_1D_SIM_bias_correction(image_binary<3>* mask, int num_iterations, float iteration_strength, float map_x_smoothing_std_dev, float map_y_smoothing_std_dev, float map_z_smoothing_std_dev, float feat1_smoothing_std_dev, int num_buckets_feat1, bool save_corrected_images_each_iteration, bool save_histogram_each_iteration, bool save_field_each_iteration) 
{
	// corrected images:
	image_scalar<float,3> *feat1_corr = new image_scalar<float,3>(this, "feat1_corr"); //Note... Used as the starting value...
	cout<<"feat1_corr->get_min()="<<feat1_corr->get_min()<<endl;
	cout<<"feat1_corr->get_max()="<<feat1_corr->get_max()<<endl;

	// bias correction field:
	image_scalar<float,3> *field = new image_scalar<float,3>(feat1_corr, "field");
	field->fill(0);	
	field->data_has_changed();

	image_scalar<float,3> *tmp_field = new image_scalar<float,3>(field, "tmp_field");


	//Parameters:
	filter_gaussian* gauss_x=new filter_gaussian(map_x_smoothing_std_dev*2+1,0,map_x_smoothing_std_dev);
	filter_gaussian* gauss_y=new filter_gaussian(map_y_smoothing_std_dev*2+1,1,map_y_smoothing_std_dev);
	filter_gaussian* gauss_z=new filter_gaussian(map_z_smoothing_std_dev*2+1,2,map_z_smoothing_std_dev);

	int bodysize = mask->get_number_of_voxels_with_value(1);
	int xsize=feat1_corr->get_size_by_dim(0);
	int ysize=feat1_corr->get_size_by_dim(1);
	int zsize=feat1_corr->get_size_by_dim(2);
	histogram_1D<float> *hist = NULL;


	for (int iter=0; iter<num_iterations; iter++){
		cout << "---------Starting iteration " << iter+1 << " out of " << num_iterations << " ---------" << endl;

		// Calculate feature space
		if(hist!=NULL){
			delete hist;
		}
		hist = feat1_corr->get_histogram_from_masked_region_3D(mask, num_buckets_feat1);
		
		// Calculate force in each bucket: Smooth, normalize and log, then derivate in each direction with sobel filter
		hist->save_histogram_to_txt_file("D:/Joel/TMP/SIM_hist_" + int2str(iter) + ".txt");
		hist->smooth_mean(10,10);
		hist->data_has_changed();
		hist->save_histogram_to_txt_file("D:/Joel/TMP/SIM_hist_" + int2str(iter) + "_smooth.txt");

		for(int z=0; z<zsize; z++){
			for(int y=0; y<ysize; y++){
				for(int x=0; x<xsize; x++){
					if(mask->get_voxel(x,y,z)){
						tmp_field->set_voxel( x,y,z, -hist->get_norm_p_log_p_gradient_for_intensity(feat1_corr->get_voxel(x,y,z)) );
					}
				}
			}
		}
		tmp_field->save_to_file( "D:/Joel/TMP/SIM_fields_" + int2str(iter) + ".vtk" );


		// Smooth in each direction
		cout << "Gauss smoothing in x-direction" << endl;
		tmp_field->filter_3D(gauss_x, 1, mask, 1);

		cout << "Gauss smoothing in y-direction" << endl;
		tmp_field->filter_3D(gauss_y, 1, mask, 1);

		cout << "Gauss smoothing in z-direction" << endl;
		tmp_field->filter_3D(gauss_z, 1, mask, 1);


		// field[i] = field[i-1] + iteration_strength*(force / mean(abs(force)))
		tmp_field->mask_out(mask);
		float sum = tmp_field->get_sum_of_voxels(std::numeric_limits<float>::min(), true, mask);
		tmp_field->scale_by_factor( iteration_strength/(sum/bodysize) ); // oklart varfˆr
		tmp_field->save_to_file( "D:/Joel/TMP/SIM_fields_" + int2str(iter) + "_smooth_scale.vtk" );
		field->combine(tmp_field, COMB_ADD);
		field->data_has_changed();
		field->save_to_file( "D:/Joel/TMP/SIM_field_sum_" + int2str(iter) + ".vtk" );
		field->add_value_to_all_voxels(1,mask);
		feat1_corr->combine(field, COMB_MULT);
		feat1_corr->map_negative_values(0);
		feat1_corr->save_to_file( "D:/Joel/TMP/SIM_feat1_corr_" + int2str(iter) + ".vtk" );
	}

	if(hist!=NULL){
		delete hist;
	}
	delete tmp_field;
	delete field;
	
	//feat1_corr->name("feat1_corr");
	//datamanagement.add(feat1_corr);
	this->name(this->name()+"_SIM_bias_corr");
	feat1_corr->scale(this->get_min(),this->get_max()); //sets the old intensity range!
	copy_data(feat1_corr,this);
	delete feat1_corr;
}


template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::vesselness_test(double hessian_sigma, double vessel_alpha1, double vessel_alpha2)
{
	typedef itk::HessianRecursiveGaussianImageFilter< theImageType > HessianFilterType;
	typename HessianFilterType::Pointer hessianFilter = HessianFilterType::New();
	hessianFilter->SetInput( this->get_image_as_itk_output() );
	hessianFilter->SetSigma( hessian_sigma );

	typedef itk::Hessian3DToVesselnessMeasureImageFilter< ELEMTYPE > VesselnessMeasureFilterType;
	typename VesselnessMeasureFilterType::Pointer vesselnessFilter = VesselnessMeasureFilterType::New();
	vesselnessFilter->SetInput( hessianFilter->GetOutput() );
	vesselnessFilter->SetAlpha1( vessel_alpha1 );
	vesselnessFilter->SetAlpha2( vessel_alpha2 );
	vesselnessFilter->Update();
	
//	theImageToOrientedCastFilterType::Pointe
    typedef itk::CastImageFilter<theImageType2, theImageType> castType;
	typename castType::Pointer caster = castType::New();
	caster->SetInput(vesselnessFilter->GetOutput());
	caster->Update();

	//JK - TODO - Copy the rotation info separately....
	replicate_itk_to_image(caster->GetOutput());
}