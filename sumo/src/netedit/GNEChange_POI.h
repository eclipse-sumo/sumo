/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
/****************************************************************************/
/// @file    GNEChange_POI.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2017
/// @version $Id$
///
// A network change in which a single POI is created or deleted
/****************************************************************************/
#ifndef GNEChange_POI_h
#define GNEChange_POI_h


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
#include "GNEChange.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNENet;
class GNEPOI;

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class GNEChange_POI
* A network change in which a single POI is created or deleted
*/
class GNEChange_POI : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_POI)

public:
    /**@brief Constructor
    * @param[in] net net in which POIgon will be inserted
    * @param[in] POI The name of the POIgon
    * @param[in] forward Whether to create/delete (true/false)
    */
    GNEChange_POI(GNENet* net, GNEPOI* POI, bool forward);

    /// @brief Destructor
    ~GNEChange_POI();

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

    /// @brief pointer to POI
    GNEPOI* myPOI;

};

#endif
/****************************************************************************/
