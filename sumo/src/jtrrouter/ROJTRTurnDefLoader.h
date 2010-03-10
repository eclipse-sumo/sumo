/****************************************************************************/
/// @file    ROJTRTurnDefLoader.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Jan 2004
/// @version $Id$
///
// Loader for the of turning percentages and source/sink definitions
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
 * @brief Loader for the of turning percentages and source/sink definitions
 *
 * This handler parses XML-descriptions of jtrrouter-definitions, including
 *  percentage ratios at junctions and definitions of sink/source edges.
 *
 * All read values are stored directly into the given network's structures
 *   (edges).
 */
class ROJTRTurnDefLoader : public SUMOSAXHandler {
public:
    /** @brief Constructor
     *
     * @param[in] net The net to add loaded turning percentages into
     */
    ROJTRTurnDefLoader(RONet &net) throw();


    /// @brief Destructor
    ~ROJTRTurnDefLoader() throw();


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
    //@}


private:
    /** @brief Begins the processing of a incoming edge definition
     *
     * Tries to retrieve the currently described incoming edge. If the
     *  edge id is not given in the attributes or the edge is not known,
     *  an error is reported.
     *
     * If everything is ok, the edge's address is stored in myEdge.
     *
     * @param[in] attrs The SAX-attributes to parse incoming edge from
     */
    void beginFromEdge(const SUMOSAXAttributes &attrs) throw();


    /** @brief Parses the probability to use a certain outgoing edge
     *
     * Tries to retreive the outgoing edge and then the probability to
     *  use it. If one of both operations could not be accomplished,
     *  an error is generated.
     *
     * If everything is ok, this means the destination edge is defined
     *  and known and the probability is valid, too, this probability
     *  is added to "myEdge", the last parsed incoming edge. As time,
     *  the previously parsed interval begin/end is used.
     *
     * @param[in] attrs The SAX-attributes to parse the destination edge and the probability to use it from
     */
    void addToEdge(const SUMOSAXAttributes &attrs) throw();


private:
    /// The network to set the information into
    RONet &myNet;

    /// @brief The begin and the end of the current interval
    SUMOTime myIntervalBegin, myIntervalEnd;

    /// @brief The current incoming edge the turning probabilities are set into
    ROJTREdge *myEdge;

    /// @brief Whether the loader has warned about deprecated usage of characters
    bool myHaveWarnedAboutDeprecatedSources, myHaveWarnedAboutDeprecatedSinks;

};


#endif

/****************************************************************************/

