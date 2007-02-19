/****************************************************************************/
/// @file    MSWeightsHandler.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A SAX-handler for loading SUMO-weights (aggregated dumps)
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
#ifndef MSWeightsHandler_h
#define MSWeightsHandler_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/sumoxml/SUMOSAXHandler.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;
class RONet;
class ROEdge;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSWeightsHandler
 * A XML-handler for network weights a simulation generates and saves using
 * the aggregated-output.
 */
class MSWeightsHandler : public SUMOSAXHandler
{
public:
    /// Constructor
    MSWeightsHandler(const OptionsCont &oc, MSNet &net,
                     const std::string &file, bool useLanes);

    /// Destructor
    ~MSWeightsHandler();

protected:
    /** the user-impemlented handler method for an opening tag */
    void myStartElement(int element, const std::string &name,
                        const Attributes &attrs);

    /** the user-implemented handler method for characters */
    void myCharacters(int element, const std::string &name,
                      const std::string &chars);

    /** the user-implemented handler method for a closing tag */
    void myEndElement(int element, const std::string &name);

private:
    /// Parses the information for which time steps the following values are valid
    void parseTimeStep(const Attributes &attrs);

    /// Parses the efforts of an edge for the previously read times
    void parseEdge(const Attributes &attrs);

    /// Parses the efforts of a lane for the previously read times
    void parseLane(const Attributes &attrs);

private:
    /// the programm settings (options/configuration)
    const OptionsCont &_options;

    /// the previously build net to store the weights in
    MSNet &_net;

    /// the name of the value to extract; called scheme for further extensions
    std::string _scheme;

    /// the begin of the time period that is currently being processed
    SUMOTime _currentTimeBeg;
    /// the end of the time period that is currently being processed
    SUMOTime _currentTimeEnd;

    /// the edge the is currently being processed
    MSEdge *_currentEdge;

    /// aggregated value over the lanes read within the current edge
    SUMOReal myAggValue;

    /// The number of lanes read for the current edge
    size_t myNoLanes;

    /// Whether the values shall be read from lanes
    bool myUseLanes;


private:
    /// we made the copy constructor invalid
    MSWeightsHandler(const MSWeightsHandler &src);

    /// we made the assignment operator invalid
    MSWeightsHandler &operator=(const MSWeightsHandler &src);

};


#endif

/****************************************************************************/

