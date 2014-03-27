/****************************************************************************/
/// @file    PlainXMLFormatter.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2012
/// @version $Id$
///
// Output formatter for plain XML output
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2012-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef PlainXMLFormatter_h
#define PlainXMLFormatter_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "OutputFormatter.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class PlainXMLFormatter
 * @brief Output formatter for plain XML output
 *
 * PlainXMLFormatter format XML like output into the output stream.
 */
class PlainXMLFormatter : public OutputFormatter {
public:
    /// @brief Constructor
    PlainXMLFormatter(const unsigned int defaultIndentation = 0);


    /// @brief Destructor
    virtual ~PlainXMLFormatter() { }


    /** @brief Writes an XML header with optional configuration
     *
     * If something has been written (myXMLStack is not empty), nothing
     *  is written and false returned.
     *
     * @param[in] into The output stream to use
     * @param[in] rootElement The root element to use
     * @param[in] attrs Additional attributes to save within the rootElement
     * @param[in] comment Additional comment (saved in front the rootElement)
     * @todo Check which parameter is used herein
     * @todo Describe what is saved
     */
    bool writeXMLHeader(std::ostream& into, const std::string& rootElement,
                        const std::string& attrs = "",
                        const std::string& comment = "");


    /** @brief Writes an XML header with optional configuration
     *
     * If something has been written (myXMLStack is not empty), nothing
     *  is written and false returned.
     *
     * @param[in] into The output stream to use
     * @param[in] rootElement The root element to use
     */
    bool writeHeader(std::ostream& into, const SumoXMLTag& rootElement);


    /** @brief Opens an XML tag
     *
     * An indentation, depending on the current xml-element-stack size, is written followed
     *  by the given xml element ("<" + xmlElement)
     * The xml element is added to the stack, then.
     *
     * @param[in] into The output stream to use
     * @param[in] xmlElement Name of element to open
     * @return The OutputDevice for further processing
     */
    void openTag(std::ostream& into, const std::string& xmlElement);


    /** @brief Opens an XML tag
     *
     * Helper method which finds the correct string before calling openTag.
     *
     * @param[in] into The output stream to use
     * @param[in] xmlElement Id of the element to open
     */
    void openTag(std::ostream& into, const SumoXMLTag& xmlElement);


    /** @brief Closes the most recently opened tag
     *
     * @param[in] into The output stream to use
     * @return Whether a further element existed in the stack and could be closed
     * @todo it is not verified that the topmost element was closed
     */
    bool closeTag(std::ostream& into);


    /** @brief writes a preformatted tag to the device but ensures that any
     * pending tags are closed
     * @param[in] into The output stream to use
     * @param[in] val The preformatted data
     */
    void writePreformattedTag(std::ostream& into, const std::string& val);


    /** @brief writes an arbitrary attribute
     *
     * @param[in] into The output stream to use
     * @param[in] attr The attribute (name)
     * @param[in] val The attribute value
     */
    template <class T>
    static void writeAttr(std::ostream& into, const std::string& attr, const T& val) {
        into << " " << attr << "=\"" << toString(val, into.precision()) << "\"";
    }


    /** @brief writes a named attribute
     *
     * @param[in] into The output stream to use
     * @param[in] attr The attribute (name)
     * @param[in] val The attribute value
     */
    template <class T>
    static void writeAttr(std::ostream& into, const SumoXMLAttr attr, const T& val) {
        into << " " << toString(attr) << "=\"" << toString(val, into.precision()) << "\"";
    }


private:
    /// @brief The stack of begun xml elements
    std::vector<std::string> myXMLStack;

    /// @brief The initial indentation level
    unsigned int myDefaultIndentation;

    /// @brief whether a closing ">" might be missing
    bool myHavePendingOpener;
};


#endif

/****************************************************************************/

