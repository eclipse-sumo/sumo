/****************************************************************************/
/// @file    GNEDetectorEntry.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
///
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
#ifndef GNEDetectorEntry_h
#define GNEDetectorEntry_h


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
 * @class GNEDetectorEntry
 * ------------
 */
class GNEDetectorEntry  : public GNEDetector {
public:
    /**@brief Constructor
     * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
     * @param[in] parent pointer to GNEDetectorE3 of this Entry belongs
     * @param[in] lane Lane of this StoppingPlace belongs
     * @param[in] pos position of the detector on the lane
     */
    GNEDetectorEntry(GNEViewNet* viewNet, GNEDetectorE3* parent, GNELane* lane, SUMOReal pos);

    /// @brief destructor
    ~GNEDetectorEntry();

    /// @brief update pre-computed geometry information
    /// @note: must be called when geometry changes (i.e. lane moved)
    void updateGeometry();

    /// @brief update pre-computed geometry information called by E3 parent
    void updateGeometryByParent();

    /// @brief Returns position of detector Entry in view
    Position getPositionInView() const;

    /// @brief get E3 Detector parent
    GNEDetectorE3* getE3Parent() const;

    /**@brief writte additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    void writeAdditional(OutputDevice& device) const;

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
    /// @brief pointer to E3 parent
    GNEDetectorE3* myE3Parent;

    /// @brief variable to save detectorEntry icon
    static GUIGlID detectorEntryGlID;

    /// @brief check if detectorEntry icon was inicilalizated
    static bool detectorEntryInitialized;

    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNEDetectorEntry(const GNEDetectorEntry&);

    /// @brief Invalidated assignment operator.
    GNEDetectorEntry& operator=(const GNEDetectorEntry&);

    /// @brief Invalidated get filename
    std::string getFilename() const;

    /// @brief Invalidated set frequency
    int getFrequency() const;

    /// @brief Invalidated set filename
    void setFrequency(SUMOReal freq);

    /// @brief Invalidated
    void setFilename(std::string filename);
};

#endif

/****************************************************************************/
