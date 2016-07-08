/****************************************************************************/
/// @file    GNEAdditional.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2016
/// @version $Id$
///
/// A abstract class for representation of additional elements
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNEAdditional_h
#define GNEAdditional_h

// ===========================================================================
// included modules
// ===========================================================================

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <utils/geom/PositionVector.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/settings/GUIPropertySchemeStorage.h>

#include "GNEAttributeCarrier.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GUIGLObjectPopupMenu;
class PositionVector;
class GNEEdge;
class GNELane;
class GNENet;
class GNEViewNet;
class GNEAdditionalSet;
class GNEAdditionalDialog;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEAdditional
 * @brief An Element which don't belongs to GNENet but has influency in the simulation
 */

class GNEAdditional : public GUIGlObject, public GNEAttributeCarrier {
public:
    /**@brief Constructor.
     * @param[in] id Gl-id of the additional element (Must be unique)
     * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
     * @param[in] pos position of view in which additional is located
     * @param[in] tag Type of xml tag that define the additional element (SUMO_TAG_BUS_STOP, SUMO_TAG_REROUTER, etc...)
     * @param[in] additionalSetParent pointer to parent, if this additional belongs to an additionalSet
     * @param[in] blocked enable or disable blocking. By default additional element isn't blocked (i.e. value is false)
     */
    GNEAdditional(const std::string& id, GNEViewNet* viewNet, Position pos, SumoXMLTag tag, GNEAdditionalSet* additionalSetParent = NULL, bool blocked = false);

    /// @brief Destructor
    ~GNEAdditional();

    /**@brief change the position of the additional geometry
     * @param[in] posx new x position of idem in the map or over lane
     * @param[in] posy new y position of item in the map
     * @param[in] undoList pointer to the undo list
     * @note if additional belongs to a Lane, posx correspond to position over lane and posy is ignored
     * @note must be implemented in ALL childrens
     */
    virtual void moveAdditional(SUMOReal posx, SUMOReal posy, GNEUndoList* undoList) = 0;

    /// @brief update pre-computed geometry information
    /// @note: must be called when geometry changes (i.e. lane moved) and implemented in ALL childrens
    virtual void updateGeometry() = 0;

    /// @brief Returns position of additional in view
    virtual Position getPositionInView() const = 0;

    /// @brief open Additional Dialog
    /// @note: if additional needs an additional dialog, this function has to be implemented in childrens (see GNERerouter and GNEVariableSpeedSignal)
    virtual void openAdditionalDialog();

    /// @brief returns the ID of additional
    const std::string& getAdditionalID() const;

    /// @brief Returns a pointer to GNEViewNet in which additional element is located
    GNEViewNet* getViewNet() const;

    /// @brief Returns additional element's shape
    PositionVector getShape() const;

    /// @brief Check if additional item is currently blocked (i.e. cannot be moved with mouse)
    bool isAdditionalBlocked() const;

    /// @brief check if additional element is inspectionable (With GNEInspectorFrame)
    bool isAdditionalInspectionable() const;

    /// @brief check if additional element is selectable (With GNESelectorFrame)
    bool isAdditionalSelectable() const;

    /// @brief check if additional element is movable
    bool isAdditionalMovable() const;

    // @brief Check if additional item is selected
    bool isAdditionalSelected() const;

    /// @brief get additionalSet parent, or NULL if don't belongs to an additionalSet
    GNEAdditionalSet* getAdditionalSetParent() const;

    /// @brief set the ID of additional
    void setAdditionalID(const std::string& id);

    /// @brief Block or unblock additional element(i.e. cannot be moved with mouse)
    void setBlocked(bool value);

    /// @brief set new position in the view
    /// @note movement cannot be undo with GNEUndoRedo
    void setPositionInView(const Position& pos);

    /// @brief writte additional element into a xml file
    /// @param[in] device device in which write parameters of additional element
    /// @note must be implemented in all childrens
    virtual void writeAdditional(OutputDevice& device, const std::string& currentDirectory) = 0;

    /// @brief get edge of additional, or NULL if additional isn't placed over an edge
    /// @note if additional is placed over a edge, this function has to be implemented in the children (See RouteProbes)
    virtual GNEEdge* getEdge() const;

    /// @brief get lane of additional, or NULL if additional isn't placed over a Lane
    /// @note if additional is placed over a lane, this function has to be implemented in the children (See StoppingPlaces and Detectors)
    virtual GNELane* getLane() const;

    /// @brief if additional is placed over an edge, remove it reference
    /// @note if additional is placed over a edge, this function has to be implemented in the children (See RouteProbes) AND called in edge destructor
    virtual void removeEdgeReference();

    /// @brief if additional is placed over a lane, remove it reference
    /// @note if additional is placed over a lane, this function has to be implemented in the children (See StoppingPlaces and Detectors) AND called in lane destructor
    virtual void removeLaneReference();

    /// @name inherited from GUIGlObject
    /// @{
    /// @brief Returns the name of the parent object
    /// @return This object's parent id
    virtual const std::string& getParentName() const = 0;

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

    /**@brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     */
    Boundary getCenteringBoundary() const;

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    virtual void drawGL(const GUIVisualizationSettings& s) const = 0;
    /// @}

    /// @name inherited from GNEAttributeCarrier
    /// @{
    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    virtual std::string getAttribute(SumoXMLAttr key) const = 0;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    virtual void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) = 0;

    /* @brief method for checking if the key and their conrrespond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value asociated to key key
     * @return true if the value is valid, false in other case
     */
    virtual bool isValid(SumoXMLAttr key, const std::string& value) = 0;
    /// @}

protected:
    /// @brief The GNEViewNet this additional element belongs
    GNEViewNet* myViewNet;

    /// @brief The position in which this additional element is located
    /// @note if this element belongs to a Lane, x() value will be the position over Lane
    Position myPosition;

    /// @brief The shape of the additional element
    /// @note must be configured in updateGeometry()
    PositionVector myShape;

    /// @brief pointer to additional set parent, if belong to set
    GNEAdditionalSet* myAdditionalSetParent;

    /// @brief base color (Default green)
    /// @note default color can be defined in the constructor of every additional
    RGBColor myBaseColor;

    /// @brief base color selected (Default blue)
    /// @note default color can be defined in the constructor of every additional
    RGBColor myBaseColorSelected;

    /// @brief pointer to additional dialog
    GNEAdditionalDialog* myAdditionalDialog;

    /// @name computed only once (for performance) in updateGeometry()
    /// @{
    /// The rotations of the shape parts
    std::vector<SUMOReal> myShapeRotations;

    /// The lengths of the shape parts
    std::vector<SUMOReal> myShapeLengths;
    /// @}

    /// @brief rotation depending of the option "Lefthand"
    bool myRotationLefthand;

    /// @name members and functions relative to block icon
    /// @{
    /// @brief set Rotation of block Icon
    /// @note must be called in updateGeometry() after setting of Shape, and use parameter "lane" if additional is placed over a lane
    void setBlockIconRotation(GNELane* lane = NULL);

    /// @brief draw lock icon
    void drawLockIcon(SUMOReal size = 0.5) const;

    /// @brief position of the block icon
    Position myBlockIconPosition;

    /// @brief The offSet of the block icon
    Position myBlockIconOffset;

    /// @brief The rotation of the block icon
    SUMOReal myBlockIconRotation;
    /// @}

    /// @brief boolean to check if additional element is blocked (i.e. cannot be moved with mouse)
    bool myBlocked;

    /// @brief boolean to check if additional element is inspectionable (With GNEInspectorFrame). By default true
    bool myInspectionable;

    /// @brief boolean to check if additional element is selectable (With GNESelectorFrame). By default true
    bool mySelectable;

    /// @brief boolean to check if additional element is movable (with the mouse). By default true
    bool myMovable;

private:
    /// @brief set attribute after validation
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief Invalidated copy constructor.
    GNEAdditional(const GNEAdditional&);

    /// @brief Invalidated assignment operator.
    GNEAdditional& operator=(const GNEAdditional&);
};

#endif
