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

// *** transferchart ***


template <class ELEMTYPE>
transfer_interpolated<ELEMTYPE >::transferchart::transferchart (histogram_1D<ELEMTYPE > * hi, int x, int y, int w, int h):Fl_Widget (x,y,w,h,"")
{   
	histimg = NULL;
	histogram = hi;

	lookupSize = 256;
	lookupStart = std::numeric_limits<ELEMTYPE>::min();
	lookupScale = lookupSize/(std::numeric_limits<ELEMTYPE>::max()-std::numeric_limits<ELEMTYPE>::min());

    lookup = new IMGELEMCOMPTYPE [lookupSize];

    //test pattern
    /*for (unsigned int i = 0; i < lookupSize; i++)
        {lookup[i] = (i*IMGELEMCOMPMAX)/lookupSize;}*/
    for (unsigned int i = 0; i < lookupSize; i++)
        {lookup[i] = ((i*4) % static_cast<int>(IMGELEMCOMPMAX));}

	intensity_knots = points_seq_func1D<float,float>(lookupStart,0,lookupStart+lookupSize,255,5);
}

template <class ELEMTYPE>
int transfer_interpolated<ELEMTYPE >::transferchart::handle (int event)
{
	float t_x = -1;		//event x position (in transfer (knots) coordinates)
	float t_y = -1;		//event y position (in transfer (knots) coordinates)
	float closest_key = -1;
	float dist = -1;
	int dx = -1;		//drag dx (in FLTK coordinates)
	int dy = -1;		//drag dx (in FLTK coordinates)
	int t_dx = -1;		//drag dx (in transfer (knots) coordinates)
	int t_dy = -1;		//drag dx (in transfer (knots) coordinates)
	int t_x_new = -1;	//new knot x (in transfer (knots) coordinates)
	int t_y_new = -1;	//new knot y (in transfer (knots) coordinates)

	switch(event)
	{
	case FL_PUSH:
		//mouse_x / mouse_y  function as buffer for the "last" coordinates...
		mouse_x = Fl::event_x();
		mouse_y = Fl::event_y();

		if(mouse_click_time!=NULL)
		{
			if( (clock()-mouse_click_time)< 1.0*CLOCKS_PER_SEC)	//double_click - if < 1.0sec since last
			{
				t_x = float(mouse_x - x())/w()*float(lookupSize);
				t_y = float(255) - float(mouse_y - y())/h()*float(255);
				dist = intensity_knots.find_dist_to_closest_point2D(t_x,t_y,closest_key);

				//JK3 no boundary checking....
				intensity_knots.set_data(closest_key,t_x,t_y,0,0,lookupSize-1,255);
				redraw();
			}
		}

		mouse_click_time = clock(); //set new "last time mouse was clicked in transfer widget"
		return 1;

	case FL_DRAG:
		cout<<"FL_DRAG"<<endl;
		//mouse_x / mouse_y  function as buffer for the "last" coordinates...
		t_x = float(mouse_x - x())/w()*float(lookupSize);
		t_y = float(255) - float(mouse_y - y())/h()*float(255);
		dist = intensity_knots.find_dist_to_closest_point2D(t_x,t_y,closest_key);

		if(dist<60)
		{
			dx = Fl::event_x() - mouse_x;
			dy = Fl::event_y() - mouse_y;
			t_dx = float(dx)/w()*float(lookupSize);
			t_dy = float(dy)/h()*float(255);
			t_x_new = t_x + t_dx;
			t_y_new = t_y + t_dy;

			//boundary conditions
			if(closest_key==0)
				t_x_new=0;

			if(closest_key==lookupSize-1)
				t_x_new=lookupSize-1;

			if(t_y_new<0)
				t_y_new=0;

			if(t_y_new>255)
				t_y_new=255;

			if(!intensity_knots.is_occupied(t_x_new))	//otherwise, points with same key will overwrite
			{
				intensity_knots.set_data(closest_key,t_x_new,t_y_new,0,0,lookupSize-1,255);
//				intensity_knots.set_data(closest_key,t_x_new,t_y_new);
			}

			mouse_x += dx;
			mouse_y += dy;
		}
		redraw();
		// ...
		return 1;
	case FL_RELEASE:
		// ...
		return 1;
	case FL_SHORTCUT:
		/*if (Fl::event_key() == 'x') {
		do_callback();
		return 1;
		}*/
		return 0;
	default:
		return 0;
	}
}

template <class ELEMTYPE>
transfer_interpolated<ELEMTYPE >::transferchart::~transferchart ()
{
	if (histimg != NULL)
	{delete histimg; }

	delete [] lookup;
}

template <class ELEMTYPE>
void transfer_interpolated<ELEMTYPE >::transferchart::draw ()
{
	fl_color(FL_GRAY);
	fl_rectf(x(), y(),w(),h());

	//erase
	/*fl_color(FL_BLACK);
	fl_rectf(x(), y(),w(),h());*/
    if (histogram != NULL) //image may be uninitialized and have no histogram, just skip
        {
        if (histimg != NULL)
            {
            if (histimg->w() != w() || histimg->h() != h())
                {
                //Fl_RGB_Image * i = histimg->copy(w(), h()); 
                //
                //delete histimg;
                //
                //histimg = i;

                delete histimg;
                histimg = NULL;
                }
            else
                {
                histimg->uncache();
                }
            }

        if (histimg == NULL)
            {
            imgdata = new IMGELEMCOMPTYPE [w()*h()*RGBpixmap_bytesperpixel];
            histimg = new Fl_RGB_Image(imgdata,w(), h(), 3);
            }

        // http://www.fltk.org/articles.php?L466
        //unsigned char *img_data = histimg->data()[0];

        //const char * imgdata = histimg->data()[0];


        histogram->render(imgdata,histimg->w(),histimg->h());

        histimg->draw(x(),y()); 

        }

    //curve
    /*float xStep = (float)lookupSize/(float)w();
    float yScale = (float)h()/(float)IMGELEMCOMPMAX;
    float start = x();
    int p_end = w();
    float startY = y()+h();

    fl_color(FL_YELLOW);

    if (p_end > 0)
        {
        for (int p = 0; p < p_end - 1 ; p++)
            {
            fl_line(start+p, startY-yScale*lookup[static_cast<int>(std::floor(p*xStep))],start+(p+1), startY-yScale*lookup[static_cast<int>(std::floor((p+1)*xStep))] );
            }
        }*/

    float xStep = (float)lookupSize/(float)w();
    float yScale = (float)h()/(float)IMGELEMCOMPMAX;
    int start = x();
    int p_end = w();
    int startY = y()+h();

	fl_color(FL_YELLOW);

    int X, nextX,
        Y, nextY;

    nextX = start;
    nextY = startY-yScale*lookup[0];

    if (p_end > 0)
        {
        for (int p = 1; p < p_end ; p++)
            {
            X = nextX;
            Y = nextY;

            nextX = start+p;
            nextY = startY-yScale*(float)lookup[static_cast<int>(std::floor((float)p*xStep))];

            fl_line(X, Y , nextX, nextY );
            }
        }
	//------------- Draw knots --------------- 

	fl_color(FL_RED);
	intensity_knots.printdata();
	float knot_x,knot_y;
	float draw_x,draw_y;
	for(unsigned int i=0;i<intensity_knots.size();i++)
	{
		intensity_knots.get_data(i,knot_x,knot_y);
		draw_x = x() + knot_x/float(lookupSize)*w();
		draw_y = y() + h() - knot_y/float(255.0)*h();
		//		cout<<"fl_circle - draw_x,y "<<draw_x<<","<<draw_y<<endl;
		fl_circle(draw_x,draw_y,2);
	}

}

// *** transfer_interpolated (base class) ***
template <class ELEMTYPE>
transfer_interpolated<ELEMTYPE >::transfer_interpolated(image_storage <ELEMTYPE > * s):transfer_base<ELEMTYPE >(s)
    {
    Fl_Group * frame = this->pane;

    frame->resize(0,0,256,128);
    frame->resizable(frame);

    chart = new transferchart (this->source->get_histogram(), frame->x(),frame->y(),frame->w(),frame->h());
    frame->end();
    }

template <class ELEMTYPE>
transfer_interpolated<ELEMTYPE >::~transfer_interpolated()
    {
    //chart is deleted by FLTK, don't worry
    }

template <class ELEMTYPE>
void transfer_interpolated<ELEMTYPE >::get (const ELEMTYPE v, RGBvalue &p)
    {
    //p.set_mono(255); //DEBUG
    p.set_mono(chart->lookup[static_cast<unsigned int>((v-chart->lookupStart)*chart->lookupScale)]);
    }

// *** transfer_linear ***

template <class ELEMTYPE >
transfer_linear<ELEMTYPE >::transfer_linear(image_storage <ELEMTYPE > * s):transfer_interpolated<ELEMTYPE >(s)
{}

// *** transfer_spline ***

template <class ELEMTYPE>
transfer_spline<ELEMTYPE >::transfer_spline(image_storage <ELEMTYPE > * s):transfer_interpolated<ELEMTYPE >(s)
{}