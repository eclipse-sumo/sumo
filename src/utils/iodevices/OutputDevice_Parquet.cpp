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
#include <config.h>

#ifdef HAVE_PARQUET

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

#ifdef HAVE_S3
#include <arrow/filesystem/s3fs.h>
#include "S3Utils.h"
#endif

// ===========================================================================
// method definitions
// ===========================================================================
OutputDevice_Parquet::OutputDevice_Parquet(const std::string& fullName)
    : OutputDevice(fullName, new ParquetFormatter()), myFullName(fullName) {
    // set the type of compression. TODO this should be based on the build options
    builder.compression(parquet::Compression::ZSTD);
    
    // Check if this is an S3 URL
    myIsS3 = fullName.substr(0, 5) == "s3://";
}


bool OutputDevice_Parquet::closeTag(const std::string& comment) {
    UNUSED_PARAMETER(comment);
    // open the file for writing, but only if the depth is >=2 (i.e. we are closing the children tag).
    //! @todo this is a bit of a hack, but it works for now
    auto formatter = dynamic_cast<ParquetFormatter*>(&this->getFormatter());
    if (formatter->getDepth() < 2) {
        // we have to clean up the stack, otherwise the file will not be written correctly
        // when it is open
        formatter->clearStack();
        // this is critical for the file to be written correctly
        return false;
    }
    if (myFile == nullptr) {
        if (formatter == nullptr) {
            throw IOError("Formatter is not a ParquetFormatter");
        }
        // Create output stream (either S3 or file-based)
        try {
            myFile = createOutputStream();
            if (myFile == nullptr) {
                throw IOError("Could not create output stream for '" + this->myFullName + "'");
            }
        } catch (const std::exception& e) {
            throw IOError("Could not open output file '" + this->myFullName + "': " + e.what());
        }

        try {
            // Lock the schema to prevent further changes after file creation
            formatter->lockSchema();
            
            // Use the optimized ParquetStream for better performance
            this->myStreamDevice = std::make_unique<ParquetStream>(parquet::ParquetFileWriter::Open(this->myFile, std::static_pointer_cast<parquet::schema::GroupNode>(
                parquet::schema::GroupNode::Make("schema", parquet::Repetition::REQUIRED, formatter->getNodeVector())
            ), this->builder.build()));

            // check if the output stream was created correctly
            if (myFile == nullptr) {
                throw IOError("Could not build output file '" + this->myFullName + "'");
            }
        } catch (const parquet::ParquetException& e) {
            throw IOError("Parquet error while creating file '" + this->myFullName + "': " + e.what());
        } catch (const std::exception& e) {
            throw IOError("Error creating Parquet file '" + this->myFullName + "': " + e.what());
        }
    }
    
    // now actually write the data
    try {
        return formatter->closeTag(getOStream());
    } catch (const parquet::ParquetException& e) {
        // Re-throw with more context
        throw IOError("Parquet error while writing row: " + std::string(e.what()));
    }
}


OutputDevice_Parquet::~OutputDevice_Parquet() {
    // have to delete the stream device before the file. This dumps unwritten data to the file
    myStreamDevice.reset();
    // close the file (if open)
    if (this->myFile.get() != nullptr) {
        try {
            arrow::Status status = this->myFile->Close();
            if (!status.ok()) {
                std::cerr << "Error closing file: " << status.ToString() << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception closing file: " << e.what() << std::endl;
        }
        
        // Clear file pointer after closing
        myFile.reset();
    }
    
#ifdef HAVE_S3
    // Finalize S3 if we used it
    if (myIsS3 && myS3FileSystem != nullptr) {
        try {
            // Release our reference to the filesystem object
            myS3FileSystem.reset();
            
            // We don't call FinalizeS3 here as it's better to let it be handled at program exit
            // to avoid issues with multiple devices being closed concurrently
        } catch (const std::exception& e) {
            std::cerr << "Exception during S3 resource cleanup: " << e.what() << std::endl;
        }
    }
#endif
}

std::shared_ptr<arrow::io::OutputStream> OutputDevice_Parquet::createOutputStream() {
    // If it's an S3 URL, create S3 output stream
    if (myIsS3) {
#ifdef HAVE_S3
        // Parse S3 URL
        std::string bucket, key;
        if (!parseS3Url(this->myFilename, bucket, key)) {
            throw IOError("Invalid S3 URL format: " + this->myFilename);
        }
        
        // Initialize S3 filesystem if not already done
        if (myS3FileSystem == nullptr) {
            try {
                // First ensure S3 is initialized
                auto s3InitStatus = arrow::fs::EnsureS3Initialized();
                if (!s3InitStatus.ok()) {
                    throw IOError("Failed to initialize S3: " + s3InitStatus.ToString());
                }
                
                // Mark that S3 was initialized globally
                std::lock_guard<std::mutex> lock(sumo::s3::s3FinalizationMutex);
                sumo::s3::s3WasInitialized = true;
                
                // Get S3 options from environment variables
                arrow::fs::S3Options options = arrow::fs::S3Options::Defaults();
                
                // Allow overriding region
                const char* region = std::getenv("SUMO_S3_REGION");
                if (region != nullptr) {
                    options.region = region;
                }
                
                // Allow overriding endpoint
                const char* endpoint = std::getenv("SUMO_S3_ENDPOINT");
                if (endpoint != nullptr) {
                    options.endpoint_override = endpoint;
                } else if (!options.region.empty()) {
                    // If no endpoint specified but region is set, construct a regional endpoint
                    options.endpoint_override = "s3." + options.region + ".amazonaws.com";
                }
                
                // Enable automatic address resolving (helps with 301 redirects)
                options.request_timeout = std::chrono::duration<double>(std::chrono::seconds(60)).count();
                options.connect_timeout = std::chrono::duration<double>(std::chrono::seconds(30)).count();
                
                // Check for access key and secret key in environment
                const char* accessKey = std::getenv("SUMO_S3_ACCESS_KEY");
                const char* secretKey = std::getenv("SUMO_S3_SECRET_KEY");
                const char* sessionToken = std::getenv("SUMO_S3_SESSION_TOKEN");
                
                if (accessKey != nullptr && secretKey != nullptr) {
                    // Use explicit credentials
                    options.ConfigureAccessKey(
                        accessKey, 
                        secretKey, 
                        sessionToken != nullptr ? sessionToken : ""
                    );
                }
                
                // Create S3 filesystem
                auto result = arrow::fs::S3FileSystem::Make(options);
                if (!result.ok()) {
                    throw IOError("Failed to create S3 filesystem: " + result.status().ToString());
                }
                
                myS3FileSystem = result.ValueOrDie();
            } catch (const std::exception& e) {
                throw IOError("Error creating S3 filesystem: " + std::string(e.what()));
            }
        }
        
        // Create output stream - construct the full S3 path with bucket name
        std::string s3Path = bucket + "/" + key;
        auto result = myS3FileSystem->OpenOutputStream(s3Path);
        if (!result.ok()) {
            throw IOError("Could not open S3 output stream: " + result.status().ToString());
        }
        
        return result.ValueOrDie();
#else
        throw IOError("S3 support not enabled in this build");
#endif
    } else {
        // Regular file output
        auto result = arrow::io::FileOutputStream::Open(this->myFilename);
        if (!result.ok()) {
            throw IOError("Could not build output file '" + this->myFullName + "' (" + 
                          std::strerror(errno) + ").");
        }
        return result.ValueOrDie();
    }
}

bool OutputDevice_Parquet::parseS3Url(const std::string& url, std::string& bucket, std::string& key) {
    // Simple S3 URL parser (s3://bucket/key)
    if (url.substr(0, 5) != "s3://") {
        return false;
    }
    
    size_t bucketStart = 5;  // After "s3://"
    size_t bucketEnd = url.find('/', bucketStart);
    
    if (bucketEnd == std::string::npos) {
        // URL format is s3://bucket with no key
        bucket = url.substr(bucketStart);
        key = "";
    } else {
        // URL format is s3://bucket/key
        bucket = url.substr(bucketStart, bucketEnd - bucketStart);
        key = url.substr(bucketEnd + 1);
    }
    
    // Error checking - bucket and key should not be empty for writing
    if (bucket.empty()) {
        return false;
    }
    
    return true;
}

#endif
/****************************************************************************/
