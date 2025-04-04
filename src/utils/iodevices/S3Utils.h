/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2004-2025 German Aerospace Center (DLR) and others.
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
/// @file    S3Utils.h
/// @author  Pranav Sateesh
/// @date    2024
///
// Shared utilities for S3 operations across output devices
/****************************************************************************/
#pragma once

#ifdef HAVE_S3
#include <mutex>

namespace sumo {
namespace s3 {
    // Mutex to guard S3 finalization
    extern std::mutex s3FinalizationMutex;
    
    // Flag to track if S3 was initialized
    extern bool s3WasInitialized;
}
}
#endif // HAVE_S3 