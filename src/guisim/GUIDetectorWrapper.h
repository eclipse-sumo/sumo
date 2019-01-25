/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIDetectorWrapper.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// The base class for detector wrapper
/****************************************************************************/
#ifndef GUIDetectorWrapper_h
#define GUIDetectorWrapper_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/geom/Position.h>
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIDetectorWrapper
 * This is the base class for detector wrapper; As detectors may have a
 * position only or additionally a length, different display mechanisms are
 * necessary. These must be implemented in class erived from this one,
 * according to the wrapped detectors' properties.
 */
class GUIDetectorWrapper : public GUIGlObject_AbstractAdd {

public:
    /// Constructor
    GUIDetectorWrapper(GUIGlObjectType type, const std::string& id);

    /// Destructor
    ~GUIDetectorWrapper();

    /// @name inherited from GUIGlObject
    //@{

    /** @brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent);
    /// @}

};


#endif

/****************************************************************************/

