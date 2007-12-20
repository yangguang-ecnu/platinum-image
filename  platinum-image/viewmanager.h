//////////////////////////////////////////////////////////////////////////
//
//  Viewmanager $Revision$
///
/// Viewmanager lists the available viewports and allows access through
/// viewport ID and various ways of requesting updates.
///
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

#ifndef __viewmanager__
#define __viewmanager__

#include <vector>

#include "viewport.h"

#include "global.h"



// this contains the setup information - should be loaded/stored, changed by interacting w. widgets, ...


class viewmanager
    {
    private:
        static const int MAXVIRTUALVIEWS = 5;
        static const int MAXHORIZONTALGRID = 20;
        static const int MAXVERTICALGRID = 20;
        
        int viewportmapping[MAXVERTICALGRID][MAXHORIZONTALGRID][MAXVIRTUALVIEWS];
        int noofvirtualviews;
        int noofhorizontaltiles[MAXVIRTUALVIEWS]; // x- & y-storlek för grid
        int noofverticaltiles[MAXHORIZONTALGRID][MAXVIRTUALVIEWS];
        float tilewidthpercent[MAXVIRTUALVIEWS][MAXHORIZONTALGRID];
        float tileheightpercent[MAXVIRTUALVIEWS][MAXHORIZONTALGRID][MAXVERTICALGRID];
        std::vector<viewport> viewports;
        bool irregular_tiles;
		
		void show_point_by_renderers ( const Vector3D & point, const std::vector<int> & rendererIDs, const unsigned int margin = 5 );	// show this point in each viewport connected to one of the renderer ids

    public:
        void setup_views(int virtualview, int windowwidth, int windowheight); // 0...antal som anv‰nds-1
        void erase_all_connections();
        void connect_views_viewports();
        void manipulate_views_setup();
        void listviewports();
        int create_viewport();
        int find_viewport_no_renderer();
        int find_viewport_not_in_view(); // returns -1 if none found
        int find_viewport_no_images ();
        int find_viewport_index(int id); // turns ID into INDEX returns NOT_FOUND_ID if none found
        std::vector<threshold_overlay *> get_overlays (thresholdparvalue *);
        void list_connections();
        void setup_demo_tiles();   //setup an (irregular) demo tile pattern
        
        void setup_regular_tiles(int t_h=2, int t_v=2, int t_vv=1);

        void connect_renderer_to_viewport(int viewportID, int rendererID);
        void data_vector_has_changed();

        void refresh_viewports();
        void refresh_viewports_after_toolswitch();
        void refresh_viewports_from_combination(int c);
        void refresh_viewports_from_geometry(int g);

        int get_viewport_id(int v, int h, int virtualview);
        int get_renderer_id(int viewportid);
		
		
		std::vector<int> viewports_from_renderers(const std::vector<int> & rendererIDs);	// return a set of viewport ids

		int viewport_from_renderer(int); // return a viewport id
		
		viewport * get_viewport( int viewportID );	// return a viewport
		
		void viewmanager::show_point_by_combination ( const Vector3D & point, const int combinationID,  const unsigned int margin = 5 );	// show this point in all viewports that has at least on of ids in combinationIDs active
	
		void viewmanager::show_point_by_data ( const Vector3D & point, const int dataID, const unsigned int margin = 5 );	// show this point in all viewports that has the dataID active
																									
    };

#endif




