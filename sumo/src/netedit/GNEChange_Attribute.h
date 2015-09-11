/****************************************************************************/
/// @file    GNEChange_Attribute.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// A network change in which the attribute of some object is modified
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNEChange_Attribute_h
#define GNEChange_Attribute_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <utils/foxtools/fxexdefs.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include "GNEChange.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNEAttributeCarrier;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange_Attribute
 * @brief the function-object for an editing operation (abstract base)
 */
class GNEChange_Attribute : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_Attribute)

public:
    /** @brief Constructor
     * @param[in] ac The attribute-carrier to be modified
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] net Optionally the net on which to apply changes
     */
    GNEChange_Attribute(GNEAttributeCarrier* ac,
                        const SumoXMLAttr key,
                        const std::string& value,
                        bool customOrigValue = false,
                        const std::string& origValue = "");


    /// @brief Destructor
    ~GNEChange_Attribute();

    FXString undoName() const;
    FXString redoName() const;
    void undo();
    void redo();

    /// @brief wether original and new value differ
    bool trueChange() {
        return myOrigValue != myNewValue;
    }


private:
    /** @brief the net to which all operations shall be applied
     * (we are not responsible for the pointer)
     */
    GNEAttributeCarrier* myAC;

    /// @brief The attribute name
    SumoXMLAttr myKey;

    /// @brief the original value
    std::string myOrigValue;

    /// @brief the original value
    std::string myNewValue;

};

#endif
/****************************************************************************/
