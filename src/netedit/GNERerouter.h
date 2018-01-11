/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNERerouter.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
//
/****************************************************************************/
#ifndef GNERerouter_h
#define GNERerouter_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNEAdditional.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEEdge;
class GNERerouterInterval;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNERerouter
 * Rerouter changes the route of a vehicle as soon as the vehicle moves onto a specified edge.
 */
class GNERerouter : public GNEAdditional {

public:
    /**@brief Constructor
     * @param[in] id The storage of gl-ids to get the one for this lane representation from
     * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
     * @param[in] pos position (center) of the rerouter in the map
     * @param[in] edges vector with the edges of rerotuer
     * @param[in] filename The path to the definition file
     * @param[in] probability The probability for vehicle rerouting
     * @param[in] off Whether the router should be inactive initially
     */
    GNERerouter(const std::string& id, GNEViewNet* viewNet, Position pos, std::vector<GNEEdge*> edges, const std::string& filename,
                double probability, bool off, double timeThreshold);

    /// @brief Destructor
    ~GNERerouter();

    /**@brief writte additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    void writeAdditional(OutputDevice& device) const;

    /// @brief open GNERerouterDialog
    void openAdditionalDialog();

    /// @name Functions related with rerouter intervals
    /// @{

    /// @brief add rerouter interval
    void addRerouterInterval(GNERerouterInterval* rerouterInterval);

    /// @brief add rerouter interval
    void removeRerouterInterval(GNERerouterInterval* rerouterInterval);

    /// @brief get rerouter intervals
    const std::vector<GNERerouterInterval*>& getRerouterIntervals() const;

    /// @brief get number of overlapped intervals
    int getNumberOfOverlappedIntervals() const;

    /// @brief sort intervals
    void sortIntervals();

    /// @}

    /// @name Functions related with geometry of element
    /// @{
    /**@brief change the position of the element geometry without saving in undoList
     * @param[in] newPosition new position of geometry
     * @note should't be called in drawGL(...) functions to avoid smoothness issues
     */
    void moveGeometry(const Position& oldPos, const Position& offset);

    /**@brief commit geometry changes in the attributes of an element after use of moveGeometry(...)
     * @param[in] oldPos the old position of additional
     * @param[in] undoList The undoList on which to register changes
     */
    void commitGeometryMoving(const Position& oldPos, GNEUndoList* undoList);

    /// @brief update pre-computed geometry information
    void updateGeometry();

    /// @brief Returns position of additional in view
    Position getPositionInView() const;
    /// @}

    /// @name inherited from GUIGlObject
    /// @{
    /// @brief Returns the name of the parent object
    /// @return This object's parent id
    const std::string& getParentName() const;

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;
    /// @}

    /// @name inherited from GNEAttributeCarrier
    /// @{
    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    std::string getAttribute(SumoXMLAttr key) const;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /* @brief method for checking if the key and their correspond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value asociated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValid(SumoXMLAttr key, const std::string& value);
    /// @}

protected:
    /// @brief position of rerouter in view
    Position myPosition;

    /// @brief filename of rerouter
    std::string myFilename;

    /// @brief probability of rerouter
    double myProbability;

    /// @brief attribute to enable or disable inactive initially
    bool myOff;

    /// @brief attribute to configure activation time threshold
    double myTimeThreshold;

    /// @brief set with the GNERerouterInterval
    std::vector<GNERerouterInterval*> myRerouterIntervals;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNERerouter(const GNERerouter&) = delete;

    /// @brief Invalidated assignment operator.
    GNERerouter& operator=(const GNERerouter&) = delete;
};

#endif

/****************************************************************************/
