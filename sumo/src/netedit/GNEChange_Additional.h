/****************************************************************************/
/// @file    GNEChange_Additional.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
/// A network change in which a additional element is created or deleted
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
#ifndef GNEChange_Additional_h
#define GNEChange_Additional_h

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
#include <netbuild/NBEdge.h>
#include "GNEChange.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNEAdditional;
class GNEViewNet;
class GNEDetectorEntry;
class GNEDetectorExit;
class GNEDetectorE3;
class GNELane;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange_Additional
 * A network change in which a additional element is created or deleted
 */
class GNEChange_Additional : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_Additional)

public:
    /**@brief Constructor for creating/deleting an additional element
     * @param[in] additional The additional element to be created/deleted
     * @param[in] forward Whether to create/delete (true/false)
     */
    GNEChange_Additional(GNEAdditional* additional, bool forward);

    /// @brief Destructor
    ~GNEChange_Additional();

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

private:
    /**@brief full information regarding the additional element that is to be created/deleted
     * @note we assume shared responsibility for the pointer (via reference counting)
     */
    GNEAdditional* myAdditional;

    /// @brief pointer to lane (used by additionals with lane parent)
    GNELane* myLaneParent;

    /// @brief pointer to E3 parent (used by Entry/exits)
    GNEDetectorE3* myE3Parent;

    /// @brief list of Entry detectors child (used by E3 Detector)
    std::vector<GNEDetectorEntry*> myEntryChilds;

    /// @brief list of Exit detectors child used (used by E3 Detector)
    std::vector<GNEDetectorExit*> myExitChilds;
};

#endif
/****************************************************************************/
