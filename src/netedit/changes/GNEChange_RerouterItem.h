/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_RerouterItem.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2017
/// @version $Id$
///
// A change in the values of Rerouters in netedit
/****************************************************************************/
#ifndef GNEChange_RerouterItem_h
#define GNEChange_RerouterItem_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <fx.h>
#include <utils/foxtools/fxexdefs.h>
#include <utils/gui/globjects/GUIGlObject.h>

#include "GNEChange.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNERerouter;
class GNERerouterInterval;
class GNEClosingLaneReroute;
class GNEDestProbReroute;
class GNERouteProbReroute;
class GNEParkingAreaReroute;

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class GNEChange_RerouterItem
* A change to the network selection
*/
class GNEChange_RerouterItem : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_RerouterItem)

public:
    /**@brief Constructor
    * @param[in] rerouterInterval rerouter interval to be added
    * @param[in] forward Whether to create/delete (true/false)
    */
    GNEChange_RerouterItem(GNERerouterInterval* rerouterInterval, bool forward);

    /**@brief Constructor
    * @param[in] destProbReroute Destiny Probability Reroute to be added
    * @param[in] forward Whether to create/delete (true/false)
    */
    GNEChange_RerouterItem(GNEDestProbReroute* destProbReroute, bool forward);

    /**@brief Constructor
    * @param[in] routeProbReroute Route Probability Reroute to be added
    * @param[in] forward Whether to create/delete (true/false)
    */
    GNEChange_RerouterItem(GNERouteProbReroute* routeProbReroute, bool forward);

    /**@brief Constructor
    * @param[in] ParkingAreaReroute to be added
    * @param[in] forward Whether to create/delete (true/false)
    */
    GNEChange_RerouterItem(GNEParkingAreaReroute* parkingAreaReroute, bool forward);

    /// @brief Destructor
    ~GNEChange_RerouterItem();

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
    /// @brief modified rerouter interval
    GNERerouterInterval* myRerouterInterval;

    /// @brief modified Closing Interval
    GNEDestProbReroute* myDestProbReroute;

    /// @brief modified parkingAreaReroute
    GNEParkingAreaReroute* myParkingAreaReroute;

    /// @brief modified Route Probability Reroute
    GNERouteProbReroute* myRouteProbReroute;
};

#endif
/****************************************************************************/
