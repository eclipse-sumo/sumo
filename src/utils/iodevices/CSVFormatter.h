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
#include <algorithm>
#include "OutputFormatter.h"
#include <utils/common/ToString.h>
#include <utils/common/Translation.h>


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
    virtual ~CSVFormatter() override { }

    /** @brief Writes an "XML header"
     *
     * For CSV output the header is only relevant if it contains additional attributes.
     *
     * @param[in] into The output stream to use
     * @param[in] rootElement The root element to use
     * @param[in] attrs Additional attributes to save within the rootElement
     * @return whether something has been written
     */
    bool writeXMLHeader(std::ostream& into, const std::string& rootElement,
                        const std::map<SumoXMLAttr, std::string>& attrs, bool /* writeMetadata */,
                        bool /* includeConfig */) override;

    /** @brief Keeps track of an open XML tag by adding a new element to the stack
     *
     * @param[in] into The output stream to use (unused)
     * @param[in] xmlElement Name of the element to open
     */
    void openTag(std::ostream& into, const std::string& xmlElement) override;

    /** @brief Keeps track of an open XML tag by adding a new element to the stack
     *
     * @param[in] into The output stream to use (unused)
     * @param[in] xmlElement Enum identifier of the element to open
     */
    void openTag(std::ostream& into, const SumoXMLTag& xmlElement) override;

    /** @brief Closes the most recently opened tag
     *
     * This is where the main action starts. This function determines whether a row is completed and can be written.
     *
     * @param[in] into The output stream to use
     * @param[in] comment A comment to write after the tag (ignored for CSV)
     * @return Whether a further element existed in the stack and could be closed
     */
    bool closeTag(std::ostream& into, const std::string& comment = "") override;

    /** @brief Writes a named attribute
     *
     * This calls checkAttr to check/add this to the known attributes and then adds the string conversion to myValues.
     *
     * @param[in] into The output stream to use (used only to determine precision)
     * @param[in] attr The attribute (name as enum value)
     * @param[in] val The attribute value
     * @param[in] isNull whether this actually a null value (writes the empty string)
     */
    template <class T>
    void writeAttr(std::ostream& into, const SumoXMLAttr attr, const T& val, const bool isNull) {
        checkAttr(attr);
        myValues.emplace_back(isNull ? "" : toString(val, into.precision()));
    }

    /** @brief Writes a named attribute
     *
     * This calls checkHeader to check/add this to the known attributes and then adds the string conversion to myValues.
     *
     * @param[in] into The output stream to use (used only to determine precision)
     * @param[in] attr The attribute (name as string)
     * @param[in] val The attribute value
     * @param[in] isNull whether this actually a null value (writes the empty string)
     */
    template <class T>
    void writeAttr(std::ostream& into, const std::string& attr, const T& val, const bool isNull) {
        assert(!myCheckColumns);
        checkHeader(attr);
        myValues.emplace_back(isNull ? "" : toString(val, into.precision()));
    }

    /** @brief Writes a time value using time2string
     *
     * @param[in] into The output stream to use (unused)
     * @param[in] attr The attribute (name as enum value)
     * @param[in] val The attribute value
     */
    void writeTime(std::ostream& /* into */, const SumoXMLAttr attr, const SUMOTime val) override {
        checkAttr(attr);
        myValues.emplace_back(time2string(val));
    }

    /** @brief Whether a complete row has been encountered and triggered header writing
     *
     * @return Whether the CSV header has been written
     */
    bool wroteHeader() const override {
        return myWroteHeader;
    }

    /** @brief Which elements are expected and which maximum depth the XML tree has.
     *
     * This is not necessary for the functionality but very useful for debugging and tracking whether expected attributes
     * are still missing (triggers an error in checkAttr). If expected is empty, no tracking takes place.
     *
     * @param[in] expected The enum values of the attrs which should be present before a row can be written.
     * @param[in] depth The maximum expected depth of nested XML elements.
     */
    void setExpectedAttributes(const SumoXMLAttrMask& expected, const int depth) override {
        myExpectedAttrs = expected;
        myMaxDepth = depth;
        myCheckColumns = expected.any();
    }

private:
    /** @brief Helper function to keep track of the written attributes and accumulate the header.
     * It checks whether the written attribute is expected in the column based format.
     * The check does only apply to the deepest level of the XML hierarchy and not to the order of the columns just to the presence.
     *
     * @param[in] attr The attribute (name as enum value)
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
            const auto colIt = std::find(myHeader.begin(), myHeader.end(), headerName);
            if (colIt == myHeader.end()) {
                for (std::string& row : myBufferedRows) {
                    row += mySeparator;
                }
                myValues.resize(myHeader.size());
                myHeader.emplace_back(headerName);
            } else {
                // there might be missing attributes inbetween, so make sure header position and value size match
                myValues.resize(std::distance(myHeader.begin(), colIt));
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

    /// @brief whether any root attribute have been encountered
    bool myHaveRootAttrs = false;

    /// @brief partial rows buffered before the schema is known (depth < myMaxDepth)
    std::vector<std::string> myBufferedRows;

    /// @brief whether the columns should be checked for completeness
    bool myCheckColumns = false;

    /// @brief which CSV columns are expected (just for checking completeness)
    SumoXMLAttrMask myExpectedAttrs;

    /// @brief which CSV columns have been set (just for checking completeness)
    SumoXMLAttrMask mySeenAttrs;
};
