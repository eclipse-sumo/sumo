/****************************************************************************/
/// @file    Command_SaveTLCoupledDet.h
/// @author  Daniel Krajzewicz
/// @date    15 Feb 2004
/// @version $Id$
///
// Realises the output of a tls values on each switch
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef Command_SaveTLCoupledDet_h
#define Command_SaveTLCoupledDet_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <microsim/MSNet.h>
#include <utils/common/DiscreteCommand.h>
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
 * Called on every tls-switch, the "execute" writes the current detector
 *  values into a file.
 * This action is build only if the user wants and describes it within the
 *  additional-files.
 */
class Command_SaveTLCoupledDet : public DiscreteCommand
{
public:
    /// Constructor
    Command_SaveTLCoupledDet(
        MSTLLogicControl::TLSLogicVariants &tlls,
        MSDetectorFileOutput *dtf, unsigned int begin,
        OutputDevice& device);

    /// Destructor
    virtual ~Command_SaveTLCoupledDet();

    /// Executes the command (see above)
    virtual bool execute();

protected:
    /// The file to write the output to
    OutputDevice& myDevice;

    /// The logic to use
    const MSTLLogicControl::TLSLogicVariants &myLogics;

    /// The detector to use
    MSDetectorFileOutput *myDetector;

    /// The last time the values were written
    unsigned int myStartTime;

private:
    /// @brief Invalidated copy constructor.
    Command_SaveTLCoupledDet(const Command_SaveTLCoupledDet&);

    /// @brief Invalidated assignment operator.
    Command_SaveTLCoupledDet& operator=(const Command_SaveTLCoupledDet&);

};


#endif

/****************************************************************************/

