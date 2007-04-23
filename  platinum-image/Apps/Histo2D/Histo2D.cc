//////////////////////////////////////////////////////////////////////////
//
//  Histo2D
//
//  Application for doing 2D histogram segmentation
//  as described in Arvid Rudling's master thesis
//
//

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

#include "platinumprogram.h"

#include <cstdio>
#include <iostream>

#include "global.h"

#include "image_label.h"

//Kullberg's volume tools
//#include "../../Src/ITK/Volume.h"

//inclusions related to histogram segmentation project
#include "itkBinaryBallStructuringElement.h" 
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkConnectedThresholdImageFilter.h"
#include "itkImageDuplicator.h"
#include "itkCastImageFilter.h"
#include "itkAndImageFilter.h"

#define theBinaryPixelType unsigned char
#define theBinaryImageType itk::Image<theBinaryPixelType ,3 >
#define theIndexedImageType itk::Image<unsigned short ,3 >
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

void segment_volume (int u,int p)
    {
    if (p == USERIO_CB_OK)
        {
        thresholdparvalue t = userIOmanagement.get_parameter<thresholdparvalue>(u,0);
        t.make_threshold_volume();
        }
    }

void diff_volumes (int u,int p)
    {
    //compare to FA-FCM liver ground truth

    const short test_int = 510;
    const short truth_int = 100;

    if (p == USERIO_CB_OK)
        {
        image_base * truth = 
            datamanagement.get_image(userIOmanagement.get_parameter<volumeIDtype>(u,0));
        image_base * test = 
            datamanagement.get_image(userIOmanagement.get_parameter<volumeIDtype>(u,1));
        image_base * mask = 
            datamanagement.get_image(userIOmanagement.get_parameter<volumeIDtype>(u,2));

        if (truth!=NULL && test != NULL && truth->same_size(test))
            {
            short size [3];

            for (int d=0; d < 3; d++)
                {size[d]=truth->get_size_by_dim(d);}

            unsigned long total_vol=size[0]*size[1]*size[2];
            unsigned long combined_vol = 0;
            //unsigned long intersect_vol = 0;
            unsigned long false_neg_vol = 0;
            unsigned long false_pos_vol = 0;
            unsigned long true_pos_vol = 0;
            unsigned long ref_vol = 0;

            cout << "Comparing " << truth->name() << " with " << test->name() << ", ";
            if (mask == NULL )
                { cout << "no mask"; }
            else
                { cout << "with mask " << mask->name(); }
            cout << endl;

            image_scalar<unsigned char> * result = dynamic_cast<image_scalar<unsigned char> *>(truth->alike(VOLDATA_UCHAR));

            for (short z = 0; z < size[2];z++)
                {
                for (short y = 0; y < size[1];y++)
                    {
                    for (short x = 0; x < size[0];x++)
                        {
                        result->set_voxel(x,y,z,0);
                        if (mask == NULL || mask->get_number_voxel(x,y,z) == 255)
                            {
                            bool liver_truth    = truth->get_number_voxel(x,y,z) == truth_int;
                            bool liver_test     = test->get_number_voxel(x,y,z) == test_int;
                            /*if (test->get_number_voxel(x,y,z) > 0 )
                            cout << test->get_number_voxel(x,y,z) << endl;*/


                            if (liver_truth)
                                {ref_vol++;}

                            if (liver_truth && liver_test)
                                {
                                result->set_voxel(x,y,z,128);
                                true_pos_vol++;
                                }
                            else
                                {
                                if (liver_truth)
                                    {
                                    result->set_voxel(x,y,z,64);
                                    false_neg_vol++;
                                    }

                                if (liver_test)
                                    {
                                    result->set_voxel(x,y,z,192);
                                    false_pos_vol++;
                                    }
                                }

                            if (liver_truth || liver_test)
                                {combined_vol++;}
                            }
                        }
                    }
                }
            datamanagement.add(result);

            ostringstream figures_stream;
            float TP = 100 * (combined_vol-false_pos_vol-false_neg_vol)/ref_vol;

            cout << "True positives: " << true_pos_vol << endl;
            cout << "False positives: " << false_pos_vol << endl;
            cout << "False negatives: " << false_neg_vol << endl;
            cout << "Combined volume: " << combined_vol << endl;
            cout << "Reference volume: " << ref_vol << endl;

            figures_stream << "True positive: " << TP << "%";

            userIOmanagement.show_message("Comparison result",figures_stream.str());
            }
        }
    }

void threshold_artifact_process (int u,int p)
    {
    if (p == USERIO_CB_OK)
        {
        image_base * input_vol = 
            datamanagement.get_image(userIOmanagement.get_parameter<volumeIDtype>(u,0));

        // *** get ITK image from input parameter
        input_vol->make_image_an_itk_reader();
        theBinaryImagePointer input;
        input = (dynamic_cast<image_label<3> * >(input_vol))->itk_image();

        int radius = userIOmanagement.get_parameter<long>(u,1);

        // *** erode ***

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

        // *** get connected volumes ***

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
        int maxvolume = 1000; //maxvalue

        itk::BinaryThresholdImageFunction<theIndexedImageType >::Pointer conVolFromSeedFunc = itk::BinaryThresholdImageFunction<theIndexedImageType >::New();
        conVolFromSeedFunc->SetInputImage(orig);
        conVolFromSeedFunc->ThresholdBetween(1,100000000);

        itk::FloodFilledImageFunctionConditionalIterator<theIndexedImageType,itk::BinaryThresholdImageFunction<theIndexedImageType > > fillIt( orig, conVolFromSeedFunc, seed );
        fillIt.GoToBegin();
        vol=0;
        while( !fillIt.IsAtEnd()){
        if(vol<maxvolume){
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

        itk::AndImageFilter<theBinaryImageType, theBinaryImageType, theBinaryImageType >::Pointer masker = itk::AndImageFilter<theBinaryImageType, theBinaryImageType, theBinaryImageType >::New ();
        masker->SetInput1 (input);
        masker->SetInput2 (dilatedImageBinary);
        masker->Update();
        theBinaryImagePointer final = masker->GetOutput ();

        // *** convert result to an image ***

        datamanagement.add( new image_label<3> (final));
        }
    }

int main(int argc, char *argv[])
    {
    platinum_init();

    //set up the window

    const int w_margin=15*2;
    int windowwidth = 800-w_margin;
    int windowheight = 600-w_margin;

    Fl_Window window(800,600);

    platinum_setup(window);

    // *** 2D histogram project ***

    int segment_ID=userIOmanagement.add_userIO("Histogram segmentation",segment_volume,"Segment");
    userIOmanagement.add_par_histogram_2D(segment_ID,"2D histogram");
    userIOmanagement.finish_userIO(segment_ID);

    int threshold_artifact_process_ID=userIOmanagement.add_userIO("Threshold artifact removal",threshold_artifact_process,"Process");
    userIOmanagement.add_par_volume(threshold_artifact_process_ID,"Input");
    userIOmanagement.add_par_int_box(threshold_artifact_process_ID,"Radius",4,1);
    userIOmanagement.finish_userIO(threshold_artifact_process_ID);

    int difference_ID=userIOmanagement.add_userIO("FA FCM comparison",diff_volumes,"Calculate");
    userIOmanagement.add_par_volume(difference_ID,"Truth");
    userIOmanagement.add_par_volume(difference_ID,"Test");
    userIOmanagement.add_par_volume(difference_ID,"Mask (optional)");
    userIOmanagement.finish_userIO(difference_ID);

    // *** 2D histogram project end ***

    //finish the window creation
    window.end();

    window.show(argc, argv);

    return Fl::run();
    }
