/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDetectorExit.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
//
/****************************************************************************/
#ifndef GNEDetectorExit_h
#define GNEDetectorExit_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNEDetector.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNEDetectorE3;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEDetectorExit
 * class for detector of type exit
 */
class GNEDetectorExit  : public GNEDetector {

public:
    /**@brief Constructor
     * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
     * @param[in] parent pointer to GNEDetectorE3 of this Exit belongs
     * @param[in] lane Lane of this StoppingPlace belongs
     * @param[in] pos position of the detector on the lane
     * @param[in] friendlyPos enable or disable friendly positions
     */
    GNEDetectorExit(GNEViewNet* viewNet, GNEDetectorE3* parent, GNELane* lane, double pos, bool friendlyPos);

    /// @brief destructor
    ~GNEDetectorExit();

    /**@brief writte additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    void writeAdditional(OutputDevice& device) const;

    /// @brief check if Position of detector is fixed
    bool isDetectorPositionFixed() const;

    /// @name Functions related with geometry of element
    /// @{
    /// @brief update pre-computed geometry information
    void updateGeometry();
    /// @}

    /// @name inherited from GUIGlObject
    /// @{
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

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNEDetectorExit(const GNEDetectorExit&);

    /// @brief Invalidated assignment operator.
    GNEDetectorExit& operator=(const GNEDetectorExit&) = delete;
};


#endif
/****************************************************************************/
