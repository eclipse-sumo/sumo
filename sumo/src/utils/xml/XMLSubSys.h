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
#include <xercesc/sax2/DefaultHandler.hpp>
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
 *  within the microsim-module.
 *
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
     *  caught and their content is reported to cerr. In this case the function
     *  returns false. If no problem occured, true is returned.
     */
    static bool init() throw();


    /**
     * @brief Closes the xml-subsystem
     *
     * Calls XMLPlatformUtils::Terminate();
     */
    static void close() throw();


    /**
     * @brief Builds a reader and assigns the handler to it
     *
     * Tries to build a SAX2XMLReader using XMLReaderFactory::createXMLReader. If this
     *  fails, 0 is returned.
     * If the reader could be built, validation is turned off, first. Then the given
     *  handler is assigned to the reader as the current DefaultHandler and 
     *  ErrorHandler.
     */
    static XERCES_CPP_NAMESPACE_QUALIFIER SAX2XMLReader *
    getSAXReader(SUMOSAXHandler &handler) throw();


    /**
     * @brief Runs the given handler on the given file; returns if everything's ok
     * 
     * Builds a reader using getSAXReader. Then, it parses the given file and
     *  deletes the reader.
     *
     * All exceptions are catched and reported to the error-instance of the MsgHandler.
     *  Also, if the reader could not be built, this is reported.
     *
     * The method returns true if everything went ok. This means, that the reader could be
     *  built, no exception was caught, and nothing was reported to the error-instance
     *  of the MsgHandler.
     */
    static bool runParser(SUMOSAXHandler &handler,
                          const std::string &file) throw();


protected:
    /**
     * @brief Sets the named feature of the given reader to the given value
     */
    static void setFeature(XERCES_CPP_NAMESPACE_QUALIFIER SAX2XMLReader &,
                           const std::string &feature, bool value) throw();


};


#endif

/****************************************************************************/

