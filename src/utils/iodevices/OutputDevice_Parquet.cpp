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
/// @file    OutputDevice_File.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    2004
///
// An output device that encapsulates an ofstream
/****************************************************************************/
#include <config.h>

#include <iostream>
#include <cstring>
#include <cerrno>
#include <utils/common/StringUtils.h>
#include <utils/common/UtilExceptions.h>

#include "OutputDevice_Parquet.h"

#include <arrow/io/file.h>
#include <arrow/util/config.h>

#include <parquet/api/reader.h>
#include <parquet/exception.h>
#include <parquet/stream_reader.h>
#include <parquet/stream_writer.h>


// ===========================================================================
// method definitions
// ===========================================================================
OutputDevice_Parquet::OutputDevice_Parquet(const std::string& fullName)
    : OutputDevice(fullName, new ParquetFormatter()) {
    // set the type of compression. TODO this should be based on the build options
    builder.compression(parquet::Compression::ZSTD);
}


bool OutputDevice_Parquet::closeTag(const std::string& comment) {
    // open the file for writing
    if (myFile == nullptr) {
        auto formatter = dynamic_cast<ParquetFormatter*>(this->getFormatter());
        if (formatter == nullptr) {
            throw IOError("Formatter is not a ParquetFormatter");
        }
        // Create a Parquet file
        PARQUET_ASSIGN_OR_THROW(
            this->myFile, arrow::io::FileOutputStream::Open(this->myFilename));

        this->myStreamDevice = new ParquetStream(parquet::ParquetFileWriter::Open(this->myFile, std::static_pointer_cast<parquet::schema::GroupNode>(
            parquet::schema::GroupNode::Make("schema", parquet::Repetition::REQUIRED, formatter->getNodeVector())
        ), this->builder.build()));

        // check if the file was opened correctly
        if (this->myFile->closed()) {
            throw IOError("Could not build output file '" + this->myFullName + "' (" + std::strerror(errno) + ").");
        }
    }
    // now actually write the data
    return getFormatter()->closeTag(getOStream());
}


OutputDevice_Parquet::~OutputDevice_Parquet() {
    delete myStreamDevice;
    arrow::Status status = this->myFile->Close();
    (void)status; // Suppress unused variable warning
}
/****************************************************************************/
