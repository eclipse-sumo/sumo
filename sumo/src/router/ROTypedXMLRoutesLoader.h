/****************************************************************************/
/// @file    ROTypedXMLRoutesLoader.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Base class for loading routes from XML-files
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOSAXReader.h>


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
class ROTypedXMLRoutesLoader : public SUMOSAXHandler {
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
    ROTypedXMLRoutesLoader(RONet& net,
                           SUMOTime begin, SUMOTime end, const std::string& file = "");


    /// @brief Destructor
    virtual ~ROTypedXMLRoutesLoader();


    /** @brief Adds routes from the file until the given time is reached
     *
     * @param[in] time The time until which route definitions shall be loaded
     * @return Whether any errors occured
     * @exception ProcessError If a major error occured
     * @see ROAbstractRouteDefLoader::readRoutesAtLeastUntil
     */
    virtual bool readRoutesAtLeastUntil(SUMOTime time);


    /** @brief Returns the time the current (last read) route starts at
     *
     * @return The least time step that was read by this reader
     */
    SUMOTime getLastReadTimeStep() const {
        return myCurrentDepart;
    }


    /** @brief Returns the information whether no routes are available from this loader anymore
     *
     * @return Whether the whole input has been processed
     */
    bool ended() const {
        return myEnded;
    }


    /** @brief Called when the document has ended
     *
     * @todo Recheck usage of route loaders; it is not very fine to have one that use a parser and other that do not
     */
    void endDocument();


protected:
    /// @brief The network to add routes to
    RONet& myNet;

    /// @brief The time for which the first route shall be loaded
    SUMOTime myBegin;

    /// @brief The time for which the first route shall be loaded
    SUMOTime myEnd;

    /// @brief The parser used
    SUMOSAXReader* myParser;

    /// @brief The currently read vehicle's depart
    SUMOTime myCurrentDepart;

    /// @brief The information whether the next route was read
    bool myNextRouteRead;

    /// @brief Information whether the whole file has been parsed
    bool myEnded;


private:
    /// @brief Invalidated copy constructor
    ROTypedXMLRoutesLoader(const ROTypedXMLRoutesLoader& src);

    /// @brief Invalidated assignment operator
    ROTypedXMLRoutesLoader& operator=(const ROTypedXMLRoutesLoader& src);

};


#endif

/****************************************************************************/

