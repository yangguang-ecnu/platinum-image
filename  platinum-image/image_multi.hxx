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
image_multi<ELEMTYPE, IMAGEDIM>::image_multi ()
    {}

template <class ELEMTYPE, int IMAGEDIM>
image_multi<ELEMTYPE, IMAGEDIM>::image_multi(std::vector<std::string> files, long width, long height, bool bigEndian, long headerSize, Vector3D voxelSize, unsigned int startFile,unsigned int increment) : image_general<ELEMTYPE, IMAGEDIM> (files, width, height, bigEndian, headerSize, voxelSize, startFile,increment)
    {
    }