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
/// @file    FileBucket.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2025
///
// class used for link Elements with their filenames
/****************************************************************************/
#pragma once
#include <config.h>

#include <unordered_set>
#include <string>

// ===========================================================================
// class definitions
// ===========================================================================

class FileBucket {

public:
    /// @brief Files that this bucket can save
    enum class Type : std::uint64_t {
        NETWORK =           1ULL << 0,    // Element can be saved in a network file
        DEMAND =            1ULL << 1,    // Element can be saved in a demand file
        MEANDATA =          1ULL << 2,    // Element can be saved in a meanData file
        ADDITIONAL =        1ULL << 3,    // Element can be saved in a additional file (always after demand and meanData)
        DATA =              1ULL << 4,    // Element can be saved in a data file
        NOTHING =           1ULL << 5,    // Element is not saved in bucket
    };

    /// @brief Constructor for default bucket
    FileBucket(FileBucket::Type type);

    /// @brief Constructor
    FileBucket(FileBucket::Type type, const std::string filename);

    /// @brief destructor
    ~FileBucket();

    /// @brief get file type
    FileBucket::Type getType() const;

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
    static std::string parseFilenames(const std::vector<FileBucket*>& fileBuckets);

private:
    /// @brief type
    const FileBucket::Type myFileType;

    /// @brief filename
    std::string myFilename;

    /// @brief Elements vinculated with this fileBucket
    std::unordered_set<const void*> myElements;

    /// @brief flag to indicate if this is a default bucket (it cannot be removed)
    const bool myDefaultBucket;

    /// @brief Invalidated default constructor.
    FileBucket() = delete;

    /// @brief Invalidated copy constructor.
    FileBucket(const FileBucket&) = delete;

    /// @brief Invalidated assignment operator
    FileBucket& operator=(const FileBucket& src) = delete;
};

/// @brief override tag parent bit operator
constexpr FileBucket::Type operator|(FileBucket::Type a, FileBucket::Type b) {
    return static_cast<FileBucket::Type>(static_cast<std::uint64_t>(a) | static_cast<std::uint64_t>(b));
}

/// @brief override tag parent bit operator
constexpr bool operator&(FileBucket::Type a, FileBucket::Type b) {
    return (static_cast<std::uint64_t>(a) & static_cast<std::uint64_t>(b)) != 0;
}
