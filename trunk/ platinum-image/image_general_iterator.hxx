//////////////////////////////////////////////////////////////////////////
//
//  image_general_iterator.hxx
//
//  Iterators for image_general and derived classes
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
image_general<ELEMTYPE, IMAGEDIM>::spiral_2d_iterator::spiral_2d_iterator(image_general<ELEMTYPE, IMAGEDIM> *im, Vector3Dint seed)
{
	the_image = im;
	seed_pos = seed;
	current_pos = seed_pos;
	current_direction=0;
	current_dist=0;
	line_length=1;
	first_line_segment=true;
	num_elements_visited=1;
	num_elements_max = the_image->get_size_by_dim(0)*the_image->get_size_by_dim(1);
}

template <class ELEMTYPE, int IMAGEDIM>
image_general<ELEMTYPE, IMAGEDIM>::spiral_2d_iterator::~spiral_2d_iterator()
{}

template <class ELEMTYPE, int IMAGEDIM>
typename image_general<ELEMTYPE, IMAGEDIM>::spiral_2d_iterator& image_general<ELEMTYPE, IMAGEDIM>::spiral_2d_iterator::operator=(const typename image_general<ELEMTYPE, IMAGEDIM>::spiral_2d_iterator& other)
{
	the_image = other.the_image;
	seed_pos = other.seed_pos;
	current_pos = other.current_pos;
    return(*this);
}

template <class ELEMTYPE, int IMAGEDIM>
bool image_general<ELEMTYPE, IMAGEDIM>::spiral_2d_iterator::operator==(const typename image_general<ELEMTYPE, IMAGEDIM>::spiral_2d_iterator& other)
{
    return( (the_image==other.the_image)&&(current_pos==other.current_pos)&&(seed_pos==other.seed_pos) );
}

template <class ELEMTYPE, int IMAGEDIM>
bool image_general<ELEMTYPE, IMAGEDIM>::spiral_2d_iterator::operator!=(const typename image_general<ELEMTYPE, IMAGEDIM>::spiral_2d_iterator& other)
{
    return( (the_image!=other.the_image)||(current_pos!=other.current_pos)||(seed_pos!=other.seed_pos) );
}

template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE& image_general<ELEMTYPE, IMAGEDIM>::spiral_2d_iterator::operator*()
{
    return *the_image->get_voxel_pointer(current_pos[0],current_pos[1],current_pos[2]);
}

template <class ELEMTYPE, int IMAGEDIM>
typename image_general<ELEMTYPE, IMAGEDIM>::spiral_2d_iterator& image_general<ELEMTYPE, IMAGEDIM>::spiral_2d_iterator::operator++()
{
	
    switch(current_direction){
		case 0:
			current_pos[0]+=1;
		    break;
		case 1:
			current_pos[1]-=1;
		    break;
		case 2:
			current_pos[0]-=1;
		    break;
		default:
			current_pos[1]+=1;
		    break;
	}

	current_dist++;
	
	if( current_dist==line_length ){
		current_dist=0;

		current_direction++;
		current_direction = current_direction%4;

		if(first_line_segment){
			first_line_segment=false;
		}else{
			line_length++;				//increase every second iteration
			first_line_segment=true;
		}
	}
	if( !the_image->is_voxelpos_within_image_3D(current_pos) ){

		cout<<"*****out******"; print_all_small();

		operator++();
//		this++; //JK - TODO - when to stop...
	}else{
		num_elements_visited++;
	}

    return *this;
}



//num_elements_max

template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE* image_general<ELEMTYPE, IMAGEDIM>::spiral_2d_iterator::begin()
{
   return *the_image->get_voxel_pointer(seed_pos[0],seed_pos[1],seed_pos[2]);
//    return(iterator(imagepointer()));
}

template <class ELEMTYPE, int IMAGEDIM>
bool image_general<ELEMTYPE, IMAGEDIM>::spiral_2d_iterator::end()
{
    return(num_elements_visited>=num_elements_max);
}


template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::spiral_2d_iterator::print_all()
{
	cout<<"--------------"<<endl;
	cout<<"seed_pos="<<seed_pos<<endl;
	cout<<"current_pos="<<current_pos<<endl;
	cout<<"current_direction="<<current_direction<<endl;
	cout<<"current_dist="<<current_dist<<endl;
	cout<<"first_line_segment="<<first_line_segment<<endl;
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::spiral_2d_iterator::print_all_small()
{
	cout<<seed_pos<<" "<<current_pos<<" "<<current_direction<<" "<<current_dist<<" ";
	cout<<line_length<<" "<<first_line_segment<<" "<<num_elements_visited<<" ("<<num_elements_max<<")"<<endl;
}
