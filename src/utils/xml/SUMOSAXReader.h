/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2023 German Aerospace Center (DLR) and others.
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
/// @file    SUMOSAXReader.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Nov 2012
///
// SAX-reader encapsulation containing binary reader
/****************************************************************************/
#pragma once
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
class IStreamInputSource;
class SUMOSAXAttributes;

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
    SUMOSAXReader(GenericSAXHandler& handler, const std::string& validationScheme, XERCES_CPP_NAMESPACE::XMLGrammarPool* grammarPool);

    /// Destructor
    ~SUMOSAXReader();

    /**
     * @brief Sets the given handler as content and error handler for the reader
     *
     * @param[in] handler The handler to assign to the reader
     */
    void setHandler(GenericSAXHandler& handler);

    /**
     * @brief Sets a new validation scheme and applies the validation settings to the XML reader
     *
     * If no new scheme is given, the settings of the current scheme are applied.
     *
     * @param[in] validationScheme The validation scheme (one of "never", "local", "auto", or "always")
     */
    void setValidation(std::string validationScheme = "");

    /**
     * @brief Parse the given file completely by calling parse of myXMLReader
     *
     * This throws a ProcessError if the file is not readable and can handle gzipped XML as well.
     *
     * @param[in] systemID file name
     */
    void parse(std::string systemID);

    /**
     * @brief Parse XML from the given string
     *
     * @param[in] content XML string
     */
    void parseString(std::string content);

    /**
     * @brief Start parsing the given file using parseFirst of myXMLReader
     *
     * @param[in] systemID file name
     * @return whether the prolog could be parsed successfully
     */
    bool parseFirst(std::string systemID);

    /**
     * @brief Continue a progressive parse started by parseFirst
     *
     * @return whether the next token could be parsed successfully
     */
    bool parseNext();

    /**
     * @brief Continue a progressive parse started by parseFirst until the given element is encountered
     *
     * The parse will continue until the section encapsulated by the element is completed
     *
     * @return whether the next section could be parsed successfully
     */
    bool parseSection(int element);

private:
    /// @brief Local Schema Resolver
    class LocalSchemaResolver : public XERCES_CPP_NAMESPACE::EntityResolver {

    public:
        /// @brief constructor
        LocalSchemaResolver(const bool haveFallback, const bool noOp);

        /// @brief resolve entity
        XERCES_CPP_NAMESPACE::InputSource* resolveEntity(const XMLCh* const publicId, const XMLCh* const systemId);

    private:
        /// @brief flag for check if we have fallback
        const bool myHaveFallback;

        /// @brief flag for check if there is an operation
        const bool myNoOp;
    };

    /**
     * @brief Builds a reader, if needed
     *
     * Tries to build a SAX2XMLReader using XMLReaderFactory::createXMLReader,
     *  if no reader has been created yet. If this
     *  fails, a ProcessError is thrown. Otherwise the validation is set matching the value of
     *  "myValidationScheme". If validation is not wanted, a WFXMLScanner is used
     *  (see http://www.ibm.com/developerworks/library/x-xercesperf.html).
     */
    void ensureSAXReader();

    /// @brief generic SAX Handler
    GenericSAXHandler* myHandler;

    /// @brief Information whether built reader/parser shall validate XML-documents against schemata
    std::string myValidationScheme;

    /// @brief Schema cache to be used for grammars which are not declared
    XERCES_CPP_NAMESPACE::XMLGrammarPool* myGrammarPool;

    /// @brief token
    XERCES_CPP_NAMESPACE::XMLPScanToken myToken;

    /// @brief XML reader
    XERCES_CPP_NAMESPACE::SAX2XMLReader* myXMLReader;

    /// @brief istream
    std::unique_ptr<std::istream> myIStream;

    /// @brief input stream
    std::unique_ptr<IStreamInputSource> myInputStream;

    /// @brief The stack of begun xml elements
    std::vector<SumoXMLTag> myXMLStack;

    /// @brief schema resolver
    LocalSchemaResolver mySchemaResolver;

    /// @brief local resolver
    LocalSchemaResolver myLocalResolver;

    /// @brief no operation resolver
    LocalSchemaResolver myNoOpResolver;

    /// @brief next section
    std::pair<int, SUMOSAXAttributes*> myNextSection;

    /// @brief invalidated copy constructor
    SUMOSAXReader(const SUMOSAXReader& s) = delete;

    /// @brief invalidated assignment operator
    const SUMOSAXReader& operator=(const SUMOSAXReader& s) = delete;
};
