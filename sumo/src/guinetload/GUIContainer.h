#ifndef GUIContainer_h
#define GUIContainer_h
//---------------------------------------------------------------------------//
//                        GUIContainer.h -
//  A loading container derived from NLContainer with additional values
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
// Revision 1.9  2005/05/04 07:55:28  dkrajzew
// added the possibility to load lane geometries into the non-gui simulation; simulation speedup due to avoiding multiplication with 1;
//
// Revision 1.8  2004/07/02 08:38:51  dkrajzew
// changes needed to implement the online-router (class derivation)
//
// Revision 1.7  2004/04/02 11:14:36  dkrajzew
// extended traffic lights are no longer template classes
//
// Revision 1.6  2004/01/26 06:49:06  dkrajzew
// work on detectors: e3-detectors loading and visualisation; variable offsets and lengths for lsa-detectors; coupling of detectors to tl-logics
//
// Revision 1.5  2003/07/22 14:58:33  dkrajzew
// changes due to new detector handling
//
// Revision 1.4  2003/07/07 08:13:15  dkrajzew
// first steps towards the usage of a real lane and junction geometry implemented
//
// Revision 1.3  2003/02/07 10:38:19  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <netload/NLContainer.h>
#include <utils/geom/Position2DVector.h>
#include <microsim/MSNet.h>
#include <string>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUINet;
class NLEdgeControlBuilder;
class OptionsCont;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUIContainer
 * As some artifacts used by the simulation differ between the gui- and the
 * normal version, a special builder is needed for both. Most artifacts used
 * by the gui-enabled simulation are simple inheritances from their normal
 * versions, so this container only overrides some methods, especially those
 * that use constructors. Here, instead of MS* classes, GUI* classes are
 * being build.
 */
class GUIContainer : public NLContainer
{
public:
    /// constructor
    GUIContainer(NLEdgeControlBuilder&edgeBuilder,
        NLJunctionControlBuilder &junctionBuilder);

    /// destructor
    ~GUIContainer();

    /// builds a GUI-net after the reading of all artifacts
    GUINet *buildGUINet(NLDetectorBuilder &db,
        const OptionsCont &options);

    /// adds information about the source and the destination junction
    void addSrcDestInfo(const std::string &id, const std::string &from,
        const std::string &to);

	    /// sets the shape of the current junction
    void addJunctionShape(const Position2DVector &shape);

    /// adds a lane
    //void addLane(const std::string &id, const bool isDepartLane,
      //  const float maxSpeed, const float length, const float changeUrge);


    //void closeLane();

protected:
    virtual MSRouteLoader *buildRouteLoader(const std::string &file);


};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

