/****************************************************************************/
/// @file    ROJTRTurnDefLoader.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Jan 2004
/// @version $Id$
///
// Loader for the description of turning percentages
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
#ifndef ROJTRTurnDefLoader_h
#define ROJTRTurnDefLoader_h
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

#include <set>
#include <string>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/importio/NamedColumnsParser.h>
#include <utils/importio/LineHandler.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ROJTREdge;
class RONet;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROJTRTurnDefLoader
 * Lays the given route over the edges using the dijkstra algorithm
 */
class ROJTRTurnDefLoader : public SUMOSAXHandler,
            public LineHandler
{
public:
    /// Constructor
    ROJTRTurnDefLoader(RONet &net);

    /// Destructor
    ~ROJTRTurnDefLoader();

    /** @brief Loads the turning definitions and additionally the sinks
        While the sinks are returned on return, the turn definitions are stored
        into the network directly */
    std::set<ROJTREdge*> load(const std::string &file);

    /** @brief used when csv instead of xml-descriptions are used */
    bool report(const std::string &line);

protected:
    /// SAX-callback for tag start
    void myStartElement(int element, const std::string &name,
                        const Attributes &attrs);

    /// SAX-callback for characters section
    void myCharacters(int element, const std::string &name,
                      const std::string &chars);

    /// SAX-callback for tag end
    void myEndElement(int element, const std::string &name);

private:
    /// Begins the processing of an interval
    void beginInterval(const Attributes &attrs);

    /// Begins the processing of a incoming definition
    void beginFromEdge(const Attributes &attrs);

    /** @brief Parses the percentage with which an outgoing edge is used
        This is added to the current incoming edge */
    void addToEdge(const Attributes &attrs);

    /// Parses the given string as a list of edge names to declare them as sinks
    void addSink(const std::string &chars);

    /// Ends the processing of an interval
    void endInterval();

    /// Ends the processing of an incoming edge
    void endFromEdge();

    /** @brief Returns a value from the columns parser
        Catches and reports errors */
    std::string getSecure(const std::string &name);

private:
    /// The begin and the end of the current interval
    unsigned int myIntervalBegin, myIntervalEnd;

    /// The network to set the information into
    RONet &myNet;

    /// The list of parsed sinks
    std::set<ROJTREdge*> mySinks;

    /// The current incoming edge the turning probabilities are set into
    ROJTREdge *myEdge;

    /// Information whether this loader was initialised for parsing lines
    bool myAmInitialised;

    /// The parser used in the case of csv-files
    NamedColumnsParser myColumnsParser;

};


#endif

/****************************************************************************/

