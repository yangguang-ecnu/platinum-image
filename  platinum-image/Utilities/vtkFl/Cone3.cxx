// Cone3.cxx adapted to illustrate the use of vtkFlRenderWindowInteractor
// $Id: Cone3.cxx,v 1.12 2005/08/08 09:59:21 cpbotha Exp $
// Study this example carefully.  Study it again.  Repeat.

// here we have all the usual VTK stuff that we need for our pipeline
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkConeSource.h>
#include <vtkSuperquadricSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkActor.h>

// and here we have the famous vtkFlRenderWindowInteractor class
#include <vtkFlRenderWindowInteractor.h>

// and of course some fltk stuff
#include <Fl/Fl_Box.H>
#include <Fl/Fl_Button.H>

// this is a callback for the quit button
void quit_cb(Fl_Widget*, void*)
{
   exit(0);
}

// this callback gets called when a non-main window is closed
// remember that one should NEVER delete() a vtkFlRenderWindowInteractor,
// since it's a vtk object.  instead, its Delete() method should be used.
// so, in the callback of the containing window, you should either terminate
// your application (if that's applicable) or if you have a multi-window 
// application, call ->Delete() on the vtkFlRWI and then take care of the
// surrounding logic.
void non_main_window_callback(Fl_Widget *w, void *flrwi)
{
   // this is a pre-VTK 5.0 kludge to get rid of the RenderWindow
   // see below for the correct VTK 5.0 (and after) way (::Finalize())
   ((vtkFlRenderWindowInteractor*)flrwi)->GetRenderWindow()->WindowRemap();
   // if this was the last ref to the RWI, everything (including the RW)
   // should be destroyed
   ((vtkFlRenderWindowInteractor*)flrwi)->Delete();
   // also take care of the FLTK part
   delete w;

   // in VTK 5.0 and newer, do it like this (recommended):
   //((vtkFlRenderWindowInteractor*)flrwi)->GetRenderWindow()->Finalize();
   //((vtkFlRenderWindowInteractor*)flrwi)->Delete();
   //delete w;
}

/**
 * This will build a VTK pipeline (given a correctly setup vtkFlRWI)
 * for visualising a cone.
 */
void create_cone_pipeline(vtkFlRenderWindowInteractor *flrwi)
{
  // create a rendering window and renderer
  vtkRenderer *ren = vtkRenderer::New();
  ren->SetBackground(0.5,0.5,0.5);
  
  vtkRenderWindow *renWindow = vtkRenderWindow::New();
  renWindow->AddRenderer(ren);
  // uncomment the statement below if things aren't rendering 100% on your
  // configuration; the debug output could give you clues as to why
  //renWindow->DebugOn();
   
  // NB: here we treat the vtkFlRenderWindowInteractor just like any other
  // old vtkRenderWindowInteractor
  flrwi->SetRenderWindow(renWindow);
  // just like with any other vtkRenderWindowInteractor(), you HAVE to call
  // Initialize() before the interactor will function.  See the docs in
  // vtkRenderWindowInteractor.h
  flrwi->Initialize();

  // create an actor and give it cone geometry
  vtkConeSource *cone = vtkConeSource::New();
  cone->SetResolution(8);
  vtkPolyDataMapper *coneMapper = vtkPolyDataMapper::New();
  coneMapper->SetInput(cone->GetOutput());
  vtkActor *coneActor = vtkActor::New();
  coneActor->SetMapper(coneMapper);
  coneActor->GetProperty()->SetColor(1.0, 0.0, 1.0);

  // assign our actor to the renderer
  ren->AddActor(coneActor);

  // We can now delete all our references to the VTK pipeline (except for
  // our reference to the vtkFlRenderWindowInteractor) as the objects
  // themselves will stick around until we dereference fl_vtk_window
  ren->Delete();
  renWindow->Delete();
  cone->Delete();
  coneMapper->Delete();
  coneActor->Delete();
}


/**
 * This will build a VTK pipeline (given a correctly setup vtkFlRWI)
 * for visualising a superquadric.
 */
void create_superquadric_pipeline(vtkFlRenderWindowInteractor *flrwi)
{
  // create a rendering window and renderer
  vtkRenderer *ren = vtkRenderer::New();
  ren->SetBackground(0.5,0.5,0.5);
  
  vtkRenderWindow *renWindow = vtkRenderWindow::New();
  renWindow->AddRenderer(ren);
  // uncomment the statement below if things aren't rendering 100% on your
  // configuration; the debug output could give you clues as to why
  //renWindow->DebugOn();
   
  // NB: here we treat the vtkFlRenderWindowInteractor just like any other
  // old vtkRenderWindowInteractor
  flrwi->SetRenderWindow(renWindow);
  // just like with any other vtkRenderWindowInteractor(), you HAVE to call
  // Initialize() before the interactor will function.  See the docs in
  // vtkRenderWindowInteractor.h
  flrwi->Initialize();

  // create an actor and give it cone geometry
  vtkSuperquadricSource *sqs = vtkSuperquadricSource::New();

  // some parameters for an interesting result
  sqs->SetToroidal(1);
  sqs->SetThickness(0.3333);
  sqs->SetPhiRoundness(0.2);
  sqs->SetThetaRoundness(0.8);
  sqs->SetSize(0.5);
  sqs->SetThetaResolution(64);
  sqs->SetPhiResolution(64);
  
  vtkPolyDataMapper *sqMapper = vtkPolyDataMapper::New();
  sqMapper->SetInput(sqs->GetOutput());
  vtkActor *sqActor = vtkActor::New();
  sqActor->SetMapper(sqMapper);

  sqActor->GetProperty()->SetColor(0.0, 1.0, 0.0);

  // assign our actor to the renderer
  ren->AddActor(sqActor);

  // We can now delete all our references to the VTK pipeline (except for
  // our reference to the vtkFlRenderWindowInteractor) as the objects
  // themselves will stick around until we dereference fl_vtk_window
  ren->Delete();
  renWindow->Delete();
  sqs->Delete();
  sqMapper->Delete();
  sqActor->Delete();
}

  

/**
 * This will create a Fl_Window with an embedded vtkFlRWI, a label
 * with some explanation and a quit button.  The pointers that are
 * passed are set correctly on return so that they can be used for
 * further processing; in our case that means actually building and
 * adding a VTK pipeline.
 */
void create_window_with_rwi(vtkFlRenderWindowInteractor *&flrwi,
                            Fl_Window *&flw,
                            char *title)
{
   // set up main FLTK window
   flw = new Fl_Window(300,330,title);
   
   // and instantiate vtkFlRenderWindowInteractor (here it acts like a
   // FLTK window, i.e. you could also instantiate it as child of a
   // Fl_Group in a window)
   flrwi = new vtkFlRenderWindowInteractor(5,5,290,260,NULL);
    
   // this will be replaced if it's the main window 
   flw->callback(non_main_window_callback, flrwi);

   // this will result in a little message under the rendering
   Fl_Box* box = new Fl_Box(5,261,290,34,
                             "3 = stereo, j = joystick, t = trackball, "
                            "w = wireframe, s = surface, p = pick; "
                            "you can also resize the window");
   box->labelsize(10);
   box->align(FL_ALIGN_WRAP);
   
   // we want a button with which the user can quit the application
   Fl_Button* quit_button = new Fl_Button(100,300,100,25,"quit");
   quit_button->callback(quit_cb,NULL);
   
   // we're done populating the flw
   flw->end();
   // if the main window gets resized, the vtk window should resize with it
   flw->resizable(flrwi);
}

int main( int argc, char *argv[] )
{

  // let's create the FIRST window with a Cone thingy ===========
  // ============================================================  
  vtkFlRenderWindowInteractor *fl_vtk_window = NULL;
  Fl_Window *main_window = NULL;

  create_window_with_rwi(fl_vtk_window, main_window,
                         "Cone3.cxx Main Window");
  // replace the non_main_window_callback with a normal quit callback
  // as this is the main window.
  main_window->callback(quit_cb, fl_vtk_window);

  // these two steps are VERY IMPORTANT, you have to show() the fltk window
  // containing the vtkFlRenderWindowInteractor, and then the
  // vtkFlRenderWindowInteractor itself
  main_window->show();
  fl_vtk_window->show();
   
  // now we get to setup our VTK rendering pipeline
  create_cone_pipeline(fl_vtk_window);


  // let's create the SECOND window with a SuperQuadric =========
  // ============================================================
 
  
  vtkFlRenderWindowInteractor *fl_vtk_window2 = NULL;
  Fl_Window *main_window2 = NULL;

  create_window_with_rwi(fl_vtk_window2, main_window2,
                         "Cone3.cxx Second Window");

  // these two steps are VERY IMPORTANT, you have to show() the fltk window
  // containing the vtkFlRenderWindowInteractor, and then the
  // vtkFlRenderWindowInteractor itself
  main_window2->show();
  fl_vtk_window2->show();
   
  // now we get to setup our VTK rendering pipeline
  create_superquadric_pipeline(fl_vtk_window2);
   
  // let's create the THIRD window with a SuperQuadric =========
  // ============================================================
 
  
  vtkFlRenderWindowInteractor *fl_vtk_window3 = NULL;
  Fl_Window *main_window3 = NULL;

  create_window_with_rwi(fl_vtk_window3, main_window3,
                         "Cone3.cxx Third Window");

  // these two steps are VERY IMPORTANT, you have to show() the fltk window
  // containing the vtkFlRenderWindowInteractor, and then the
  // vtkFlRenderWindowInteractor itself
  main_window3->show();
  fl_vtk_window3->show();
   
  // now we get to setup our VTK rendering pipeline
  create_superquadric_pipeline(fl_vtk_window3);

  // Now that everything's ready, we can finally start the app ==
  // ============================================================
   
  // this is the standard way of "starting" a fltk application
  int fl_ret = Fl::run();
  // very huge NB: note that we ->Delete() the vtkFlRenderWindowInteractor
  // once we do this, the rest of the vtk pipeline will really disappear
  fl_vtk_window->Delete();
  // and after we've done that, we can delete the main_window
  delete main_window;
   
  return fl_ret;
}
