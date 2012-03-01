/****************************************************************************/
/// @file    BinaryFormatter.h
/// @author  Michael Behrisch
/// @date    2012
/// @version $Id$
///
// Output formatter for plain XML output
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
#ifndef BinaryFormatter_h
#define BinaryFormatter_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/FileHelpers.h>
#include "OutputFormatter.h"


// ===========================================================================
// class declarations
// ===========================================================================
class Position;
class PositionVector;
class Boundary;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class BinaryFormatter
 * @brief Output formatter for plain XML output
 *
 * BinaryFormatter format XML like output into the output stream.
 */
class BinaryFormatter : public OutputFormatter {
public:
    /// @brief data types in binary output
    enum DataType {
        /// @brief 
        BF_BYTE,
        /// @brief
        BF_INTEGER,
        /// @brief
        BF_FLOAT,
        /// @brief
        BF_STRING,
        /// @brief
        BF_LIST,
        /// @brief
        BF_XML_TAG_START,
        /// @brief
        BF_XML_TAG_END,
        /// @brief
        BF_XML_ATTRIBUTE,
        /// @brief
        BF_EDGE,
        /// @brief
        BF_LANE,
        /// @brief
        BF_POSITION_2D,
        /// @brief
        BF_POSITION_3D,
        /// @brief
        BF_COLOR,
        /// @brief
        BF_NODE_TYPE,
        /// @brief
        BF_EDGE_FUNCTION
    };

    /// @brief Constructor
    BinaryFormatter();


    /// @brief Destructor
    virtual ~BinaryFormatter() { }


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
    bool writeXMLHeader(std::ostream& into, const std::string& rootElement,
                        const std::string xmlParams = "",
                        const std::string& attrs = "",
                        const std::string& comment = "");


    /** @brief Opens an XML tag
     *
     * An indentation, depending on the current xml-element-stack size, is written followed
     *  by the given xml element ("<" + xmlElement)
     * The xml element is added to the stack, then.
     *
     * @param[in] into The output stream to use
     * @param[in] xmlElement Name of element to open
     * @returns The OutputDevice for further processing
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


    /** @brief Ends the most recently opened element start.
     *
     * Does nothing.
     *
     * @param[in] into The output stream to use
     */
    void closeOpener(std::ostream& into);


    /** @brief Closes the most recently opened tag
     *
     * @param[in] into The output stream to use
     * @param[in] name whether abbreviated closing is performed
     * @returns Whether a further element existed in the stack and could be closed
     * @todo it is not verified that the topmost element was closed
     */
    bool closeTag(std::ostream& into, bool abbreviated=false);


    /** @brief writes an arbitrary attribute
     *
     * @param[in] into The output stream to use
     * @param[in] attr The attribute (name)
     * @param[in] val The attribute value
     */
    void writeAttr(std::ostream& into, const std::string& attr, const std::string& val);


    /** @brief writes a named attribute
     *
     * @param[in] into The output stream to use
     * @param[in] attr The attribute (name)
     * @param[in] val The attribute value
     */
    template <typename T>
    static void writeAttr(std::ostream& into, const SumoXMLAttr attr, const T& val);


    /** @brief writes a named float attribute
     *
     * @param[in] into The output stream to use
     * @param[in] attr The attribute (name)
     * @param[in] val The attribute value
     */
    static void writeAttr(std::ostream& into, const SumoXMLAttr attr, const SUMOReal& val);


    /** @brief writes a named integer attribute
     *
     * @param[in] into The output stream to use
     * @param[in] attr The attribute (name)
     * @param[in] val The attribute value
     */
    static void writeAttr(std::ostream& into, const SumoXMLAttr attr, const int& val);


    /** @brief writes a node type attribute
     *
     * @param[in] into The output stream to use
     * @param[in] attr The attribute (name)
     * @param[in] val The attribute value
     */
    static void writeAttr(std::ostream& into, const SumoXMLAttr attr, const SumoXMLNodeType& val);


    /** @brief writes an edge function attribute
     *
     * @param[in] into The output stream to use
     * @param[in] attr The attribute (name)
     * @param[in] val The attribute value
     */
    static void writeAttr(std::ostream& into, const SumoXMLAttr attr, const SumoXMLEdgeFunc& val);


    /** @brief writes a position attribute
     *
     * @param[in] into The output stream to use
     * @param[in] attr The attribute (name)
     * @param[in] val The attribute value
     */
    static void writeAttr(std::ostream& into, const SumoXMLAttr attr, const Position& val);


    /** @brief writes a position vector attribute
     *
     * @param[in] into The output stream to use
     * @param[in] attr The attribute (name)
     * @param[in] val The attribute value
     */
    static void writeAttr(std::ostream& into, const SumoXMLAttr attr, const PositionVector& val);


    /** @brief writes a boundary attribute
     *
     * @param[in] into The output stream to use
     * @param[in] attr The attribute (name)
     * @param[in] val The attribute value
     */
    static void writeAttr(std::ostream& into, const SumoXMLAttr attr, const Boundary& val);


private:
    /** @brief writes the header for an arbitrary attribute
     *
     * @param[in] into The output stream to use
     * @param[in] attr The attribute (name)
     * @param[in] type The attribute type
     */
    static inline void writeAttrHeader(std::ostream& into, const SumoXMLAttr attr, const DataType type) {
        FileHelpers::writeByte(into, static_cast<unsigned char>(BF_XML_ATTRIBUTE));
        FileHelpers::writeInt(into, attr);
        FileHelpers::writeByte(into, static_cast<unsigned char>(type));
    }


    /** @brief writes a list of strings
     *
     * @param[in] into The output stream to use
     * @param[in] list the list to write
     */
    void writeStringList(std::ostream& into, const std::vector<std::string>& list);


private:
    /// @brief The stack of begun xml elements
    std::vector<SumoXMLTag> myXMLStack;


};


template <typename T>
void BinaryFormatter::writeAttr(std::ostream& into, const SumoXMLAttr attr, const T& val) {
    BinaryFormatter::writeAttrHeader(into, attr, BF_STRING);
    FileHelpers::writeString(into, toString(val, into.precision()));
}


#endif

/****************************************************************************/

