/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GUILane.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
///
// Representation of a lane in the micro simulation (gui-version)
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/fxheader.h>
#include <string>
#include <utility>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
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
class TesselatedPolygon;
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
     * @param[in] friction The initial friction on this lane
     * @param[in] length The lane's length
     * @param[in] edge The edge this lane belongs to
     * @param[in] numericalID The numerical id of the lane
     * @param[in] shape The shape of the lane
     * @param[in] width The width of the lane
     * @param[in] permissions Encoding of vehicle classes that may drive on this lane
     * @see SUMOVehicleClass
     * @see MSLane
     */
    GUILane(const std::string& id, double maxSpeed, double friction,
            double length, MSEdge* const edge, int numericalID,
            const PositionVector& shape, double width,
            SVCPermissions permissions,
            SVCPermissions changeLeft, SVCPermissions changeRight,
            int index, bool isRampAccel,
            const std::string& type,
            const PositionVector& outlineShape);


    /// @brief Destructor
    ~GUILane();

    /** @brief Returns the name of the parent object (if any)
     * @note Inherited from GUIGlObject
     * @return This object's parent id
     */
    std::string getParentName() const override {
        return getEdge().getID();
    }

    void addSecondaryShape(const PositionVector& shape) override;

    double getLengthGeometryFactor(bool secondaryShape) const override {
        return secondaryShape ? myLengthGeometryFactor2 :  myLengthGeometryFactor;
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
    const VehCont& getVehiclesSecure() const override;


    /** @brief Allows to use the container for microsimulation again
     *
     * Unlocks "myLock" preventing usage by microsimulation.
     * @see MSLane::releaseVehicles
     */
    void releaseVehicles() const override;
    /// @}



    /// @name Vehicle movement (longitudinal)
    /// @{

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    void planMovements(const SUMOTime t) override;

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    void setJunctionApproaches(const SUMOTime t) const override;

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    void executeMovements(const SUMOTime t) override;

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    void integrateNewVehicles() override;
    ///@}


    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    void detectCollisions(SUMOTime timestep, const std::string& stage) override;


    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    MSVehicle* removeVehicle(MSVehicle* remVehicle, MSMoveReminder::Notification notification, bool notify) override;

    /// @brief remove parking vehicle
    void removeParking(MSBaseVehicle* veh) override;

    /** @brief Sets the information about a vehicle lapping into this lane
     *
     * This vehicle is added to myVehicles and may be distinguished from regular
     * vehicles by the disparity between this lane and v->getLane()
     * @param[in] v The vehicle which laps into this lane
     * @return This lane's length
     */
    double setPartialOccupation(MSVehicle* v) override;

    /** @brief Removes the information about a vehicle lapping into this lane
     * @param[in] v The vehicle which laps into this lane
     */
    void resetPartialOccupation(MSVehicle* v) override;

    /// @name inherited from GUIGlObject
    //@{

    /** @brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) override;

    /** @brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent) override;

    /** @brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    Boundary getCenteringBoundary() const override;

    /** @brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const override;

    double getClickPriority() const override;
    //@}

    const PositionVector& getShape(bool secondary) const override;
    const std::vector<double>& getShapeRotations(bool secondary) const;
    const std::vector<double>& getShapeLengths(bool secondary) const;

    double firstWaitingTime() const;

    /// @brief whether any of the neighboring lanes is not a bidi-lane
    bool neighLaneNotBidi() const;

    /// @brief draw lane borders and white markings
    void drawMarkings(const GUIVisualizationSettings& s, double scale) const;

    /// @brief bike lane markings on top of an intersection
    void drawBikeMarkings() const;

    /// @brief bike lane markings on top of an intersection
    void drawJunctionChangeProhibitions() const;

    /// @brief direction indicators for lanes
    void drawDirectionIndicators(double exaggeration, bool spreadSuperposed, bool s2) const;

    /// @brief draw intersection positions of foe internal lanes with this one
    void debugDrawFoeIntersections() const;

    double getEdgeLaneNumber() const;

    /** @brief Returns the stored traveltime for the edge of this lane
     */
    double getStoredEdgeTravelTime() const;

    /** @brief Returns the loaded weight (effort) for the edge of this lane
     */
    double getLoadedEdgeWeight() const;

    void setReachability(double value) {
        myReachability = value;
    }

    double getReachability() const {
        return myReachability;
    }

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

    /// @brief gets the color value according to the current scheme index
    double getColorValue(const GUIVisualizationSettings& s, int activeScheme) const override;

    /// @brief gets the color value according to the current scheme index including values for things that set the color indirectly
    double getColorValueWithFunctional(const GUIVisualizationSettings& s, int activeScheme) const;

    /// @brief return color value based on cached settings
    double getColorValueForTracker() const;

    /// @brief gets the scaling value according to the current scheme index
    double getScaleValue(const GUIVisualizationSettings& s, int activeScheme, bool s2) const;

    /// @brief whether this lane is selected in the GUI
    bool isSelected() const override;

    /* @brief sets the color according to the current scheme index and some lane function
     * @param[in] id override active scheme when calling from meso gui
     */
    bool setFunctionalColor(const GUIColorer& c, RGBColor& col, int activeScheme = -1) const;

    /// @brief whether to draw this lane as a railway
    bool drawAsRailway(const GUIVisualizationSettings& s) const;

protected:
    /// moves myTmpVehicles int myVehicles after a lane change procedure
    void swapAfterLaneChange(SUMOTime t) override;

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
    void incorporateVehicle(MSVehicle* veh, double pos, double speed, double posLat,
                            const MSLane::VehCont::iterator& at,
                            MSMoveReminder::Notification notification = MSMoveReminder::NOTIFICATION_DEPARTED) override;

private:
    /// @brief helper methods
    void drawLinkNo(const GUIVisualizationSettings& s) const;
    void drawTLSLinkNo(const GUIVisualizationSettings& s, const GUINet& net) const;
    void drawLinkRules(const GUIVisualizationSettings& s, const GUINet& net) const;
    void drawLinkRule(const GUIVisualizationSettings& s, const GUINet& net, const MSLink* link,
                      const PositionVector& shape, double x1, double x2) const;
    void drawArrows(bool secondaryShape) const;
    void drawLane2LaneConnections(double exaggeration, bool s2) const;


    /// @brief add intermediate points at segment borders
    PositionVector splitAtSegments(const PositionVector& shape);

    /// @brief get number of vehicles waiting for departure on this lane
    double getPendingEmits() const;

private:
    void initRotations(const PositionVector& shape,
                       std::vector<double>& rotations,
                       std::vector<double>& lengths,
                       std::vector<RGBColor>& colors);

    /// @brief sets multiple colors according to the current scheme index and some lane function
    bool setMultiColor(const GUIVisualizationSettings& s, const GUIColorer& c, RGBColor& col) const;

    /// @brief sets the color according to the currente settings
    RGBColor setColor(const GUIVisualizationSettings& s) const;

    /// @brief whether to draw this lane as a waterway
    bool drawAsWaterway(const GUIVisualizationSettings& s) const;

    /// @brief whether this lane or its parent edge is selected in the GUI
    bool isLaneOrEdgeSelected() const;

    std::vector<RGBColor>& getShapeColors(bool secondary) const;

    /// The rotations of the shape parts
    std::vector<double> myShapeRotations;
    std::vector<double> myShapeRotations2;

    /// The lengths of the shape parts
    std::vector<double> myShapeLengths;
    std::vector<double> myShapeLengths2;

    /// The color of the shape parts (cached)
    mutable std::vector<RGBColor> myShapeColors;
    mutable std::vector<RGBColor> myShapeColors2;

    /// @brief the meso segment index for each geometry segment
    std::vector<int> myShapeSegments;
    /// @brief the shape indices where the meso segment changes (for segmentsIndex > 0)
    std::vector<int> mySegmentStartIndex;

    /// @brief Half of lane width, for speed-up
    double myHalfLaneWidth;

    /// @brief Quarter of lane width, for speed-up
    double myQuarterLaneWidth;

    /// @brief the time distance from a particular edge
    double myReachability = INVALID_DOUBLE;

    /// @brief list of parkingAreas on this lane
    mutable std::vector<MSParkingArea*>* myParkingAreas;

    /// @brief An object that stores the tesselation
    mutable TesselatedPolygon* myTesselation;

#ifdef HAVE_OSG
    osg::Geometry* myGeom;
#endif

    /// @brief state for dynamic lane closings
    bool myAmClosed;

    /// @brief secondary shape for visualization
    PositionVector myShape2;
    double myLengthGeometryFactor2;

    /// @brief cached for tracking color value
    static GUIVisualizationSettings* myCachedGUISettings;

private:
    /// The mutex used to avoid concurrent updates of the vehicle buffer
    mutable FXMutex myLock;

    /// @brief special color to signify alternative coloring scheme
    static const RGBColor MESO_USE_LANE_COLOR;


};
