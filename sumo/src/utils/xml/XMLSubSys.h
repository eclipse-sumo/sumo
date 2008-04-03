/****************************************************************************/
/// @file    XMLSubSys.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 1 Jul 2002
/// @version $Id$
///
// Utility methods for initialising, closing and using the XML-subsystem
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
#ifndef XMLSubSys_h
#define XMLSubSys_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class SUMOSAXHandler;


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
 *  we call XMLSubSys::init() once at the beginning of our application and
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
 *
 * @todo make schema checking optional
 */
class XMLSubSys
{
public:
    /**
     * @brief Initialises the xml-subsystem, returns whether the initialisation succeeded.
     *
     * Calls XMLPlatformUtils::Initialize(). If this fails, the exception is
     *  caught and its content is reported using a ProcessError. Otheriwse, a
     *  static SAX2XMLReader is built (stored in myReader)
     * @exception ProcessError If the initialisation fails
     */
    static void init() throw(ProcessError);


    /**
     * @brief Closes the xml-subsystem
     *
     * Deletes the built reader and calls XMLPlatformUtils::Terminate();
     */
    static void close() throw();


    /**
     * @brief Builds a reader and assigns the handler to it
     *
     * Tries to build a SAX2XMLReader using XMLReaderFactory::createXMLReader. If this
     *  fails, 0 is returned.
     *
     * If the reader could be built, validation is turned off, first. Then the given
     *  handler is assigned to the reader as the current DefaultHandler and
     *  ErrorHandler.
     *
     * @param[in] handler The handler to assign to the built reader
     * @return The build Xerces-SAX-reader, 0 if something failed
     */
    static XERCES_CPP_NAMESPACE_QUALIFIER SAX2XMLReader * getSAXReader(SUMOSAXHandler &handler) throw();


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
     * @param[in] file The file to run the parser at
     * @return true if the parsing was done without errors, false otherwise (error was printed)
     */
    static bool runParser(SUMOSAXHandler &handler,
                          const std::string &file) throw();


protected:
    /**
     * @brief Sets the named feature of the given reader to the given value
     *
     * The given feature name is translated into an XMLCh* and set.
     *
     * @param[in] reader The reader to set the feature of
     * @param[in] feature Name of the feature to set
     * @param[in] value Value of the feature to set
     */
    static void setFeature(XERCES_CPP_NAMESPACE_QUALIFIER SAX2XMLReader &reader,
                           const std::string &feature, bool value) throw();


private:
    /// The XML Reader used for repeated parsing
    static XERCES_CPP_NAMESPACE_QUALIFIER SAX2XMLReader * myReader;

};


#endif

/****************************************************************************/

