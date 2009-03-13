/****************************************************************************/
/// @file    OptionsLoader.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 17 Dec 2001
/// @version $Id$
///
// A SAX-Handler for loading options
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
#ifndef OptionsLoader_h
#define OptionsLoader_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <string>


// ===========================================================================
// xerces 2.2 compatibility
// ===========================================================================
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class OptionsLoader
 * @brief A SAX-Handler for loading options
 */
class OptionsLoader : public HandlerBase {
public:
    /** @brief Constructor
     *
     * @param[in] file The name of the file to parse
     * @param[in] verbose Whether the parsing shall been done verbose
     */
    OptionsLoader(const std::string &file) throw();


    /** destructor */
    ~OptionsLoader() throw();




    /// @name Handlers for the SAX DocumentHandler interface
    /// @{

    /** @brief Called on the occurence of the beginning of a tag
     *
     * Sets the name of the last item
     */
    virtual void startElement(const XMLCh* const name,
                              AttributeList& attributes);


    /** @brief Called on the occurence of the beginning of a tag
     *
     * Tries to get the named option and to set it
     * @todo Describe better
     */
    void characters(const XMLCh* const chars, const unsigned int length);


    /** @brief Called on the end of an element
     *
     * Resets the element name
     */
    void endElement(const XMLCh* const name);
    /// @}




    /// @name Handlers for the SAX ErrorHandler interface
    /// @{

    /** @brief Called on an XML-warning
     *
     * The warning is reported to the the warning-instance of MsgHandler
     */
    void warning(const SAXParseException& exception);


    /** @brief Called on an XML-error
     *
     * The warning is reported to the the error-instance of MsgHandler
     */
    void error(const SAXParseException& exception);


    /** @brief Called on an XML-fatal error
     *
     * The warning is reported to the the error-instance of MsgHandler
     */
    void fatalError(const SAXParseException& exception);
    /// @}



    /** @brief Returns the information whether an error occured */
    bool errorOccured() const throw();


private:
    /** @brief Tries to set the named option to the given value (for bool-Options)
     *
     * Checks the item whether it was default before setting it.
     * Returns the information whether the item was set before (was not a default value)
     *
     * @param[in] The name of the option to set
     * @param The new value for the option
     * @return Whether the option could be set
     */
    bool setSecure(const std::string &name, bool value) const throw();


    /** @brief Tries to set the named option to the given value
     *
     * Checks the item whether it was default before setting it.
     * Returns the information whether the item was set before (was not a default value)
     *
     * @param[in] The name of the option to set
     * @param The new value for the option
     * @return Whether the option could be set
     */
    bool setSecure(const std::string &name, const std::string &value) const throw();


private:
    /** invalid copy constructor */
    OptionsLoader(const OptionsLoader &s);


    /** invalid assignment operator */
    OptionsLoader &operator=(const OptionsLoader &s);


private:
    /** the information whether an error occured */
    bool  myError;

    /** the parsed file */
    const std::string myFile;

    /** the options to fill */
    OptionsCont &myOptions;

    /** the name of the currently parsed option */
    std::string myItem;

};


#endif

/****************************************************************************/

