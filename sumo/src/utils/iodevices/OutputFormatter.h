/****************************************************************************/
/// @file    OutputFormatter.h
/// @author  Michael Behrisch
/// @date    2012
/// @version $Id$
///
// Abstract base class for output formatters
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
#ifndef OutputFormatter_h
#define OutputFormatter_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <utils/xml/SUMOXMLDefinitions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class Boundary;
class Position;
class PositionVector;
class RGBColor;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class OutputFormatter
 * @brief Abstract base class for output formatters
 *
 * OutputFormatter format XML like output into the output stream.
 *  There are only two implementation at the moment, "normal" XML
 *  and binary XML.
 */
class OutputFormatter {
public:
    /// @brief Destructor
    virtual ~OutputFormatter() { }


    /** @brief Writes an XML header with optional configuration
     *
     * If something has been written (myXMLStack is not empty), nothing
     *  is written and false returned.
     *
     * @param[in] into The output stream to use
     * @param[in] rootElement The root element to use
     * @param[in] xmlParams Additional parameters (such as encoding) to include in the <?xml> declaration
     * @param[in] attrs Additional attributes to save within the rootElement
     * @param[in] comment Additional comment (saved in front the rootElement)
     * @todo Check which parameter is used herein
     * @todo Describe what is saved
     */
    virtual bool writeXMLHeader(std::ostream& into, const std::string& rootElement,
                        const std::string xmlParams = "",
                        const std::string& attrs = "",
                        const std::string& comment = "") = 0;


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
    virtual void openTag(std::ostream& into, const std::string& xmlElement) = 0;


    /** @brief Opens an XML tag
     *
     * Helper method which finds the correct string before calling openTag.
     *
     * @param[in] into The output stream to use
     * @param[in] xmlElement Id of the element to open
     */
    virtual void openTag(std::ostream& into, const SumoXMLTag& xmlElement) = 0;


    /** @brief Ends the most recently opened element start.
     *
     * Writes more or less nothing but ">" and a line feed.
     *
     * @param[in] into The output stream to use
     */
    virtual void closeOpener(std::ostream& into) = 0;


    /** @brief Closes the most recently opened tag
     *
     * @param[in] into The output stream to use
     * @param[in] name whether abbreviated closing is performed
     * @return Whether a further element existed in the stack and could be closed
     * @todo it is not verified that the topmost element was closed
     */
    virtual bool closeTag(std::ostream& into, bool abbreviated=false) = 0;


    /** @brief writes an arbitrary attribute
     *
     * @param[in] into The output stream to use
     * @param[in] attr The attribute (name)
     * @param[in] val The attribute value
     */
    virtual void writeAttr(std::ostream& into, const std::string& attr, const std::string& val) = 0;

};


#endif

/****************************************************************************/

