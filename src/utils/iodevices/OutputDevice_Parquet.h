/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2004-2024 German Aerospace Center (DLR) and others.
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
/// @file    OutputDevice_Parquet.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @author  Max Schrader
/// @date    2024
///
// An output device that encapsulates an Parquet file
/****************************************************************************/
#pragma once

#include <config.h>

#ifdef HAVE_PARQUET

#include <iostream>
#include "OutputDevice.h"
#include "ParquetFormatter.h"

#include <arrow/io/file.h>
#include <arrow/util/config.h>
#include <parquet/exception.h>
#include <parquet/stream_reader.h>
#include <parquet/stream_writer.h>

// Include S3 filesystem
#ifdef HAVE_S3
#include <arrow/filesystem/s3fs.h>
#endif



/**
 * @class OutputDevice_Parquet
 * @brief An output device that encapsulates an parquet stream writer
 *
 * Please note that the device is responsible for the stream and deletes
 *  it (it should not be deleted elsewhere).
 */
class OutputDevice_Parquet : public OutputDevice {
public:
    /** @brief Constructor
     * @param[in] fullName The name of the output file to use
     * @exception IOError Should not be thrown by this implementation
     */
    OutputDevice_Parquet(const std::string& fullName);

    /// @brief Destructor
    ~OutputDevice_Parquet() override;

    /** @brief implements the close tag logic. This is where the file is first opened and the   schema is created.
     * This exploits the fact that for *most* SUMO files, all the fields are present at the first close tag event.
     */
    bool closeTag(const std::string& comment) override;

    /** @brief writes a line feed if applicable. overriden from the base class to do nothing
     */
    void lf() {};

    // null the setPrecision method
    void setPrecision(int precision) override {
        UNUSED_PARAMETER(precision);
    };

    void setOSFlags(std::ios_base::fmtflags flags) override {
        UNUSED_PARAMETER(flags);
    };

    /// @brief Lock the schema to prevent clearing
    void lockSchema() {
        auto formatter = dynamic_cast<ParquetFormatter*>(&this->getFormatter());
        if (formatter != nullptr) {
            formatter->lockSchema();
        }
    }

    /// @brief Enable data writing in the formatter
    void enableDataWrite() {
        auto formatter = dynamic_cast<ParquetFormatter*>(&this->getFormatter());
        if (formatter != nullptr) {
            formatter->enableDataWrite();
        }
    }

    /// @brief Disable data writing in the formatter
    void disableDataWrite() {
        auto formatter = dynamic_cast<ParquetFormatter*>(&this->getFormatter());
        if (formatter != nullptr) {
            formatter->disableDataWrite();
        }
    }

protected:

    /// @brief Returns whether the output device is a parquet
    OutputWriterType getType() const override {
        return OutputWriterType::PARQUET;
    }

    /// do I allow optional attributes
    bool allowOptionalAttributes = false;

private:
    /// The wrapped ofstream
    std::shared_ptr<arrow::io::OutputStream> myFile = nullptr;
    // the builder for the writer properties
    parquet::WriterProperties::Builder builder;
    // the schema
    std::shared_ptr<parquet::schema::GroupNode> schema;

    /// am I redirecting to /dev/null
    bool myAmNull = false;

    /// my full name
    std::string myFullName;

    parquet::schema::NodeVector myNodeVector;
    
    /// flag indicating if this is an S3 output
    bool myIsS3 = false;
    
#ifdef HAVE_S3
    /// S3 filesystem instance if writing to S3
    std::shared_ptr<arrow::fs::S3FileSystem> myS3FileSystem = nullptr;
#endif

    /** @brief Create output stream based on the filename (S3 or local)
     * @return Arrow output stream
     */
    std::shared_ptr<arrow::io::OutputStream> createOutputStream();
    
    /** @brief Parse S3 URL and extract bucket and key
     * @param url S3 URL in format s3://bucket/path
     * @param[out] bucket Bucket name
     * @param[out] key Object key
     * @return Success status
     */
    bool parseS3Url(const std::string& url, std::string& bucket, std::string& key);
};

#endif // HAVE_PARQUET