/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEFileBucket.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2025
///
// class used for link Elements with their filenames
/****************************************************************************/
#pragma once
#include <config.h>

#include <unordered_set>
#include <string>

#include <netedit/GNETagProperties.h>

// ===========================================================================
// class definitions
// ===========================================================================

class GNEFileBucket {

public:
    /// @brief Constructor for default bucket
    GNEFileBucket(GNETagProperties::File fileType);

    /// @brief Constructor
    GNEFileBucket(GNETagProperties::File fileType, const std::string filename);

    /// @brief destructor
    ~GNEFileBucket();

    /// @brief get file type
    GNETagProperties::File getFileType() const;

    /// @brief get filename
    const std::string& getFilename() const;

    /// @brief set filename
    void setFilename(const std::string& filename);

    /// @brief check if this is a default bucket
    bool isDefaultBucket() const;

    /// @brief check if this bucket is empty
    bool isEmpty() const;

    /// @brief function related with Elements
    /// @{

    /// @brief add element in bucket
    void addElement(const void* element);

    /// @brief remove element
    void removeElement(const void* element);

    /// @brief check if the given element exist in this bucket
    bool hasElement(const void* element) const;

    /// @}

    /// @brief parse filenames
    static std::string parseFilenames(const std::vector<GNEFileBucket*>& fileBuckets);

private:
    /// @brief fileType
    const GNETagProperties::File myFileType;

    /// @brief filename
    std::string myFilename;

    /// @brief Elements vinculated with this fileBucket
    std::unordered_set<const void*> myElements;

    /// @brief flag to indicate if this is a default bucket (it cannot be removed)
    const bool myDefaultBucket;

    /// @brief Invalidated default constructor.
    GNEFileBucket() = delete;

    /// @brief Invalidated copy constructor.
    GNEFileBucket(const GNEFileBucket&) = delete;

    /// @brief Invalidated assignment operator
    GNEFileBucket& operator=(const GNEFileBucket& src) = delete;
};
