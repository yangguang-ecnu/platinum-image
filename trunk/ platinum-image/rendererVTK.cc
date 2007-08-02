//$Id:

// This file is part of the Platinum library.
// Copyright (c) 2007 Uppsala University.
//
//    The Platinum library is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    The Platinum library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with the Platinum library; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#include "rendererVTK.h"

rendererVTK::rendererVTK():renderer_base()
{
    //offscreen rendering with VTK:
    //http://www.erdc.hpc.mil/documentation/Tips_Tricks/vtk
    //however, we won't do that
    
    /*
    // Create an actor.
    vtkActor *actor = vtkActor::New();
    actor‑>SetMapper(mapper);

    // Set up on‑screen rendering.
    vtkRenderWindow *renWin = vtkRenderWindow::New();
    renWin‑>AddRenderer(ren1);
    
    // Setup the VTK rendering pipeline.
    fprintf(stderr,"Rendering....");
    renWin‑>Render();
     */
}

void rendererVTK::draw(void){
    /*if (RenderWindow!=NULL)
        {

        // make sure the vtk part knows where and how large we are
        UpdateSize( this->w(), this->h() );
        
        // make sure the GL context exists and is current:
        // after a hide() and show() sequence e.g. there is no context yet
        // and the Render() will fail due to an invalid context.
        // see Fl_Gl_Window::show()
        make_current();
        
        RenderWindow->SetWindowId( (void *)fl_xid( this ) );
#if !defined(WIN32) && !defined(__APPLE__)
        RenderWindow->SetDisplayId( fl_display );
#endif
        // get vtk to render to the Fl_Gl_Window
        Render();
        }
    */
}