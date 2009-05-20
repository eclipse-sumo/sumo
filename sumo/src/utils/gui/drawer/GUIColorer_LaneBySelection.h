/****************************************************************************/
/// @file    GUIColorer_LaneBySelection.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//
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
#ifndef GUIColorer_LaneBySelection_h
#define GUIColorer_LaneBySelection_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GUIBaseColorer.h"
#include <utils/common/RGBColor.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <guisim/GUIEdge.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
template<class T>
class GUIColorer_LaneBySelection : public GUIBaseColorer<T> {
public:
    GUIColorer_LaneBySelection() { }

    virtual ~GUIColorer_LaneBySelection() { }

    void setGlColor(const T& i) const {
#ifdef HAVE_MESOSIM
        if (gSelected.isSelected(i.getType(), i.getGlID())) {
#else
        const GUIEdge *e = static_cast<const GUIEdge * const>(i.getMSEdge());
        if (gSelected.isSelected(i.getType(), i.getGlID()) || gSelected.isSelected(e->getType(), e->getGlID())) {
#endif
            glColor3f(0, .4f, .8f);
        } else {
            glColor3f(0.7f, 0.7f, 0.7f);
        }
    }

    virtual ColorSetType getSetType() const {
        return CST_SET;
    }

};


#endif

/****************************************************************************/

