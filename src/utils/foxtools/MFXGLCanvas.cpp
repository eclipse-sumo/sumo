/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MFXGLCanvas.h
/// @author  Mirko Barthauer
/// @date    09.01.2025
///
// GL canvas ready for OpenGL3.3
/****************************************************************************/
#include <windows.h>
//#include <GL/glew.h>
#include "MFXGLCanvas.h"
#ifdef WIN32
#include <GL/wglew.h>
#else
#include <GL/glxew.h>
#endif

//#include "MFXGLVisual.h"

//#include <utils/common/MsgHandler.h>

#ifndef HAVE_GL_H
#define HAVE_GL_H
#endif

/*
  OLD JEROEN COMMENTS:
  Notes:
  - Since this only adds SetPixelFormat, perhaps not a bad idea to contemplate
    moving this call to SetPixelFormat somewhere else [candidates are FXGLVisual,
    FXWindow, or FXGLContext].
  - We may opt to have GLContext be created just prior to the first use.
*/


/*******************************************************************************/


// Object implementation
FXIMPLEMENT(MFXGLCanvas,FXGLCanvas,NULL,0)


// For serialization
MFXGLCanvas::MFXGLCanvas(){
    flags|=FLAG_ENABLED|FLAG_SHOWN;
    sgnext=this;
    sgprev=this;
    ctx=0;
}


// Make a canvas
MFXGLCanvas::MFXGLCanvas(FXComposite* p,MFXGLVisual *vis,FXObject* tgt,FXSelector sel,FXuint opts,FXint x,FXint y,FXint w,FXint h):
    FXGLCanvas(p, (FXGLVisual*)vis, tgt,sel,opts,x,y,w,h){
    flags|=FLAG_ENABLED|FLAG_SHOWN;
    visual=(FXGLVisual*)vis;
    sgnext=this;
    sgprev=this;
    ctx=0;
}


// Make a canvas sharing display lists
MFXGLCanvas::MFXGLCanvas(FXComposite* p,MFXGLVisual *vis,MFXGLCanvas* sharegroup,FXObject* tgt,FXSelector sel,FXuint opts,FXint x,FXint y,FXint w,FXint h):
    FXGLCanvas(p, (FXGLVisual*)vis, tgt,sel,opts,x,y,w,h){
    flags|=FLAG_ENABLED|FLAG_SHOWN;
    visual= (FXGLVisual*)vis;
    if(sharegroup){
        sgnext=sharegroup;
        sgprev=sharegroup->sgprev;
        sharegroup->sgprev=this;
        sgprev->sgnext=this;
    }
    else{
        sgnext=this;
        sgprev=this;
    }
    ctx=0;
}



#ifdef WIN32
const char* 
MFXGLCanvas::GetClass() const { return "FXGLCanvas"; } // the name is linked to a WIN API Window definition
#endif


// Return TRUE if it is sharing display lists
FXbool MFXGLCanvas::isShared() const { return TRUE /*sgnext != this*/; }


// Create X window (GL CANVAS)
void MFXGLCanvas::create(){
    FXTRACE((50, "MFXGLCanvas::create\n"));

    if (wglCreateContextAttribsARB == nullptr && wglChoosePixelFormatARB == nullptr) {
        WNDCLASSA window_class{ 0 };
        window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        window_class.lpfnWndProc = DefWindowProcA;
        window_class.hInstance = GetModuleHandle(0);
        window_class.lpszClassName = "Dummy_WGL_djuasiodwa";

        if (!RegisterClassA(&window_class)) {
            const DWORD error = GetLastError();
            FXTRACE((1, "MFXGLCanvas::create register dummy window class error %u\n", error));
            throw FX::FXWindowException("Failed to register dummy OpenGL window.");
        }

        HWND dummy_window = CreateWindowExA(
            0,
            window_class.lpszClassName,
            "Dummy OpenGL Window",
            0,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            window_class.hInstance,
            0);

        if (!dummy_window) {
            const DWORD error = GetLastError();
            FXTRACE((1, "MFXGLCanvas::create dummy window error %u\n", error));
            throw FX::FXWindowException("Failed to create dummy OpenGL window.");
        }

        HDC dummy_dc = ::GetDC((HWND)dummy_window);

        PIXELFORMATDESCRIPTOR pfd{ 0 };
        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.cColorBits = 32;
        pfd.cAlphaBits = 8;
        pfd.iLayerType = PFD_MAIN_PLANE;
        pfd.cDepthBits = 24;
        pfd.cStencilBits = 8;


        int pixel_format = ChoosePixelFormat(dummy_dc, &pfd);
        if (!pixel_format) {
            FXTRACE((50, "MFXGLCanvas::create Failed to find a suitable pixel format.\n"));
            throw FX::FXWindowException("Failed to find a suitable pixel format.");
        }
        if (!SetPixelFormat(dummy_dc, pixel_format, &pfd)) {
            FXTRACE((50, "MFXGLCanvas::create Failed to set the pixel format.\n"));
            throw FX::FXWindowException("Failed to set the pixel format.");
        }

        HGLRC dummy_context = wglCreateContext(dummy_dc);
        if (!dummy_context) {
            FXTRACE((50, "MFXGLCanvas::create Failed to create a dummy OpenGL rendering context.\n"));
            throw FX::FXWindowException("Failed to create a dummy OpenGL rendering context.");
        }

        if (!wglMakeCurrent(dummy_dc, dummy_context)) {
            FXTRACE((50, "MFXGLCanvas::create Failed to activate dummy OpenGL rendering context.\n"));
            throw FX::FXWindowException("Failed to activate dummy OpenGL rendering context.");
        }

        myWglCreateContextAttribsARB = (wglCreateContextAttribsARB_type*)wglGetProcAddress(
            "wglCreateContextAttribsARB");
        myWglChoosePixelFormatARB = (wglChoosePixelFormatARB_type*)wglGetProcAddress(
            "wglChoosePixelFormatARB");

        wglMakeCurrent(dummy_dc, 0);
        wglDeleteContext(dummy_context);
        ::ReleaseDC(dummy_window, dummy_dc);
        DestroyWindow(dummy_window);
    }

    FXTRACE((50, "MFXGLCanvas::create before FXWindow\n"));
    FXWindow::create();
    FXTRACE((50, "MFXGLCanvas::create after FXWindow\n"));

#ifdef HAVE_GL_H
    if(!ctx){
        void *sharedctx=NULL;

        // Must have GL info available
        if(!visual->getInfo()){
            throw FXWindowException("unable to create GL window.");
        }

        // Sharing display lists with other context
        if(sgnext!=this){

            // Find another member of the group which is already created, and get its context
            MFXGLCanvas *canvas=sgnext;
            while(canvas!=this){
            sharedctx=canvas->ctx;
            if(sharedctx) break;
            canvas=canvas->sgnext;
        }

        // The visuals have to match, the book says...
        if(sgnext->getVisual()!=canvas->getVisual()){
            throw FXWindowException("unable to create GL window.");
        }
    }

    #ifndef WIN32

    // Make context
    ctx=glXCreateContext((Display*)getApp()->getDisplay(),(XVisualInfo*)visual->getInfo(),(GLXContext)sharedctx,TRUE);
    if(!ctx){
        throw FXWindowException("unable to create GL window.");
    }

    #else

    FXTRACE((50, "MFXGLCanvas::create before SetPixelFormat\n"));

    // Now we can choose a pixel format the modern way, using wglChoosePixelFormatARB.
    int pixel_format_attribs[] = {
        WGL_DRAW_TO_WINDOW_ARB,     GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB,     GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB,      GL_TRUE,
        WGL_ACCELERATION_ARB,       WGL_FULL_ACCELERATION_ARB,
        WGL_PIXEL_TYPE_ARB,         WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB,         32,
        WGL_DEPTH_BITS_ARB,         24,
        WGL_STENCIL_BITS_ARB,       8,
        0
    };

    HDC hdc = ::GetDC((HWND)xid);

    int pixel_format;
    UINT num_formats;
    myWglChoosePixelFormatARB(hdc, pixel_format_attribs, 0, 1, &pixel_format, &num_formats);
    if (!num_formats) {
        throw FXWindowException("Failed to set the OpenGL 3.3 pixel format.");
    }

    PIXELFORMATDESCRIPTOR pfd;
    DescribePixelFormat(hdc, pixel_format, sizeof(pfd), &pfd);
    if (!SetPixelFormat(hdc, pixel_format, &pfd)) {
        throw FXWindowException("Failed to set the OpenGL 3.3 pixel format.");
    }

#if _DEBUG
    int glFlags = 0;
    glFlags |= WGL_CONTEXT_DEBUG_BIT_ARB;
#endif

    // Specify that we want to create an OpenGL 3.3 core profile context
    int gl33_attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB, // WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
#if _DEBUG
        WGL_CONTEXT_FLAGS_ARB, glFlags,
#endif
        0,
    };

    ctx = myWglCreateContextAttribsARB(hdc, 0, gl33_attribs);
    if (!ctx) {
        FXTRACE((50, "MFXGLCanvas::create Failed to create OpenGL 3.3 context.\n"));
        throw FXWindowException("Failed to create OpenGL 3.3 context.");
    }

    if (!wglMakeCurrent(hdc, (HGLRC)ctx)) {
        FXTRACE((50, "MFXGLCanvas::create Failed to activate OpenGL 3.3 rendering context.\n"));
        throw FXWindowException("Failed to activate OpenGL 3.3 rendering context.");
    }

    // Make that the pixel format of the device context
    /*
    HDC hdc=::GetDC((HWND)xid);
    if(!SetPixelFormat(hdc,(FXint)(FXival)visual->getVisual(),(PIXELFORMATDESCRIPTOR*)visual->getInfo())){
        throw FXWindowException("unable to create GL window.");
    }


    
    FXTRACE((50, "MFXGLCanvas::create before wglCreateContext\n"));

    // Make context
    ctx=(void*)wglCreateContext(hdc);
    if(!ctx){
        throw FXWindowException("unable to create GL window.");
    }

    FXTRACE((50, "MFXGLCanvas::create before wglShareLists\n"));
    */

    // I hope I didn't get this backward; the new context obviously has no
    // display lists yet, but the old one may have, as it has already been around
    // for a while.  If you see this fail and can't explain why, then that might
    // be what's going on.  Report this to jeroen@fox-toolkit.org
    // not to be used with modern OpenGL?
    /*
    if(sharedctx && !wglShareLists((HGLRC)sharedctx,(HGLRC)ctx)){
        throw FXWindowException("unable to create GL window.");
    }
    */

    GLenum err = glewInit();
    FXTRACE((1, "GLEW init returned %u\n", err));

    if (err == GLEW_OK) {
        glClearColor(1.f, 1.f, 1.f, 0.f);
#if _DEBUG
        int contextFlags = 0;
        glGetIntegerv(GL_CONTEXT_FLAGS, &contextFlags);
        if (contextFlags & GL_CONTEXT_FLAG_DEBUG_BIT) {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(GLDebugMessageCallback, 0);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }
#endif
    }

    ::ReleaseDC((HWND)xid,hdc);

    FXTRACE((50, "MFXGLCanvas::create end\n"));

    #endif
    }
#endif
}


// Detach the GL Canvas
void MFXGLCanvas::detach(){
#ifdef HAVE_GL_H
  if(ctx){
    // Will this leak memory?
    ctx=0;
    }
#endif
  FXWindow::detach();
  }


// Destroy the GL Canvas
void MFXGLCanvas::destroy(){
#ifdef HAVE_GL_H
  if(ctx){
#ifndef WIN32
    glXDestroyContext((Display*)getApp()->getDisplay(),(GLXContext)ctx);
#else
    wglDeleteContext((HGLRC)ctx);
#endif
    ctx=0;
    }
#endif
  FXWindow::destroy();
  }


long
MFXGLCanvas::onUpdate(FXObject* sender, FXSelector sel, void* ptr) {
    // TODO: change projection info due to updated window size
    return FXGLCanvas::onUpdate(sender, sel, ptr);
}


//  Make the rendering context of GL Canvas current
FXbool
MFXGLCanvas::makeCurrent(){
#ifdef HAVE_GL_H
  if(ctx){
#ifndef WIN32
    return glXMakeCurrent((Display*)getApp()->getDisplay(),xid,(GLXContext)ctx);
#else
    HDC hdc=::GetDC((HWND)xid);
    /*
    if(visual->colormap){
      SelectPalette(hdc,(HPALETTE)visual->colormap,FALSE);
      RealizePalette(hdc);
    }
    */
    BOOL bStatus=wglMakeCurrent(hdc,(HGLRC)ctx);
    return bStatus;
#endif
    }
#endif
  return FALSE;
  }


//  Make the rendering context of GL Canvas current
FXbool
MFXGLCanvas::makeNonCurrent(){
#ifdef HAVE_GL_H
  if(ctx){
#ifndef WIN32
    return glXMakeCurrent((Display*)getApp()->getDisplay(),None,(GLXContext)NULL);
#else
    // According to "Steve Granja" <sjgranja@hks.com>,
    // ::ReleaseDC is still necessary even for owned DC's.
    // So release it here to prevent resource leak.
    ::ReleaseDC((HWND)xid,wglGetCurrentDC());
    BOOL bStatus=wglMakeCurrent(NULL,NULL);
    return bStatus;
#endif
    }
#endif
  return FALSE;
  }


// Return current context, if any
void*
MFXGLCanvas::getCurrentContext(){
#ifdef HAVE_GL_H
#ifndef WIN32
  return (void*)glXGetCurrentContext();
#else
  return (void*)wglGetCurrentContext();
#endif
#else
  return NULL;
#endif
  }


//  Return TRUE if this window's context is current
FXbool
MFXGLCanvas::isCurrent() const {
#ifdef HAVE_GL_H
  if(ctx){
#ifndef WIN32
    return (glXGetCurrentContext() == (GLXContext)ctx);
#else
    return (wglGetCurrentContext() == (HGLRC)ctx);
#endif
    }
#endif
  return FALSE;
  }


// Used by GL to swap the buffers in double buffer mode, or flush a single buffer
void
MFXGLCanvas::swapBuffers(){
#ifdef HAVE_GL_H
#ifndef WIN32
  glXSwapBuffers((Display*)getApp()->getDisplay(),xid);
#else
  // SwapBuffers(wglGetCurrentDC());
  // wglSwapLayerBuffers(wglGetCurrentDC(),WGL_SWAP_MAIN_PLANE);
  HDC hdc=wglGetCurrentDC();
  if(wglSwapLayerBuffers(hdc,WGL_SWAP_MAIN_PLANE)==FALSE){
    SwapBuffers(hdc);
    }
#endif
#endif
  }


// Save object to stream
void
MFXGLCanvas::save(FXStream& store) const {
  FXWindow::save(store);
  store << sgnext;
  store << sgprev;
  }


// Load object from stream
void
MFXGLCanvas::load(FXStream& store){
  FXWindow::load(store);
  store >> sgnext;
  store >> sgprev;
  }


// Close and release any resources
MFXGLCanvas::~MFXGLCanvas(){
  sgnext->sgprev=sgprev;
  sgprev->sgnext=sgnext;
  sgnext=(MFXGLCanvas*)-1L;
  sgprev=(MFXGLCanvas*)-1L;
#ifdef HAVE_GL_H
  if(ctx){
#ifndef WIN32
    glXDestroyContext((Display*)getApp()->getDisplay(),(GLXContext)ctx);
#else
    wglDeleteContext((HGLRC)ctx);
#endif
    }
#endif
  }


