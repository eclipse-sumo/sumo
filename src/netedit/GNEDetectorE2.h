/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDetectorE2.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
//
/****************************************************************************/
#ifndef GNEDetectorE2_h
#define GNEDetectorE2_h


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
// class definitions
// ===========================================================================
/**
 * @class GNEDetectorE2
 * class for detector of type E2
 */
class GNEDetectorE2 : public GNEDetector {

public:
    /**@brief Constructor
     * @param[in] id The storage of gl-ids to get the one for this lane representation from
     * @param[in] lane Lane of this StoppingPlace belongs
     * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
     * @param[in] pos position of the detector on the lane
     * @param[in] length The length of the detector in meters.
     * @param[in] freq the aggregation period the values the detector collects shall be summed up.
     * @param[in] filename The path to the output file.
     * @param[in] cont attribute to enable or disable splitByType
     * @param[in] timeThreshold The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting
     * @param[in] speedThreshold The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting
     * @param[in] speedThreshold The minimum distance to the next standing vehicle in order to make this vehicle count as a participant to the jam
     * @param[in] friendlyPos enable or disable friendly positions
     */
    GNEDetectorE2(const std::string& id, GNELane* lane, GNEViewNet* viewNet, double pos, double length, double freq, const std::string& filename,
                  bool cont, const double timeThreshold, double speedThreshold, double jamThreshold, bool friendlyPos);

    /// @brief Destructor
    ~GNEDetectorE2();

    /**@brief writte additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    void writeAdditional(OutputDevice& device) const;

    /// @brief check if Position of detector is fixed
    bool isDetectorPositionFixed() const;

    /// @brief get absolute lenght
    double getAbsoluteLenght() const;

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

protected:
    /// @brief relative length [0,1]
    double myRelativeLength;

    /// @brief attribute to enable or disable splitByType
    bool myCont;

    /// @brief The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting
    double myTimeThreshold;

    /// @brief The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting
    double mySpeedThreshold;

    /// @brief The minimum distance to the next standing vehicle in order to make this vehicle count as a participant to the jam
    double myJamThreshold;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNEDetectorE2(const GNEDetectorE2&) = delete;

    /// @brief Invalidated assignment operator.
    GNEDetectorE2& operator=(const GNEDetectorE2&) = delete;
};

#endif
/****************************************************************************/
