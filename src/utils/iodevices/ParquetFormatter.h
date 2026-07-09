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
/// @file    ParquetFormatter.h
/// @author  Michael Behrisch
/// @date    2025-06-17
///
// Output formatter for Parquet output
/****************************************************************************/
#pragma once
#include <config.h>

#include <memory>
#include <ostream>
#include <utils/common/ToString.h>
#include "OutputFormatter.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ParquetFormatter
 * @brief Output formatter for Parquet output
 *
 * All arrow/parquet types are hidden in the Impl so that callers
 * (notably OutputDevice.h) do not pay the include cost.
 */
class ParquetFormatter : public OutputFormatter {
public:
    /// @brief Constructor
    // for some motivation on the default batch size see https://stackoverflow.com/questions/76782018/what-is-actually-meant-when-referring-to-parquet-row-group-size
    ParquetFormatter(const std::string& columnNames, const std::string& compression = "", const int batchSize = 1000000);

    /// @brief Destructor (out-of-line: Impl is incomplete here)
    ~ParquetFormatter() override;

    /** @brief Writes an "XML header"
     *
     * For Parquet output the header is only relevant if it contains additional attributes.
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
     * @param[in] comment A comment to write after the tag (ignored for Parquet)
     * @return Whether a further element existed in the stack and could be closed
     */
    bool closeTag(std::ostream& into, const std::string& comment = "") override;

    /** @brief Writes a named attribute
     *
     * @param[in] into The output stream to use (unused)
     * @param[in] attr The attribute (name as enum value)
     * @param[in] val The attribute value
     * @param[in] isNull whether this actually a null value (adds nullptr to myValues)
     */
    template <class T>
    void writeAttr(std::ostream& /* into */, const SumoXMLAttr attr, const T& val, const bool isNull, const bool escape) {
        if (isNull) {
            writeNullAttr(attr);
        } else {
            writeStringAttr(attr, toString(val));
        }
    }

    /** @brief Writes a named attribute
     *
     * @param[in] into The output stream to use (unused)
     * @param[in] attr The attribute (name as string value)
     * @param[in] val The attribute value
     * @param[in] isNull whether this actually a null value (adds nullptr to myValues)
     */
    template <class T>
    void writeAttr(std::ostream& /* into */, const std::string& attr, const T& val, const bool isNull, const bool escape) {
        if (isNull) {
            writeNullAttr(attr);
        } else {
            writeStringAttr(attr, toString(val));
        }
    }

    /// @brief typed overloads (non-template) -- picked by overload resolution over the template
    void writeAttr(std::ostream& into, const SumoXMLAttr attr, const double& val, const bool isNull, const bool escape);
    void writeAttr(std::ostream& into, const SumoXMLAttr attr, const int& val, const bool isNull, const bool escape);
    void writeAttr(std::ostream& into, const std::string& attr, const double& val, const bool isNull, const bool escape);
    void writeAttr(std::ostream& into, const std::string& attr, const int& val, const bool isNull, const bool escape);

    /** @brief Writes a time value
     *
     * Currently this writes a string if human readable times are activated and a double otherwise
     *
     * @param[in] into The output stream to use (unused)
     * @param[in] attr The attribute (name as enum value)
     * @param[in] val The attribute value
     * @todo use one of Parquet's time types
     */
    void writeTime(std::ostream& into, const SumoXMLAttr attr, const SUMOTime val) override;

    /** @brief Whether a complete row has been encountered and triggered writing
     *
     * @return Whether the Parquet writer has been initialized and a first row has been written
     */
    bool wroteHeader() const override;

    /** @brief Which elements are expected and which maximum depth the XML tree has.
     *
     * This is not necessary for the functionality but very useful for debugging and tracking whether expected attributes
     * are still missing (triggers an error in checkAttr). If expected is empty, no tracking takes place.
     *
     * The depth parameter is only for performance. If a tag at this depth is closed for the first time,
     * the header is being written and buffered rows may be flushed. Setting it to a large value (which is also the default)
     * means you are on the safe side if more attributes or tags show up later but it may result in buffering the complete
     * output before writing the first line.
     * Setting it to 0 triggers auto detection which means the first time a tag is closed the maximum depth will be determined.
     *
     * @param[in] expected The enum values of the attrs which should be present before a row can be written.
     * @param[in] depth The maximum expected depth of nested XML elements.
     */
    void setExpectedAttributes(const SumoXMLAttrMask& expected, const int depth) override;

private:
    /// @brief non-template helpers; defined in the .cpp where arrow/parquet are available
    void writeStringAttr(const SumoXMLAttr attr, const std::string& val);
    void writeStringAttr(const std::string& attr, const std::string& val);
    void writeNullAttr(const SumoXMLAttr attr);
    void writeNullAttr(const std::string& attr);

    /// @brief opaque arrow/parquet state
    struct Impl;
    std::unique_ptr<Impl> myImpl;
};
