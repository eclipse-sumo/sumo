/****************************************************************************/
/// @file    MSVTypeProbe.h
/// @author  Tino Morenz
/// @date    Wed, 24.10.2007
/// @version $Id$
///
// Writes positions of vehicles that have a certain (named) type
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSVTypeProbe_h
#define MSVTypeProbe_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/common/Command.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/Named.h>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSVTypeProbe
 * @brief Writes positions of vehicles that have a certain (named) type
 *
 * This device allows to log the data of all running vehicles of the
 *  specified vehicle type, i.e. vehicle id, edge, lane=, position
 *  on lane, x/y coordinates and speed.
 *
 * A frequency can be specified to generate the output in certain intervals,
 *  (e.g. every 10 seconds).
 *
 * @see Command
 * @see Named
 */

class MSVTypeProbe : public Named, public Command {
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the vehicle type probe
     * @param[in] vType The vtype of which vehicles to report must be ("" for all vehicles)
     * @param[in] od The output device to write into
     * @param[in] frequency The output frequency [ms]
     */
    MSVTypeProbe(const std::string &id, const std::string &vType,
                 OutputDevice &od, SUMOTime frequency) throw();


    /// @brief Destructor
    virtual ~MSVTypeProbe() throw();



    /// @name Derived from Command
    /// @{

    /** @brief Writes values into the given stream
     *
     * This method goes through all runing vehicles; if a vehicle
     *  has a type with the same id as the wished one, it is reported.
     * When the type "" is wished, all vehicles are reported
     *
     * @param[in] currentTime The current simulation time (unused)
     * @return Always myFrequency (time till next output)
     * @exception ProcessError not here
     * @see Command
     */
    SUMOTime execute(SUMOTime currentTime) throw(ProcessError);
    /// @}


private:
    /// @brief The id of the vehicle type vehicles must have to be reported
    std::string myVType;

    /// @brief The device to write into
    OutputDevice &myOutputDevice;

    /// @brief The frequency of reporting
    SUMOTime myFrequency;


private:
    /// @brief Invalidated copy constructor.
    MSVTypeProbe(const MSVTypeProbe&);

    /// @brief Invalidated assignment operator.
    MSVTypeProbe& operator=(const MSVTypeProbe&);


};

#endif

/****************************************************************************/

