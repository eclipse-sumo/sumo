/****************************************************************************/
/// @file    GNEDetectorE3.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
///
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
#ifndef GNEDetectorE3_h
#define GNEDetectorE3_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNEAdditionalSet.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNEDetectorE3EntryExit;
class GNELane;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEDetectorE3
 * ------------
 */
class GNEDetectorE3 : public GNEAdditionalSet {
public:
    /**@brief GNEDetectorE3 Constructor
     * @param[in] id The storage of gl-ids to get the one for this lane representation from
     * @param[in] viewNet pointer to GNEViewNet of this additionalSet element belongs
     * @param[in] pos position (center) of the detector in the map
     * @param[in] freq the aggregation period the values the detector collects shall be summed up.
     * @param[in] filename The path to the output file
     * @param[in] timeThreshold The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting
     * @param[in] speedThreshold The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting
     * @param[in] blocked set initial blocking state of item
     */
    GNEDetectorE3(const std::string& id, GNEViewNet* viewNet, Position pos, int freq, const std::string& filename, SUMOTime timeThreshold, SUMOReal speedThreshold, bool blocked);

    /// @brief GNEDetectorE3 6Destructor
    ~GNEDetectorE3();

    /// @brief update pre-computed geometry information
    /// @note: must be called when geometry changes (i.e. lane moved)
    void updateGeometry();

    /// @brief Returns position of detector E3 in view
    Position getPositionInView() const;

    /**@brief change the position of the E3 geometry
     * @param[in] posx new x position of item in the map
     * @param[in] posy new y position of item in the map
     * @param[in] undoList pointer to the undo list
     */
    void moveAdditional(SUMOReal posx, SUMOReal posy, GNEUndoList* undoList);

    /**@brief writte additionalSet element into a xml file
     * @param[in] device device in which write parameters of additionalSet element
     */
    void writeAdditional(OutputDevice& device, const std::string& currentDirectory);

    /// @name inherited from GUIGlObject
    /// @{
    /// @brief Returns the name of the parent object
    /// @return This object's parent id
    const std::string& getParentName() const;

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

    /* @brief method for setting the attribute and letting the object perform additionalSet changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /* @brief method for checking if the key and their conrrespond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value asociated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValid(SumoXMLAttr key, const std::string& value);
    /// @}

protected:
    /// @brief frequency of E3 detector
    int myFreq;

    /// @brief fielname of E3 detector
    std::string myFilename;

    /// @brief The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting
    SUMOTime myTimeThreshold;

    /// @brief The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting
    SUMOReal mySpeedThreshold;

    /// @brief vector with the GNEDetectorE3EntryExits of the detector
    std::vector<GNEDetectorE3EntryExit*> myGNEDetectorE3EntryExits;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNEDetectorE3(const GNEDetectorE3&);

    /// @brief Invalidated assignment operator.
    GNEDetectorE3& operator=(const GNEDetectorE3&);
};

#endif
/****************************************************************************/