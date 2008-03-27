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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <set>
#include <string>
#include <utils/xml/SUMOSAXHandler.h>
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
    ~ROJTRTurnDefLoader() throw();

    /** @brief Loads the turning definitions and additionally the sinks
        While the sinks are returned on return, the turn definitions are stored
        into the network directly */
    void load(const std::string &file);

    /** @brief used when csv instead of xml-descriptions are used */
    bool report(const std::string &line) throw(ProcessError);

protected:
    /// @name inherited from GenericSAXHandler
    //@{

    /** @brief Called on the opening of a tag;
     *
     * @param[in] element ID of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myStartElement
     */
    void myStartElement(SumoXMLTag element,
                        const SUMOSAXAttributes &attrs) throw(ProcessError);


    /** @brief Called when characters occure
     *
     * @param[in] element ID of the last opened element
     * @param[in] chars The read characters (complete)
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myCharacters
     */
    void myCharacters(SumoXMLTag element,
                      const std::string &chars) throw(ProcessError);


    /** @brief Called when a closing tag occures
     *
     * @param[in] element ID of the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myEndElement
     */
    void myEndElement(SumoXMLTag element) throw(ProcessError);
    //@}


private:
    /// Begins the processing of an interval
    void beginInterval(const SUMOSAXAttributes &attrs);

    /// Begins the processing of a incoming definition
    void beginFromEdge(const SUMOSAXAttributes &attrs);

    /** @brief Parses the percentage with which an outgoing edge is used
        This is added to the current incoming edge */
    void addToEdge(const SUMOSAXAttributes &attrs);

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

