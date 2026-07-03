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
/// @file    ParquetFormatter.cpp
/// @author  Michael Behrisch
/// @date    2025-06-17
///
// An output formatter for Parquet files
/****************************************************************************/
#include <config.h>

#ifdef _MSC_VER
#pragma warning(push)
/* Disable warning about unused parameters */
#pragma warning(disable: 4100)
/* Disable warning about hidden function arrow::io::Writable::Write */
#pragma warning(disable: 4266)
/* Disable warning about padded memory layout */
#pragma warning(disable: 4324)
/* Disable warning about this in initializers */
#pragma warning(disable: 4355)
/* Disable warning about changed memory layout due to virtual base class */
#pragma warning(disable: 4435)
/* Disable warning about declaration hiding class member */
#pragma warning(disable: 4458)
/* Disable warning about implicit conversion of int to bool */
#pragma warning(disable: 4800)
#endif
#include <arrow/api.h>
#include <arrow/io/api.h>
#include <parquet/arrow/writer.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include "ParquetFormatter.h"


// ===========================================================================
// helper class definitions
// ===========================================================================
class ArrowOStreamWrapper : public arrow::io::OutputStream {
public:
    ArrowOStreamWrapper(std::ostream& out)
        : myOStream(out), myAmOpen(true) {}

    arrow::Status Close() override {
        myAmOpen = false;
        return arrow::Status::OK();
    }

    arrow::Status Flush() override {
        myOStream.flush();
        return arrow::Status::OK();
    }

    arrow::Result<int64_t> Tell() const override {
        return myOStream.tellp();
    }

    bool closed() const override {
        return !myAmOpen;
    }

    arrow::Status Write(const void* data, int64_t nbytes) override {
        if (!myAmOpen) {
            return arrow::Status::IOError("Write on closed stream");
        }
        myOStream.write(reinterpret_cast<const char*>(data), nbytes);
        if (!myOStream) {
            return arrow::Status::IOError("Failed to write to ostream");
        }
        return arrow::Status::OK();
    }

private:
    std::ostream& myOStream;
    bool myAmOpen;
};


// ===========================================================================
// ParquetFormatter::Impl definition
// ===========================================================================
struct ParquetFormatter::Impl {
    Impl(const std::string& columnNames, const int batchSize)
        : myHeaderFormat(columnNames), myBatchSize(batchSize) {}

    /// @brief the format to use for the column names
    const std::string myHeaderFormat;

    /// @brief the compression to use
    parquet::Compression::type myCompression = parquet::Compression::UNCOMPRESSED;

    /// @brief the number of rows to write per batch
    const int myBatchSize;

    /// @brief the currently read tag (only valid when generating the header)
    std::string myCurrentTag;

    /// @brief the table schema
    std::shared_ptr<arrow::Schema> mySchema = arrow::schema({});

    /// @brief the output stream writer
    std::unique_ptr<parquet::arrow::FileWriter> myParquetWriter;

    /// @brief the content array builders for the table
    std::vector<std::shared_ptr<arrow::ArrayBuilder> > myBuilders;

    /// @brief The number of attributes in the currently open XML elements
    std::vector<int> myXMLStack;

    /// @brief the current attribute / column values
    std::vector<std::shared_ptr<arrow::Scalar> > myValues;

    /// @brief the maximum depth of the XML hierarchy
    int myMaxDepth = 2;

    /// @brief whether the schema has been constructed completely
    bool myWroteHeader = false;

    /// @brief whether the columns should be checked for completeness
    bool myCheckColumns = false;

    /// @brief whether there is still unwritten data
    bool myNeedsWrite = false;

    /// @brief whether any root attribute have been encountered
    bool myHaveRootAttrs = false;

    /// @brief the attributes which are expected for a complete row (including null values)
    SumoXMLAttrMask myExpectedAttrs;

    /// @brief the attributes already seen (including null values)
    SumoXMLAttrMask mySeenAttrs;

    /// @brief column-name lookup honoring the headerFormat option
    std::string getAttrString(const std::string& attrString) const {
        if (myHeaderFormat == "plain") {
            return attrString;
        }
        if (myHeaderFormat == "auto") {
            for (const auto& field : mySchema->fields()) {
                if (field->name() == attrString) {
                    return myCurrentTag + "_" + attrString;
                }
            }
            return attrString;
        }
        return myCurrentTag + "_" + attrString;
    }

    void checkAttr(const SumoXMLAttr attr) {
        if (myCheckColumns && myMaxDepth == (int)myXMLStack.size()) {
            mySeenAttrs.set(attr);
            if (!myExpectedAttrs.test(attr)) {
                throw ProcessError(TLF("Unexpected attribute '%', this file format does not support Parquet output yet.", toString(attr)));
            }
        }
    }

    template <class ATTR_TYPE, class BUILDER>
    void checkBuilder(const ATTR_TYPE& attr, const std::shared_ptr<arrow::DataType>& (*dataType)()) {
        myNeedsWrite = true;
        if (!myWroteHeader) {
            const std::string fieldName = getAttrString(toString(attr));
            int idx = 0;
            for (const auto& field : mySchema->fields()) {
                if (field->name() == fieldName) {
                    // there might be missing attributes inbetween, so make sure the position of the attribute in the header matches the current number of values
                    while (idx > myValues.size()) {
                        myValues.push_back(nullptr);
                    }
                    return;
                }
                idx++;
            }
            mySchema = *mySchema->AddField(mySchema->num_fields(), arrow::field(fieldName, dataType()));
            auto builder = std::make_shared<BUILDER>();
            if (!myBuilders.empty()) {
                if (myBuilders.back()->length() > 0) {
                    PARQUET_THROW_NOT_OK(builder->AppendNulls(myBuilders.back()->length()));
                }
                while (myValues.size() < myBuilders.size()) {
                    myValues.push_back(nullptr);
                }
            }
            myBuilders.push_back(builder);
        }
    }
};


// ===========================================================================
// member method definitions
// ===========================================================================
ParquetFormatter::ParquetFormatter(const std::string& columnNames, const std::string& compression, const int batchSize)
    : OutputFormatter(OutputFormatterType::PARQUET), myImpl(std::make_unique<Impl>(columnNames, batchSize)) {
    if (compression == "snappy") {
        myImpl->myCompression = parquet::Compression::SNAPPY;
    } else if (compression == "gzip") {
        myImpl->myCompression = parquet::Compression::GZIP;
    } else if (compression == "brotli") {
        myImpl->myCompression = parquet::Compression::BROTLI;
    } else if (compression == "zstd") {
        myImpl->myCompression = parquet::Compression::ZSTD;
    } else if (compression == "lz4") {
        myImpl->myCompression = parquet::Compression::LZ4;
    } else if (compression == "bz2") {
        myImpl->myCompression = parquet::Compression::BZ2;
    } else if (compression != "" && compression != "uncompressed") {
        WRITE_ERRORF("Unknown compression: %", compression);
    }
    if (!arrow::util::Codec::IsAvailable(myImpl->myCompression)) {
        WRITE_WARNINGF("Compression '%' not available, falling back to uncompressed.", compression);
        myImpl->myCompression = parquet::Compression::UNCOMPRESSED;
    }
}


ParquetFormatter::~ParquetFormatter() = default;


bool
ParquetFormatter::writeXMLHeader(std::ostream& into, const std::string& rootElement,
                                 const std::map<SumoXMLAttr, std::string>& attrs, bool /* writeMetadata */,
                                 bool /* includeConfig */) {
    if (attrs.size() > 2) {
        myImpl->myHaveRootAttrs = true;
        openTag(into, rootElement);
        for (const auto& a : attrs) {
            if (a.first != SUMO_ATTR_XMLNS && a.first != SUMO_ATTR_SCHEMA_LOCATION) {
                writeAttr(into, a.first, a.second, false);
            }
        }
        return true;
    }
    return false;
}


void
ParquetFormatter::openTag(std::ostream& /* into */, const std::string& xmlElement) {
    myImpl->myXMLStack.push_back((int)myImpl->myValues.size());
    if (!myImpl->myWroteHeader) {
        myImpl->myCurrentTag = xmlElement;
    }
    if (myImpl->myMaxDepth == (int)myImpl->myXMLStack.size() && myImpl->myWroteHeader && myImpl->myCurrentTag != xmlElement) {
        WRITE_WARNINGF("Encountered mismatch in XML tags (expected % but got %). Column names may be incorrect.", myImpl->myCurrentTag, xmlElement);
    }
}


void
ParquetFormatter::openTag(std::ostream& /* into */, const SumoXMLTag& xmlElement) {
    myImpl->myXMLStack.push_back((int)myImpl->myValues.size());
    if (!myImpl->myWroteHeader) {
        myImpl->myCurrentTag = toString(xmlElement);
    }
    if (myImpl->myMaxDepth == (int)myImpl->myXMLStack.size() && myImpl->myWroteHeader && myImpl->myCurrentTag != toString(xmlElement)) {
        WRITE_WARNINGF("Encountered mismatch in XML tags (expected % but got %). Column names may be incorrect.", myImpl->myCurrentTag, toString(xmlElement));
    }
}


bool
ParquetFormatter::closeTag(std::ostream& into, const std::string& /* comment */) {
    if (myImpl->myMaxDepth == 0) {
        // the auto detection case: the first closed tag determines the depth
        myImpl->myMaxDepth = (int)myImpl->myXMLStack.size();
    }
    const bool eof = myImpl->myXMLStack.empty() || (myImpl->myHaveRootAttrs && myImpl->myXMLStack.size() == 1);
    if ((myImpl->myMaxDepth == (int)myImpl->myXMLStack.size() || eof) && !myImpl->myWroteHeader) {
        // we are at the correct depth or the document has ended (XML stack is empty)
        // so we should initialize the writer with the schema (if not done yet)
        if (!myImpl->myCheckColumns) {
            WRITE_WARNING("Column based formats are still experimental. Autodetection only works for homogeneous output.");
        }
        auto arrow_stream = std::make_shared<ArrowOStreamWrapper>(into);
        std::shared_ptr<parquet::WriterProperties> props = parquet::WriterProperties::Builder().compression(myImpl->myCompression)->build();
        myImpl->myParquetWriter = *parquet::arrow::FileWriter::Open(*myImpl->mySchema, arrow::default_memory_pool(), arrow_stream, props);
        myImpl->myWroteHeader = true;
    }
    bool writeBatch = false;
    if (myImpl->myNeedsWrite) {
        if (myImpl->myCheckColumns && (int)myImpl->myXMLStack.size() == myImpl->myMaxDepth && myImpl->myExpectedAttrs != myImpl->mySeenAttrs) {
            for (int i = 0; i < (int)myImpl->myExpectedAttrs.size(); ++i) {
                if (myImpl->myExpectedAttrs.test(i) && !myImpl->mySeenAttrs.test(i)) {
                    WRITE_ERRORF("Incomplete attribute set, '%' is missing. This file format does not support Parquet output yet.",
                                 toString((SumoXMLAttr)i));
                }
            }
        }
        int index = 0;
        for (auto& builder : myImpl->myBuilders) {
            const auto val = index < (int)myImpl->myValues.size() ? myImpl->myValues[index++] : nullptr;
            PARQUET_THROW_NOT_OK(val == nullptr ? builder->AppendNull() : builder->AppendScalar(*val));
        }
        writeBatch = myImpl->myWroteHeader && myImpl->myBuilders.back()->length() >= myImpl->myBatchSize;
        myImpl->mySeenAttrs.reset();
        myImpl->myNeedsWrite = false;
    }
    if (writeBatch || (eof && !myImpl->myBuilders.empty())) {
        std::vector<std::shared_ptr<arrow::Array> > data;
        for (auto& builder : myImpl->myBuilders) {
            std::shared_ptr<arrow::Array> column;
            PARQUET_THROW_NOT_OK(builder->Finish(&column));
            data.push_back(column);
            // builder.reset();
        }
        auto batch = arrow::RecordBatch::Make(myImpl->mySchema, data.back()->length(), data);
        PARQUET_THROW_NOT_OK(myImpl->myParquetWriter->WriteRecordBatch(*batch));
    }
    if (!myImpl->myXMLStack.empty()) {
        if ((int)myImpl->myValues.size() > myImpl->myXMLStack.back()) {
            myImpl->myValues.resize(myImpl->myXMLStack.back());
        }
        myImpl->myXMLStack.pop_back();
    }
    return false;
}


void
ParquetFormatter::writeAttr(std::ostream& into, const SumoXMLAttr attr, const double& val, const bool isNull) {
    myImpl->checkAttr(attr);
    if (attr == SUMO_ATTR_X || attr == SUMO_ATTR_Y || into.precision() > 2) {
        myImpl->checkBuilder<SumoXMLAttr, arrow::DoubleBuilder>(attr, arrow::float64);
        myImpl->myValues.push_back(isNull ? nullptr : std::make_shared<arrow::DoubleScalar>(val));
    } else {
        myImpl->checkBuilder<SumoXMLAttr, arrow::FloatBuilder>(attr, arrow::float32);
        myImpl->myValues.push_back(isNull ? nullptr : std::make_shared<arrow::FloatScalar>((float)val));
    }
}


void
ParquetFormatter::writeAttr(std::ostream& /* into */, const SumoXMLAttr attr, const int& val, const bool isNull) {
    myImpl->checkAttr(attr);
    myImpl->checkBuilder<SumoXMLAttr, arrow::Int32Builder>(attr, arrow::int32);
    myImpl->myValues.push_back(isNull ? nullptr : std::make_shared<arrow::Int32Scalar>(val));
}


void
ParquetFormatter::writeAttr(std::ostream& into, const std::string& attr, const double& val, const bool isNull) {
    assert(!myImpl->myCheckColumns);
    if (into.precision() > 2) {
        myImpl->checkBuilder<std::string, arrow::DoubleBuilder>(attr, arrow::float64);
        myImpl->myValues.push_back(isNull ? nullptr : std::make_shared<arrow::DoubleScalar>(val));
    } else {
        myImpl->checkBuilder<std::string, arrow::FloatBuilder>(attr, arrow::float32);
        myImpl->myValues.push_back(isNull ? nullptr : std::make_shared<arrow::FloatScalar>((float)val));
    }
}


void
ParquetFormatter::writeAttr(std::ostream& /* into */, const std::string& attr, const int& val, const bool isNull) {
    assert(!myImpl->myCheckColumns);
    myImpl->checkBuilder<std::string, arrow::Int32Builder>(attr, arrow::int32);
    myImpl->myValues.push_back(isNull ? nullptr : std::make_shared<arrow::Int32Scalar>(val));
}


void
ParquetFormatter::writeStringAttr(const SumoXMLAttr attr, const std::string& val) {
    myImpl->checkAttr(attr);
    myImpl->checkBuilder<SumoXMLAttr, arrow::StringBuilder>(attr, arrow::utf8);
    myImpl->myValues.push_back(std::make_shared<arrow::StringScalar>(val));
}


void
ParquetFormatter::writeStringAttr(const std::string& attr, const std::string& val) {
    assert(!myImpl->myCheckColumns);
    myImpl->checkBuilder<std::string, arrow::StringBuilder>(attr, arrow::utf8);
    myImpl->myValues.push_back(std::make_shared<arrow::StringScalar>(val));
}


void
ParquetFormatter::writeNullAttr(const SumoXMLAttr attr) {
    myImpl->checkAttr(attr);
    myImpl->checkBuilder<SumoXMLAttr, arrow::StringBuilder>(attr, arrow::utf8);
    myImpl->myValues.push_back(nullptr);
}


void
ParquetFormatter::writeNullAttr(const std::string& attr) {
    assert(!myImpl->myCheckColumns);
    myImpl->checkBuilder<std::string, arrow::StringBuilder>(attr, arrow::utf8);
    myImpl->myValues.push_back(nullptr);
}


void
ParquetFormatter::writeTime(std::ostream& /* into */, const SumoXMLAttr attr, const SUMOTime val) {
    if (!gHumanReadableTime) {
        // always float64 for machine-readable time, regardless of stream precision
        myImpl->checkBuilder<SumoXMLAttr, arrow::DoubleBuilder>(attr, arrow::float64);
        myImpl->myValues.push_back(std::make_shared<arrow::DoubleScalar>(STEPS2TIME(val)));
        return;
    }
    writeStringAttr(attr, time2string(val));
}


bool
ParquetFormatter::wroteHeader() const {
    return myImpl->myWroteHeader;
}


void
ParquetFormatter::setExpectedAttributes(const SumoXMLAttrMask& expected, const int depth) {
    myImpl->myExpectedAttrs = expected;
    myImpl->myMaxDepth = depth;
    myImpl->myCheckColumns = expected.any();
}


/****************************************************************************/
