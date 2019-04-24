/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
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
     * @param[in] type GUIGlObjectType of stoppingPlace
     * @param[in] tag Type of xml tag that define the StoppingPlace (SUMO_TAG_BUS_STOP, SUMO_TAG_CHARGING_STATION, etc...)
     * @param[in] lane Lane of this StoppingPlace belongs
     * @param[in] startPos Start position of the StoppingPlace
     * @param[in] endPos End position of the StoppingPlace
     * @param[in] nam Name of stoppingPlace
     * @param[in] friendlyPos enable or disable friendly position
     * @param[in] block movement enable or disable additional movement
     */
    GNEStoppingPlace(const std::string& id, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag, GNELane* lane, const std::string& startPos, const std::string& endPos,
                     const std::string& name, bool friendlyPosition, bool blockMovement);

    /// @brief Destructor
    ~GNEStoppingPlace();

    /// @name members and functions relative to write additionals into XML
    /// @{
    /// @brief check if current additional is valid to be writed into XML
    bool isAdditionalValid() const;

    /// @brief return a string with the current additional problem
    std::string getAdditionalProblem() const;

    /// @brief fix additional problem
    void fixAdditionalProblem();
    /// @}

    /**@brief check if the position of an stoppingPlace over a lane is valid (without modifications)
    * @param[in] startPosStr Start position of stoppingPlace in string format
    * @param[in] endPosStr End position of stoppingPlace in string format
    * @param[in] laneLength Length of the lane
    * @param[in] friendlyPos Attribute of stoppingPlace
    * @return true if the stoppingPlace position is valid, false in otherweise
    */
    static bool checkStoppinPlacePosition(const std::string& startPosStr, const std::string& endPosStr, const double laneLength, const bool friendlyPos);

    /**@brief check if the position of an stoppingPlace over a la can be fixed
    * @param[in] startPosStr Start position of stoppingPlace in string format (note: it can be modified)
    * @param[in] endPosStr End position of stoppingPlace in string format (note: it can be modified)
    * @param[in] laneLength Length of the lane in which stopping place is placed
    * @param[in] friendlyPos boolean attribute of stoppingPlace
    * @return true if the stoppingPlace position was sucesfully fixed, false in other case
    */
    static bool fixStoppinPlacePosition(std::string& startPosStr, std::string& endPosStr, const double laneLength, const bool friendlyPos);

    /// @brief get start Position
    double getStartPosition() const;

    /// @brief get end Position
    double getEndPosition() const;

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
    virtual void updateGeometry() = 0;

    /// @brief Returns position of additional in view
    Position getPositionInView() const;

    /// @brief Returns the boundary to which the view shall be centered in order to show the object
    virtual Boundary getCenteringBoundary() const = 0;
    /// @}

    /// @name inherited from GNEAdditional
    /// @{
    /// @brief Returns the name of the parent object
    /// @return This object's parent id
    std::string getParentName() const;

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

    /// @brief get PopPup ID (Used in AC Hierarchy)
    std::string getPopUpID() const;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    std::string getHierarchyName() const;
    /// @}

protected:
    /// @brief The relative start position this stopping place is located at (optional, if empty takes 0)
    std::string  myStartPosition;

    /// @brief The  position this stopping place is located at (optional, if empty takes the lane lenght)
    std::string myEndPosition;

    /// @brief Flag for friendly position
    bool myFriendlyPosition;

    /// @brief The position of the sign
    Position mySignPos;

    /// @brief circle width resolution for all stopping places
    static const double myCircleWidth;

    /// @brief squared circle width resolution for all stopping places
    static const double myCircleWidthSquared;

    /// @brief inner circle width resolution for all stopping places
    static const double myCircleInWidth;

    /// @brief text inner circle width resolution for all stopping places
    static const double myCircleInText;

    /// @brief set geometry common to all stopping places
    void setStoppingPlaceGeometry(double movingToSide);

    /// @brief get start position over lane that is applicable to the shape
    double getStartGeometryPositionOverLane() const;

    /// @brief get end position over lane that is applicable to the shape
    double getEndGeometryPositionOverLane() const;

private:
    /// @brief set attribute after validation
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief Invalidate set new position in the view
    void setPosition(const Position& pos) = delete;
};


#endif
