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
/// @file    S3Utils.cpp
/// @author  Pranav Sateesh
/// @date    2024
///
// Shared utilities for S3 operations across output devices
/****************************************************************************/
#include <config.h>

#ifdef HAVE_S3
#include "S3Utils.h"
#include <arrow/filesystem/s3fs.h>
#include <iostream>

namespace sumo {
namespace s3 {
    // Define the global variables declared in the header
    std::mutex s3FinalizationMutex;
    bool s3WasInitialized = false;

    // Helper class to ensure FinalizeS3 is called at exit
    class S3Finalizer {
    public:
        ~S3Finalizer() {
            // Lock mutex before checking/finalizing
            std::lock_guard<std::mutex> lock(s3FinalizationMutex);
            if (s3WasInitialized) {
                arrow::Status status = arrow::fs::FinalizeS3();
                if (!status.ok()) {
                    // Use std::cerr for critical exit messages
                    std::cerr << "Error finalizing S3: " << status.ToString() << std::endl;
                }
                s3WasInitialized = false; // Mark as finalized
            }
        }
    };

    // Global instance of the finalizer. Its destructor will be called at program exit.
    S3Finalizer globalS3Finalizer;

} // namespace s3
} // namespace sumo

#endif // HAVE_S3 