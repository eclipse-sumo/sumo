/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2002-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    XMLSubSys.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 1 Jul 2002
///
// Utility methods for initialising, closing and using the XML-subsystem
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <vector>
#include <xercesc/sax2/SAX2XMLReader.hpp>


// ===========================================================================
// class declarations
// ===========================================================================
class GenericSAXHandler;
class SUMOSAXHandler;
class SUMOSAXReader;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class XMLSubSys
 * @brief Utility methods for initialising, closing and using the XML-subsystem
 *
 * The Xerces-parsers need an initialisation and should also be closed.
 *
 * As we use xerces for both the input files and the configuration we
 *  would have to check whether the system was initialised before. Instead,
 *  we call XMLSubSys::init(bool) once at the beginning of our application and
 *  XMLSubSys::close() at the end.
 *
 * Closing and initialising the XML subsystem is necessary. Still, we never
 *  encountered any problems with it. Once, after some modifications, SUMO
 *  crashed when closing the XML sub system. The reason was a memory leak
 *  within the microsim-module. On initialisation, a SAX2XMLReader is built
 *  which can be used during later process. It is destroyed when the subsystem
 *  is closed.
 *
 * In addition to initialisation and shutdown, this module allows to build
 *  SAXReaders and/or running a given handler on a given file without
 *  dealing with the reader at all.
 */
class XMLSubSys {
public:
    /**
     * @brief Initialises the xml-subsystem.
     *
     * Calls XMLPlatformUtils::Initialize(). If this fails, the exception is
     *  caught and its content is reported using a ProcessError.
     *
     * @exception ProcessError If the initialisation fails
     */
    static void init();


    /**
    * @brief Enables or disables validation.
    *
    * The setting is only valid for parsers created after the call. Existing parsers are not adapted.
    *
    * @param[in] validationScheme Whether validation of XML-documents against schemata shall be enabled
    * @param[in] netValidationScheme Whether validation of SUMO networks against schemata shall be enabled
    */
    static void setValidation(const std::string& validationScheme, const std::string& netValidationScheme, const std::string& routeValidationScheme);


    /**
     * @brief Closes the xml-subsystem
     *
     * Deletes the built reader and calls XMLPlatformUtils::Terminate();
     */
    static void close();


    /**
     * @brief Builds a reader and assigns the handler to it
     *
     * Tries to build a SAX2XMLReader using "getSAXReader()". If this
     *  fails, 0 is returned. Otherwise, the given handler is assigned
     *  to the reader as the current DefaultHandler and ErrorHandler.
     *
     * @param[in] handler The handler to assign to the built reader
     * @param[in] isNet   whether a network gets loaded
     * @param[in] isRoute whether routes get loaded
     * @return The built Xerces-SAX-reader, 0 if something failed
     * @see getSAXReader()
     */
    static SUMOSAXReader* getSAXReader(SUMOSAXHandler& handler,
                                       const bool isNet = false, const bool isRoute = false);


    /**
     * @brief Sets the given handler for the default reader
     *
     * Uses the reader built on init() which is stored in myReader.
     *
     * @param[in] handler The handler to assign to the built reader
     */
    static void setHandler(GenericSAXHandler& handler);


    /**
     * @brief Runs the given handler on the given file; returns if everything's ok
     *
     * Uses the reader built on init() which is stored in myReader to parse the given
     *  file.
     *
     * All exceptions are catched and reported to the error-instance of the MsgHandler.
     *  Also, if the reader could not be built, this is reported.
     *
     * The method returns true if everything went ok. This means, that the reader could be
     *  built, no exception was caught, and nothing was reported to the error-instance
     *  of the MsgHandler.
     *
     * @param[in] handler The handler to assign to the built reader
     * @param[in] file    The file to run the parser at
     * @param[in] isNet   whether a network gets loaded
     * @param[in] isRoute whether routes get loaded
     * @param[in] isExternal whether it is an external file like matsim or opendrive
     * @param[in] catchExceptions whether exceptions on parsing should be caught or transferred into a ProcessError
     * @return true if the parsing was done without errors, false otherwise (error was printed)
     */
    static bool runParser(GenericSAXHandler& handler, const std::string& file,
                          const bool isNet = false, const bool isRoute = false,
                          const bool isExternal = false, const bool catchExceptions = true);


private:
    static std::string warnLocalScheme(const std::string& newScheme, const bool haveSUMO_HOME);

private:
    /// @brief The XML Readers used for repeated parsing
    static std::vector<SUMOSAXReader*> myReaders;

    /// @brief Information whether the reader is parsing
    static int myNextFreeReader;

    /// @brief Information whether built reader/parser shall validate XML-documents against schemata
    static std::string myValidationScheme;

    /// @brief Information whether built reader/parser shall validate SUMO networks against schemata
    static std::string myNetValidationScheme;

    /// @brief Information whether built reader/parser shall validate SUMO routes against schemata
    static std::string myRouteValidationScheme;

    /// @brief Schema cache to be used for grammars which are not declared
    static XERCES_CPP_NAMESPACE::XMLGrammarPool* myGrammarPool;

    /// @brief Whether a warning about missing SUMO_HOME should be emitted
    static bool myNeedsValidationWarning;

};
