/****************************************************************************/
/// @file    GNEPOI.h
/// @author  Jakob Erdmann
/// @date    Sept 2012
/// @version $Id$
///
// A class for visualizing and editing POIS in netedit (adapted from
// GUIPointOfInterest and NLHandler)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

#include <string>
#include <utility>
#include <utils/gui/globjects/GUIPointOfInterest.h>
#include <utils/xml/SUMOSAXHandler.h>

#include "GNEAttributeCarrier.h"


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

    //friend class GNEChange_POI;

public:
    /** @brief Constructor
     */
    GNEPOI(const std::string& id, const std::string& type,
           const RGBColor& color, SUMOReal layer, SUMOReal angle, const std::string& imgFile,
           const Position& pos, SUMOReal width, SUMOReal height);


    /// @brief Destructor
    virtual ~GNEPOI() ;


    /** @brief reposition the node at pos and informs the edges
     * @param[in] pos The new position
     * @note: those operations are not added to the undoList. This is handled in
     * registerMove to avoids merging lots of tiny movements
     */
    //void move(Position pos);


    /// @brief registers completed movement with the undoList
    //void registerMove(GNEUndoList *undoList);


    //@name inherited from GNEAttributeCarrier
    //@{
    std::string getAttribute(SumoXMLAttr key) const;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    bool isValid(SumoXMLAttr key, const std::string& value);
    //@}

    /// @brief save POIs to file
    static void saveToFile(const std::string& file);

private:
    /// @brief Invalidated copy constructor.
    GNEPOI(const GNEPOI&);

    /// @brief Invalidated assignment operator.
    GNEPOI& operator=(const GNEPOI&);

    void setAttribute(SumoXMLAttr key, const std::string& value);

};


#endif

/****************************************************************************/

