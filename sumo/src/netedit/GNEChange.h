/****************************************************************************/
/// @file    GNEChange.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// The reification of a NETEDIT editing operation (see command pattern)
// inherits from FXCommand and is used to for undo/redo
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNEChange_h
#define GNEChange_h


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
