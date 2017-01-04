/****************************************************************************/
/// @file    GNEPOI.h
/// @author  Jakob Erdmann
/// @date    Sept 2012
/// @version $Id$
///
// A class for visualizing and editing POIS in netedit (adapted from
// GUIPointOfInterest and NLHandler)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNEPOI_h
#define GNEPOI_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNENetElement.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GeoConvHelper;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEPOI
 *
 * In the case the represented junction's shape is empty, the boundary
 *  is computed using the junction's position to which an offset of 1m to each
 *  side is added.
 */
class GNEPOI : public GUIPointOfInterest, public GNEAttributeCarrier {

    /// @brief declare friend class
    //friend class GNEChange_POI;

public:
    /// @brief Constructor
    GNEPOI(GNENet* net, const std::string& id, const std::string& type,
           const RGBColor& color, SUMOReal layer, SUMOReal angle, const std::string& imgFile,
           const Position& pos, SUMOReal width, SUMOReal height);

    /// @brief Destructor
    virtual ~GNEPOI();

    /**@brief reposition the POI at pos and informs the edges
     * @param[in] pos The new position
     * @note: those operations are not added to the undoList. This is handled in
     * registerMove to avoids merging lots of tiny movements
     */
    void move(Position pos);

    /// @brief registers completed movement with the undoList
    //void registerMove(GNEUndoList *undoList);

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

    /// @brief save POIs to file
    static void saveToFile(const std::string& file);

protected:
    /// @brief the net for querying updates
    GNENet* myNet;

private:
    /// @brief Invalidated copy constructor.
    GNEPOI(const GNEPOI&);

    /// @brief Invalidated assignment operator.
    GNEPOI& operator=(const GNEPOI&);

    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);
};


#endif

/****************************************************************************/

