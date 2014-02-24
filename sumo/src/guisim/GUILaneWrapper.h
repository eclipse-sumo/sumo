/****************************************************************************/
/// @file    GUILaneWrapper.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 25 Nov 2002
/// @version $Id$
///
// A MSLane extended for visualisation purposes.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#include <string>
#include <utility>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/settings/GUIColorer.h>
#include <utils/common/VectorHelper.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUINet;
class GUIGLObjectPopupMenu;
class MSLane;
class MSEdge;
class PositionVector;
#ifdef HAVE_OSG
namespace osg {
class Geometry;
}
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUILaneWrapper
 * @brief A MSLane extended for visualisation purposes.
 *
 * The extensions consist of information about the position of the lane and
 *  it's direction. Further, a mechanism to avoid concurrent visualisation
 *  and simulation what may cause problems when vehicles disappear is
 *  implemented using a mutex.
 */
class GUILaneWrapper : public GUIGlObject {
public:
    /** @brief Constructor
     * @param[in] lane The lane to be represented
     * @param[in] shape The shape of the lane (!!! this is a member of the lane - no need to give it additionally)
     */
    GUILaneWrapper(MSLane& lane, const PositionVector& shape, unsigned int index);


    /// @brief Destructor
    virtual ~GUILaneWrapper();



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



    /** @brief Returns the represented lane
     * @return The lane represented by this wrapper
     */
    const MSLane& getLane() const {
        return myLane;
    }


    const PositionVector& getShape() const;
    const std::vector<SUMOReal>& getShapeRotations() const;
    const std::vector<SUMOReal>& getShapeLengths() const;

    SUMOReal firstWaitingTime() const;


    /// Returns true if the given lane id the lane this wrapper wraps the geometry of
    bool forLane(const MSLane& lane) const;


    /// @brief draw lane borders and white markings
    void drawMarkings(const GUIVisualizationSettings& s) const;

    /// @brief draw crossties for railroads
    void drawCrossties(const GUIVisualizationSettings& s) const;

    /// Returns the number of links
    unsigned int getLinkNumber() const;

    SUMOReal getHalfWidth() const {
        return myHalfLaneWidth;
    }


    /// @name Current state retrieval
    //@{

    /** @brief Returns the sum of last step CO2 emissions normed by the lane's length
     * @return CO2 emissions of vehicles on this lane during the last step, normed by the lane length
     */
    SUMOReal getNormedHBEFA_CO2Emissions() const;


    /** @brief Returns the sum of last step CO emissions normed by the lane's length
     * @return CO emissions of vehicles on this lane during the last step, normed by the lane length
     */
    SUMOReal getNormedHBEFA_COEmissions() const;


    /** @brief Returns the sum of last step PMx emissions normed by the lane's length
     * @return PMx emissions of vehicles on this lane during the last step, normed by the lane length
     */
    SUMOReal getNormedHBEFA_PMxEmissions() const;


    /** @brief Returns the sum of last step NOx emissions normed by the lane's length
     * @return NOx emissions of vehicles on this lane during the last step, normed by the lane length
     */
    SUMOReal getNormedHBEFA_NOxEmissions() const;


    /** @brief Returns the sum of last step HC emissions normed by the lane's length
     * @return HC emissions of vehicles on this lane during the last step, normed by the lane length
     */
    SUMOReal getNormedHBEFA_HCEmissions() const;


    /** @brief Returns the sum of last step fuel comsumption normed by the lane's length
     * @return Fuel comsumption of vehicles on this lane during the last step, normed by the lane length
     */
    SUMOReal getNormedHBEFA_FuelConsumption() const;

	

    /** @brief Returns the sum of last step CO2 emissions normed by the lane's length
     * @return CO2 emissions of vehicles on this lane during the last step, normed by the lane length
     */
    SUMOReal getNormedPHEMlight_CO2Emissions() const;


    /** @brief Returns the sum of last step CO emissions normed by the lane's length
     * @return CO emissions of vehicles on this lane during the last step, normed by the lane length
     */
    SUMOReal getNormedPHEMlight_COEmissions() const;


    /** @brief Returns the sum of last step PMx emissions normed by the lane's length
     * @return PMx emissions of vehicles on this lane during the last step, normed by the lane length
     */
    SUMOReal getNormedPHEMlight_PMxEmissions() const;


    /** @brief Returns the sum of last step NOx emissions normed by the lane's length
     * @return NOx emissions of vehicles on this lane during the last step, normed by the lane length
     */
    SUMOReal getNormedPHEMlight_NOxEmissions() const;


    /** @brief Returns the sum of last step HC emissions normed by the lane's length
     * @return HC emissions of vehicles on this lane during the last step, normed by the lane length
     */
    SUMOReal getNormedPHEMlight_HCEmissions() const;


    /** @brief Returns the sum of last step fuel comsumption normed by the lane's length
     * @return Fuel comsumption of vehicles on this lane during the last step, normed by the lane length
     */
    SUMOReal getNormedPHEMlight_FuelConsumption() const;
	/// @}


    SUMOReal getEdgeLaneNumber() const;

    /** @brief Returns the stored traveltime for the edge of this lane
     */
    SUMOReal getStoredEdgeTravelTime() const;

#ifdef HAVE_OSG
    void setGeometry(osg::Geometry* geom) {
        myGeom = geom;
    }

    void updateColor(const GUIVisualizationSettings& s);
#endif

private:
    /// @brief helper methods
    void drawLinkNo() const;
    void drawTLSLinkNo(const GUINet& net) const;
    void drawLinkRules(const GUINet& net) const;
    void drawArrows() const;
    void drawLane2LaneConnections() const;

    /// @brief gets the color value according to the current scheme index
    SUMOReal getColorValue(size_t activeScheme) const;

    /// @brief sets the color according to the currente settings
    void setColor(const GUIVisualizationSettings& s) const;

private:
    /// The assigned lane
    MSLane& myLane;

    /// The shape of the lane
    const PositionVector& myShape;

    /// The rotations of the shape parts
    std::vector<SUMOReal> myShapeRotations;

    /// The lengths of the shape parts
    std::vector<SUMOReal> myShapeLengths;

    /// @brief Half of lane width, for speed-up
    SUMOReal myHalfLaneWidth;

    /// @brief Quarter of lane width, for speed-up
    SUMOReal myQuarterLaneWidth;

    /// The lane index
    unsigned int myIndex;

#ifdef HAVE_OSG
    osg::Geometry* myGeom;
#endif

private:
    /// @brief Invalidated copy constructor.
    GUILaneWrapper(const GUILaneWrapper&);

    /// @brief Invalidated assignment operator.
    GUILaneWrapper& operator=(const GUILaneWrapper&);
};


#endif

/****************************************************************************/

