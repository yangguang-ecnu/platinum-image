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

		float alpha_l = PI*fa1/180.0;
		float alpha_s = PI*fa2/180.0;
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

	return cg_line.direction[0];
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
	return b2;
}

template <class ELEMTYPE, int IMAGEDIM>
image_binary<3>* image_scalar<ELEMTYPE, IMAGEDIM>::appl_wb_segment_lungs_from_sum_image(int initial_upper_thres, image_binary<3> *body_mask)
{
	
	image_binary<3> *half_body_mask = binary_copycast<3>(body_mask);	// = image_binary<3>(body_mask);

	half_body_mask->fill_region_3D(1,130,body_mask->get_size_by_dim(1)-1,0);
	half_body_mask->erode_3D_26Nbh();
	half_body_mask->save_to_file("D:/Joel/TMP/half_body_small.vtk");

	histogram_1D<ELEMTYPE> *h = this->get_histogram_from_masked_region_3D(half_body_mask);
	h->save_histogram_to_txt_file("D:/Joel/TMP/half_body_sum_hist.txt");

	cout<<"lung_tresh="<<h->get_intensity_at_included_num_pix_from_lower_int(2,this->get_num_voxels_per_dm3()*2)<<endl;


	image_binary<3> *lungs = this->threshold(0,initial_upper_thres);
//	lungs->name(id+"_lungs");

	//clear a priori regions.... y = 0...50 and 100...
	lungs->fill_region_3D(1,0,50,0);
	lungs->fill_region_3D(1,100,lungs->get_size_by_dim(1)-1,0);
//	lungs->save_to_VTK_file(base+"__c01_Lungs.vtk");

//	cout<<"Mask body (lungs)..."<<endl;
	lungs->mask_out(body_mask);
//	lungs->save_to_VTK_file(base+"__c02_Lungs_masked.vtk");

//	cout<<"Erode lungs..."<<endl;
	image_binary<3> *tlungmask = new image_binary<3>(lungs);
//	tlungmask->name(id+"_tlungmask");
	tlungmask->erode_3D(7);
//	tlungmask->save_to_VTK_file(base+"__c03_Lungmask_eroded.vtk");

//	cout<<"Dilate lungs..."<<endl;
	//there is a bug in dilate3D... An in-slice line (with 2 segments...) is sometimes seen....
	tlungmask->dilate_3D(20);
//	tlungmask->save_to_VTK_file(base+"__c04_Lungmask_dilated.vtk");

	lungs->mask_out(tlungmask);
//	lungs->save_to_VTK_file(base+"__c05_Lungs_masked.vtk");

	delete tlungmask;
	return lungs;
}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::appl_wb_segment_find_crotch_pos_from_water_percent_image(int &pos_x, int &pos_y, int mip_thres)
{
	cout<<"appl_wb_segment_find_crotch_pos_from_water_percent_image..."<<endl;

//	cout<<"create MIP (z)..."<<endl;
	image_scalar<ELEMTYPE,IMAGEDIM> *tmip = this->create_projection_3D(2);
	tmip->name("_tmip");
	tmip->save_to_VTK_file("D:/Joel/TMP/_wpMIP1.vtk");

	cout<<"threshold MIP..."<<endl;
	image_binary<3> *tbin = tmip->threshold(mip_thres);  //95% MIP - water content....
	tbin->name("_tbin");
	tbin->save_to_VTK_file("D:/Joel/TMP/_wpMIP2_thres.vtk");


	cout<<"filter bin..."<<endl;
//	tbin->dilate_2D();
	tbin->erode_2D();
	tbin->dilate_2D();
	tbin->save_to_VTK_file("D:/Joel/TMP/_wpMIP3_open2D.vtk");


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
image_binary<3>* image_scalar<ELEMTYPE, IMAGEDIM>::appl_wb_segment_VAT_mask_from_this_water_percent_abd_subvolume(image_binary<3> *bin_body)
{
	cout<<"Erode body_mini..."<<endl;
	image_binary<> *body_mini = new image_binary<>(bin_body);
	body_mini->save_to_VTK_file("D:/Joel/TMP/g01_body_mini.vtk");
//	body_mini->erode_3D(9);
	body_mini->erode_3D_26Nbh();
	body_mini->erode_3D_26Nbh();
	body_mini->erode_3D_26Nbh();
	body_mini->save_to_VTK_file("D:/Joel/TMP/g01_body_mini2.vtk");

	cout<<"Mask VAT-mask..."<<endl;
	image_scalar<ELEMTYPE, IMAGEDIM> *abd = new image_scalar<ELEMTYPE, IMAGEDIM>(this);
	abd->mask_out(body_mini);
//	abd->save_to_VTK_file(base+"__g02_VAT_mini_mask.vtk");


	cout<<"Threshold, erode2D, largest object..."<<endl;
	image_binary<> *vat_mini = abd->threshold(500);
//	vat_mini->save_to_VTK_file(base+"__g03_VAT_mini_thres.vtk");

	vat_mini->erode_2D();
//	vat_mini->save_to_VTK_file(base+"__g04_VAT_mini_erode.vtk");

	vat_mini->largest_object_3D();
//	vat_mini->save_to_VTK_file(base+"__g05_VAT_mini_largest_object.vtk");

	cout<<"Convex Hull..."<<endl;
	vat_mini->convex_hull_line_filling_3D(0);
	vat_mini->convex_hull_line_filling_3D(2);
//	vat_mini->save_to_VTK_file(base+"__g06_VAT_convex_lines.vtk");

//	datamanagement.add(body_mini);
//	datamanagement.add(vat_mask);
//	datamanagement.add(vat_mini);
	delete body_mini;
	delete abd;

	return vat_mini;
}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::appl_wb_SIM_bias_correction(image_scalar<ELEMTYPE, IMAGEDIM>* second_feature, int num_iterations, float iteration_strength, float map_x_smoothing_std_dev, float map_y_smoothing_std_dev, float map_z_smoothing_std_dev, float feat1_smoothing_std_dev, float feat2_smoothing_std_dev, int initial_thres_body_mask, int num_buckets_feat1, int num_buckets_feat2, bool save_corrected_images_each_iteration, bool save_histogram_each_iteration, bool save_field_each_iteration) 
{
	image_scalar<float,3> *temp1 = dynamic_cast<image_scalar<float,3 >*>(this);
	image_scalar<float,3> *feat1 = new image_scalar<float,3>(temp1); delete temp1;
	image_scalar<float,3> *temp2 = dynamic_cast<image_scalar<float,3 >*>(second_feature);
	image_scalar<float,3> *feat2 = new image_scalar<float,3>(temp2); delete temp2;
		
	cout << "Calculating binary mask..." << endl;
	image_scalar<float,3> *sum = new image_scalar<float,3>(feat1);
	sum->combine(feat2, COMB_ADD);
	image_binary<3>* mask=sum->appl_wb_segment_body_from_sum_image(initial_thres_body_mask);
	image_binary<3>* lungs=sum->appl_wb_segment_lungs_from_sum_image(initial_thres_body_mask, mask);
	mask->combine(lungs, COMB_SUB);
	delete sum; delete lungs;
		
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
	int bodysize=mask->get_number_of_voxels_with_value(1);
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
		image_scalar<unsigned short,3> *temp3 = feat1_corr->create2Dhistogram_3D(feat2_corr, true, num_buckets_feat1, num_buckets_feat2, mask);
		image_scalar<float,3> *hist = new image_scalar<float,3>(temp3); delete temp3;
	
		// Expand
		image_scalar<float,3> *hist_expanded = dynamic_cast<image_scalar<float,3 >*>(hist->expand_borders(1, 1, 0));
		//image_scalar<float,3> *hist_expanded = new image_scalar<float, 3>(num_buckets_feat1 + 2, num_buckets_feat2 + 2,1);
		//hist_expanded->fill(0);
		//for (int i=0; i<num_buckets_feat1; i++) {
		//	for (int j=0; j<num_buckets_feat2; j++) {
		//		hist_expanded->set_voxel(i+1,j+1,0,hist->get_voxel(i,j,0));
		//	}
		//}
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
		//image_scalar<float,3> *feat1_force = new image_scalar<float,3>(num_buckets_feat1, num_buckets_feat2, 1);
		//image_scalar<float,3> *feat2_force = new image_scalar<float,3>(num_buckets_feat1, num_buckets_feat2, 1);
		//for (int i=0; i<num_buckets_feat1; i++) {
		//	for (int j=0; j<num_buckets_feat2; j++) {
		//		feat1_force->set_voxel(i,j,0,feat1_force_expanded->get_voxel(i+1,j+1,0));
		//		feat2_force->set_voxel(i,j,0,feat2_force_expanded->get_voxel(i+1,j+1,0));
		//	}
		//}
		delete feat1_force_expanded; delete feat2_force_expanded;
	
		// Map forces to inhomogeniety field map as Force = feat1_force * feat1_intensity + feat2_force * feat2_intensity
		image_scalar<float,3> *inh_map = new image_scalar<float,3>(field);
		feat1_min=feat1_corr->get_min(); feat2_min=feat2_corr->get_min();
		feat1_scale = float(feat1_corr->get_max()-feat1_min)*1.000001/float(num_buckets_feat1);
		feat2_scale = float(feat2_corr->get_max()-feat2_min)*1.000001/float(num_buckets_feat2);
		for (int x=0; x<xsize; x++) {
			for (int y=0; y<ysize; y++) {
				for (int z=0; z<zsize; z++) {
					if (mask->get_voxel(x,y,z))	{
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
		inh_map->filter_3D(gauss_x, 1, mask, 1);
		cout << "Gauss smoothing in y-direction" << endl;
		inh_map->filter_3D(gauss_y, 1, mask, 1);
		cout << "Gauss smoothing in z-direction" << endl;
		inh_map->filter_3D(gauss_z, 1, mask, 1);

		// field[i] = field[i-1] + iteration_strength*(force / mean(abs(force)))
		inh_map->mask_out(mask);
		inh_map->scale_by_factor(iteration_strength/((inh_map->get_sum_of_voxels(true, mask))/bodysize)); // oklart varför
		field->combine(inh_map, COMB_ADD);
		field->data_has_changed();
			
		if (save_field_each_iteration) {
			fields[iter] = new image_scalar<float,3>(field);
			str = "Field, iteration " + int2str(iter+1);
			fields[iter]->name(str);
			datamanagement.add(fields[iter]);
		}
		delete inh_map;
	
		//Beräkna ekv (7) i SIM-paper
		feat1_corr->fill(0); feat2_corr->fill(0);
		feat1_corr->combine(field, COMB_ADD); feat2_corr->combine(field, COMB_ADD);
		feat1_corr->add_value_to_all_voxels(1); feat2_corr->add_value_to_all_voxels(1);
		feat1_corr->combine(feat1, COMB_MULT); feat2_corr->combine(feat2, COMB_MULT);
		feat1_corr->data_has_changed(); feat2_corr->data_has_changed();

		if (save_histogram_each_iteration) {	
			histograms[iter] = feat1_corr->create2Dhistogram_3D(feat2_corr, false, num_buckets_feat1, num_buckets_feat2, mask);
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
	delete feat1; delete feat2; delete mask;
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