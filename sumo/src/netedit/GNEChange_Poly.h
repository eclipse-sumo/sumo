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
/// @file    GNEChange_Poly.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// A network change in which a single poly is created or deleted
/****************************************************************************/
#ifndef GNEChange_Poly_h
#define GNEChange_Poly_h


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
class GNEPoly;

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class GNEChange_Poly
* A network change in which a single poly is created or deleted
*/
class GNEChange_Poly : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_Poly)

public:
    /**@brief Constructor
    * @param[in] net net in which polygon will be inserted
    * @param[in] poly The name of the polygon
    * @param[in] forward Whether to create/delete (true/false)
    */
    GNEChange_Poly(GNENet* net, GNEPoly* poly, bool forward);

    /// @brief Destructor
    ~GNEChange_Poly();

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

    /// @brief pointer to polygon
    GNEPoly* myPoly;

};

#endif
/****************************************************************************/
