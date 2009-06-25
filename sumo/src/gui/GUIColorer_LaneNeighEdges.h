/****************************************************************************/
/// @file    GUIColorer_LaneNeighEdges.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// �missingDescription�
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
#ifndef GUIColorer_LaneNeighEdges_h
#define GUIColorer_LaneNeighEdges_h


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
#include <utils/gui/globjects/GUIGlObjectStorage.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <microsim/MSEdge.h>
#include <guisim/GUILaneWrapper.h>
#include <GL/gl.h>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIColorer_LaneNeighEdges
 */
template<class T>
class GUIColorer_LaneNeighEdges : public GUIBaseColorer<T> {
public:
    GUIColorer_LaneNeighEdges(GUISUMOAbstractView *parent) : myParent(parent) { }

    virtual ~GUIColorer_LaneNeighEdges() { }

    void setGlColor(const T& i) const {
        int id = myParent->getTrackedID();
        if (id>0) {
            GUIVehicle *o = static_cast<GUIVehicle*>(GUIGlObjectStorage::gIDStorage.getObjectBlocking(id));
            if (o==0) {
                glColor3d(0,0,0);
                return;
            }
            const MSEdge *vehicleEdge = o->getLane().getEdge();
//            const std::vector<MSEdge*> &neigh = vehicleEdge->getNeighborEdges();
//            if (find(neigh.begin(), neigh.end(), i.getMSEdge()) != neigh.end()) {
//                glColor3d(0,1,0);
//            } else {
            glColor3d(.4,0,.4);
//            }
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

