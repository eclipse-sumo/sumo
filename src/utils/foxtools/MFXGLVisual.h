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
// GL visual ready for OpenGL3.3
/****************************************************************************/
#pragma once
#include <config.h>

#include "fxheader.h"
#ifdef WIN32
#define NOMINMAX
#endif
#include <fx3d.h>
#ifdef WIN32
#undef NOMINMAX
#endif

/// Visual describes pixel format of a drawable
class MFXGLVisual : public FXGLVisual {
    FXDECLARE(MFXGLVisual)
    friend class FX::FXWindow;
    friend class FX::FXImage;
    friend class FX::FXIcon;
    friend class FX::FXBitmap;
    friend class FX::FXDCWindow;
    friend class MFXGLCanvas;
protected:
    FXint        redSize;             // Desired #bits for red
    FXint        greenSize;           // Desired #bits for green
    FXint        blueSize;            // Desired #bits for blue
    FXint        alphaSize;           // Desired #bits for alpha
    FXint        depthSize;           // Desired #bits for Z
    FXint        stencilSize;         // Desired #bits for stencil
    FXint        accumRedSize;        // Desired #bits for accum red
    FXint        accumGreenSize;      // Desired #bits for accum green
    FXint        accumBlueSize;       // Desired #bits for accum blue
    FXint        accumAlphaSize;      // Desired #bits for accum alpha
protected:
    MFXGLVisual();
private:
    MFXGLVisual(const MFXGLVisual&);
    MFXGLVisual&operator=(const MFXGLVisual&);
public:

    /// Construct default visual
    MFXGLVisual(FXApp* a,FXuint flags);

    /**
    * Test if OpenGL is possible, and what level is supported.
    * Because of remote display capability, the display server may
    * support a different level of OpenGL than the client; it may
    * even support no OpenGL at all!  This function returns the lesser
    * of the client support level and the display server support level.
    */
    static FXbool supported(FXApp* application,int& major,int& minor);

    /// Create visual
    virtual void create();

    /// Detach visual
    virtual void detach();

    /// Destroy visual
    virtual void destroy();

    /// Get sizes for bit-planes
    FXint getRedSize() const { return redSize; }
    FXint getGreenSize() const { return greenSize; }
    FXint getBlueSize() const { return blueSize; }
    FXint getAlphaSize() const { return alphaSize; }
    FXint getDepthSize() const { return depthSize; }
    FXint getStencilSize() const { return stencilSize; }
    FXint getAccumRedSize() const { return accumRedSize; }
    FXint getAccumGreenSize() const { return accumGreenSize; }
    FXint getAccumBlueSize() const { return accumBlueSize; }
    FXint getAccumAlphaSize() const { return accumAlphaSize; }

    /// Set sizes for bit-planes
    void setRedSize(FXint rs){ redSize=rs; }
    void setGreenSize(FXint gs){ greenSize=gs; }
    void setBlueSize(FXint bs){ blueSize=bs; }
    void setAlphaSize(FXint as){ alphaSize=as; }
    void setDepthSize(FXint ds){ depthSize=ds; }
    void setStencilSize(FXint ss){ stencilSize=ss; }
    void setAccumRedSize(FXint rs){ accumRedSize=rs; }
    void setAccumGreenSize(FXint gs){ accumGreenSize=gs; }
    void setAccumBlueSize(FXint bs){ accumBlueSize=bs; }
    void setAccumAlphaSize(FXint as){ accumAlphaSize=as; }

    /// Get ACTUAL sizes for bit-planes
    FXint getActualRedSize() const;
    FXint getActualGreenSize() const;
    FXint getActualBlueSize() const;
    FXint getActualAlphaSize() const;
    FXint getActualDepthSize() const;
    FXint getActualStencilSize() const;
    FXint getActualAccumRedSize() const;
    FXint getActualAccumGreenSize() const;
    FXint getActualAccumBlueSize() const;
    FXint getActualAccumAlphaSize() const;

    /// Is it double buffered?
    FXbool isDoubleBuffer() const;

    /// Is it stereo?
    FXbool isStereo() const;

    /// Is it hardware-accelerated?
    FXbool isAccelerated() const;

    /// Does it swap by copying instead of flipping buffers
    FXbool isBufferSwapCopy() const;

    /// Save visual info to a stream
    virtual void save(FXStream& store) const;

    /// Load visual info to a stream
    virtual void load(FXStream& store);

    /// Destructor
    virtual ~MFXGLVisual();
};


/// Create a display list of bitmaps from font glyphs in a font
extern FXAPI void glUseFXFont(FXFont* font,int first,int count,int list);