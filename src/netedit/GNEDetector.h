/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDetector.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
// A abstract class to define common parameters of detectors placed over lanes
/****************************************************************************/
#ifndef GNEDetector_h
#define GNEDetector_h


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
 * @class GNEdetector
 * @briefA abstract class to define common parameters and functions of detectors
 */
class GNEDetector : public GNEAdditional {

public:
    /**@brief Constructor.
     * @param[in] id Gl-id of the detector (Must be unique)
     * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
     * @param[in] tag Type of xml tag that define the detector (SUMO_TAG_E1DETECTOR, SUMO_TAG_LANE_AREA_DETECTOR, etc...)
     * @param[in] icon GUIIcon associated to the detector
     * @param[in] lane Lane of this detector belongs
     * @param[in] pos position of the detector on the lane
     * @param[in] freq the aggregation period the values the detector collects shall be summed up.
     * @param[in] filename The path to the output file.
     * @param[in] friendlyPos enable or disable friendly positions
     * @param[in] additionalParent additional parent of this detector
     */
    GNEDetector(const std::string& id, GNEViewNet* viewNet, SumoXMLTag tag, GUIIcon icon, GNELane* lane, double pos, double freq, const std::string& filename, bool friendlyPos, GNEAdditional* additionalParent);

    /// @brief Destructor
    ~GNEDetector();

    /**@brief writte additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    virtual void writeAdditional(OutputDevice& device) const = 0;

    /// @brief check if Position of detector is fixed
    virtual bool isDetectorPositionFixed() const = 0;

    /// @brief get lane
    GNELane* getLane() const;

    /// @brief get absolute position over Lane
    double getAbsolutePositionOverLane() const;

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

    /// @name inherited from GUIGLObject
    /// @{
    /**@brief Returns the name of the parent object
     * @return This object's parent id
     */
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
    /// @brief The lane in which this detector is placed
    GNELane* myLane;

    /// @brief position of detector over Lane
    double myPositionOverLane;

    /// @brief The aggregation period the values the detector collects shall be summed up.
    double myFreq;

    /// @brief The path to the output file
    std::string myFilename;

    /// @brief Flag for friendly position
    bool myFriendlyPosition;

private:
    /// @brief set attribute after validation
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief Invalidate return position of additional
    const Position& getPosition() const = delete;

    /// @brief Invalidate set new position in the view
    void setPosition(const Position& pos) = delete;
};

#endif

/****************************************************************************/
