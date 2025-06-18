/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2012-2025 German Aerospace Center (DLR) and others.
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

#include <ostream>

#include <arrow/api.h>
#include <arrow/io/interfaces.h>
#include <arrow/status.h>
#include <parquet/arrow/writer.h>

#include <utils/common/ToString.h>
#include "OutputFormatter.h"


// ===========================================================================
// class definitions
// ===========================================================================
class OstreamOutputStream : public arrow::io::OutputStream {
public:
    explicit OstreamOutputStream(std::ostream& out)
        : out_(out), position_(0), is_open_(true) {}

    arrow::Status Close() override {
        is_open_ = false;
        return arrow::Status::OK();
    }

    arrow::Status Flush() override {
        out_.flush();
        return arrow::Status::OK();
    }

    arrow::Result<int64_t> Tell() const override {
        return position_;
    }

    bool closed() const override {
        return !is_open_;
    }

    arrow::Status Write(const void* data, int64_t nbytes) override {
        if (!is_open_) {
            return arrow::Status::IOError("Write on closed stream");
        }

        out_.write(reinterpret_cast<const char*>(data), nbytes);
        if (!out_) {
            return arrow::Status::IOError("Failed to write to ostream");
        }

        position_ += nbytes;
        return arrow::Status::OK();
    }

private:
    std::ostream& out_;
    int64_t position_;
    bool is_open_;
};


/**
 * @class ParquetFormatter
 * @brief Output formatter for Parquet output
 */
class ParquetFormatter : public OutputFormatter {
public:
    /// @brief Constructor
    ParquetFormatter(const int batchSize = 100);

    /// @brief Destructor
    virtual ~ParquetFormatter() { }

    /** @brief This is not an xml format so it does nothing
     */
    bool writeXMLHeader(std::ostream& into, const std::string& rootElement,
                        const std::map<SumoXMLAttr, std::string>& attrs,
                        bool includeConfig = true);

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

    /** @brief writes a preformatted tag to the device but ensures that any
     * pending tags are closed
     * @param[in] into The output stream to use
     * @param[in] val The preformatted data
     */
    void writePreformattedTag(std::ostream& /* into */, const std::string& /* val */) {
        throw ProcessError("This file format does not support Parquet output yet.");
    }

    inline void checkAttr(const SumoXMLAttr attr) {
        if (myMaxDepth == (int)myXMLStack.size()) {
            mySeenAttrs.set(attr);
            if (!myExpectedAttrs.test(attr)) {
                throw ProcessError(TLF("Unexpected attribute '%', this file format does not support Parquet output yet.", toString(attr)));
            }
        }
    }

    /** @brief writes a named attribute
     *
     * @param[in] into The output stream to use
     * @param[in] attr The attribute (name)
     * @param[in] val The attribute value
     */
    template <class T>
    void writeAttr(std::ostream& /* into */, const SumoXMLAttr attr, const T& val) {
        checkAttr(attr);
        if (!myWroteHeader) {
            mySchema = *mySchema->AddField(mySchema->num_fields(), arrow::field(toString(attr), arrow::utf8()));
            myBuilders.push_back(std::make_shared<arrow::StringBuilder>());
        }
        myValues.push_back(std::make_shared<arrow::StringScalar>(toString(val)));
    }

    void writeNull(std::ostream& /* into */, const SumoXMLAttr attr) {
        checkAttr(attr);
        myValues.push_back(nullptr);
    }

    bool wroteHeader() const {
        return myWroteHeader;
    }

    void setExpectedAttributes(const SumoXMLAttrMask& expected, const int depth = 2) {
        myExpectedAttrs = expected;
        myMaxDepth = depth;
    }

private:
    const int myBatchSize;
    /// @brief the table schema
    std::shared_ptr<arrow::Schema> mySchema = arrow::schema({});
    std::unique_ptr<parquet::arrow::FileWriter> myParquetWriter;

    /// @brief the content array builders for the table
    std::vector<std::shared_ptr<arrow::ArrayBuilder> > myBuilders;

    /// @brief The stack of begun xml elements
    std::vector<int> myXMLStack;

    std::vector<std::shared_ptr<arrow::Scalar> > myValues;

    /// @brief the maximum depth of the XML hierarchy
    int myMaxDepth;

    bool myWroteHeader;

    SumoXMLAttrMask myExpectedAttrs;
    SumoXMLAttrMask mySeenAttrs;
};


// ===========================================================================
// specialized template implementations
// ===========================================================================
template <>
inline void ParquetFormatter::writeAttr(std::ostream& /* into */, const SumoXMLAttr attr, const double& val) {
    checkAttr(attr);
    if (!myWroteHeader) {
        mySchema = *mySchema->AddField(mySchema->num_fields(), arrow::field(toString(attr), arrow::float64()));
        myBuilders.push_back(std::make_shared<arrow::DoubleBuilder>());
    }
    myValues.push_back(std::make_shared<arrow::DoubleScalar>(val));
}


template <>
inline void ParquetFormatter::writeAttr(std::ostream& /* into */, const SumoXMLAttr attr, const std::string& val) {
    checkAttr(attr);
    if (!myWroteHeader) {
        mySchema = *mySchema->AddField(mySchema->num_fields(), arrow::field(toString(attr), arrow::utf8()));
        myBuilders.push_back(std::make_shared<arrow::StringBuilder>());
    }
    myValues.push_back(std::make_shared<arrow::StringScalar>(val));
}
