/****************************************************************************/
/// @file    GUILaneWrapper.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 25 Nov 2002
/// @version $Id$
///
// A MSLane extended for visualisation purposes.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
class Position2DVector;


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
     * @param[in] idStorage The storage of gl-ids to get the one for this lane representation from
     * @param[in] lane The lane to be represented
     * @param[in] shape The shape of the lane (!!! this is a member of the lane - no need to give it additionally)
     */
    GUILaneWrapper(GUIGlObjectStorage &idStorage,
                   MSLane &lane, const Position2DVector &shape) throw();


    /// @brief Destructor
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
     * @see GUIGlObject::getMicrosimID
     */
    const std::string &getMicrosimID() const throw();


    /** @brief Returns the type of the object as coded in GUIGlObjectType
     *
     * @return GLO_LANE (is a lane)
     * @see GUIGlObject::getType
     * @see GUIGlObjectType
     */
    GUIGlObjectType getType() const throw() {
        return GLO_LANE;
    }


    /** @brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    Boundary getCenteringBoundary() const throw();


    /** @brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings &s) const throw();
    //@}



    /** @brief Returns the represented lane
     * @return The lane represented by this wrapper
     */
    const MSLane &getLane() const throw() {
        return myLane;
    }


    const Position2DVector &getShape() const;
    const DoubleVector &getShapeRotations() const;
    const DoubleVector &getShapeLengths() const;

    SUMOReal firstWaitingTime() const;


    /// Returns the fastest known lane speed
    static SUMOReal getOverallMaxSpeed();

    /// Returns true if the given lane id the lane this wrapper wraps the geometry of
    bool forLane(const MSLane &lane) const;


    void drawBordersGL(const GUIVisualizationSettings &s) const throw();


    /// Returns the number of links
    unsigned int getLinkNumber() const;


    /// @name Current state retrieval
    //@{

    /** @brief Returns the sum of last step CO2 emissions normed by the lane's length
     * @return CO2 emissions of vehicles on this lane during the last step, normed by the lane length
     */
    SUMOReal getNormedHBEFA_CO2Emissions() const throw();


    /** @brief Returns the sum of last step CO emissions normed by the lane's length
     * @return CO emissions of vehicles on this lane during the last step, normed by the lane length
     */
    SUMOReal getNormedHBEFA_COEmissions() const throw();


    /** @brief Returns the sum of last step PMx emissions normed by the lane's length
     * @return PMx emissions of vehicles on this lane during the last step, normed by the lane length
     */
    SUMOReal getNormedHBEFA_PMxEmissions() const throw();


    /** @brief Returns the sum of last step NOx emissions normed by the lane's length
     * @return NOx emissions of vehicles on this lane during the last step, normed by the lane length
     */
    SUMOReal getNormedHBEFA_NOxEmissions() const throw();


    /** @brief Returns the sum of last step HC emissions normed by the lane's length
     * @return HC emissions of vehicles on this lane during the last step, normed by the lane length
     */
    SUMOReal getNormedHBEFA_HCEmissions() const throw();


    /** @brief Returns the sum of last step fuel comsumption normed by the lane's length
     * @return Fuel comsumption of vehicles on this lane during the last step, normed by the lane length
     */
    SUMOReal getNormedHBEFA_FuelConsumption() const throw();
    /// @}



    SUMOReal getEdgeLaneNumber() const;

    class Colorer : public GUIColorer<GUILaneWrapper> {
    public:
        Colorer();
        SUMOReal getColorValue(const GUILaneWrapper& lane) const;
    };




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

