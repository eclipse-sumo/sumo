/****************************************************************************/
/// @file    GUIGLObjectToolTip.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The tooltip class for displaying information about gl-objects
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIGLObjectToolTip_h
#define GUIGLObjectToolTip_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <utils/geom/Boundary.h>
#include <string>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIGlObject;
class GUISUMOAbstractView;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIGLObjectToolTip
 * @brief The tooltip class for displaying information about gl-objects
 */
class GUIGLObjectToolTip : public FXToolTip {
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
    long onTipShow(FXObject*,FXSelector,void*);
    long onTipHide(FXObject*,FXSelector,void*);

    FXint getDefaultWidth();

    FXint getDefaultHeight();

private:
    /// sets the tooltip displaying information about the given object at the given position
    void setObjectTip(GUIGlObject *object,
                      FXint x, FXint y);

    /// removes the tooltip (undisplays it)
    void eraseTip();

private:
    /// The parent window
    GUISUMOAbstractView *myParent;

    /// The height of the displayed text
    int myTextHeight;

    /// The used font
    FXFont *myFont;

    /// The name of the object currently displayed
    std::string myObjectName;

    /// The current width and height
    FXint myWidth, myHeight;

    /// The last position of the tool tip
    FXint myLastXPos, myLastYPos;

    /// The object for which the tool tip is displayed
    GUIGlObject *myObject;

protected:
    /// FOX default constructor
    GUIGLObjectToolTip() { }


};


#endif

/****************************************************************************/

