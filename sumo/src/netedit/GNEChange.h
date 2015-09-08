/****************************************************************************/
/// @file    GNEChange.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// The reification of a NETEDIT editing operation (see command pattern)
// inherits from FXCommand and is used to for undo/redo
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
    /** @brief Constructor
     * @param[in] net The net on which to apply changes
     * @param[in] forward The direction of this change
     */
    GNEChange(GNENet* net, bool forward);


    /// @brief Destructor
    ~GNEChange() {};

    virtual FXuint size() const {
        return 1;
    }

    virtual FXString undoName() const {
        return "Undo";
    }
    virtual FXString redoName() const {
        return "Redo";
    }
    virtual void undo() {};
    virtual void redo() {};


protected:
    /** @brief the net to which operations shall be applied or which shall be
     * informed about gui updates
     * (we are not responsible for the pointer)
     */
    GNENet* myNet;

    /** @brief we group antagonistic commands (create junction/delete
     * junction) and keep them apart by this flag
     */
    bool myForward;
};

#endif
/****************************************************************************/
