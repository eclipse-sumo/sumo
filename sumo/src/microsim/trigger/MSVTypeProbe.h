/****************************************************************************/
/// @file    MSVTypeProbe.h
/// @author  Tino Morenz
/// @date    Wed, 24.10.2007
/// @version $Id: $
///
// A probe for a specific vehicle type
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
#include <microsim/trigger/MSTrigger.h>
#include <utils/iodevices/OutputDevice.h>


// ===========================================================================
// class declarations
// ===========================================================================



// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSVTypeProbe
 * @brief A probe for a specific vehicle type
 *
 * This device allows to log the data of all running vehicles of the
 * specified vehicle type, i.e. vehicle id, edge, lane=, position
 * on lane, x/y coordinates and speed. A frquency can be specified to
 * generate the output in certain intervals (e.g. every 10 seconds)
 */

class MSVTypeProbe : public MSTrigger
{
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the vehicle type probe
     * @param[in] net The net the vehicle type probe belongs to
     * @param[in] file Name of the file to read the definitions from
     * @param[in] vType !!!describe
     * @param[in] probeFreq !!!describe
     * @todo Recheck and describe parameter
     */
    MSVTypeProbe(const std::string &id, MSNet &net,
                 const std::string &file, const std::string &vType,
                 SUMOTime probeFreq) throw();

    virtual ~MSVTypeProbe() throw();

protected:
    SUMOTime execute(SUMOTime currentTime) throw(ProcessError);

    void writeXMLProlog();
    void writeXMLEpilog();

    MSNet &myNet;
    SUMOTime myProbeFreq;
    std::string myId;
    std::string myFileName;
    std::string myVType;
    OutputDevice &myOutDev;
    std::vector<const MSVehicle *> myVehicles;
};

#endif

/****************************************************************************/

