/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// The reification of a NETEDIT editing operation (see command pattern)
// inherits from FXCommand and is used to for undo/redo
/****************************************************************************/
#ifndef GNEChange_h
#define GNEChange_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <fx.h>
#include <vector>
#include <utils/foxtools/fxexdefs.h>

// ===========================================================================
// class declarations
// ===========================================================================
class GNENet;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange
 * @brief the function-object for an editing operation (abstract base)
 */
class GNEChange : public FXCommand {
    FXDECLARE_ABSTRACT(GNEChange)

public:
    /**@brief Constructor
     * @param[in] net GNENet net on which to apply changes
     * @param[in] forward The direction of this change
     */
    GNEChange(GNENet* net, bool forward);

    /// @brief Destructor
    ~GNEChange();

    /// @brief return actual size
    virtual FXuint size() const;

    /// @brief return undoName
    virtual FXString undoName() const;

    /// @brief return rendoName
    virtual FXString redoName() const;

    /// @brief undo action/operation
    virtual void undo();

    /// @brief redo action/operation
    virtual void redo();

protected:
    /**@brief the net to which operations shall be applied or which shall be
     * informed about gui updates
     * (we are not responsible for the pointer)
     */
    GNENet* myNet;

    /**@brief we group antagonistic commands (create junction/delete
     * junction) and keep them apart by this flag
     */
    bool myForward;
};

#endif
/****************************************************************************/
