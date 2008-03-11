/****************************************************************************/
/// @file    GUILaneWrapper.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 25 Nov 2002
/// @version $Id$
///
// A MSLane extended for visualisation purposes.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUILaneWrapper_h
#define GUILaneWrapper_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/VectorHelper.h>
#include <string>
#include <utility>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/output/MSE2Collector.h>
#include <microsim/MSLink.h>
#include <utils/geom/Position2D.h>
#include <microsim/logging/LoggedValue_TimeFloating.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/drawer/GUILaneRepresentation.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicle;
class MSNet;
class GUINet;
class Position2DVector;
class GUIGLObjectPopupMenu;
class MSEdge;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUILaneWrapper
 * @brief A MSLane extended for visualisation purposes.
 *
 * The extensions consist of information about the position
 * of the lane and it's direction. Further, a mechanism to avoid concurrent
 * visualisation and simulation what may cause problems when vehicles
 * disappear is implemented using a mutex
 */
class GUILaneWrapper : public GUILaneRepresentation
{
public:
    /// constructor
    GUILaneWrapper(GUIGlObjectStorage &idStorage,
                   MSLane &lane, const Position2DVector &shape) throw();

    /// destructor
    virtual ~GUILaneWrapper() throw();

    /// @name inherited from GUIGlObject
    //@{

    /** @brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu *getPopUpMenu(GUIMainWindow &app,
                                       GUISUMOAbstractView &parent) throw();


    /** @brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow *getParameterWindow(GUIMainWindow &app,
            GUISUMOAbstractView &parent) throw();


    /** @brief Returns the id of the object as known to microsim
     *
     * @return The id of the lane
     * @see GUIGlObject::microsimID
     */
    const std::string &microsimID() const throw();

    /// Returns the type of the object as coded in GUIGlObjectType
    GUIGlObjectType getType() const throw();

    /** @brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    Boundary getCenteringBoundary() const throw();
    //@}

    /** returns the length of the lane */
    SUMOReal getLength() const;

    /** returns the "visualisation length"; this length may differ to the
        real length */
    SUMOReal visLength() const;

    const Position2DVector &getShape() const;
    const DoubleVector &getShapeRotations() const;
    const DoubleVector &getShapeLengths() const;

    /** returns the purpose (source, sink, normal) of the parent edge */
    MSEdge::EdgeBasicFunction getPurpose() const;

    /** returns the lane's maximum speed */
    SUMOReal maxSpeed() const;

    SUMOReal firstWaitingTime() const;
    SUMOReal getDensity() const;

    /// Returns the fastest known lane speed
    static SUMOReal getOverallMaxSpeed();

    /// Returns true if the given lane id the lane this wrapper wraps the geometry of
    bool forLane(const MSLane &lane) const;


    SUMOReal getEdgeEffort(SUMOTime t) const;

    const MSLane::VehCont &getVehiclesSecure();
    void releaseVehicles();


    /// @name access to a lane's links
    //@{

    /// Returns the number of links
    size_t getLinkNumber() const;

    /// Returns the state of the numbered link
    MSLink::LinkState getLinkState(size_t pos) const;

    /// Returns the direction of the numbered link
    MSLink::LinkDirection getLinkDirection(size_t pos) const;

    /// Returns the lane approached by the numbered link
    MSLane *getLinkLane(size_t pos) const;

    /// Returns the numbered link's respond (junction) index
    int getLinkRespondIndex(size_t pos) const;

    /// Returns the tl-logic's gl-id the numbered link is controlled by
    unsigned int getLinkTLID(const GUINet &net, size_t pos) const;

    /// Returns the numbered link's tls index
    int getLinkTLIndex(const GUINet &net, size_t pos) const;
    //@}


    const MSEdge * const getMSEdge() const;

    SUMOReal getEdgeLaneNumber() const;

    void selectSucessors();

    const MSLane &getLane() const {
        return myLane;
    }


protected:
    /** the visualisation length; As sources and sinks may be shorter/longer
        as their visualisation length, a second length information is necessary */
    SUMOReal myVisLength;

    /// The assigned lane
    MSLane &myLane;

    /// The shape of the lane
    const Position2DVector &myShape;

    /// The rotations of the shape parts
    DoubleVector myShapeRotations;

    /// The lengths of the shape parts
    DoubleVector myShapeLengths;

    /// The maximum velocity over all lanes
    static SUMOReal myAllMaxSpeed;

private:
    /// @brief Invalidated copy constructor.
    GUILaneWrapper(const GUILaneWrapper&);

    /// @brief Invalidated assignment operator.
    GUILaneWrapper& operator=(const GUILaneWrapper&);

};


#endif

/****************************************************************************/

