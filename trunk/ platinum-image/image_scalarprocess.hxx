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

			for (int z=0; z < datasize[2]; z++){
				for (int y=0; y < datasize[1]; y++){
					for (int x=0; x < datasize[0]; x++){
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
