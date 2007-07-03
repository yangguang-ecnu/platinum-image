//$Id$

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
    lookup = NULL;
    histogram = hi;

    callback (transfer_base<ELEMTYPE >::redraw_image_cb);
    user_data(static_cast<REDRAWCALLBACKPTYPE>(hi->image()));

    calc_lookup_params(256);

    intensity_knots = points_seq_func1D<float,unsigned char>(0,0,lookupSize,255,5);
//    intensity_knots = points_seq_func1D<float,float>(0,0,lookupSize,255,5);
    }

template <class ELEMTYPE >
void transfer_interpolated<ELEMTYPE >::update ()
    {
    chart->update();
    }

template <class ELEMTYPE>
void transfer_interpolated<ELEMTYPE >::transferchart::calc_lookup_params (int newSize)
    {
    //ELEMTYPE old_max = histogram->max();
    //ELEMTYPE old_min = histogram->min();

    histogram->calculate(); //do NOT call this directly elsewhere, instead, call calc_lookup_params()
    //when there is reason to recalculate histogram since all the stuff below
    //has to be recalculated as well

    if (newSize > 0 && newSize != lookupSize)
        {
        if (lookup != NULL)
            {
            delete lookup;
            }

        lookup = new IMGELEMCOMPTYPE [newSize];

        std::map<float,unsigned char>::iterator MITR = intensity_knots.begin();
        std::map<float,unsigned char>::iterator MEND = intensity_knots.end();
//        std::map<float,float>::iterator MITR = intensity_knots.begin();
  //      std::map<float,float>::iterator MEND = intensity_knots.end();

        //scale knots to new lookup table size
        /*for (;MITR != MEND;MITR++)
        {
        (*MITR).first = ((*MITR).first*newSize)/lookupSize;
        }*/

        lookupSize = newSize;
        }

    lookupStart = histogram->min();
    lookupScale = static_cast<float>(lookupSize)/(histogram->max()-histogram->min());
    lookupEnd = histogram->max();
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

			if(dist>70){	// add point
				if(!intensity_knots.is_occupied(t_x))	//otherwise, points with same key will overwrite
				{
					intensity_knots.insert(t_x, t_y);
				}

			}else if(dist>5){ //move point
				if(!intensity_knots.is_occupied(t_x))	//otherwise, points with same key will overwrite
				{
					intensity_knots.set_data(closest_key, t_x, t_y);
				}

			}else{ //remove point
				intensity_knots.erase(closest_key);
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
					redraw();
				}

			}
			return 1;
		}
        return 0;

	case FL_RELEASE:
		update();      //recalculates histogram, renders lookup table and redraws
        do_callback(); //redraw image that the transfer function is attached to
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
    fl_push_clip (x(),y(),w(),h());

    fl_color(FL_GRAY);
    fl_rectf(x(), y(),w(),h());

    if (histogram != NULL) //image may be uninitialized and have no histogram, just skip
        {

        //TODO: find out why uncache doesn't work as expected (crashes) on OSX
        //and us it to speed this up
        // http://www.fltk.org/articles.php?L466

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

       

        histogram->render(imgdata,histimg->w(),histimg->h());
        histimg->draw(x(),y()); 

        delete histimg;
        histimg = NULL;
        }

    // *** draw curve ***

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
    float knot_x;
	unsigned char knot_y;
    float draw_x,draw_y;
    for(unsigned int i=0;i<intensity_knots.size();i++)
        {
        intensity_knots.get_data(i,knot_x,knot_y);
        draw_x = x() + knot_x/float(lookupSize)*w();
        draw_y = y() + h() - float(knot_y)/float(255.0)*h();
        //		cout<<"fl_circle - draw_x,y "<<draw_x<<","<<draw_y<<endl;
        fl_circle(draw_x,draw_y,2);
        }
    fl_pop_clip();
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
            {p.set_mono(chart->lookup[chart->lookupSize -1]);}
        }
    else
        { p.set_mono(chart->lookup[0]); }
}



// *** transfer_linear ***

template <class ELEMTYPE >
transfer_linear<ELEMTYPE >::transfer_linear(image_storage <ELEMTYPE > * s):transfer_interpolated<ELEMTYPE >(s)
{
    //pane->choice ();
    Fl_Group * frame = this->pane;

    this->chart = new transferchart_linear (this->source->get_histogram(), frame->x(),frame->y(),frame->w(),frame->h());
    frame->end();
}

template <class ELEMTYPE >
transfer_linear<ELEMTYPE >::transferchart_linear::transferchart_linear (histogram_1D<ELEMTYPE > * hi, int x, int y, int w, int h): transfer_interpolated<ELEMTYPE >::transferchart (hi, x, y, w, h)
{
    update(); //must be called here, base class constructor gets the abstract one
}

template <class ELEMTYPE >
void transfer_linear<ELEMTYPE >::transferchart_linear::update ()
    {
    //this function may be called when the *image* has changed;
    //first recalculate min/max etc.
    this->calc_lookup_params();

    //render lookup chart
    std::map<float,unsigned char>::iterator MITR = this->intensity_knots.begin();
    std::map<float,unsigned char>::iterator MEND = this->intensity_knots.end();
//    std::map<float,float>::iterator MITR = intensity_knots.begin();
  //  std::map<float,float>::iterator MEND = intensity_knots.end();

    std::pair<float,float> thisPt,nextPt;

    thisPt = *MITR;
    ++MITR;

    for (;MITR != MEND;MITR++)
        {
        nextPt = *MITR;

        unsigned long deltaX = nextPt.first - thisPt.first;

        float slope = (nextPt.second-thisPt.second)/deltaX;

        for (unsigned long p = 0;p < min(deltaX,this->lookupSize); p++)
            {
            int x = p + static_cast<int>(thisPt.first);
            float value = thisPt.second + (p*slope);
            this->lookup [x] = value;
            }

        thisPt = nextPt;
        }

    //redraw to show changes
    this->redraw();
    }





// *** transfer_spline ***

template <class ELEMTYPE>
transfer_spline<ELEMTYPE >::transfer_spline(image_storage <ELEMTYPE > * s):transfer_interpolated<ELEMTYPE >(s)
{
    Fl_Group * frame = this->pane;

    this->chart = new transferchart_spline (this->source->get_histogram(), frame->x(),frame->y(),frame->w(),frame->h());
    frame->end();
}

template <class ELEMTYPE >
transfer_spline<ELEMTYPE >::transferchart_spline::transferchart_spline (histogram_1D<ELEMTYPE > * hi, int x, int y, int w, int h): transfer_interpolated<ELEMTYPE >::transferchart (hi, x, y, w, h)
{
    update(); //must be called here, base class constructor gets the abstract one
}

template <class ELEMTYPE >
void transfer_spline<ELEMTYPE >::transferchart_spline::update ()
{
	//this function may be called when the *image* has changed;
	//first recalculate min/max etc.
	this->calc_lookup_params();

	float *y2 = new float[this->intensity_knots.size()+2];
	y2 = this->intensity_knots.get_spline_derivatives();

    for (unsigned long p = 0;p < this->lookupSize; p++)
	{
		this->lookup [p] = this->intensity_knots.get_value_interp_spline(p,y2);
	}

	delete y2;

	//redraw to show changes
	this->redraw();
}