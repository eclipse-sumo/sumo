#ifndef GUIGLObjectToolTip_h
#define GUIGLObjectToolTip_h
//---------------------------------------------------------------------------//
//                        GUIGLObjectToolTip.h -
//  A tooltip floating over a window
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.1  2004/11/23 10:38:31  dkrajzew
// debugging
//
// Revision 1.2  2004/10/29 06:01:55  dksumo
// renamed boundery to boundary
//
// Revision 1.1  2004/10/22 12:50:50  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.1  2004/03/19 12:56:11  dkrajzew
// porting to FOX
//
// Revision 1.6  2003/06/05 11:37:31  dkrajzew
// class templates applied
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <fx.h>
#include <utils/geom/Boundary.h>
#include <utils/foxtools/FXMutex.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIGlObject;


/* =========================================================================
 * class definitions
 * ======================================================================= */
class GUIGLObjectToolTip : public FXToolTip
{
    FXDECLARE(GUIGLObjectToolTip)
public:
    /// Constructor
    GUIGLObjectToolTip(FXWindow *a);

    /// Destructor
    ~GUIGLObjectToolTip();

    /// an abstract view may set or erase the tooltip
    friend class GUISUMOAbstractView;
    friend class GUIRouterSUMOAbstractView;

    long onPaint(FXObject*,FXSelector,void* ptr);

    FXint getDefaultWidth();

    FXint getDefaultHeight();

private:
    /// sets the tooltip displaying information about the given object at the given position
    void setObjectTip(GUIGlObject *object,
        size_t x, size_t y);

    /// removes the tooltip (undisplays it)
    void eraseTip();

private:
    /// The parent window
    GUISUMOAbstractView *myParent;

    /// The last position of the tooltip
    int myLastX, myLastY;

    /// A lock to avoid parallel setting and removing
    FXEX::FXMutex *_lock;

    /// The height of the displayed text
    int _textHeight;

    FXFont *myFont;
    GUIGlObject *_object;

    FXint _width, _height;


protected:
    GUIGLObjectToolTip() { }
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

