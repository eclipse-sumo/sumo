/****************************************************************************/
/// @file    GNEPOI.h
/// @author  Jakob Erdmann
/// @date    Sept 2012
/// @version $Id$
///
// A class for visualizing and editing POIS in netedit (adapted from
// GUIPointOfInterest and NLHandler)
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
#ifndef GNEPOI_h
#define GNEPOI_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNEShape.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GeoConvHelper;
class GNELane;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEPOI
 *
 * In the case the represented junction's shape is empty, the boundary
 *  is computed using the junction's position to which an offset of 1m to each
 *  side is added.
 */
class GNEPOI : public GUIPointOfInterest, public GNEShape {

public:
    /// @brief needed to avoid diamond Problem between GUIPolygon and GNEShape
    using GNEShape::getID;

    /** @brief Constructor
     * @param[in] net net in which this polygon is placed
     * @param[in] id The name of the POI
     * @param[in] type The (abstract) type of the POI
     * @param[in] color The color of the POI
     * @param[in] layer The layer of the POI
     * @param[in] angle The rotation of the POI
     * @param[in] imgFile The raster image of the shape
     * @param[in] pos The position of the POI
     * @param[in] width The width of the POI image
     * @param[in] height The height of the POI image
     * @param[in] movementBlocked if movement of POI is blocked
     */
    GNEPOI(GNENet* net, const std::string& id, const std::string& type, const RGBColor& color, double layer, double angle,
           const std::string& imgFile, const Position& pos, double width, double height, bool movementBlocked);

    /// @brief Destructor
    ~GNEPOI();

    /**@brief change the position of the element geometry without saving in undoList
    * @param[in] newPosition new position of geometry
    * @note should't be called in drawGL(...) functions to avoid smoothness issues
    */
    void moveGeometry(const Position& oldPos, const Position &offset);

    /**@brief commit geometry changes in the attributes of an element after use of moveGeometry(...)
    * @param[in] oldPos the old position of additional
    * @param[in] undoList The undoList on which to register changes
    */
    void commitGeometryMoving(const Position& oldPos, GNEUndoList* undoList);

    /// @name inherited from GNEShape
    /// @{
    /**@brief writte shape element into a xml file
    * @param[in] device device in which write parameters of additional element
    */
    void writeShape(OutputDevice& device);

    /// @brief Returns position of additional in view
    Position getPositionInView() const;
    /// @}

    /// @name inherited from GUIGlObject
    /// @{
    /**@brief Returns the name of the parent object
     * @return This object's parent id
     */
    const std::string& getParentName() const;

    /**@brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /**@brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /// @brief Returns the boundary to which the view shall be centered in order to show the object
    Boundary getCenteringBoundary() const;

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;
    /// @}

    /// @name inherited from GNEAttributeCarrier
    /// @{
    /**@brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    std::string getAttribute(SumoXMLAttr key) const;

    /**@brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /**@brief method for checking if the key and their correspond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value asociated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValid(SumoXMLAttr key, const std::string& value);
    /// @}

protected:

    /// @brief lane in which this POI can be placed
    GNELane* myLane;

    /// @brief position over lane
    double myPositionOverLane;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNEPOI(const GNEPOI&) = delete;

    /// @brief Invalidated assignment operator.
    GNEPOI& operator=(const GNEPOI&) = delete;
};


#endif

/****************************************************************************/

