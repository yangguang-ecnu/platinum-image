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
void image_scalar<ELEMTYPE, IMAGEDIM>::smooth_ITK(Vector3D radius)
{
	image_general<ELEMTYPE, IMAGEDIM>::make_image_an_itk_reader();		// without 'image_general<ELEMTYPE, IMAGEDIM>::' generates compiler error

	typename itk::MeanImageFilter<theImageType,theImageType>::Pointer filter = itk::MeanImageFilter<theImageType,theImageType>::New();

	typename theSizeType r;
	//	r[0] = 1; // radius along x
	//	r[1] = 1; // radius along y
	//	r[2] = 1; // radius along z
	r[0] = radius[0];
	r[1] = radius[1];
	r[2] = radius[2];

	filter->SetRadius(r);
	filter->SetInput(this->ITKimportfilter->GetOutput());
	filter->Update();

	this->ITKimportimage = filter->GetOutput();
	image_general<ELEMTYPE, IMAGEDIM>::replicate_itk_to_image();
}



template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::smooth_3D(Vector3D r)
{
	cout<<"smooth_3D..."<<endl;
	image_scalar<ELEMTYPE, IMAGEDIM> *res = new image_scalar<ELEMTYPE, IMAGEDIM>(this,0);

	res->fill(0); //not very time efficient... the outer borders would be enough...

	if(r[0]>0 && r[1]>0 && r[2]>0)
	{
		float num_neighbours = (1+2*r[0])*(1+2*r[1])*(1+2*r[2]);
		float sum = 0;
		float mean1 = 0;
		float tmp = 0;
		float limit = 100;
		float num_counted = 0;

		for(int k=r[2]; k < this->datasize[2]-r[2]; k++){
			cout<<"k="<<k<<endl;
			for(int j=r[1]; j < this->datasize[1]-r[1]; j++){
				for(int i=r[0]; i < this->datasize[0]-r[0]; i++){

					sum = 0;
					for(int n=k-r[2]; n<=k+r[2]; n++){
						for(int m=j-r[1]; m<=j+r[1]; m++){
							for(int l=i-r[0]; l<=i+r[0]; l++){
								sum += float(this->get_voxel(l,m,n));
							}
						}
					}
					mean1 = sum/num_neighbours;
//					res->set_voxel(i,j,k,);

					sum = 0;
					num_counted = 0;
					for(int n=k-r[2]; n<=k+r[2]; n++){
						for(int m=j-r[1]; m<=j+r[1]; m++){
							for(int l=i-r[0]; l<=i+r[0]; l++){
								tmp = float(this->get_voxel(l,m,n));
								if(abs(tmp-mean1)<limit)
								{
									sum += tmp;
									num_counted++;
								}
							}
						}
					}
					res->set_voxel(i,j,k,sum/num_counted);	//mean2...
				}
			}
		}
	}

	copy_data(res,this);
	//JK shouldn't res be deleted...
//	delete res
}

	

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::region_grow_3D(Vector3D seed, ELEMTYPE min_intensity, ELEMTYPE max_intensity)
{

	image_scalar<ELEMTYPE, IMAGEDIM> *res = new image_scalar<ELEMTYPE, IMAGEDIM>(this,0);
	res->fill(0);
	res->set_voxel(seed[0],seed[1],seed[2],255);

	int sx = this->datasize[0];
	int sy = this->datasize[1];
	int sz = this->datasize[2];
	cout<<sx<<" "<<sy<<" "<<sz<<endl;

	stack<Vector3D> s;
	s.push(seed);
	Vector3D pos;
	Vector3D pos2;
	ELEMTYPE val;

	while(s.size()>0){
		pos = s.top();
		s.pop();
		val = this->get_voxel(pos[0],pos[1],pos[2]);

//		if(val>=min_intensity && val<=max_intensity){

			for(int x=std::max(0,int(pos[0]-1)); x<=std::min(int(pos[0]+1),sx-1); x++){
				for(int y=std::max(0,int(pos[1]-1)); y<=std::min(int(pos[1]+1),sy-1); y++){
					for(int z=std::max(0,int(pos[2]-1)); z<=std::min(int(pos[2]+1),sz-1); z++){
						val = this->get_voxel(x,y,z);

						if(val>=min_intensity && val<=max_intensity && res->get_voxel(x,y,z)==0){
							pos2[0]=x; pos2[1]=y; pos2[2]=z;
							s.push(pos2);
							res->set_voxel(x,y,z,255);
						}

					}
				}
			}

	}

//	cout<<"region_grow_3D --> Done...(s.size()="<<s.size()<<")"<<endl;
//	res->save_to_VTK_file("c:\\Joel\\TMP\\region_grow.vtk");
//	cout<<"before..."<<endl;
	copy_data(res,this);
//	cout<<"before2..."<<endl;
	delete res;
//	cout<<"after..."<<endl;
}

template <class ELEMTYPE, int IMAGEDIM>
void image_scalar<ELEMTYPE, IMAGEDIM>::region_grow_robust_3D(Vector3D seed, ELEMTYPE min_intensity, ELEMTYPE max_intensity, int nr_accepted_neighbours, int radius)
{
	image_scalar<ELEMTYPE, IMAGEDIM> *res = new image_scalar<ELEMTYPE, IMAGEDIM>(this,0);
	res->fill(0);
	res->set_voxel(seed[0],seed[1],seed[2],255);

	image_scalar<ELEMTYPE, IMAGEDIM> *neighb = new image_scalar<ELEMTYPE, IMAGEDIM>(this,0);
	neighb->fill(0);

	int sx = this->datasize[0];
	int sy = this->datasize[1];
	int sz = this->datasize[2];
	cout<<sx<<" "<<sy<<" "<<sz<<endl;
	int r = radius;

	stack<Vector3D> s;
	s.push(seed);
	Vector3D pos;
	Vector3D pos2;
	ELEMTYPE val;
	ELEMTYPE val2;
	int nr_acc=0;

	while(s.size()>0){
		pos = s.top();
		s.pop();
		val = this->get_voxel(pos[0],pos[1],pos[2]);

		if(val>=min_intensity && val<=max_intensity){

			for(int x=std::max(0,int(pos[0]-1)); x<=std::min(int(pos[0]+1),sx-1); x++){
				for(int y=std::max(0,int(pos[1]-1)); y<=std::min(int(pos[1]+1),sy-1); y++){
					for(int z=std::max(0,int(pos[2]-1)); z<=std::min(int(pos[2]+1),sz-1); z++){
						val = this->get_voxel(x,y,z);
						if(val>=min_intensity && val<=max_intensity && res->get_voxel(x,y,z)==0){

							//check number of accepted neighbours...
							nr_acc=0;
							for(int a=std::max(0,int(x-r)); a<=std::min(int(x+r),sx-1); a++){
								for(int b=std::max(0,int(y-r)); b<=std::min(int(y+r),sy-1); b++){
									for(int c=std::max(0,int(z-r)); c<=std::min(int(z+r),sz-1); c++){
										val2 = this->get_voxel(a,b,c);
										if(val2>=min_intensity && val2<=max_intensity){
											nr_acc++;
										}
									}
								}
							}
							neighb->set_voxel(x,y,z,nr_acc);

							if(nr_acc>=nr_accepted_neighbours){
								pos2[0]=x; pos2[1]=y; pos2[2]=z;
								s.push(pos2);
								res->set_voxel(x,y,z,255);
							}

						}//if
					}//z
				}//y
			}//x

		}//if val...
	}//while

//	cout<<"region_grow_robust_3D --> Done..."<<endl;
//	cout<<"s.size()="<<s.size()<<endl;
//	res->save_to_VTK_file("c:\\Joel\\TMP\\region_grow.vtk");
//	neighb->save_to_VTK_file("c:\\Joel\\TMP\\region_grow_nr_acc.vtk");

	copy_data(res,this);

	delete res;
	delete neighb;
}

/*
typedef itk::MedianImageFilter<theImageType, theImageType>					theMedianFilterType;
typedef theMedianFilterType::Pointer										theMedianFilterPointer;

typedef itk::GradientMagnitudeImageFilter<theImageType, theImageType>		theGradMagnFilterType;
typedef itk::GradientMagnitudeRecursiveGaussianImageFilter<theImageType, theImageType>	theGradMagnRecGaussFilterType;
typedef itk::DanielssonDistanceMapImageFilter<theImageType, theImageType>	theDistFilterType;
typedef itk::DiscreteGaussianImageFilter<theImageType, theImageType>		theDiscreteGaussFilterType;

*/
