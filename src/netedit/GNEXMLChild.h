/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEXMLChild.h
/// @author  Pablo Alvarez Lopez
/// @date    May 2019
/// @version $Id$
///
// A abstract class for representation of elements that are written within another element
/****************************************************************************/
#ifndef GNEXMLChild_h
#define GNEXMLChild_h

// ===========================================================================
// included modules
// ===========================================================================

#include <config.h>

#include <utils/gui/globjects/GUIGlObjectTypes.h>

#include "GNEAttributeCarrier.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEAdditional;
class GNEDemandElement;
class GNEShape;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEXMLChild
 * @brief An special type of Attribute carrier that owns hierarchical elements
 */
class GNEXMLChild {

public:
    /**@brief default Constructor
     * @param[in] AC original AC
     */
    GNEXMLChild(GNEAttributeCarrier* AC);

    /**@brief Parameter Constructor
     * @param[in] AC original AC
     * @param[in] ACinWhichWrite AC in which write AC
     */
    GNEXMLChild(GNEAttributeCarrier* AC, GNEAttributeCarrier* ACinWhichWrite);

    /// @brief Destructor
    ~GNEXMLChild();

    /// @brief set XML Child (
    void setXMLChild(GNEAttributeCarrier* ACinWhichWrite);

    /// @brief get XML child
    GNEAttributeCarrier* getXMLChild() const;

private:
    /// @brief AC in which write myAC
    GNEAttributeCarrier* myACinWhichWrite;

    /// @brief pointer to AC (needed to avoid diamond problem)
    GNEAttributeCarrier* myAC;

    /// @brief Invalidated copy constructor.
    GNEXMLChild(const GNEXMLChild&) = delete;

    /// @brief Invalidated assignment operator.
    GNEXMLChild& operator=(const GNEXMLChild&) = delete;
};

#endif
