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
//	tmip->save_to_VTK_file("c:/Joel/TMP/COMBI__d01_wpMIP.vtk");

	cout<<"threshold MIP..."<<endl;
	image_binary<3> *tbin = tmip->threshold(mip_thres);  //95% MIP - water content....
	tbin->name("_tbin");
//	tbin->save_to_VTK_file("c:/Joel/TMP/COMBI__d02_bin.vtk");

	cout<<"filter bin..."<<endl;
//	tbin->dilate_2D();
	tbin->erode_2D();
	tbin->dilate_2D();
//	tbin->save_to_VTK_file("c:/Joel/TMP/COMBI__d03_bin_close_open.vtk");


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
	body_mini->erode_3D(6);
//	body_mini->save_to_VTK_file(base+"__g01_body_mini.vtk");

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