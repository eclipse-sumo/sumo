/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNETAZEdge.h
/// @author  Pablo Alvarez Lopez
/// @date    Apr 2017
/// @version $Id$
///
//
/****************************************************************************/
#ifndef GNETAZEdge_h
#define GNETAZEdge_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <vector>
#include <utils/common/UtilExceptions.h>
#include <utils/xml/SUMOXMLDefinitions.h>

#include "GNEAdditional.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNETAZ;
class GNEEdge;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNETAZEdge
 * class used to represent a interval used in Traffic Assignment Zones
 */
class GNETAZEdge : public GNEAdditional {

public:
    /// @brief constructor
    GNETAZEdge(GNEAdditional* TAZParent, GNEEdge* edge, double departWeight, double arrivalWeight);

    /// @brief destructor
    ~GNETAZEdge();

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

    /// @brief Returns position of additional in view
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

    /* @brief method for setting the attribute and letting the object perform additional changes
    * @param[in] key The attribute key
    * @param[in] value The new value
    * @param[in] undoList The undoList on which to register changes
    * @param[in] net optionally the GNENet to inform about gui updates
    */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /* @brief method for setting the attribute and letting the object perform additional changes
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
    /// @brief edge
    GNEEdge *myEdge;

    /// @brief depart Weight
    double myDepartWeight;

    /// @brief arrival Weight
    double myArrivalWeight;

private:
    /// @brief method for setting the attribute and nothing else
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNETAZEdge(const GNETAZEdge&) = delete;

    /// @brief Invalidated assignment operator
    GNETAZEdge& operator=(const GNETAZEdge&) = delete;
};

#endif

/****************************************************************************/
