#platinumlib CMakelists $Revision$

PROJECT(Platinum CXX)
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

# This file is part of the Platinum library.
# Copyright (c) 2007 Uppsala University.
#
#
#    The Platinum library is free software; you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    The Platinum library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with the Platinum library; if not, write to the Free Software
#    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

INCLUDE_REGULAR_EXPRESSION(".*[.](cc|h|cxx|cp)$")

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wunused")

FIND_PACKAGE( ITK )
IF ( ITK_FOUND )
  INCLUDE( ${USE_ITK_FILE} )
ENDIF( ITK_FOUND )

INCLUDE_DIRECTORIES( ${ITK_INCLUDE_DIR} )

FIND_PACKAGE ( FLTK )
IF (   USE_FLTK_FILE   )
  INCLUDE(  ${USE_FLTK_FILE}  )
ENDIF(   USE_FLTK_FILE   )

INCLUDE_DIRECTORIES( ${FLTK_INCLUDE_DIR}  )

LINK_DIRECTORIES(${ITK_LIBRARY_DIR} ${FLTK_LIBRARY_DIR})

SET(UTILITIES_VXL Utilities/vxl/contrib/)
INCLUDE_DIRECTORIES( ${UTILITIES_VXL} )

SET(UTILITIES_FLTK Utilities/fltk)
INCLUDE_DIRECTORIES( ${UTILITIES_FLTK} )			#Location of: Fl_Table.H/cxx Fl_Table_Row.H/cxx

IF(WIN32)
  #dirent.h has to be hidden from UNIX compilers
  INCLUDE_DIRECTORIES( Utilities/dirent-win32 )
  SET(WIN32_DIRENT_SRC Utilities/dirent-win32/dirent.c)
  SET(WIN32_DIRENT_HEAD Utilities/dirent-win32/dirent.h)
ENDIF(WIN32)

OPTION(BUILD_SHARED "Build Platinum as a shared library" OFF)

SET (PLATINUM_SOURCE_FILES bruker.h color.h colormap.h cursor3D.h data_base.h ${WIN32_DIRENT_HEAD} datamanager.h datawidget.h error.h fileutils.h FLTK2Dregionofinterest.h FLTKuserIOparameters.h FLTKutilities.h FLTKviewport.h global.h histogram.h image_base.h  image_binary.h image_complex.h image_general.h image_integer.h image_label.h image_multi.h image_scalar.h image_storage.h listedfactory.h platinumprogram.h point_base.h points_seq_func1D.h point3D.h rawimporter.h dicom_importer.h renderer_base.h renderer_VTK.h rendererMPR.h rendergeometry.h rendercombination.h rendermanager.h ptmath.h threshold.h transfer.h transferfactory.h transferfactory.hxx userIO.h userIOmanager.h viewport.h viewporttool.h viewmanager.h ${UTILITIES_FLTK}/Fl_Table.H ${UTILITIES_FLTK}/Fl_Table_Row.H bruker.cc color.cc colormap.cc cursor3D.cc data_base.cc datamanager.cc datawidget.cc ${WIN32_DIRENT_SRC} error.cc fileutils.cc FLTK2Dregionofinterest.cc FLTKuserIOparameters.cc FLTKutilities.cc FLTKviewport.cc histogram.cc histogram.hxx image_base.cc image_binary.hxx image_binaryprocess.hxx image_complex.hxx image_general.hxx image_generalfile.hxx image_storage_iterator.hxx image_integer.hxx image_label.hxx image_multi.hxx image_scalar.hxx image_storage.hxx point_base.cc points_seq_func1D.hxx rawimporter.cc dicom_importer.cc renderer_base.cc rendererMPR.cc renderer_VTK.cc  rendergeometry.cc rendercombination.cc rendermanager.cc ptmath.cc threshold.cc transfer.hxx transfer_interpolated.hxx userIO.cc userIOmanager.cc transferfactory.cc viewport.cc viewmanager.cc ${UTILITIES_FLTK}/Fl_Table.cxx ${UTILITIES_FLTK}/Fl_Table_Row.cxx ${WIN32_DIRENT_SRC} CMakeLists.txt)

IF(BUILD_SHARED)
  ADD_LIBRARY(platinumlib SHARED ${PLATINUM_SOURCE_FILES})
ELSE(BUILD_SHARED)
  ADD_LIBRARY(platinumlib STATIC ${PLATINUM_SOURCE_FILES})
ENDIF(BUILD_SHARED)

LINK_DIRECTORIES (${Platinum_BINARY_DIR})

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

# _DEBUG flag will be defined when in debug mode
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

# Relax syntax requirements for GCC, since MSVC cannot shape up 
IF(NOT MSVC)
  ADD_DEFINITIONS(-fpermissive)
ENDIF(NOT MSVC)

IF(WIN32)
  OPTION(FLTK_HAS_DEPENDENT_NAMES "FLtk debug lib is named fltkd" OFF)
  MARK_AS_ADVANCED(FLTK_HAS_DEPENDENT_NAMES)
  TARGET_LINK_LIBRARIES (platinumlib wsock32 comctl32)
  IF (FLTK_HAS_DEPENDENT_NAMES)
    TARGET_LINK_LIBRARIES (platinumlib debug fltkgld optimized fltkgl debug fltkd optimized fltk)
  ELSE(FLTK_HAS_DEPENDENT_NAMES)
    TARGET_LINK_LIBRARIES (platinumlib fltk_gl fltk )
  ENDIF(FLTK_HAS_DEPENDENT_NAMES)
ENDIF(WIN32)

IF(UNIX)
  LINK_DIRECTORIES(/usr/local/lib )
  IF(APPLE)
  ELSE(APPLE)
    LINK_DIRECTORIES (/usr/X11R6/lib)
  ENDIF (APPLE)
  TARGET_LINK_LIBRARIES (platinumlib fltk_gl fltk )
  #TARGET_LINK_LIBRARIES(platinumlib GLU GL m  Xext X11  supc++)
ENDIF(UNIX)

TARGET_LINK_LIBRARIES(platinumlib ITKCommon ITKIO ${FLTK_LIBRARIES})

#test program for quick compile cycles
#MACOSX_BUNDLE only affects OS X builds, and is required for GUI to function
ADD_EXECUTABLE(PlatinumTest MACOSX_BUNDLE test_app.cc)

IF(APPLE)
   FIND_PROGRAM(ITK_APPLE_RESOURCE Rez /Developer/Tools)
   IF(ITK_APPLE_RESOURCE)
      ADD_CUSTOM_COMMAND(
         TARGET PlatinumTest.app/Contents/Resources/PlatinumTest.rsrc POST_BUILD
         COMMAND ${ITK_APPLE_RESOURCE} ARGS -useDF ${FLTK_INCLUDE_DIR}/mac.r
         WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
         )
   ENDIF(ITK_APPLE_RESOURCE)
ENDIF(APPLE)

TARGET_LINK_LIBRARIES(PlatinumTest platinumlib)

ADD_DEPENDENCIES(PlatinumTest platinumlib)

#SOURCE_GROUP(Headers REGULAR_EXPRESSION ".*[.]h$")
#SOURCE_GROUP(Headers FILES global.h platinumprogram.h ptmath.h)

SOURCE_GROUP(ImageVarieties FILES image_binary.h image_complex.h image_integer.h image_label.h image_multi.h image_scalar.h image_binary.hxx image_complex.hxx image_integer.hxx image_label.hxx image_multi.hxx image_scalar.hxx)

SOURCE_GROUP(Data FILES datamanager.h data_base.h image_base.h image_general.h image_storage_iterator.h image_storage.h point_base.h points_seq_func1D.h rawimporter.h dicom_importer.h datamanager.cc data_base.cc image_base.cc image_general.hxx image_generalfile.hxx image_storage_iterator.hxx image_storage.hxx point_base.cc point3D.h points_seq_func1D.hxx rawimporter.cc dicom_importer.cc)

SOURCE_GROUP(Processing FILES image_binaryprocess.hxx)

SOURCE_GROUP(View FILES viewport.h FLTKviewport.h viewmanager.h renderer_base.h renderer_VTK.h rendererMPR.h rendercombination.h rendergeometry.h rendermanager.h transfer.h viewport.cc FLTKviewport.cc viewmanager.cc renderer_base.cc renderer_VTK.cc rendererMPR.cc rendercombination.cc rendergeometry.cc rendermanager.cc transfer.hxx transfer_interpolated.hxx)

SOURCE_GROUP(Interaction FILES datawidget.h userIO.h FLTKuserIOparameters.h transferfactory.h transferfactory.hxx userIOmanager.h  datawidget.cc userIO.cc FLTKuserIOparameters.cc transferfactory.cc userIOmanager.cc viewporttool.h test_app.cc)

SOURCE_GROUP(Helpers FILES bruker.h color.h colormap.h cursor3D.h error.h fileutils.h FLTK2Dregionofinterest.h FLTKutilities.h ${UTILITIES_FLTK}/Fl_Table.H ${UTILITIES_FLTK}/Fl_Table_Row.H listedfactory.h histogram.h threshold.h  bruker.cc color.cc colormap.cc cursor3D.cc ${WIN32_DIRENT_SRC} error.cc fileutils.cc FLTK2Dregionofinterest.cc FLTKutilities.cc ${UTILITIES_FLTK}/Fl_Table.cxx ${UTILITIES_FLTK}/Fl_Table_Row.cxx histogram.cc histogram.hxx ptmath.cc threshold.cc)
#SOURCE_GROUP(Helpers FILES bruker.h color.h colormap.h cursor3D.h error.h fileutils.h FLTK2Dregionofinterest.h FLTKutilities.h listedfactory.h histogram.h threshold.h bruker.cc color.cc colormap.cc cursor3D.cc ${WIN32_DIRENT_SRC} error.cc fileutils.cc FLTK2Dregionofinterest.cc FLTKutilities.cc histogram.cc histogram.hxx ptmath.cc threshold.cc)

#add CMakelists for inclusion in version control
SOURCE_GROUP(Scripts CMakeLists.txt)

#Write PlatinumConfig.cmake so client programs can be built with Platinum

#CMAKE_EXPORT_BUILD_SETTINGS(${Platinum_BINARY_DIR}/PlatinumBuildSettings.cmake)
SET(Platinum_LIBRARY_DEPENDS_FILE ${Platinum_BINARY_DIR}/PlatinumLibraryDepends.cmake)
EXPORT_LIBRARY_DEPENDENCIES(${Platinum_BINARY_DIR}/PlatinumLibraryDepends.cmake)
CONFIGURE_FILE("${Platinum_SOURCE_DIR}/CMake/PlatinumConfig.cmake.in"
  "${Platinum_BINARY_DIR}/PlatinumConfig.cmake" @ONLY IMMEDIATE)