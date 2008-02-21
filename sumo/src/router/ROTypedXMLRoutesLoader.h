/****************************************************************************/
/// @file    ROTypedXMLRoutesLoader.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Base class for loading routes from XML-files
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
// xerces 2.2 compatibility
// ===========================================================================
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif


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
     */
    ROTypedXMLRoutesLoader(ROVehicleBuilder &vb, RONet &net,
                           SUMOTime begin, SUMOTime end, const std::string &file="");


    /// @brief Destructor
    virtual ~ROTypedXMLRoutesLoader() throw();


    /** @brief Initialises the handler for reading
     * 
     * Normally, is used to parse the first element.
     *
     * @param[in] options Options to use (if needed)
     * @todo Recheck usage of route loaders; it is not very fine to have one that use a parser and other that do not
     */
    virtual bool init(OptionsCont &options);


    /** @brief Closes reading of routes
     * 
     * Normally, the parser is reset; still, both loaders that work on memory 
     *  (RORDGenerator_Random and RORDGenerator_ODAmounts) do not have a parser
     *  at all. Due to this, the method may be overridden.
     *
     * @todo Recheck usage of route loaders; it is not very fine to have one that use a parser and other that do not
     */
    virtual void closeReading();


    /** @brief Called when the document has ended
     *
     * @todo Recheck usage of route loaders; it is not very fine to have one that use a parser and other that do not
     */
    void endDocument();


    /** @brief Returns the information whether no routes are available from this loader anymore
     *
     * @return Whether all routes have been parsed
     */
    bool ended() const;


protected:
    /** @brief Reads until the specified time is reached
     *
     * @param[in] time The time at which the loader shall stop parsing
     * @todo recheck/refactor
     */
    bool myReadRoutesAtLeastUntil(SUMOTime time);


protected:
    /// @name Virtual methods to implement by derived classes
    /// @{

    /** Returns the information whether a route was read
     *
     * @return Whether a further route was read
     * @todo recheck/refactor
     */
    virtual bool nextRouteRead() = 0;


    /** @brief Returns Initialises the reading of a further route
     *
     * @todo recheck/refactor
     */
    virtual void beginNextRoute() = 0;
    /// @}


protected:
    /// @brief The parser used
    SAX2XMLReader *myParser;

    /// @brief Information about the current position within the file
    XMLPScanToken myToken;

    /// @brief Information whether the whole file has been parsed
    bool myEnded;

};


#endif

/****************************************************************************/

