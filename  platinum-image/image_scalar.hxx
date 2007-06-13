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


template <class ELEMTYPE, int IMAGEDIM >
void image_scalar<ELEMTYPE, IMAGEDIM >::interpolate_trilinear_3D_vxl(image_scalar<ELEMTYPE, IMAGEDIM > *src_im)
	{	
//		cout<<"interpolate_trilinear_3D_vxl..."<<endl;
		Vector3D phys;		//physical location in target image
		Vector3D phys2;		//physical location in source image
		double val;
		int nx = src_im->datasize[0];
		int ny = src_im->datasize[1];
		int nz = src_im->datasize[2];
		int xstep = 1;
		int ystep = nx;
		int zstep = nx*ny;
//		cout<<"nx,ny,nz "<<nx<<","<<ny<<","<<nz<<endl;
//		cout<<"steps    "<<xstep<<","<<ystep<<","<<zstep<<endl;
		float tmp;

		for (int k=0; k < this->datasize[2]; k++)
		{
			for (int j=0; j < this->datasize[1]; j++)
			{
				for (int i=0; i < this->datasize[0]; i++)
				{
					//template<class T>
					//inline double mil3d_safe_trilin_interp_3d(double x, double y, double z, const T* data, int nx, int ny, int nz, int xstep, int ystep, int zstep)
					phys = this->get_physical_pos_for_voxel(i,j,k);
					tmp = src_im->get_voxel(i,j,k);
//					cout<<"src_im->get_voxel="<<tmp<<endl;
					
					val = mil3d_safe_trilin_interp_3d<ELEMTYPE>(phys[0], phys[1], phys[2], src_im->imageptr, nx, ny, nz, xstep, ystep, zstep);
//					cout<<"phys = "<<phys<<"-->"<<val<<endl;
					this->set_voxel(i,j,k,val);
				}
			}
		}
	}