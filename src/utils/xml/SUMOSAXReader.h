/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    SUMOSAXReader.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Nov 2012
/// @version $Id$
///
// SAX-reader encapsulation containing binary reader
/****************************************************************************/
#ifndef SUMOSAXReader_h
#define SUMOSAXReader_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <memory>
#include <vector>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax/EntityResolver.hpp>
#include <xercesc/sax/InputSource.hpp>
#include "SUMOXMLDefinitions.h"


// ===========================================================================
// class declarations
// ===========================================================================
class GenericSAXHandler;
class BinaryInputDevice;
class IStreamInputSource;


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
    class LocalSchemaResolver : public XERCES_CPP_NAMESPACE::EntityResolver {
    public:
        XERCES_CPP_NAMESPACE::InputSource* resolveEntity(const XMLCh* const publicId, const XMLCh* const systemId);
        void setHandler(GenericSAXHandler& handler);
    private:
        GenericSAXHandler* myHandler;
    };

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

    std::unique_ptr<std::istream> myIStream;

    std::unique_ptr<IStreamInputSource> myInputStream;

    char mySbxVersion;

    /// @brief The stack of begun xml elements
    std::vector<SumoXMLTag> myXMLStack;

    LocalSchemaResolver mySchemaResolver;

private:
    /// @brief invalidated copy constructor
    SUMOSAXReader(const SUMOSAXReader& s);

    /// @brief invalidated assignment operator
    const SUMOSAXReader& operator=(const SUMOSAXReader& s);

};


#endif

/****************************************************************************/
