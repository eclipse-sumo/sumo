/****************************************************************************/
/// @file    GNEAdditional.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2016
/// @version $Id$
///
/// A abstract class for representation of additional elements
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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
     * @param[in] icon GUIIcon associated to the additional
     */
    GNEAdditional(const std::string& id, GNEViewNet* viewNet, Position pos, SumoXMLTag tag, GUIIcon icon);

    /// @brief Destructor
    ~GNEAdditional();

    /**@brief change the position of the additional geometry
     * @param[in] offset offset of movement
     * @note simply call the function moveAdditionalGeometry(SUMOReal offsetx, SUMOReal offsety)
     */
    void moveAdditionalGeometry(const Position& offset);

    /**@brief change the position of the additional geometry
     * @param[in] offsetx horizontal offset of movement
     * @param[in] offsety vertical offset of movement
     * @note if additional belongs to a Lane, offsety is ignored
     * @note must be implemented in ALL childrens
     */
    virtual void moveAdditionalGeometry(SUMOReal offsetx, SUMOReal offsety) = 0;

    /**@brief updated geometry changes in the attributes of additional
     * @param[in] oldPos old position X of additional
     * @param[in] undoList The undoList on which to register changes
     * @note simply call function commmitAdditionalGeometryMoved(SUMOReal oldPosx, SUMOReal oldPosy, GNEUndoList* undoList)
     */
    void commmitAdditionalGeometryMoved(const Position& oldPos, GNEUndoList* undoList);

    /**@brief updated geometry changes in the attributes of additional
     * @param[in] oldPosx old position X of additional
     * @param[in] oldPosy old position Y of additional
     * @param[in] undoList The undoList on which to register changes
     * @note if additional belongs to a Lane, oldPosy is ignored
     * @note must be implemented in ALL childrens
     */
    virtual void commmitAdditionalGeometryMoved(SUMOReal oldPosx, SUMOReal oldPosy, GNEUndoList* undoList) = 0;

    /// @brief update pre-computed geometry information
    /// @note: must be called when geometry changes (i.e. lane moved) and implemented in ALL childrens
    virtual void updateGeometry() = 0;

    /// @brief Returns position of additional in view
    virtual Position getPositionInView() const = 0;

    /// @brief open Additional Dialog
    /// @note: if additional needs an additional dialog, this function has to be implemented in childrens (see GNERerouter and GNEVariableSpeedSign)
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

    /// @brief set the ID of additional
    void setAdditionalID(const std::string& id);

    /// @brief set new position in the view
    /// @note movement cannot be undo with GNEUndoRedo
    void setPositionInView(const Position& pos);

    /**@brief writte additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     * @note must be implemented in all derived classes
     */
    virtual void writeAdditional(OutputDevice& device) const = 0;

    /// @brief get edge of additional, or NULL if additional isn't placed over an edge
    GNEEdge* getEdge() const;

    /// @brief get lane of additional, or NULL if additional isn't placed over a Lane
    GNELane* getLane() const;

    /// @name inherited from GUIGlObject
    /// @{
    /// @brief Returns the name (ID) of the parent object
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

    /**@brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    virtual void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) = 0;

    /**@brief method for checking if the key and their conrrespond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value asociated to key key
     * @return true if the value is valid, false in other case
     */
    virtual bool isValid(SumoXMLAttr key, const std::string& value) = 0;
    /// @}

protected:
    /**@brief change edge of additional
     * @throw exception if additional doesn't belong to an edge
     * @throw expcetion if edge doesn't exist
     */
    void changeEdge(const std::string& edgeID);

    /**@brief change lane of additional
     * @throw exception if additional doesn't belong to a lane
     * @throw expcetion if edge doesn't exist
     */
    void changeLane(const std::string& laneID);

protected:
    /// @brief The GNEViewNet this additional element belongs
    GNEViewNet* myViewNet;

    /**@brief The edge this additional belongs.
     * @note is NULL if additional doesnt' belongs to a edge
     */
    GNEEdge* myEdge;

    /**@brief The lane this additional belongs.
     * @note is NULL if additional doesnt' belongs to a lane
     */
    GNELane* myLane;

    /**@brief The position in which this additional element is located
     * @note if this element belongs to a Lane, x() value will be the position over Lane
     */
    Position myPosition;

    /**@brief The shape of the additional element
     * @note must be configured in updateGeometry()
     */
    PositionVector myShape;

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
    /**@brief set Rotation of block Icon
     * @note must be called in updateGeometry() after setting of Shape, and use parameter "lane" if additional is placed over a lane
     */
    void setBlockIconRotation(GNELane* lane = NULL);

    /// @brief draw lock icon
    void drawLockIcon(SUMOReal size = 0.5) const;

    /// @brief draw connections between Parent and childrens
    void drawParentAndChildrenConnections() const;

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

    /**@brief base color (Default green)
     * @note default color can be defined in the constructor of every additional
     */
    RGBColor myBaseColor;

    /**@brief base color selected (Default blue)
     * @note default color can be defined in the constructor of every additional
     */
    RGBColor myBaseColorSelected;

    /// @brief pointer to additional dialog
    GNEAdditionalDialog* myAdditionalDialog;

    /// @brief Matrix with the Vertex's positions of connections between Additional Parent an their childs
    std::vector<std::vector<Position> > myConnectionPositions;

private:
    /// @brief set attribute after validation
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief Invalidated copy constructor.
    GNEAdditional(const GNEAdditional&);

    /// @brief Invalidated assignment operator.
    GNEAdditional& operator=(const GNEAdditional&);
};

#endif
