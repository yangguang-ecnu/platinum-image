//////////////////////////////////////////////////////////////////////////
//
//  global constants and macros $Revision$
//
//  $LastChangedBy$
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

#ifndef __global_h__
#define __global_h__
//#include <string.h>

#define PLATINUM_MAJOR_VER 0
#define PLATINUM_MINOR_VER 1

#define FLTK_LABEL_SIZE 14
#define FLTK_SMALL_LABEL 11
#define FLTK_SCROLLBAR_SIZE 16

#define BUTTONHEIGHT 25
#define DATALISTINITWIDTH 270

#define DATALISTINITHEIGHT 300

#define NOT_FOUND_ID -1
#define NA 0 // not applicable

#define RGBpixmap_bytesperpixel 3
#define RGBApixmap_bytesperpixel 4

#define IMGELEMCOMPTYPE unsigned char	//image element computer type...
#define IMGELEMCOMPMAX 255.0
#define RADDR 0
#define GADDR 1
#define BADDR 2
#define AADDR 3

#define FILEPOSTYPE char

#define VOLUMEMAXDIMENSION 3

#define IMGLABELMAX 19

enum ROImode {ROI_RECT, ROI_CIRCLE};
enum COMBINE_MODE {COMB_ADD, COMB_SUB, COMB_MULT, COMB_DIV, COMB_MAX, COMB_MIN, COMB_MEAN_NON_ZERO, COMB_MAGN}; //and, xor, mask...
enum PROJECTION_MODE {PROJ_MEAN, PROJ_MAX}; 
enum DICOM_LOADER_TYPE {DCM_LOAD_ALL, DCM_LOAD_SERIES_ID_ONLY}; 	//LOAD_ALL is used by dicom_importer to load all selected files (ignoring series_ID)....
enum POINT_TYPE { MAX_GRAD_MAG_X, MAX_GRAD_MAG_Y, MAX_GRAD_MAG_Z, MAX_GRAD_MAG_XY, MAX_GRAD_MAG_XZ, MAX_GRAD_MAG_YZ, MAX_GRAD_MAG_XYZ };
enum GRAD_MAG_TYPE { X, Y, Z, XYZ, XY, XZ, YZ };
enum WEIGHT_TYPE { CHESSBOARD };
enum preset_direction {Z_DIR, Y_DIR, X_DIR, Z_DIR_NEG, Y_DIR_NEG, X_DIR_NEG};
enum SPACE_TYPE {VOXEL_SPACE, PHYSICAL_SPACE};

//Dicom Tag definitions
//More DICOM tag definitions can be found at:  http://medical.nema.org/dicom/2007/07_06pu.pdf
//Note that it is important to use "lower case letters" for the gdcm package...

#define DCM_CREATION_DATE "0008|0012" //20050114	//string
#define DCM_CREATION_TIME "0008|0013" //082633.000	//string
#define DCM_STUDY_DATE "0008|0020"					//string
#define DCM_SERIES_DATE "0008|0021"					//string
#define DCM_SERIES_ID "0020|000e"					//string
#define DCM_MODALITY "0008|0060"					//string
#define DCM_IMAGE_TYPE "0008|0008"					//string
#define DCM_STUDY_DESCRIPTION "0008|1030"			//string
#define DCM_SERIES_DESCRIPTION "0008|103e"			//string

#define DCM_PATIENT_NAME "0010|0010"				//string
#define DCM_PATIENT_ID "0010|0020"					//string
#define DCM_PATIENT_BIRTH_DATE "0010|0030"			//string
#define DCM_PATIENT_SEX "0010|0040"					//string
#define DCM_PATIENT_WEIGHT "0010|1030"				//float

#define DCM_SCANNING_SEQ "0018|0020"				//string (GR)
#define DCM_SEQ_VARIANT "0018|0021"					//string (SS)
#define DCM_SCAN_OPTION "0018|0022"					//string (SP)
#define DCM_MR_ACQ_TYPE "0018|0023"					//string (2D)
#define DCM_TR "0018|0080"							//float
#define DCM_TE "0018|0081"							//float
#define DCM_FLIP "0018|1314"						//float
#define DCM_NSA "0018|0083"							//int
#define DCM_IMAGING_FREQUENCY "0018|0084"			//float (63.9013)
#define DCM_SLICE_THICKNESS "0018|0050"				//float
#define DCM_SLICE_SPACING "0018|0088"				//float
#define DCM_FOV "0018|1100"							//float (Reconstruction diam?)
#define DCM_B0 "0018|0087"							//float
#define DCM_NO_PHASE_ENC "0018|0089"				//int
#define DCM_PERCENT_SAMPLING "0018|0093"			//float
#define DCM_RFOV "0018|0094"						//float
#define DCM_PROTOCOL_NAME "0018|1030"				//string
#define DCM_TRANSMIT_COIL "0018|1251"				//string
#define DCM_RECEIVE_COIL "0018|1250"				//string
#define DCM_PHASE_ENCODING_DIRECTION "0018|1312"	//string (COL)
#define DCM_PATIENT_POSITION "0018|5100"			//string (FFS)
#define DCM_PULSE_SEQUENCE_NAME "0018|9005"			//string

#define DCM_SERIES_NUMBER "0020|0011"				//int (201)
#define DCM_ACQ_NUMBER "0020|0012"					//int (2)
#define DCM_IMAGE_POSITION_PATIENT "0020|0032"		//string (-265.0\-317.646\-614.334)
#define DCM_IMAGE_ORIENTATION_PATIENT "0020|0037"	//string (1.0\0.0\0.0\0.0\1.0\0.0)

#define DCM_ROWS "0028|0010"						//int (256)
#define DCM_COLUMNS "0028|0011"						//int (256)
#define DCM_PIXEL_SPACING "0028|0030"				//string (2.07031\2.07031 // dx\dy)
#define DCM_BITS_ALLOCATED "0028|0100"				//int (16)
#define DCM_BITS_STORED "0028|0101"					//int (12)
#define DCM_HIGH_BIT "0028|0102"					//int (11)
#define DCM_WINDOW_CENTER "0028|1050"				//float (396.0)
#define DCM_WINDOW_WIDTH "0028|1051"				//float (792.0)
#define DCM_RESCALE_INTERCEPT "0028|1052"			//float (0.0)
#define DCM_RESCALE_SLOPE "0028|1053"				//float (1.94286)
#define DCM_RESCALE_TYPE "0028|1054"				//string (normalized)

#define DCM_SCALE_INTERCEPT "0029|1052"				//float
#define DCM_SCALE_SLOPE "0029|1053"					//float
#define DCM_SCALE_INTERCEPT2 "2005|100d"			//float (0.0)
#define DCM_SCALE_SLOPE2 "2005|100e"				//float (0.0037668764)

#endif