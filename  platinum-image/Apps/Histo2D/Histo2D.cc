//////////////////////////////////////////////////////////////////////////
//
//  Histo2D $Revision$
//
//  Application for doing 2D histogram segmentation
//  as described in Arvid Rudling's master thesis
//
//  $LastChangedBy$

//   Copyright (c) 2007, Arvid Rudling and Uppsala university
//   All rights reserved.
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions are met:
//       * Redistributions of source code must retain the above copyright
//         notice, this list of conditions and the following disclaimer.
//       * Redistributions in binary form must reproduce the above copyright
//         notice, this list of conditions and the following disclaimer in the
//         documentation and/or other materials provided with the distribution.
//       * Neither the name of the Uppsala university nor the
//         names of its contributors may be used to endorse or promote products
//         derived from this software without specific prior written permission.
//
//   THIS SOFTWARE IS PROVIDED BY ARVID RUDLING AND UPPSALA UNIVERSITY ``AS IS'' AND ANY
//   EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//   DISCLAIMED. IN NO EVENT SHALL ARVID RUDLING AND/OR UPPSALA UNIVERSITY BE LIABLE FOR ANY
//   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <cmath>

#include "../../platinumprogram.h"

#include <cstdio>
#include <iostream>

#include "../../global.h"

#include "../../image_binary.hxx"

//inclusions related to histogram segmentation project
#include "itkBinaryBallStructuringElement.h" 
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkConnectedThresholdImageFilter.h"
#include "itkImageDuplicator.h"
#include "itkCastImageFilter.h"
#include "itkAndImageFilter.h"

//!binary type for binary ITK images
//!(ITK does not support true 'bool')
#define theBinaryPixelType unsigned char

//#define theBinaryImageType itk::Image<theBinaryPixelType ,3 >
#define theBinaryImageType itk::OrientedImage<theBinaryPixelType ,3 >
#define theIndexedPixelType unsigned char
//#define theIndexedImageType itk::Image<theIndexedPixelType ,3 >
#define theIndexedImageType itk::OrientedImage<theIndexedPixelType ,3 >
#define theBinaryIteratorType itk::ImageRegionIterator<theBinaryImageType >
#define theIndexedIteratorType itk::ImageRegionIterator<theIndexedImageType >
#define theIndexType theBinaryImageType::RegionType::IndexType
#define theBinaryImagePointer theBinaryImageType::Pointer
#define theIndexedImagePointer theIndexedImageType::Pointer
#define theSEType itk::BinaryBallStructuringElement<theBinaryPixelType ,3 >
#define theErodeFilterType itk::BinaryErodeImageFilter<theBinaryImageType,theBinaryImageType,theSEType >
#define	theDilateFilterType itk::BinaryDilateImageFilter<theBinaryImageType,theBinaryImageType,theSEType >

int visid = -1;
int arg(int argc, char **argv, int &i) {
    if (argv[i][1] == 'v') {
        if (i+1 >= argc) return 0;
        visid = atoi(argv[i+1]);
        i += 2;
        return 2;
        }
return 0;
    }

void segment_image (int u,int p)
    {
    if (p == USERIO_CB_OK)
        {
        thresholdparvalue t = userIOmanagement.get_parameter<thresholdparvalue>(u,0);
        t.make_threshold_image();
        }
    }

void diff_images (int u,int p)
    {
    //compare to FA-FCM liver ground truth

    const short test_int = 510;
    const short truth_int = 100;

    if (p == USERIO_CB_OK)
        {
        userIOmanagement.progress_update(1,"Converting data",3);
        
        image_integer<theIndexedPixelType, 3> * truth_in =
            dynamic_cast<image_integer<theIndexedPixelType, 3> * >(
            datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(u,0)));
            //using dynamic_cast, reason:
            //for this application the input is supposed to be an unsigned char
            //and nothing else.
            //Note that truth_in is checked below to be != NULL
        image_binary<3> * test = binary_copycast<3>(
            datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(u,1)));
        image_binary<3> * mask = binary_copycast<3> (
            datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(u,2)));
            //using binary_copycast for these because:
            //1. a binary image is desired
            //2. the input image is binary formatted (1/0 only)

        image_binary<3>::iterator u = test->begin();

        if (truth_in!=NULL && test != NULL && truth_in->same_size(test))
            {
            userIOmanagement.progress_update(2,"Comparing " + truth_in->name() + " with " + test->name(),3);
            
            short size [3];

            for (int d=0; d < 3; d++)
                {size[d]=truth_in->get_size_by_dim(d);}

            unsigned long total_vol=size[0]*size[1]*size[2];
            unsigned long combined_vol = 0;
            //unsigned long intersect_vol = 0;
            unsigned long false_neg_vol = 0;
            unsigned long false_pos_vol = 0;
            unsigned long true_pos_vol = 0;
            unsigned long ref_vol = 0;

            cout << "Comparing " << truth_in->name() << " with " << test->name() << ", ";
            if (mask == NULL )
                { cout << "no mask"; }
            else
                { cout << "with mask " << mask->name(); }
            cout << endl;

            //convert values to image_label notation
            
            image_label<3> * truth = new image_label<3>(test,false);

            image_integer<theIndexedPixelType, 3>::iterator a = truth_in->begin();
            image_label<3>::iterator b = truth->begin();

            while (b != truth->end())
                {
                switch (*a)
                    {
                    case 100: //liver
                        *b = 3;
                        break;
                    case 225: //muscle
                        *b = 2;
                        break;
                    case 255: //fat
                        *b = 1;
                        break;

                    default:
                        *b = 0;
                    }

                a++;b++;
                }

            image_label<3> * result = new image_label<3>(test,false);
            result->erase();

            image_binary<3>::iterator i = test->begin();
            image_binary<3>::iterator m = i; //any value would do
            if (mask != NULL)
                { m = mask->begin(); }
            image_label<3>::iterator t = truth->begin();
            image_label<3>::iterator o = result->begin();

            while (o != result->end())
                {
                *o = 0;
                if (mask == NULL || *m == true)
                    {
                    bool liver_truth    = (*t == 3);
                    bool liver_test     = *i;

                    if (liver_truth)
                        {ref_vol++;}

                    if (liver_truth && liver_test)
                        {
                        *o = 3;
                        true_pos_vol++;
                        }
                    else
                        {
                        if (liver_truth)
                            {
                            *o = 1;
                            false_neg_vol++;
                            }

                        if (liver_test)
                            {
                            *o = 2;
                            false_pos_vol++;
                            }
                        }

                    if (liver_truth || liver_test)
                        {combined_vol++;}
                    }

                i++;o++;t++;m++;
                }
            
            userIOmanagement.progress_update(3,"Cleaning up " + truth_in->name() + " with " + test->name(),3);

            datamanagement.add(result);

            delete test;        //test was created inside this function using binary_copycast
            if (mask != NULL)   //mask was created inside this function using binary_copycast
                { delete mask; }
            delete truth;       //truth was created with new

            //truth_in is not deleted, since it was only recast with
            //dynamic_cast and not copied

            //result is not deleted, since it is added into datamanagement

            ostringstream figures_stream;
            float TP = 100 * (combined_vol-false_pos_vol-false_neg_vol)/ref_vol;

            cout << "True positives: " << true_pos_vol << endl;
            cout << "False positives: " << false_pos_vol << endl;
            cout << "False negatives: " << false_neg_vol << endl;
            cout << "Combined image: " << combined_vol << endl;
            cout << "Reference image: " << ref_vol << endl;

            figures_stream << "True positive: " << TP << "%";

            userIOmanagement.show_message("Comparison result",figures_stream.str());
            
            userIOmanagement.progress_update();
            }
        }
    }

void threshold_artifact_process (int u,int p)
    {
    if (p == USERIO_CB_OK)
        {
        //image_integer<unsigned char, 3> * fat_image = scalar_copycast<image_integer<unsigned char, 3>, unsigned char, 3>(datamanagement.get_image( userIOmanagement.get_parameter<imageIDtype>(u,0)) );

        /*image_scalar<unsigned char,3> * boo = scalar_copycast<image_scalar,unsigned char,3>(userIOmanagement.get_parameter<imageIDtype>(datamanagement.get_image(userIOmanagement..get_parameter<imageIDtype>(u,0)
            )));

        delete boo;*/
        
        userIOmanagement.progress_update(1,"Converting data",5);
        
        image_label<3> * input_vol = label_copycast<3>(
            datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(u,0)));
            //using label_copycast, reason:
            //a binary image is suitable but
            //ITK::image cannot handle images with 'bool' value
            //to convert to these, the source (Platinum) type has to be a bigger scalar type,
            //image_label is a good choice

        // *** get ITK image from input parameter
        theBinaryImagePointer input;
        //input_vol->make_image_an_itk_reader(); //Depricated
        //input = input_vol->itk_image(); //Depricated
		input = input_vol->get_image_as_itk_output();

        int radius = userIOmanagement.get_parameter<long>(u,1);

        // *** erode ***
        
        userIOmanagement.progress_update(2,"Eroding",5);

        theBinaryPixelType erodeValue = 1; //input_vol->get_max();
        theErodeFilterType::Pointer  binaryErode  = theErodeFilterType::New();
        //typename theWriterType::Pointer writer = theWriterType::New(); 

        theSEType  erodeStructElem;
        erodeStructElem.SetRadius( radius );  // 1 --> 3x3x3 structuring element
        erodeStructElem.CreateStructuringElement();

        binaryErode->SetKernel(erodeStructElem);
        binaryErode->SetErodeValue( erodeValue );
        binaryErode->SetInput( input );
        binaryErode->Update();
        theBinaryImagePointer erodedImageBinary = binaryErode->GetOutput();

        /*itk::CastImageFilter<theBinaryImageType, theIndexedImageType >::Pointer deepener = itk::CastImageFilter<theBinaryImageType, theIndexedImageType >::New ();
        deepener->SetInput (erodedImageBinary);
        deepener->Update();
        theIndexedImagePointer erodedImage = deepener->GetOutput ();*/

        // *** get connected images ***

        /*itk::ImageDuplicator<theIndexedImageType >::Pointer duplicator = itk::ImageDuplicator<theIndexedImageType >::New ();
        duplicator->SetInputImage (erodedImage);
        duplicator->Update();
        theIndexedImagePointer orig = duplicator->GetOutput ();

        theIndexedImagePointer connected = theIndexedImageType::New();
        connected->SetRegions(orig->GetRequestedRegion());
        connected->CopyInformation(orig);
        connected->Allocate();

        //Volume orig = Volume(theImage);
        //Volume connected = Volume(theImage);

        //connected.scaleValues(0.0);
        //ShiftScaleImageFilter::Pointer  scaler  = ShiftScaleImageFilter::New();

        //get iterators for the images
        theIndexedIteratorType it = theIndexedIteratorType(orig, orig->GetBufferedRegion());
        it.GoToBegin();

        theIndexedIteratorType it2 = theIndexedIteratorType(connected, connected->GetBufferedRegion());
        it2.GoToBegin();

        itk::BinaryThresholdImageFunction<theIndexedImageType >::Pointer thresholdFunc = itk::BinaryThresholdImageFunction<theIndexedImageType >::New();
        thresholdFunc->SetInputImage(orig);
        thresholdFunc->ThresholdBetween(1,100000000);

        while( !it.IsAtEnd() ){
        if(it.Value()>0 && it2.Value()==0){ 
        //getConnectedVolumeFromSeed

        double vol;

        theIndexType seed = it.GetIndex();
        int maximage = 1000; //maxvalue

        itk::BinaryThresholdImageFunction<theIndexedImageType >::Pointer conVolFromSeedFunc = itk::BinaryThresholdImageFunction<theIndexedImageType >::New();
        conVolFromSeedFunc->SetInputImage(orig);
        conVolFromSeedFunc->ThresholdBetween(1,100000000);

        itk::FloodFilledImageFunctionConditionalIterator<theIndexedImageType,itk::BinaryThresholdImageFunction<theIndexedImageType > > fillIt( orig, conVolFromSeedFunc, seed );
        fillIt.GoToBegin();
        vol=0;
        while( !fillIt.IsAtEnd()){
        if(vol<maximage){
        vol++;
        }
        ++fillIt;
        }
        //getConnectedVolumeFromSeed end

        if(vol>1){
        itk::FloodFilledImageFunctionConditionalIterator<theIndexedImageType ,itk::BinaryThresholdImageFunction<theIndexedImageType > > it3( connected, thresholdFunc, it.GetIndex() );
        it3.GoToBegin();
        while( !it3.IsAtEnd()){
        it3.Set(vol);
        ++it3;
        }
        }
        }
        ++it;
        ++it2;	//ganska viktig.... ;-)
        }*/

        // *** dilate ***

        userIOmanagement.progress_update(3,"Dilating",5);
        
        theBinaryPixelType dilateValue = 1;
        theDilateFilterType::Pointer  binaryDilate  = theDilateFilterType::New();

        theSEType  dilateStructElem;
        dilateStructElem.SetRadius( radius * 2 );
        dilateStructElem.CreateStructuringElement();

        binaryDilate->SetKernel(dilateStructElem);
        binaryDilate->SetDilateValue( dilateValue );
        binaryDilate->SetInput( erodedImageBinary );
        binaryDilate->Update();
        theBinaryImagePointer dilatedImageBinary = binaryDilate->GetOutput();

        /*itk::CastImageFilter<theIndexedImageType, theBinaryImageType >::Pointer reducer = itk::CastImageFilter<theIndexedImageType, theBinaryImageType >::New ();
        reducer->SetInput (connected);
        reducer->Update();
        theBinaryImagePointer final = reducer->GetOutput ();*/

        // *** apply as mask to original
        
        userIOmanagement.progress_update(4,"Applying mask",5);

        itk::AndImageFilter<theBinaryImageType, theBinaryImageType, theBinaryImageType >::Pointer masker = itk::AndImageFilter<theBinaryImageType, theBinaryImageType, theBinaryImageType >::New ();
        masker->SetInput1 (input);
        masker->SetInput2 (dilatedImageBinary);
        masker->Update();
        theBinaryImagePointer final = masker->GetOutput ();

        delete input_vol; //input_vol was created inside this function using label_copycast

        // *** convert result to an image ***
        userIOmanagement.progress_update(5,"Adding image",5);

//        datamanagement.add(new image_label<3> (final));
		image_label<3> *f = new image_label<3>(final);
		datamanagement.add(f);
        
        userIOmanagement.progress_update();
        }
    }

int main(int argc, char *argv[])
    {
    platinum_init();

    //set up the window

    const int w_margin=15*2;
    int windowwidth = 800-w_margin;
    int windowheight = 600-w_margin;

    // myFl_Overlay_Window window(800,600);
	Fl_Window window(800, 600);
	
    platinum_setup(window);

    // *** 2D histogram project ***

    int segment_ID=userIOmanagement.add_userIO("Histogram segmentation",segment_image,"Segment");
    userIOmanagement.add_par_histogram_2D(segment_ID,"2D histogram");
    userIOmanagement.finish_userIO(segment_ID);

    int threshold_artifact_process_ID=userIOmanagement.add_userIO("Threshold artifact removal",threshold_artifact_process,"Process");
    userIOmanagement.add_par_image(threshold_artifact_process_ID,"Input");
    userIOmanagement.add_par_longint_box(threshold_artifact_process_ID,"Radius",4,1);
    userIOmanagement.finish_userIO(threshold_artifact_process_ID);

    int difference_ID=userIOmanagement.add_userIO("FA FCM comparison",diff_images,"Calculate");
    userIOmanagement.add_par_image(difference_ID,"Truth");
    userIOmanagement.add_par_image(difference_ID,"Test");
    userIOmanagement.add_par_image(difference_ID,"Mask (optional)");
    userIOmanagement.finish_userIO(difference_ID);

    // *** 2D histogram project end ***

    //finish the window creation
    window.end();

    window.show(argc, argv);

    return Fl::run();
    }
