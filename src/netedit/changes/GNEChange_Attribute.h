/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_Attribute.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// A network change in which the attribute of some object is modified
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
class GNEAdditional;
class GNEShape;

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
    /**@brief Constructor
     * @param[in] ac The attribute-carrier to be modified
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] testingMode flag to indicate if netedit is running in testing mode
     */
    GNEChange_Attribute(GNEAttributeCarrier* ac,
                        const SumoXMLAttr key,
                        const std::string& value,
                        bool customOrigValue = false,
                        const std::string& origValue = "");


    /// @brief Destructor
    ~GNEChange_Attribute();

    /// @name inherited from GNEChange
    /// @{
    /// @brief get undo Name
    FXString undoName() const;

    /// @brief get Redo name
    FXString redoName() const;

    /// @brief undo action
    void undo();

    /// @brief redo action
    void redo();
    /// @}

    /// @brief wether original and new value differ
    bool trueChange();

private:
    /**@brief the net to which all operations shall be applied
     * @note we are not responsible for the pointer
     */
    GNEAttributeCarrier* myAC;

    /// @brief The attribute name
    SumoXMLAttr myKey;

    /// @brief the original value
    std::string myOrigValue;

    /// @brief the original value
    std::string myNewValue;

    /// @brief used if Ac is an additional element
    GNEAdditional* myAdditional;

    /// @brief used if Ac is a shape element
    GNEShape* myShape;
};

#endif
/****************************************************************************/
