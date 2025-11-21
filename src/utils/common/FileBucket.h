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

#include <string>

// ===========================================================================
// class definitions
// ===========================================================================

class FileBucket {

public:
    /// @brief Files that this bucket can save
    enum class Type : int {
        SUMOCONFIG =        1 << 0, // Bucket for sumo configs (usually only one)
        NETCONVERTCONFIG =  1 << 1, // Bucket for netconvert configs (usually only one)
        NETEDITCONFIG =     1 << 2, // Bucket for netedit configs (usually only one)
        NETWORK =           1 << 3, // Bucket for network elements (usually only one)
        DEMAND =            1 << 4, // Bucket for demand elements
        MEANDATA =          1 << 5, // Bucket for meanData elements
        ADDITIONAL =        1 << 6, // Bucket for additional elements (always after demand and meanData)
        DATA =              1 << 7, // Bucket for data elements
        AUTOMATIC =         1 << 8, // Element choose automatic the bucket
        NOTHING =           1 << 9, // Element is not saved in bucket
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

    /// @brief function related with Elements
    /// @{

    /// @brief add element in bucket
    void addElement();

    /// @brief remove element
    void removeElement();

    /// @brief check if this bucket is empty
    bool isEmpty() const;

    /// @}

private:
    /// @brief type
    const FileBucket::Type myFileType;

    /// @brief filename
    std::string myFilename;

    /// @brief number of elements vinculated with this file bucket
    int myNumElements = 0;

    /// @brief flag to indicate if this is a default bucket (it cannot be removed)
    const bool myDefaultBucket;

    /// @brief Invalidated default constructor.
    FileBucket() = delete;

    /// @brief Invalidated copy constructor.
    FileBucket(const FileBucket&) = delete;

    /// @brief Invalidated assignment operator
    FileBucket& operator=(const FileBucket& src) = delete;
};

/// @brief override type bit operator
constexpr FileBucket::Type operator|(FileBucket::Type a, FileBucket::Type b) {
    return static_cast<FileBucket::Type>(static_cast<int>(a) | static_cast<int>(b));
}

/// @brief override type bit operator
constexpr bool operator&(FileBucket::Type a, FileBucket::Type b) {
    return (static_cast<int>(a) & static_cast<int>(b)) != 0;
}
