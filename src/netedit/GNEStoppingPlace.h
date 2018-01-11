/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEStoppingPlace.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
/// @version $Id$
///
// A abstract class to define common parameters of lane area in which vehicles can halt (GNE version)
/****************************************************************************/
#ifndef GNEStoppingPlace_h
#define GNEStoppingPlace_h


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
// class definitions
// ===========================================================================
/**
 * @class GNEStoppingPlace
 * @briefA abstract class to define common parameters and functions of stopping places
 */
class GNEStoppingPlace : public GNEAdditional {

public:
    /**@brief Constructor.
     * @param[in] id Gl-id of the stopping place (Must be unique)
     * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
     * @param[in] tag Type of xml tag that define the StoppingPlace (SUMO_TAG_BUS_STOP, SUMO_TAG_CHARGING_STATION, etc...)
     * @param[in] icon GUIIcon associated to the stopping place
     * @param[in] lane Lane of this StoppingPlace belongs
     * @param[in] startPos Start position of the StoppingPlace
     * @param[in] endPos End position of the StoppingPlace
     * @param[in] nam Name of stoppingPlace
     * @param[in] friendlyPos enable or disable friendly position
     */
    GNEStoppingPlace(const std::string& id, GNEViewNet* viewNet, SumoXMLTag tag, GUIIcon icon, GNELane* lane, double startPos, double endPos, const std::string& name, bool friendlyPosition);

    /// @brief Destructor
    ~GNEStoppingPlace();

    /**@brief writte additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    virtual void writeAdditional(OutputDevice& device) const = 0;

    /// @brief get Lane
    GNELane* getLane() const;

    /// @brief get absolute start Position
    double getAbsoluteStartPosition() const;

    /// @brief get absolute end Position
    double getAbsoluteEndPosition() const;

    /// @brief check if Position of stoppingPlace are fixed
    bool areStoppingPlacesPositionsFixed() const;

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
    virtual void updateGeometry() = 0;

    /// @brief Returns position of additional in view
    Position getPositionInView() const;
    /// @}

    /// @name inherited from GNEAdditional
    /// @{
    /// @brief Returns the name of the parent object
    /// @return This object's parent id
    const std::string& getParentName() const;

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
    /// @brief The lane in which this lane is placed
    GNELane* myLane;

    /// @brief The relative [0,1] start position this stopping place is located at
    double myStartPosRelative;

    /// @brief The relative [0,1] end position this stopping place is located at
    double myEndPosRelative;

    /// @brief name of stoppingPlace
    std::string myName;

    /// @brief Flag for friendly position
    bool myFriendlyPosition;

    /// @brief The position of the sign
    Position mySignPos;

    /// @brief set geometry common to all stopping places
    void setStoppingPlaceGeometry();

private:
    /// @brief set attribute after validation
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief Invalidate set new position in the view
    void setPosition(const Position& pos) = delete;
};


#endif
