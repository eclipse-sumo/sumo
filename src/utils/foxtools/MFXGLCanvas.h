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
#pragma once
#include <config.h>

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

#ifndef __glew_h__
#include <GL/glew.h>
#endif
#include "fxheader.h"

#ifdef WIN32
#define NOMINMAX
#endif
#include <fx3d.h>
#ifdef WIN32
#undef NOMINMAX
#endif

typedef HGLRC WINAPI wglCreateContextAttribsARB_type(HDC hdc, HGLRC hShareContext, const int* attribList);
typedef BOOL WINAPI wglChoosePixelFormatARB_type(HDC hdc, const int* piAttribIList,
    const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats);

class MFXGLVisual;

/// MFXGLCanvas, an area drawn by another object
class MFXGLCanvas : public FXGLCanvas {
    FXDECLARE(MFXGLCanvas)
private:
    MFXGLCanvas* sgnext;  // Share group next in share list
    MFXGLCanvas* sgprev;  // Share group previous in share list
protected:
    void* ctx;     // GL Context
    wglCreateContextAttribsARB_type* myWglCreateContextAttribsARB = nullptr;
    wglChoosePixelFormatARB_type* myWglChoosePixelFormatARB = nullptr;

protected:
    MFXGLCanvas();
private:
    MFXGLCanvas(const FXGLCanvas&);
    MFXGLCanvas& operator=(const FXGLCanvas&);

#ifdef WIN32
    virtual const char* GetClass() const;
#endif
    public:
        /**
        * Construct an OpenGL-capable canvas, with its own private display list.
        */
        MFXGLCanvas(FXComposite* p, MFXGLVisual* vis, FXObject* tgt = NULL, FXSelector sel = 0, FXuint opts = 0, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0);

        /**
        * Construct an OpenGL-capable canvas, sharing display
        * list with another GL canvas.  This canvas becomes a member
        * of a display list share group.  All members of the display
        * list share group have to have the same visual.
        */
        MFXGLCanvas(FXComposite* p, MFXGLVisual* vis, MFXGLCanvas* sharegroup, FXObject* tgt = NULL, FXSelector sel = 0, FXuint opts = 0, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0);

        /// Return TRUE if it is sharing display lists
        FXbool isShared() const;

        /// Create all of the server-side resources for this window
        void create();

        /// Detach the server-side resources for this window
        void detach();

        /// Destroy the server-side resources for this window
        void destroy();

        /// @brief overload to forward the window size to modern OpenGL (projection using the width/height)
        long onUpdate(FXObject* sender, FXSelector, void*);

        /// Make OpenGL context current prior to performing OpenGL commands
        FXbool makeCurrent();

        /// Make OpenGL context non current
        FXbool makeNonCurrent();

        /// Return TRUE if this window's context is current
        FXbool isCurrent() const;

        /// Return current context, if any
        static void* getCurrentContext();

        /// Get GL context handle
        void* getContext() const { return ctx; }

        /// Swap front and back buffer
        void swapBuffers();

        /// Save object to stream
        void save(FXStream& store) const;

        /// Load object from stream
        void load(FXStream& store);

        /// Destructor
        virtual ~MFXGLCanvas();

    };

/// @brief debug callback function to print the GL (error) message
/// taken from https://gist.github.com/liam-middlebrook/c52b069e4be2d87a6d2f
static void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length,
    const GLchar* msg, const void* data)
{
    char* _source;
    char* _type;
    char* _severity;

    switch (source) {
    case GL_DEBUG_SOURCE_API:
        _source = "API";
        break;

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        _source = "WINDOW SYSTEM";
        break;

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        _source = "SHADER COMPILER";
        break;

    case GL_DEBUG_SOURCE_THIRD_PARTY:
        _source = "THIRD PARTY";
        break;

    case GL_DEBUG_SOURCE_APPLICATION:
        _source = "APPLICATION";
        break;

    case GL_DEBUG_SOURCE_OTHER:
        _source = "UNKNOWN";
        break;

    default:
        _source = "UNKNOWN";
        break;
    }

    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        _type = "ERROR";
        break;

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        _type = "DEPRECATED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        _type = "UDEFINED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_PORTABILITY:
        _type = "PORTABILITY";
        break;

    case GL_DEBUG_TYPE_PERFORMANCE:
        _type = "PERFORMANCE";
        break;

    case GL_DEBUG_TYPE_OTHER:
        _type = "OTHER";
        break;

    case GL_DEBUG_TYPE_MARKER:
        _type = "MARKER";
        break;

    default:
        _type = "UNKNOWN";
        break;
    }

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        _severity = "HIGH";
        break;

    case GL_DEBUG_SEVERITY_MEDIUM:
        _severity = "MEDIUM";
        break;

    case GL_DEBUG_SEVERITY_LOW:
        _severity = "LOW";
        break;

    case GL_DEBUG_SEVERITY_NOTIFICATION:
        _severity = "NOTIFICATION";
        break;

    default:
        _severity = "UNKNOWN";
        break;
    }

    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, msg);

    //WRITE_MESSAGEF("%: % of % severity, raised from %: %\n", id, _type, _severity, _source, msg);
}