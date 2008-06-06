/****************************************************************************/
/// @file    MSVTypeProbe.h
/// @author  Tino Morenz
/// @date    Wed, 24.10.2007
/// @version $Id: $
///
// Writes positions of vehicles that have a certain (named) type
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
#include <microsim/output/MSDetectorFileOutput.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/Named.h>


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
 *  (e.g. every 10 seconds) and is used via the detector control by
 *  calling the appropriate methods derived from MSDetectorFileOutput.
 *
 * @see MSDetectorFileOutput
 * @see Named
 */

class MSVTypeProbe : public MSDetectorFileOutput, public Named
{
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the vehicle type probe
     * @param[in] vType The vtype of which vehicles to report must be ("" for all vehicles)
     */
    MSVTypeProbe(const std::string &id, 
                 const std::string &vType) throw();


    /// @brief Destructor
    virtual ~MSVTypeProbe() throw();



    /// @name Methods inherited from MSDetectorFileOutput.
    /// @{

    /** @brief Writes values into the given stream
     *
     * This method goes through all runing vehicles; if a vehicle
     *  has a type with the same id as the wished one, it is reported.
     * When the type "" is wished, all vehicles are reported
     *
     * @param[in] dev The output device to write the data into
     * @param[in] startTime First time step the data were gathered
     * @param[in] stopTime Last time step the data were gathered
     * @see MSDetectorFileOutput::writeXMLOutput
     * @exception IOError If an error on writing occures (!!! not yet implemented)
     */
    void writeXMLOutput(OutputDevice &dev,
        SUMOTime startTime, SUMOTime stopTime) throw(IOError);


    /** @brief Opens the XML-output using "detector" as root element
     *
     * @param[in] dev The output device to write the root into
     * @see MSDetectorFileOutput::writeXMLDetectorProlog
     * @todo What happens with the additional information if several detectors use the same output?
     * @exception IOError If an error on writing occures (!!! not yet implemented)
     */
    void writeXMLDetectorProlog(OutputDevice &dev) const throw(IOError);
    /// @}


private:
    /// @brief The id of the vehicle type vehicles must have to be reported
    std::string myVType;


private:
    /// @brief Invalidated copy constructor.
    MSVTypeProbe(const MSVTypeProbe&);

    /// @brief Invalidated assignment operator.
    MSVTypeProbe& operator=(const MSVTypeProbe&);


};

#endif

/****************************************************************************/

