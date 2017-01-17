/****************************************************************************/
/// @file    GUILane.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Representation of a lane in the micro simulation (gui-version)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUILane_h
#define GUILane_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <string>
#include <utility>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <utils/foxtools/MFXMutex.h>
#include <utils/geom/Position.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/settings/GUIPropertySchemeStorage.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUINet;
class MSVehicle;
class MSNet;
#ifdef HAVE_OSG
namespace osg {
class Geometry;
}
#endif

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUILane
 * @brief Representation of a lane in the micro simulation (gui-version)
 *
 * An extended MSLane. A mechanism to avoid concurrent
 * visualisation and simulation what may cause problems when vehicles
 * disappear is implemented using a mutex.
 */
class GUILane : public MSLane, public GUIGlObject {
public:
    /** @brief Constructor
     *
     * @param[in] id The lane's id
     * @param[in] maxSpeed The speed allowed on this lane
     * @param[in] length The lane's length
     * @param[in] edge The edge this lane belongs to
     * @param[in] numericalID The numerical id of the lane
     * @param[in] shape The shape of the lane
     * @param[in] width The width of the lane
     * @param[in] permissions Encoding of vehicle classes that may drive on this lane
     * @see SUMOVehicleClass
     * @see MSLane
     */
    GUILane(const std::string& id, SUMOReal maxSpeed,
            SUMOReal length, MSEdge* const edge, int numericalID,
            const PositionVector& shape, SUMOReal width,
            SVCPermissions permissions, int index);


    /// @brief Destructor
    ~GUILane();

    /** @brief Returns the name of the parent object (if any)
     * @note Inherited from GUIGlObject
     * @return This object's parent id
     */
    const std::string& getParentName() const {
        return getEdge().getID();
    }


    /// @name Access to vehicles
    /// @{

    /** @brief Returns the vehicles container; locks it for microsimulation
     *
     * Locks "myLock" preventing usage by microsimulation.
     *
     * Please note that it is necessary to release the vehicles container
     *  afterwards using "releaseVehicles".
     * @return The vehicles on this lane
     * @see MSLane::getVehiclesSecure
     */
    const VehCont& getVehiclesSecure() const;


    /** @brief Allows to use the container for microsimulation again
     *
     * Unlocks "myLock" preventing usage by microsimulation.
     * @see MSLane::releaseVehicles
     */
    void releaseVehicles() const;
    /// @}



    /// @name Vehicle movement (longitudinal)
    /// @{

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    void planMovements(const SUMOTime t);

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    bool executeMovements(SUMOTime t, std::vector<MSLane*>& into);

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    bool integrateNewVehicle(SUMOTime t);
    ///@}


    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    void detectCollisions(SUMOTime timestep, const std::string& stage);


    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    MSVehicle* removeVehicle(MSVehicle* remVehicle, MSMoveReminder::Notification notification, bool notify);


    /** @brief Sets the information about a vehicle lapping into this lane
     *
     * This vehicle is added to myVehicles and may be distinguished from regular
     * vehicles by the disparity between this lane and v->getLane()
     * @param[in] v The vehicle which laps into this lane
     * @return This lane's length
     */
    SUMOReal setPartialOccupation(MSVehicle* v);

    /** @brief Removes the information about a vehicle lapping into this lane
     * @param[in] v The vehicle which laps into this lane
     */
    void resetPartialOccupation(MSVehicle* v);


    /// @name inherited from GUIGlObject
    //@{

    /** @brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app,
                                       GUISUMOAbstractView& parent);


    /** @brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app,
            GUISUMOAbstractView& parent);


    /** @brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    Boundary getCenteringBoundary() const;


    /** @brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;
    //@}



    const PositionVector& getShape() const;
    const std::vector<SUMOReal>& getShapeRotations() const;
    const std::vector<SUMOReal>& getShapeLengths() const;

    SUMOReal firstWaitingTime() const;

    /// @brief draw lane borders and white markings
    void drawMarkings(const GUIVisualizationSettings& s, SUMOReal scale) const;

    /// @brief bike lane markings on top of an intersection
    void drawBikeMarkings() const;

    /// @brief draw crossties for railroads or pedestrian crossings
    void drawCrossties(SUMOReal length, SUMOReal spacing, SUMOReal halfWidth) const;

    /// @brief direction indicators for lanes
    void drawDirectionIndicators() const;

    SUMOReal getEdgeLaneNumber() const;

    /** @brief Returns the stored traveltime for the edge of this lane
     */
    SUMOReal getStoredEdgeTravelTime() const;

    /** @brief Returns the loaded weight (effort) for the edge of this lane
     */
    SUMOReal getLoadedEdgeWeight() const;

#ifdef HAVE_OSG
    void setGeometry(osg::Geometry* geom) {
        myGeom = geom;
    }

    void updateColor(const GUIVisualizationSettings& s);

#endif

    /// @brief close this lane for traffic
    void closeTraffic(bool rebuildAllowed = true);

    bool isClosed() const {
        return myAmClosed;
    }

protected:
    /// moves myTmpVehicles int myVehicles after a lane change procedure
    void swapAfterLaneChange(SUMOTime t);

    /** @brief Inserts the vehicle into this lane, and informs it about entering the network
     *
     * Calls the vehicles enterLaneAtInsertion function,
     *  updates statistics and modifies the active state as needed
     * @param[in] veh The vehicle to be incorporated
     * @param[in] pos The position of the vehicle
     * @param[in] speed The speed of the vehicle
     * @param[in] posLat The lateral position of the vehicle
     * @param[in] at
     * @param[in] notification The cause of insertion (i.e. departure, teleport, parking) defaults to departure
     * @see MSLane::incorporateVehicle
     */
    virtual void incorporateVehicle(MSVehicle* veh, SUMOReal pos, SUMOReal speed, SUMOReal posLat,
                                    const MSLane::VehCont::iterator& at,
                                    MSMoveReminder::Notification notification = MSMoveReminder::NOTIFICATION_DEPARTED);

private:
    /// @brief helper methods
    void drawLinkNo(const GUIVisualizationSettings& s) const;
    void drawTLSLinkNo(const GUIVisualizationSettings& s, const GUINet& net) const;
    void drawTextAtEnd(const std::string& text, const PositionVector& shape, SUMOReal x, const GUIVisualizationTextSettings& settings) const;
    void drawLinkRules(const GUIVisualizationSettings& s, const GUINet& net) const;
    void drawLinkRule(const GUIVisualizationSettings& s, const GUINet& net, MSLink* link, const PositionVector& shape, SUMOReal x1, SUMOReal x2) const;
    void drawArrows() const;
    void drawLane2LaneConnections() const;


    /// @brief add intermediate points at segment borders
    PositionVector splitAtSegments(const PositionVector& shape);

private:
    /// @brief gets the color value according to the current scheme index
    SUMOReal getColorValue(int activeScheme) const;

    /// @brief gets the scaling value according to the current scheme index
    SUMOReal getScaleValue(int activeScheme) const;

    /// @brief sets the color according to the current scheme index and some lane function
    bool setFunctionalColor(int activeScheme) const;

    /// @brief sets multiple colors according to the current scheme index and some lane function
    bool setMultiColor(const GUIColorer& c) const;

    /// @brief sets the color according to the currente settings
    void setColor(const GUIVisualizationSettings& s) const;

    /// @brief whether to draw this lane as a railway
    bool drawAsRailway(const GUIVisualizationSettings& s) const;

    /// @brief whether to draw this lane as a waterway
    bool drawAsWaterway(const GUIVisualizationSettings& s) const;

    /// The rotations of the shape parts
    std::vector<SUMOReal> myShapeRotations;

    /// The lengths of the shape parts
    std::vector<SUMOReal> myShapeLengths;

    /// The color of the shape parts (cached)
    mutable std::vector<RGBColor> myShapeColors;

    /// @brief the meso segment index for each geometry segment
    std::vector<int> myShapeSegments;

    /// @brief Half of lane width, for speed-up
    SUMOReal myHalfLaneWidth;

    /// @brief Quarter of lane width, for speed-up
    SUMOReal myQuarterLaneWidth;

#ifdef HAVE_OSG
    osg::Geometry* myGeom;
#endif

    /// @brief state for dynamic lane closings
    bool myAmClosed;

private:
    /// The mutex used to avoid concurrent updates of the vehicle buffer
    mutable MFXMutex myLock;


};


#endif

/****************************************************************************/

