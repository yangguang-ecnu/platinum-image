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

    lookup = new IMGELEMCOMPTYPE (lookupSize);

    //test pattern
    for (unsigned int i = 0; i < lookupSize; i++)
        {lookup[i] = ((i*4) % static_cast<int>(IMGELEMCOMPMAX));}
    }

template <class ELEMTYPE>
int transfer_interpolated<ELEMTYPE >::transferchart::handle (int event)
    {
    switch(event)
        {
        case FL_PUSH:
            // ...
            redraw();
            return 1;
        case FL_DRAG:
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
    //erase
    /*fl_color(FL_BLACK);
    fl_rectf(x(), y(),w(),h());*/

    //histogram

    IMGELEMCOMPTYPE * imgdata = NULL;

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
        imgdata = new unsigned char [w()*h()];
        histimg = new Fl_RGB_Image(imgdata,w(), h(), 3);
        }
    // http://www.fltk.org/articles.php?L466
    //unsigned char *img_data = histimg->data()[0];
    const char *buf = histimg->data()[0];

    histogram->render(imgdata,histimg->w(),histimg->h());

    histimg->draw(x(),y()); 

    //curve
    float xStep = lookupSize/w();
    float yScale = h()/IMGELEMCOMPMAX;
    int start = x();
    int end = w();
    int startY = y()+h();

    fl_color(FL_YELLOW);

    if (end > 0)
        {
        for (int p = 0; p < end; p++)
            {
            fl_line(start+p, startY-yScale*lookup[static_cast<unsigned int>(p*xStep)], start+(p+1)*xStep, startY-yScale*lookup[static_cast<unsigned int>((p+1)*xStep)]);
            }
        }

    }

// *** transfer_interpolated (base class) ***
template <class ELEMTYPE>
transfer_interpolated<ELEMTYPE >::transfer_interpolated(image_storage <ELEMTYPE > * s):transfer_base<ELEMTYPE >(s)
    {
    chart = new transferchart (this->source->get_histogram(), this->pane->x(),this->pane->y(),this->pane->w(),this->pane->h());
    this->pane->end();
    }

template <class ELEMTYPE>
transfer_interpolated<ELEMTYPE >::~transfer_interpolated()
    {
    delete chart;
    }

template <class ELEMTYPE>
void transfer_interpolated<ELEMTYPE >::get (const ELEMTYPE v, RGBvalue &p)
    {
    p.set_mono(255);
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