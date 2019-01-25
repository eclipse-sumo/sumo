/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Command_SaveTLCoupledDet.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    15 Feb 2004
/// @version $Id$
///
// Writes e2 state on each tls switch
/****************************************************************************/
#ifndef Command_SaveTLCoupledDet_h
#define Command_SaveTLCoupledDet_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <microsim/traffic_lights/MSTLLogicControl.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSTrafficLightLogic;
class MSDetectorFileOutput;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Command_SaveTLCoupledDet
 * @brief Writes e2 state on each tls switch
 *
 * @todo Problem: The detector may not save the last state (on simulation end)
 */
class Command_SaveTLCoupledDet : public MSTLLogicControl::OnSwitchAction {
public:
    /** @brief Constructor
     *
     * @param[in] tlls The logic to observe
     * @param[in] dtf The detector used to generate the values
     * @param[in] begin The begin simulation time
     * @param[in] device The output device to write the detector values into
     */
    Command_SaveTLCoupledDet(MSTLLogicControl::TLSLogicVariants& tlls,
                             MSDetectorFileOutput* dtf, SUMOTime begin, OutputDevice& device);


    /// @brief Destructor
    virtual ~Command_SaveTLCoupledDet();


    /** @brief Executes the command
     *
     * Called when an active tls program switches, this method calls
     *  "writeXMLOutput" of its detector (e2; the values are resetted there).
     *
     * @see MSDetectorFileOutput::writeXMLOutput
     * @see MSE2Collector::writeXMLOutput
     */
    virtual void execute();


protected:
    /// @brief The file to write the output to
    OutputDevice& myDevice;

    /// @brief The logic to use
    const MSTLLogicControl::TLSLogicVariants& myLogics;

    /// @brief The detector to use
    MSDetectorFileOutput* myDetector;

    /// @brief The last time the values were written
    SUMOTime myStartTime;


private:
    /// @brief Invalidated copy constructor.
    Command_SaveTLCoupledDet(const Command_SaveTLCoupledDet&);

    /// @brief Invalidated assignment operator.
    Command_SaveTLCoupledDet& operator=(const Command_SaveTLCoupledDet&);

};


#endif

/****************************************************************************/

