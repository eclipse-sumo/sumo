#ifndef GUIColorer_LaneNeighEdges_h
#define GUIColorer_LaneNeighEdges_h
//---------------------------------------------------------------------------//
//                        GUIColorer_LaneNeighEdges.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Fri, 29.04.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
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
// Revision 1.2  2006/11/02 14:16:51  behrisch
// Missing includes
//
// Revision 1.1  2006/10/12 07:50:42  dkrajzew
// c2c visualisation added (unfinished)
//
// Revision 1.8  2006/07/06 06:31:37  dkrajzew
// removed exception throwing for unreachable code
//
// Revision 1.7  2006/01/09 11:50:20  dkrajzew
// new visualization settings implemented
//
// Revision 1.6  2005/10/07 11:36:47  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.5  2005/09/22 13:30:40  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.4  2005/09/15 11:05:28  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/07/12 11:57:55  dkrajzew
// level 3 warnings removed; code style adapted
//
// Revision 1.3  2005/06/14 11:14:23  dksumo
// documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <utils/gui/drawer/GUIBaseColorer.h>
#include <utils/gfx/RGBColor.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <microsim/MSEdge.h>
#include <guisim/GUILaneWrapper.h>
#include <GL/gl.h>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
template<class _T>
class GUIColorer_LaneNeighEdges : public GUIBaseColorer<_T> {
public:
	GUIColorer_LaneNeighEdges(GUIViewTraffic *parent) : myParent(parent) { }

    virtual ~GUIColorer_LaneNeighEdges() { }

	void setGlColor(const _T& i) const {
        int id = myParent->getTrackedID();
        if(id>0) {
            GUIVehicle *o = static_cast<GUIVehicle*>(gIDStorage.getObjectBlocking(id));
            if(o==0) {
                glColor3d(0,0,0);
                return;
            }
            const MSEdge *vehicleEdge = o->getLane().getEdge();
            const std::map<std::string, MSEdge*> &neigh = vehicleEdge->getNeighborEdges();
            if(neigh.find(i.getMSEdge()->getID())!=neigh.end()) {
                glColor3d(0,1,0);
            } else {
                glColor3d(.4,0,.4);
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

    GUIViewTraffic *myParent;
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
