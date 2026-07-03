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
                        bool /* includeConfig */);

    void openTag(std::ostream& into, const std::string& xmlElement) override;
    void openTag(std::ostream& into, const SumoXMLTag& xmlElement) override;
    bool closeTag(std::ostream& into, const std::string& comment = "") override;

    /** @brief writes a named attribute
     *
     * Generic template: stringifies the value (only when not null, preserving the
     * original lazy evaluation) and forwards to the typed non-template
     * helpers defined in the .cpp.
     */
    template <class T>
    void writeAttr(std::ostream& /* into */, const SumoXMLAttr attr, const T& val, const bool isNull) {
        if (isNull) {
            writeNullAttr(attr);
        } else {
            writeStringAttr(attr, toString(val));
        }
    }

    template <class T>
    void writeAttr(std::ostream& /* into */, const std::string& attr, const T& val, const bool isNull) {
        if (isNull) {
            writeNullAttr(attr);
        } else {
            writeStringAttr(attr, toString(val));
        }
    }

    /// @brief typed overloads (non-template) -- picked by overload resolution over the template
    void writeAttr(std::ostream& into, const SumoXMLAttr attr, const double& val, const bool isNull);
    void writeAttr(std::ostream& into, const SumoXMLAttr attr, const int& val, const bool isNull);
    void writeAttr(std::ostream& into, const std::string& attr, const double& val, const bool isNull);
    void writeAttr(std::ostream& into, const std::string& attr, const int& val, const bool isNull);

    void writeTime(std::ostream& into, const SumoXMLAttr attr, const SUMOTime val) override;

    bool wroteHeader() const override;

    void setExpectedAttributes(const SumoXMLAttrMask& expected, const int depth = 2) override;

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
