/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNETrip.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
/// @version $Id$
///
// Representation of Trips in NETEDIT
/****************************************************************************/
#ifndef GNETrip_h
#define GNETrip_h


// ===========================================================================
// included modules
// ===========================================================================

#include <utils/vehicle/SUMOVehicleParameter.h>

#include "GNEDemandElement.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNETrip
 */
class GNETrip : public GNEDemandElement, public SUMOVehicleParameter {

public:
    /// @brief constructor
    GNETrip(GNEViewNet* viewNet, const std::string &tripID, GNEDemandElement* vehicleType, GNEEdge *from, GNEEdge *to);

    /// @brief parameter constructor
    GNETrip(GNEViewNet* viewNet, const SUMOVehicleParameter &tripParameter, GNEDemandElement* vehicleType, GNEEdge *from, GNEEdge *to);

    /// @brief destructor
    ~GNETrip();

    /// @name Functions related with geometry of element
    /// @{
    /**@brief change the position of the element geometry without saving in undoList
     * @param[in] offset Position used for calculate new position of geometry without updating RTree
     */
    void moveGeometry(const Position& offset);

    /**@brief commit geometry changes in the attributes of an element after use of moveGeometry(...)
     * @param[in] undoList The undoList on which to register changes
     */
    void commitGeometryMoving(GNEUndoList* undoList);

    /// @brief update pre-computed geometry information
    void updateGeometry(bool updateGrid);

    /// @brief Returns position of demand element in view
    Position getPositionInView() const;
    /// @}

    /// @name inherited from GUIGlObject
    /// @{
    /**@brief Returns the name of the parent object
     * @return This object's parent id
     */
    std::string getParentName() const;

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;
    /// @}

    /// @brief inherited from GNEAttributeCarrier
    /// @{
    /* @brief method for getting the Attribute of an XML key
    * @param[in] key The attribute key
    * @return string with the value associated to key
    */
    std::string getAttribute(SumoXMLAttr key) const;

    /* @brief method for setting the attribute and letting the object perform demand element changes
    * @param[in] key The attribute key
    * @param[in] value The new value
    * @param[in] undoList The undoList on which to register changes
    * @param[in] net optionally the GNENet to inform about gui updates
    */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /* @brief method for setting the attribute and letting the object perform demand element changes
    * @param[in] key The attribute key
    * @param[in] value The new value
    * @param[in] undoList The undoList on which to register changes
    */
    bool isValid(SumoXMLAttr key, const std::string& value);

    /// @brief get PopPup ID (Used in AC Hierarchy)
    std::string getPopUpID() const;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    std::string getHierarchyName() const;
    /// @}

protected:
    /// @brief type of flow
    GNEDemandElement* myVehicleType;

    /// @brief GNEEdge from
    GNEEdge* myFrom;

    /// @brief GNEEdge to
    GNEEdge* myTo;

private:
    /// @brief method for setting the attribute and nothing else
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNETrip(const GNETrip&) = delete;

    /// @brief Invalidated assignment operator
    GNETrip& operator=(const GNETrip&) = delete;
};

#endif
/****************************************************************************/
