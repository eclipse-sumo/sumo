/****************************************************************************/
/// @file    SUMOSAXReader.h
/// @author  Michael Behrisch
/// @date    Nov 2012
/// @version $Id$
///
// SAX-reader encapsulation containing binary reader
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef SUMOSAXReader_h
#define SUMOSAXReader_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <xercesc/sax2/SAX2XMLReader.hpp>


// ===========================================================================
// class declarations
// ===========================================================================
class GenericSAXHandler;
class BinaryInputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SUMOSAXReader
 * @brief SAX-reader encapsulation containing binary reader
 *
 * This class generates on demand either a SAX2XMLReader or parses the SUMO
 * binary xml. The interface is inspired by but not identical to
 * SAX2XMLReader.
 */
class SUMOSAXReader {
public:
    /**
     * @brief Constructor
     *
     * @param[in] file The name of the processed file
     */
    SUMOSAXReader(GenericSAXHandler& handler, const XERCES_CPP_NAMESPACE::SAX2XMLReader::ValSchemes validationScheme);

    /// Destructor
    ~SUMOSAXReader();

    /**
     * @brief Sets the given handler as content and error handler for the reader
     *
     * @param[in] handler The handler to assign to the reader
     */
    void setHandler(GenericSAXHandler& handler);

    void setValidation(const XERCES_CPP_NAMESPACE::SAX2XMLReader::ValSchemes validationScheme);

    void parse(std::string systemID);

    void parseString(std::string content);

    bool parseFirst(std::string systemID);

    bool parseNext();

private:
    /**
     * @brief Builds a reader
     *
     * Tries to build a SAX2XMLReader using XMLReaderFactory::createXMLReader. If this
     *  fails, 0 is returned. Otherwise the validation is set matching the value of
     *  "myEnableValidation". If validation is not wanted, a WFXMLScanner is used
     *  (see http://www.ibm.com/developerworks/library/x-xercesperf.html).
     *
     * @return The built Xerces-SAX-reader, 0 if something failed
     */
    XERCES_CPP_NAMESPACE::SAX2XMLReader* getSAXReader();


private:
    GenericSAXHandler* myHandler;

    /// @brief Information whether built reader/parser shall validate XML-documents against schemata
    XERCES_CPP_NAMESPACE::SAX2XMLReader::ValSchemes myValidationScheme;

    XERCES_CPP_NAMESPACE::XMLPScanToken myToken;

    XERCES_CPP_NAMESPACE::SAX2XMLReader* myXMLReader;

    BinaryInputDevice* myBinaryInput;

private:
    /// @brief invalidated copy constructor
    SUMOSAXReader(const SUMOSAXReader& s);

    /// @brief invalidated assignment operator
    const SUMOSAXReader& operator=(const SUMOSAXReader& s);

};


#endif

/****************************************************************************/
