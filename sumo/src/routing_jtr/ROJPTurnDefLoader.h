#ifndef ROJPTurnDefLoader_h
#define ROJPTurnDefLoader_h
//---------------------------------------------------------------------------//
//                        ROJPTurnDefLoader.h -
//      Loader for the description of turning percentages
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 20 Jan 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.2  2004/11/23 10:26:59  dkrajzew
// debugging
//
// Revision 1.1  2004/02/06 08:43:46  dkrajzew
// new naming applied to the folders (jp-router is now called jtr-router)
//
// Revision 1.1  2004/01/26 06:09:11  dkrajzew
// initial commit for jp-classes
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <set>
#include <string>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/importio/NamedColumnsParser.h>
#include <utils/importio/LineHandler.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class ROJPEdge;
class RONet;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROJPTurnDefLoader
 * Lays the given route over the edges using the dijkstra algorithm
 */
class ROJPTurnDefLoader : public SUMOSAXHandler,
                          public LineHandler
{
public:
    /// Constructor
    ROJPTurnDefLoader(RONet &net);

    /// Destructor
    ~ROJPTurnDefLoader();

    /** @brief Loads the turning definitions and additionally the sinks
        While the sinks are returned on return, the turn definitions are stored
        into the network directly */
    std::set<ROJPEdge*> load(const std::string &file);

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
    std::set<ROJPEdge*> mySinks;

    /// The current incoming edge the turning percentages are set into
    ROJPEdge *myEdge;

    /// Information whether this loader was initialised for parsing lines
    bool myAmInitialised;

    /// The parser used in the case of csv-files
    NamedColumnsParser myColumnsParser;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

