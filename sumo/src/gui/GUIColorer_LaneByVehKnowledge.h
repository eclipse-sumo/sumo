/****************************************************************************/
/// @file    GUIColorer_LaneByVehKnowledge.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//	»missingDescription«
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
#ifndef GUIColorer_LaneByVehKnowledge_h
#define GUIColorer_LaneByVehKnowledge_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/gui/drawer/GUIBaseColorer.h>
#include <utils/common/RGBColor.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <microsim/MSEdge.h>
#include <guisim/GUILaneWrapper.h>
#include <guisim/GUIVehicle.h>
#include <GL/gl.h>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIColorer_LaneByVehKnowledge
 */
template<class T>
class GUIColorer_LaneByVehKnowledge : public GUIBaseColorer<T> {
public:
    GUIColorer_LaneByVehKnowledge(GUISUMOAbstractView *parent) : myParent(parent) { }

    virtual ~GUIColorer_LaneByVehKnowledge() { }

    void setGlColor(const T& i) const {
        int id = myParent->getTrackedID();
        if (id>0) {
            GUIVehicle *o = static_cast<GUIVehicle*>(gIDStorage.getObjectBlocking(id));
            if (o==0) {
                glColor3d(0,0,0);
                return;
            }
            if (o->knowsEdgeTest((MSEdge&) *i.getMSEdge()))  {
                glColor3d(0,0,0);
            } else {
                glColor3d(1,.8,.8);
            }
        } else {
            glColor3d(0,0,0);
        }
        return;
    }

    void setGlColor(SUMOReal val) const {
        glColor3d(val, val, val);
    }

    virtual ColorSetType getSetType() const {
        return CST_STATIC; // !!! (should be "set")
    }

    GUISUMOAbstractView *myParent;
};


#endif

/****************************************************************************/

