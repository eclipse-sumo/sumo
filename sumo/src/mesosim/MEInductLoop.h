/****************************************************************************/
/// @file    MEInductLoop.h
/// @author  Daniel Krajzewicz
/// @date    Tue, May 2005
/// @version $Id: MEInductLoop.h 96 2007-06-06 07:40:46Z behr_mi $
///
// An induction loop for mesoscopic simulation
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
#include <utils/iodevices/OutputDevice.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MESegment;
class GUIDetectorWrapper;
class GUIGlObjectStorage;
class GUILaneWrapper;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MEInductLoop
 * @brief An induction loop for mesoscopic simulation
 */
class MEInductLoop
            : public MSDetectorFileOutput
{
public:
    MEInductLoop(const std::string& id,
                 MESegment * s,
                 SUMOReal positionInMeters,
                 SUMOTime deleteDataAfterSeconds);


    ~MEInductLoop();

    const std::string &getID()
    {
        return myID;
    }

    /**
     * @name Inherited MSDetectorFileOutput methods.
     *
     * @see MSDetectorFileOutput
     */
    //@{
    /**
     * Get a header for file output via MSDetector2File
     *
     * @return The static XML-Header which explains the output of getXMLOutput.
     *
     * @see MSDetector2File
     * @see getXMLOutput
     */
    void writeXMLHeader(OutputDevice &dev) const;


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
    void writeXMLOutput(OutputDevice &dev,
                        SUMOTime startTime, SUMOTime stopTime);

    /**
     * Get an opening XML-element containing information about the detector.
     *
     * @return <detector type="inductionloop" id="det_id"
     * lane="lane_id" pos="det_pos">
     *
     * @see MSDetector2File
     * @see getXMLDetectorInfoEnd
     */
    void writeXMLDetectorInfoStart(OutputDevice &dev) const;


    /**
     * Get the closing XML-element to getXMLDetectorInfoStart
     *
     * @return </detector>
     *
     * @see MSDetector2File
     * @see getXMLDetectorInfoStart
     */
    void writeXMLDetectorInfoEnd(OutputDevice &dev) const;

    /**
     * Get the data-clean up interval in timesteps.
     *
     */
    SUMOTime getDataCleanUpSteps(void) const;
    //@}

    MESegment &getSegment() const
    {
        return *mySegment;
    }


    /**
     * Deletes data from vehicleDataContM if deleteDataAfterStepsM
     * is over. Is called via MSEventControl.
     *
     * @return deleteDataAfterStepsM to recur the event.
     *
     * @see MSEventControl
     */
    /*
    SUMOTime deleteOldData( void );
    */

protected:
    const SUMOReal posM;          /**< Detector's position on lane [cells]. */

    SUMOTime deleteDataAfterStepsM; /**< Deletion interval. */

    std::string myID;

    MESegment *mySegment;

private:
    static std::string xmlHeaderM;
    static std::string xmlDetectorInfoEndM;

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

