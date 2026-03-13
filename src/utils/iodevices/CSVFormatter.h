/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2012-2026 German Aerospace Center (DLR) and others.
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
/// @file    CSVFormatter.h
/// @author  Michael Behrisch
/// @date    2025-06-12
///
// Output formatter for CSV output
/****************************************************************************/
#pragma once
#include <config.h>

#include <memory>
#include "OutputFormatter.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class CSVFormatter
 * @brief Output formatter for CSV output
 */
class CSVFormatter : public OutputFormatter {
public:
    /// @brief Constructor
    CSVFormatter(const std::string& columnNames, const char separator = ';');

    /// @brief Destructor
    virtual ~CSVFormatter() { }

    /** @brief Keeps track of an open XML tag by adding a new element to the stack
     *
     * @param[in] into The output stream to use (unused)
     * @param[in] xmlElement Name of element to open (unused)
     * @return The OutputDevice for further processing
     */
    void openTag(std::ostream& into, const std::string& xmlElement);

    /** @brief Keeps track of an open XML tag by adding a new element to the stack
     *
     * @param[in] into The output stream to use (unused)
     * @param[in] xmlElement Name of element to open (unused)
     */
    void openTag(std::ostream& into, const SumoXMLTag& xmlElement);

    /** @brief Closes the most recently opened tag
     *
     * @param[in] into The output stream to use
     * @return Whether a further element existed in the stack and could be closed
     * @todo it is not verified that the topmost element was closed
     */
    bool closeTag(std::ostream& into, const std::string& comment = "");

    /** @brief writes a named attribute
     *
     * @param[in] into The output stream to use
     * @param[in] attr The attribute (name)
     * @param[in] val The attribute value
     */
    template <class T>
    void writeAttr(std::ostream& into, const SumoXMLAttr attr, const T& val) {
        checkAttr(attr);
        myValues.emplace_back(toString(val, into.precision()));
    }

    template <class T>
    void writeAttr(std::ostream& into, const std::string& attr, const T& val) {
        assert(!myCheckColumns);
        checkHeader(attr);
        myValues.emplace_back(toString(val, into.precision()));
    }

    void writeNull(std::ostream& /* into */, const SumoXMLAttr attr) {
        checkAttr(attr);
        myValues.emplace_back("");
    }

    void writeTime(std::ostream& /* into */, const SumoXMLAttr attr, const SUMOTime val) {
        checkAttr(attr);
        myValues.emplace_back(time2string(val));
    }

    bool wroteHeader() const {
        return myWroteHeader;
    }

    void setExpectedAttributes(const SumoXMLAttrMask& expected, const int depth = 2) {
        myExpectedAttrs = expected;
        myMaxDepth = depth;
        myCheckColumns = expected.any();
    }

private:
    /** @brief Helper function to keep track of the written attributes and accumulate the header.
     * It checks whether the written attribute is expected in the column based format.
     * The check does only apply to the deepest level of the XML hierarchy and not to the order of the columns just to the presence.
     *
     * @param[in] attr The attribute (name)
     */
    inline void checkAttr(const SumoXMLAttr attr) {
        if (myCheckColumns && myMaxDepth == (int)myXMLStack.size()) {
            mySeenAttrs.set(attr);
            if (!myExpectedAttrs.test(attr)) {
                throw ProcessError(TLF("Unexpected attribute '%', this file format does not support CSV output yet.", toString(attr)));
            }
        }
        checkHeader(attr);
    }

    template <class ATTR_TYPE>
    inline void checkHeader(const ATTR_TYPE& attr) {
        myNeedsWrite = true;
        if (!myWroteHeader) {
            std::string headerName = toString(attr);
            if (myHeaderFormat != "plain" && !(myHeaderFormat == "auto" && std::find(myHeader.begin(), myHeader.end(), headerName) == myHeader.end())) {
                headerName = myCurrentTag + "_" + headerName;
            }
            if (std::find(myHeader.begin(), myHeader.end(), headerName) == myHeader.end()) {
                for (std::string& row : myBufferedRows) {
                    row += mySeparator;
                }
                while (myValues.size() < myHeader.size()) {
                    myValues.emplace_back("");
                }
                myHeader.emplace_back(headerName);
            }
        }
    }

    /// @brief the format to use for the column names
    const std::string myHeaderFormat;

    /// @brief The value separator
    const char mySeparator;

    /// @brief the CSV header
    std::vector<std::string> myHeader;

    /// @brief the currently read tag (only valid when generating the header)
    std::string myCurrentTag;

    /// @brief The number of attributes in the currently open XML elements
    std::vector<int> myXMLStack;

    /// @brief the current attribute / column values
    std::vector<std::string> myValues;

    /// @brief the maximum depth of the XML hierarchy (excluding the root element)
    int myMaxDepth = 2;

    /// @brief whether the CSV header line has been written
    bool myWroteHeader = false;

    /// @brief whether any attribute has been written since the last row was emitted
    bool myNeedsWrite = false;

    /// @brief partial rows buffered before the schema is known (depth < myMaxDepth)
    std::vector<std::string> myBufferedRows;

    /// @brief whether the columns should be checked for completeness
    bool myCheckColumns = false;

    /// @brief which CSV columns are expected (just for checking completeness)
    SumoXMLAttrMask myExpectedAttrs;

    /// @brief which CSV columns have been set (just for checking completeness)
    SumoXMLAttrMask mySeenAttrs;
};
