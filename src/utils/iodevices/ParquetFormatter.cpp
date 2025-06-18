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
/// @file    ParquetFormatter.cpp
/// @author  Michael Behrisch
/// @date    2025-06-17
///
// An output formatter for Parquet files
/****************************************************************************/
#include <config.h>

#include <arrow/io/api.h>

#include <utils/common/ToString.h>
#include "ParquetFormatter.h"


// ===========================================================================
// member method definitions
// ===========================================================================
ParquetFormatter::ParquetFormatter(const int batchSize)
    : OutputFormatter(OutputFormatterType::PARQUET), myBatchSize(batchSize), myMaxDepth(0), myWroteHeader(false) {
}


bool
ParquetFormatter::writeXMLHeader(std::ostream& /* into */, const std::string& /* rootElement */,
                                 const std::map<SumoXMLAttr, std::string>& /* attrs */, bool /* includeConfig */) {
    return false;
}


void
ParquetFormatter::openTag(std::ostream& /* into */, const std::string& /* xmlElement */) {
    myXMLStack.push_back(myValues.size());
}


void
ParquetFormatter::openTag(std::ostream& /* into */, const SumoXMLTag& /* xmlElement */) {
    myXMLStack.push_back(myValues.size());
}


bool
ParquetFormatter::closeTag(std::ostream& into, const std::string& /* comment */) {
    if (myMaxDepth == 0 || (myMaxDepth == (int)myXMLStack.size() && !myWroteHeader)) {
        auto arrow_stream = std::make_shared<OstreamOutputStream>(into);
        myParquetWriter = *parquet::arrow::FileWriter::Open(*mySchema, arrow::default_memory_pool(), arrow_stream);
        if (myMaxDepth == 0) {
            myMaxDepth = (int)myXMLStack.size();
            myExpectedAttrs = mySeenAttrs;
        }
        myWroteHeader = true;
    }
    bool writeBatch = false;
    if ((int)myXMLStack.size() == myMaxDepth) {
        if (myExpectedAttrs != mySeenAttrs) {
            throw ProcessError(TLF("Incomplete attribute set '%', this file format does not support Parquet output yet.", toString(mySeenAttrs)));
        }
        int index = 0;
        for (auto& builder : myBuilders) {
            const auto status = builder->AppendScalar(myValues[index++]);  // TODO evaluate status
        }
        writeBatch = myBuilders.back()->length() > myBatchSize;
        mySeenAttrs.reset();
    }
    if (writeBatch || myXMLStack.empty()) {
        std::vector<std::shared_ptr<arrow::Array> > data;
        for (auto& builder : myBuilders) {
            std::shared_ptr<arrow::Array> column;
            const auto status = builder->Finish(&column);  // TODO evaluate status
            data.push_back(column);
            // builder.reset();
        }
        auto batch = arrow::RecordBatch::Make(mySchema, myBuilders.back()->length(), data);
        const auto status = myParquetWriter->WriteRecordBatch(*batch);  // TODO evaluate status
    }
    if (!myXMLStack.empty()) {
        while ((int)myValues.size() > myXMLStack.back()) {
            myValues.pop_back();
        }
        myXMLStack.pop_back();
    }
    return false;
}


/****************************************************************************/
