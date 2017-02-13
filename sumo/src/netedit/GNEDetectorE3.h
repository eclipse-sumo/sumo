/****************************************************************************/
/// @file    GNEDetectorE3.h
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

#include "GNEAdditional.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNELane;
class GNEDetectorEntry;
class GNEDetectorExit;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEDetectorE3
 * Class for multy Entry/multy Exits detectors
 */
class GNEDetectorE3 : public GNEAdditional {
    /// @brief declare friend class (needed to manage Entry/Exit childs)
    friend class GNEChange_Additional;

public:
    /**@brief GNEDetectorE3 Constructor
     * @param[in] id The storage of gl-ids to get the one for this lane representation from
     * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
     * @param[in] pos position (center) of the detector in the map
     * @param[in] freq the aggregation period the values the detector collects shall be summed up.
     * @param[in] filename The path to the output file
     * @param[in] timeThreshold The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting
     * @param[in] speedThreshold The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting
     */
    GNEDetectorE3(const std::string& id, GNEViewNet* viewNet, Position pos, SUMOReal freq, const std::string& filename, const SUMOReal timeThreshold, SUMOReal speedThreshold);

    /// @brief GNEDetectorE3 6Destructor
    ~GNEDetectorE3();

    /// @brief update pre-computed geometry information
    /// @note: must be called when geometry changes (i.e. lane moved)
    void updateGeometry();

    /// @brief Returns position of detector E3 in view
    Position getPositionInView() const;

    /// @brief change the position of the E3 geometry
    void moveAdditionalGeometry(SUMOReal offsetx, SUMOReal offsety);

    /// @brief updated geometry changes in the attributes of additional
    void commmitAdditionalGeometryMoved(SUMOReal oldPosx, SUMOReal oldPosy, GNEUndoList* undoList);

    /**@brief writte additionals element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    void writeAdditional(OutputDevice& device) const;

    /// @brief gererate a new ID for an Entry detector child
    std::string generateEntryID();

    /// @brief gererate a new ID for an Exit detector child
    std::string generateExitID();

    /// @brief add an Entry child
    void addEntryChild(GNEDetectorEntry* entry);

    /// @brief delete an Entry child
    void removeEntryChild(GNEDetectorEntry* entry);

    /// @brief add an Exit child
    void addExitChild(GNEDetectorExit* exit);

    /// @brief delete an Exit child
    void removeExitChild(GNEDetectorExit* exit);

    /// @brief get number of entry childs
    int getNumberOfEntryChilds() const;

    /// @brief get number of exit childs
    int getNumberOfExitChilds() const;

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

    /* @brief method for setting the attribute and letting the object perform additional changes
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
    SUMOReal myFreq;

    /// @brief fielname of E3 detector
    std::string myFilename;

    /// @brief The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting
    SUMOReal myTimeThreshold;

    /// @brief The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting
    SUMOReal mySpeedThreshold;

    /// @brief vector with the GNEDetectorE3EntryExits of the detector
    std::vector<GNEDetectorEntry*> myGNEDetectorEntrys;

    /// @brief vector with the GNEDetectorE3EntryExits of the detector
    std::vector<GNEDetectorExit*> myGNEDetectorExits;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief update Connection's geometry
    void updateGeometryConnections();

    /// @brief Invalidated copy constructor.
    GNEDetectorE3(const GNEDetectorE3&);

    /// @brief Invalidated assignment operator.
    GNEDetectorE3& operator=(const GNEDetectorE3&);
};

#endif
/****************************************************************************/
