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
	current_radius=1;
	current_direction=0;
	current_dist=0;
	first_line_segment=true;
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
//    ptr++;
	//JK do the spiral logic...
	//current_pos;
	//current_radius=1;
	//current_direction=0;
	//current_dist=0;
	
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


	if(current_dist==2*current_radius-1){
		current_dist=0;

		current_direction++;
		current_direction = current_direction%4;

		if(first_line_segment){			//change radius after traversing two line segments...
			first_line_segment=false;
		}else{
			current_radius++;
			first_line_segment=true;
		}

	}

    return *this;
}

template <class ELEMTYPE, int IMAGEDIM>
void image_general<ELEMTYPE, IMAGEDIM>::spiral_2d_iterator::print_all()
{
	cout<<"--------------"<<endl;
	//the_image = im;
	cout<<"seed_pos="<<seed_pos<<endl;
	cout<<"current_pos="<<current_pos<<endl;
	cout<<"current_radius="<<current_radius<<endl;
	cout<<"current_dist="<<current_dist<<endl;
	cout<<"first_line_segment="<<first_line_segment<<endl;
}

