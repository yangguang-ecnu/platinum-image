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
	lookupStart = std::numeric_limits<ELEMTYPE>::min(); //TODO: change to using image min/max values
	lookupScale = lookupSize/(std::numeric_limits<ELEMTYPE>::max()-std::numeric_limits<ELEMTYPE>::min());
    lookupEnd = std::numeric_limits<ELEMTYPE>::max()*lookupScale + lookupStart;
    
    lookup = new IMGELEMCOMPTYPE [lookupSize];

	intensity_knots = points_seq_func1D<float,float>(lookupStart,0,lookupStart+lookupSize,255,5);
}

template <class ELEMTYPE>
int transfer_interpolated<ELEMTYPE >::transferchart::handle (int event)
{
	float t_x = -1;		//event x position (in transfer (knots) coordinates)
	float t_y = -1;		//event y position (in transfer (knots) coordinates)
	float dist = -1;
	float closest_key = -1;
	int t_dx = -1;		//drag dx (in transfer (knots) coordinates)
	int t_dy = -1;		//drag dx (in transfer (knots) coordinates)
	int t_x_new = -1;	//new knot x (in transfer (knots) coordinates)
	int t_y_new = -1;	//new knot y (in transfer (knots) coordinates)

	switch(event)
	{
	case FL_PUSH:

		if(Fl::event_clicks() >0) //test for double click ( Fl::event_clicks() --> ...non-zero if the most recent FL_PUSH or FL_KEYBOARD was a "double click"...)
		{
			t_x = float(Fl::event_x() - x())/w()*float(lookupSize);
			t_y = float(255) - float(Fl::event_y() - y())/h()*float(255);
			dist = intensity_knots.find_dist_to_closest_point2D(t_x,t_y,closest_key);

			if(!intensity_knots.is_occupied(t_x))	//otherwise, points with same key will overwrite
			{
				intensity_knots.set_data(closest_key,t_x,t_y);
			}
			redraw();
		}
		return 1;

	case FL_DRAG:
		std::cout<<"FL_DRAG dx="<<Fl::event_dx()<<" dy="<<Fl::event_dy()<<std::endl;

		if(Fl::event_x()>=x() && Fl::event_x()<=x()+w() && Fl::event_y()>=y() && Fl::event_y()<=y()+h())
		{
			t_x = float(Fl::event_x() - x())/w()*float(lookupSize);
			t_y = float(255) - float(Fl::event_y() - y())/h()*float(255);
			dist = intensity_knots.find_dist_to_closest_point2D(t_x,t_y,closest_key);

			if(dist<70)
			{
				t_dx = float(Fl::event_dx())/w()*float(lookupSize);
				t_dy = float(Fl::event_dy())/h()*float(255);
				t_x_new = t_x + t_dx;
				t_y_new = t_y + t_dy;

				if(!intensity_knots.is_occupied(t_x_new))	//otherwise, points with same key will overwrite
				{
					intensity_knots.set_data(closest_key,t_x_new,t_y_new);
				}

			}
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
        /*if (histimg != NULL)
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
            }*/

        if (histimg == NULL)
            {
            imgdata = new IMGELEMCOMPTYPE [w()*h()*RGBpixmap_bytesperpixel];
            histimg = new Fl_RGB_Image(imgdata,w(), h(), 3);
            }

        //TODO: find out why uncache doesn't work as expected (crashes) on OSX
        //and us it to speed this up
        // http://www.fltk.org/articles.php?L466
        
        histogram->render(imgdata,histimg->w(),histimg->h());
        histimg->draw(x(),y()); 
        
        delete histimg;
        histimg = NULL;
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
    this->pane->resize(0,0,256,128);
    this->pane->resizable(this->pane);
    }

template <class ELEMTYPE>
transfer_interpolated<ELEMTYPE >::~transfer_interpolated()
    {
    //chart is deleted by FLTK, don't worry
    }

template <class ELEMTYPE>
void transfer_interpolated<ELEMTYPE >::get (const ELEMTYPE v, RGBvalue &p)
{
    if (v >= chart->lookupStart)
        {
        if (v < chart->lookupEnd)
            {
            p.set_mono(chart->lookup[static_cast<unsigned int>((v-chart->lookupStart)*chart->lookupScale)]);
            }
        else
            {p.set_mono(IMGELEMCOMPMAX);}
        }
    else
        { p.set_mono(0); }
}

// *** transfer_linear ***

template <class ELEMTYPE >
transfer_linear<ELEMTYPE >::transfer_linear(image_storage <ELEMTYPE > * s):transfer_interpolated<ELEMTYPE >(s)
{
    Fl_Group * frame = this->pane;

    this->chart = new transferchart_linear (this->source->get_histogram(), frame->x(),frame->y(),frame->w(),frame->h());
    frame->end();
}

template <class ELEMTYPE >
transfer_linear<ELEMTYPE >::transferchart_linear::transferchart_linear (histogram_1D<ELEMTYPE > * hi, int x, int y, int w, int h): transfer_interpolated<ELEMTYPE >::transferchart (hi, x, y, w, h)
{
    //test pattern
    /*for (unsigned int i = 0; i < lookupSize; i++)
    {lookup[i] = (i*IMGELEMCOMPMAX)/lookupSize;}*/
    for (unsigned int i = 0; i < this->lookupSize; i++)
        {this->lookup[i] = ((i*4) % static_cast<int>(IMGELEMCOMPMAX));}    
}

template <class ELEMTYPE >
void transfer_linear<ELEMTYPE >::transferchart_linear::render_lookup (IMGELEMCOMPTYPE lookup [],int lookupSize)
    {
    //TODO: remove lookupSize
    std::map<float,float>::iterator MITR,MEND;
    std::pair<float,float> thisPt,nextPt;

    thisPt = *MITR;
    ++MITR;

    for (;MITR != MEND;MITR++)
        {
        nextPt = *MITR;

        int deltaX = nextPt.first - thisPt.first;

        for (int x = 0;x < deltaX; x++)
            {
            lookup [x] = thisPt.second + (x*nextPt.second)/deltaX;
            }

        thisPt = nextPt;
        }
    }

// *** transfer_spline ***

template <class ELEMTYPE>
transfer_spline<ELEMTYPE >::transfer_spline(image_storage <ELEMTYPE > * s):transfer_interpolated<ELEMTYPE >(s)
{}