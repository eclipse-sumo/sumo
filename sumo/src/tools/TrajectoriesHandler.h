/****************************************************************************/
/// @file    TrajectoriesHandler.h
/// @author  Michael Behrisch
/// @date    14.03.2014
/// @version $Id$
///
// An XML-Handler for amitran and netstate trajectories
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2014-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef TrajectoriesHandler_h
#define TrajectoriesHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utility>
#include <utils/xml/SUMOSAXHandler.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TrajectoriesHandler
 * @brief An XML-Handler for amitran and netstate trajectories
 *
 * This SUMOSAXHandler parses vehicles and their speeds.
 */
class TrajectoriesHandler : public SUMOSAXHandler {
public:
    static const int INVALID_VALUE = -999999;

public:
    /** @brief Constructor
     *
     * @param[in] file The file that will be processed
     */
    TrajectoriesHandler(const bool computeA, const bool computeAForward, const bool accelZeroCorrection,
                        const SUMOEmissionClass defaultClass,
                        const SUMOReal defaultSlope, std::ostream* stdOut, OutputDevice* xmlOut);


    /// @brief Destructor
    ~TrajectoriesHandler();

    const PollutantsInterface::Emissions computeEmissions(const std::string id,
            const SUMOEmissionClass c, SUMOReal& v,
            SUMOReal& a, SUMOReal& s);

    bool writeEmissions(std::ostream& o, const std::string id,
                        const SUMOEmissionClass c,
                        SUMOReal t, SUMOReal& v,
                        SUMOReal& a, SUMOReal& s);

    bool writeXMLEmissions(const std::string id,
                           const SUMOEmissionClass c,
                           SUMOTime t, SUMOReal& v,
                           SUMOReal a = INVALID_VALUE, SUMOReal s = INVALID_VALUE);

    void writeSums(std::ostream& o, const std::string id);

    void writeNormedSums(std::ostream& o, const std::string id, const SUMOReal factor);


protected:
    /// @name inherited from GenericSAXHandler
    //@{

    /** @brief Called when an opening-tag occurs
     *
     * Processes vehicle and motionState elements.
     *
     * @param[in] element The enum of the currently opened element
     * @param[in] attrs Attributes of the currently opened element
     * @exception ProcessError If an error within the parsed file occurs
     * @see GenericSAXHandler::myStartElement
     */
    void myStartElement(int element,
                        const SUMOSAXAttributes& attrs);
    //@}


private:
    const bool myComputeA;
    const bool myComputeAForward;
    const bool myAccelZeroCorrection;
    const SUMOEmissionClass myDefaultClass;
    const SUMOReal myDefaultSlope;
    std::ostream* myStdOut;
    OutputDevice* myXMLOut;
    std::map<std::string, SUMOReal> myLastV;
    SUMOTime myCurrentTime;
    SUMOReal myStepSize;
    std::map<std::string, PollutantsInterface::Emissions> mySums;
    std::map<std::string, SUMOEmissionClass> myEmissionClassByType;
    std::map<std::string, SUMOEmissionClass> myEmissionClassByVehicle;


private:
    /// @brief invalidated copy constructor
    TrajectoriesHandler(const TrajectoriesHandler& s);

    /// @brief invalidated assignment operator
    TrajectoriesHandler& operator=(const TrajectoriesHandler& s);


};


#endif

/****************************************************************************/

