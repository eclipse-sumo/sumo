#ifndef GUILaneWrapper_h
#define GUILaneWrapper_h
//---------------------------------------------------------------------------//
//                        GUILaneWrapper.h -
//  Holds geometrical values for a lane
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 25 Nov 2002
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
// Revision 1.15  2004/07/02 08:54:11  dkrajzew
// some design issues
//
// Revision 1.14  2004/03/19 12:57:54  dkrajzew
// porting to FOX
//
// Revision 1.13  2003/11/12 14:01:54  dkrajzew
// visualisation of tl-logics added
//
// Revision 1.12  2003/11/11 08:11:05  dkrajzew
// logging (value passing) moved from utils to microsim
//
// Revision 1.11  2003/09/05 14:59:54  dkrajzew
// first tries for an implementation of aggregated views
//
// Revision 1.10  2003/08/21 12:50:49  dkrajzew
// retrival of a links direction added
//
// Revision 1.9  2003/07/30 08:54:14  dkrajzew
// the network is capable to display the networks state, now
//
// Revision 1.8  2003/07/22 14:59:27  dkrajzew
// changes due to new detector handling
//
// Revision 1.7  2003/07/07 08:14:48  dkrajzew
// first steps towards the usage of a real lane and junction geometry implemented
//
// Revision 1.6  2003/06/05 06:29:50  dkrajzew
// first tries to build under linux: warnings removed; moc-files included Makefiles added
//
// Revision 1.5  2003/05/20 09:26:57  dkrajzew
// data retrieval for new views added
//
// Revision 1.4  2003/04/14 08:27:17  dkrajzew
// new globject concept implemented
//
// Revision 1.3  2003/03/20 16:19:28  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.2  2003/03/12 16:52:06  dkrajzew
// centering of objects debuggt
//
// Revision 1.1  2003/02/07 10:39:17  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <utils/common/DoubleVector.h>
#include <string>
#include <utility>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSE2Collector.h>
#include <microsim/MSLink.h>
#include <utils/geom/Position2D.h>
#include <utils/foxtools/FXMutex.h>
#include <microsim/logging/LoggedValue_TimeFloating.h>
#include <gui/GUIGlObject.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSVehicle;
class MSNet;
class GUINet;
class Position2DVector;
class GUIGLObjectPopupMenu;
class MSEdge;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUILaneWrapper
 * An extended MSLane. The extensions consist of information about the position
 * of the lane and it's direction. Further, a mechanism to avoid concurrent
 * visualisation and simulation what may cause problems when vehicles
 * disappear is implemented using a mutex
 */
class GUILaneWrapper :
            public GUIGlObject {
public:
    /// constructor
    GUILaneWrapper( GUIGlObjectStorage &idStorage,
        MSLane &lane, const Position2DVector &shape);

    /// destructor
    virtual ~GUILaneWrapper();

    /// Returns a popup-menu fpr lanes
    GUIGLObjectPopupMenu *getPopUpMenu(GUIApplicationWindow &app,
        GUISUMOAbstractView &parent);

    GUIParameterTableWindow *getParameterWindow(
        GUIApplicationWindow &app, GUISUMOAbstractView &parent);

    /// returns the id of the object as known to microsim
    std::string microsimID() const;

    /** returns the length of the lane */
    double getLength() const;

    /** returns the begin position of the lane */
    const Position2D &getBegin() const;

    /** returns the end position of the lane */
    const Position2D &getEnd() const;

    /** returns the direction of the lane (!!! describe better) */
    const Position2D &getDirection() const;

    /** returns the rotation of the lane (!!! describe better) */
    double getRotation() const;

    /** returns the "visualisation length"; this length may differ to the
        real length */
    double visLength() const;

    /** returns the purpose (source, sink, normal) of the parent edge */
    MSEdge::EdgeBasicFunction getPurpose() const;

    /** returns the lane's maximum speed */
    double maxSpeed() const;

    /// Returns the fastest known lane speed
    static double getOverallMaxSpeed();

    /// Returns true if the given lane id the lane this wrapper wraps the geometry of
    bool forLane(const MSLane &lane) const;

    /// Returns the type of the object as coded in GUIGlObjectType
    GUIGlObjectType getType() const;

    const Position2DVector &getShape() const;
    const DoubleVector &getShapeRotations() const;
    const DoubleVector &getShapeLengths() const;

    const MSLane::VehCont &getVehiclesSecure();
    void releaseVehicles();

    size_t getLinkNumber() const;

    /// Returns the state of the numbered link
    MSLink::LinkState getLinkState(size_t pos) const;

    /// Returns the direction of the numbered link
    MSLink::LinkDirection getLinkDirection(size_t pos) const;

    /// Returns the tl-logic the numbered link is controlled by
    unsigned int getLinkTLID(const GUINet &net, size_t pos) const;

    double getAggregatedNormed(E2::DetType what,
        size_t aggregationPosition) const;

    double getAggregatedFloat(E2::DetType what) const;

    const MSEdge& getMSEdge() const;


protected:
    bool active() const { return true; }

private:
    void buildAggregatedValuesStorage();

protected:
    /// the begin position of the lane
    Position2D _begin;

    /// the end position of the lane
    Position2D _end;

    /// the direction of the lane
    Position2D _direction;

    /// the direction of the lane
    double _rotation;

    /** the visualisation length; As sources and sinks may be shorter/longer
        as their visualisation length, a second length information is necessary */
    double _visLength;

    /// The assigned lane
    MSLane &myLane;

    /// The shape of the lane
    const Position2DVector &myShape;

    /// The rotations of the shape parts
    DoubleVector myShapeRotations;

    /// The lengths of the shape parts
    DoubleVector myShapeLengths;

    /// The maximum velocity over all lanes
    static double myAllMaxSpeed;

    LoggedValue_TimeFloating<double> **myAggregatedValues;

    float myAggregatedFloats[3];

    static size_t myAggregationSizes[];



};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

