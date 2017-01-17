/****************************************************************************/
/// @file    MEInductLoop.h
/// @author  Daniel Krajzewicz
/// @date    Tue, May 2005
/// @version $Id$
///
// An induction loop for mesoscopic simulation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MEInductLoop_h
#define MEInductLoop_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <deque>
#include <map>
#include <functional>
#include <microsim/output/MSDetectorFileOutput.h>
#include <microsim/output/MSMeanData_Net.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/Named.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MESegment;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MEInductLoop
 * @brief An induction loop for mesoscopic simulation
 */
class MEInductLoop : public MSDetectorFileOutput {
public:
    MEInductLoop(const std::string& id,
                 MESegment* s,
                 SUMOReal positionInMeters,
                 const std::string& vTypes);


    ~MEInductLoop();

    /**
     * @name Inherited MSDetectorFileOutput methods.
     *
     * @see MSDetectorFileOutput
     */
    //@{
    /** @brief Opens the XML-output using "detector" as root element
     *
     * @param[in] dev The output device to write the root into
     * @see MSDetectorFileOutput::writeXMLDetectorProlog
     */
    void writeXMLDetectorProlog(OutputDevice& dev) const {
        dev.writeXMLHeader("detector", "det_e1meso_file.xsd");
    }


    /**
     * Get the XML-formatted output of all the get*-methods except
     * getTimestepsSinceLastDetection.
     *
     * @param lastNTimesteps take data out of the interval
     * (now-lastNTimesteps, now].
     *
     * @return XML-formatted output of all the get*-methods except
     * getTimestepsSinceLastDetection.
     *
     * @see MSDetector2File
     */
    void writeXMLOutput(OutputDevice& dev,
                        SUMOTime startTime, SUMOTime stopTime);
    //@}

protected:
    /// @brief mesoscopic edge segment the loop lies on
    MESegment* const mySegment;

    /// @brief position from the start of the edge / lane
    const SUMOReal myPosition;

    /// @brief data collector for the loop
    MSMeanData_Net::MSLaneMeanDataValues myMeanData;

private:

    /// Hidden default constructor.
    MEInductLoop();

    /// Hidden copy constructor.
    MEInductLoop(const MEInductLoop&);

    /// Hidden assignment operator.
    MEInductLoop& operator=(const MEInductLoop&);
};

#endif

/****************************************************************************/

