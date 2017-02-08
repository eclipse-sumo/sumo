/****************************************************************************/
/// @file    GNEStoppingPlace.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
/// @version $Id$
///
/// A abstract class to define common parameters of lane area in which vehicles can halt (GNE version)
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
// class declarations
// ===========================================================================

class GUIGLObjectPopupMenu;
class PositionVector;
class GNENet;

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
     */
    GNEStoppingPlace(const std::string& id, GNEViewNet* viewNet, SumoXMLTag tag, GUIIcon icon, GNELane* lane, SUMOReal startPos, SUMOReal endPos);

    /// @brief Destructor
    ~GNEStoppingPlace();

    /// @brief update pre-computed geometry information
    virtual void updateGeometry() = 0;

    /// @brief Returns position of StoppingPlace in view
    Position getPositionInView() const;

    /// @brief change the position of the StoppingPlace geometry
    void moveAdditionalGeometry(SUMOReal offsetx, SUMOReal offsety);

    /// @brief updated geometry changes in the attributes of additional
    void commmitAdditionalGeometryMoved(SUMOReal oldPosx, SUMOReal oldPosy, GNEUndoList* undoList);

    /**@brief writte additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     * @param[in] currentDirectory current directory in which this additional are writted
     */
    virtual void writeAdditional(OutputDevice& device) const = 0;

    /// @brief Returns the Start position of the stoppingPlace
    SUMOReal getStartPosition() const;

    /// @brief Returns the End position of the stoppingPlace
    SUMOReal getEndPosition() const;

    /**@brief Set a new Start position in StoppingPlace
     * @param[in] startPos new Start position of StoppingPlace
     * @throws InvalidArgument if value of startPos isn't valid
     */
    void setStartPosition(SUMOReal startPos);

    /**@brief Set a new End position in StoppingPlace
     * @param[in] endPos new End position of StoppingPlace
     * @throws InvalidArgument if value of endPos isn't valid
     */
    void setEndPosition(SUMOReal endPos);

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
    /// @brief The start position this stopping place is located at
    SUMOReal myStartPos;

    /// @brief The end position this stopping place is located at
    SUMOReal myEndPos;

    /// @brief The position of the sign
    Position mySignPos;

    /// @brief sign color (Default yellow)
    RGBColor mySignColor;

    /// @brief sign selected color (Default blue)
    RGBColor mySignColorSelected;

    /// @brief Text color (Default cyan)
    RGBColor myTextColor;

    /// @brief Text color selected (Default blue)
    RGBColor myTextColorSelected;

private:
    /// @brief set attribute after validation
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief Invalidate set new position in the view
    void setPosition(const Position& pos);
};


#endif
