/****************************************************************************/
/// @file    ROTypedXMLRoutesLoader.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Base class for loading routes from XML-files
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef ROTypedXMLRoutesLoader_h
#define ROTypedXMLRoutesLoader_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/framework/XMLPScanToken.hpp>
#include <utils/xml/SUMOSAXHandler.h>
#include "ROAbstractRouteDefLoader.h"


// ===========================================================================
// class declarations
// ===========================================================================
class RONet;
class Options;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROTypedXMLRoutesLoader
 * @brief Base class for loading routes from XML-files
 *
 * Base class for loaders which load route definitions which use XML-derived
 *  formats. Some methods as the initialisation and the file processing, together
 *  with the need for a parser are common to all such loaders.
 *
 * @todo recheck/refactor
 */
class ROTypedXMLRoutesLoader : public ROAbstractRouteDefLoader,
            public SUMOSAXHandler
{
public:
    /** @brief Constructor
     *
     * @param[in] vb The vehicle builder to use
     * @param[in] net The network to add vehicles to
     * @param[in] begin The time step import shall start at
     * @param[in] end The time step import shall stop at
     * @param[in] file Name of the used input file
     * @exception ProcessError If an error occured during initialisation of parsing the xml-file
     */
    ROTypedXMLRoutesLoader(RONet &net,
                           SUMOTime begin, SUMOTime end, const std::string &file="") throw(ProcessError);


    /// @brief Destructor
    virtual ~ROTypedXMLRoutesLoader() throw();


    /// @name inherited from ROAbstractRouteDefLoader
    //@{

    /** @brief Adds routes from the file until the given time is reached
     *
     * @param[in] time The time until which route definitions shall be loaded
     * @param[in] skipping Whether routes shall not be added
     * @return Whether any errors occured
     * @exception ProcessError If a major error occured
     * @see ROAbstractRouteDefLoader::readRoutesAtLeastUntil
     */
    bool readRoutesAtLeastUntil(SUMOTime time, bool skipping) throw(ProcessError);


    /** @brief Returns the information whether no routes are available from this loader anymore
     *
     * @return Whether the whole input has been processed
     */
    bool ended() const throw() {
        return myEnded;
    }
    /// @}


    /** @brief Called when the document has ended
     *
     * @todo Recheck usage of route loaders; it is not very fine to have one that use a parser and other that do not
     */
    void endDocument();



protected:
    /// @name Virtual methods to implement by derived classes
    /// @{

    /** Returns the information whether a route was read
     *
     * @return Whether a further route was read
     * @todo recheck/refactor
     */
    virtual bool nextRouteRead() throw() = 0;


    /** @brief Returns Initialises the reading of a further route
     *
     * @todo recheck/refactor
     */
    virtual void beginNextRoute() throw() = 0;
    /// @}


protected:
    /// @brief The parser used
    SAX2XMLReader *myParser;

    /// @brief Information about the current position within the file
    XMLPScanToken myToken;

    /// @brief Information whether the whole file has been parsed
    bool myEnded;


private:
    /// @brief Invalidated copy constructor
    ROTypedXMLRoutesLoader(const ROTypedXMLRoutesLoader &src);

    /// @brief Invalidated assignment operator
    ROTypedXMLRoutesLoader &operator=(const ROTypedXMLRoutesLoader &src);

};


#endif

/****************************************************************************/

