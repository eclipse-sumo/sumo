/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Command_SaveTLCoupledLaneDet.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    15 Feb 2004
/// @version $Id$
///
// Writes e2 state of a link for the time the link has yellow/red
/****************************************************************************/
#ifndef Command_SaveTLCoupledLaneDet_h
#define Command_SaveTLCoupledLaneDet_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <microsim/MSLink.h>
#include "Command_SaveTLCoupledDet.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Command_SaveTLCoupledLaneDet
 * @brief Writes e2 state of a link for the time the link has yellow/red
 *
 * @todo Problem: The detector may not save the last state (on simulation end)
 */
class Command_SaveTLCoupledLaneDet : public Command_SaveTLCoupledDet {
public:
    /** @brief Constructor
     *
     * @param[in] tlls The logic to observe
     * @param[in] dtf The detector used to generate the values
     * @param[in] begin The begin simulation time
     * @param[in] device The output device to write the detector values into
     * @param[in] link The link that shall be observed
     */
    Command_SaveTLCoupledLaneDet(MSTLLogicControl::TLSLogicVariants& tlls,
                                 MSDetectorFileOutput* dtf, SUMOTime begin, OutputDevice& device,
                                 MSLink* link);


    /// @brief Destructor
    ~Command_SaveTLCoupledLaneDet();


    /** @brief Executes the command
     *
     * Called when an active tls program switches, this method checks whether the
     *  tls signal responsible for the link has switched to green. If so, the
     *  values collected so far are written using "writeXMLOutput".
     *  Otherwise, the values are reset.
     *
     * @see MSDetectorFileOutput::writeXMLOutput
     * @see MSE2Collector::writeXMLOutput
     */
    void execute();


private:
    /// @brief The link to observe
    MSLink* myLink;

    /// @brief The state the link had the last time
    LinkState myLastState;

    /// @brief Whether the last link state was already saved
    bool myHadOne;


private:
    /// @brief Invalidated copy constructor.
    Command_SaveTLCoupledLaneDet(const Command_SaveTLCoupledLaneDet&);

    /// @brief Invalidated assignment operator.
    Command_SaveTLCoupledLaneDet& operator=(const Command_SaveTLCoupledLaneDet&);

};


#endif

/****************************************************************************/

