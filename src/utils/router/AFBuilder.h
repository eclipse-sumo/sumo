/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    AFBuilder.h
/// @author  Ruediger Ebendt
/// @date    01.11.2023
///
// Arc flags builder for the arc flag router
/****************************************************************************/
#pragma once
#include <config.h>
#include <vector>
// uncomment to disable assert()
// #define NDEBUG
#include <cassert>

#include "AFBuild.h"
#include "FlippedEdge.h"

//#define AFBL_DEBUG_LEVEL_0
//#define AFBL_DEBUG_LEVEL_1
//#define AFBL_DEBUG_LEVEL_2

#ifdef AFBL_DEBUG_LEVEL_2
#define AFBL_DEBUG_LEVEL_1
#endif

#ifdef AFBL_DEBUG_LEVEL_1
#define AFBL_DEBUG_LEVEL_0
#endif

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class AFBuilder
 * @brief Builds arc flags for shortest path search with the arc flag router
 */

template<class E, class N, class V>
class AFBuilder {
public:
    typedef typename AFInfo<E>::FlagInfo FlagInfo;
    typedef AbstractLookupTable<FlippedEdge<E, N, V>, V> FlippedLookupTable;
    
    /** @brief Constructor
     * @param[in] numberOfLevels The number of levels
     * @param[in] edges The container with all edges of the network
     * @param[in] unbuildIsWarning The flag indicating whether network unbuilds should issue warnings or errors
     * @param[in] flippedOperation The operation for a backward graph with flipped edges
     * @param[in] flippedLookup The lookup table for a backward graph with flipped edges
     * @param[in] havePermissions The flag indicating whether edges have permissions which must be respected
     * @param[in] haveRestrictions The flag indicating whether edges have restrictions which must be respected
     * @param[in] toProhibit The list of explicitly prohibited edges
     */ 
    AFBuilder(int numberOfLevels, const std::vector<E*>& edges, bool unbuildIsWarning, 
        typename SUMOAbstractRouter<FlippedEdge<E, N, V>, V>::Operation flippedOperation, 
        const std::shared_ptr<const FlippedLookupTable> flippedLookup = nullptr,
        const bool havePermissions = false, const bool haveRestrictions = false, 
        const std::vector<FlippedEdge<E, N, V>*>* toProhibit = nullptr) :
        myEdges(edges),
        myNumberOfLevels(numberOfLevels), 
        myNumberOfArcFlags(2 * (myNumberOfLevels - 1)), 
#ifdef AFBL_DEBUG_LEVEL_0
        myArcFlagsFileName("arcflags.csv"),
#endif
        myAmClean(true) 
    {
        for (const E* const edge : edges) {
            myFlagInfos.push_back(new FlagInfo(edge));
        }
        // build the backward graph with flipped edges / nodes in advance
#ifdef AFBL_DEBUG_LEVEL_0
        std::cout << "Building flipped edges (" << edges.size() << " edges) / nodes..." << std::endl;
#endif
        for (const E* const edge : edges) {
            myFlippedEdges.push_back(edge->getFlippedRoutingEdge());
        }
        for (FlippedEdge<E, N, V>* flippedEdge : myFlippedEdges) {
            flippedEdge->init();
        }
#ifdef AFBL_DEBUG_LEVEL_0
        std::cout << "Flipped edges / nodes are ready." << std::endl;
#endif
        myFlippedPartition = new KDTreePartition<FlippedEdge<E, N, V>, FlippedNode<E, N, V>, V>(myNumberOfLevels, 
            myFlippedEdges, havePermissions, haveRestrictions);
#ifdef AFBL_DEBUG_LEVEL_0
        std::cout << "Instantiating arc flag build..." << std::endl;
#endif
        myArcFlagBuild = new AFBuild<E, N, V>(myFlippedEdges, myFlippedPartition, numberOfLevels, unbuildIsWarning,
            flippedOperation, flippedLookup, havePermissions, haveRestrictions, toProhibit);

#ifdef AFBL_DEBUG_LEVEL_0
        std::cout << "Arc flag build is instantiated (but still uninitialized)." << std::endl;
#endif
    } // end of constructor

    /// @brief Destructor
    ~AFBuilder();

    /// @brief Returns the arc flag build
    AFBuild<E, N, V>* getArcFlagBuild() {
        return myArcFlagBuild;
    }
    /// @brief Returns the edges
    const std::vector<E*>& getEdges() {
        return myEdges;
    }
    /// @brief Resets the builder
    void reset();
    /** @brief Build the arc flag information for the arc flag router
     * @param[in] msTime The start time of the routes in milliseconds
     * @param[in] The vehicle
     * @return The vector with the arc flag information
     */ 
    std::vector<FlagInfo*>& build(SUMOTime msTime, const V* const vehicle);
    /** @brief Converts a SHARC level number to a partition level number
     * @param[in] sHARCLevel The SHARC level
     * @return The partition level number
     */
    int sHARCLevel2PartitionLevel(int sHARCLevel) {
        return AFRouter<E, N, V>::sHARCLevel2PartitionLevel(sHARCLevel, myNumberOfLevels);
    }

protected:
#ifdef AFBL_DEBUG_LEVEL_0
    /** @brief Loads already precomputed arc flags from a CSV file (for testing purposes)   
     * @param[in] fileName The name of the CSV file
     */
    void loadFlagsFromCsv(const std::string fileName);
    /** @brief Saves computed arc flags to a CSV file (for testing purposes)
     * @param[in] fileName The name of the CSV file
     */
    void saveFlagsToCsv(const std::string fileName);
    /** @brief Returns true iff a file with the given name exists
     * @param[in] name The name of the file to test for existence 
     * @return true iff a file with the given name exists
     */
    bool fileExists(const std::string& name) {
        std::ifstream f(name.c_str());
        return f.good();
    }
#endif
    /// @brief The edges
    const std::vector<E*>& myEdges;
    /// @brief The flipped (backward) edges
    std::vector<FlippedEdge<E, N, V>*> myFlippedEdges;
    /// @brief The k-d tree partition of the backward graph with flipped edges
    KDTreePartition<FlippedEdge<E, N, V>, FlippedNode<E, N, V>, V>* myFlippedPartition;
    /// @brief The flag informations
    std::vector<FlagInfo*> myFlagInfos;
    /// @brief The arc flag build
    AFBuild<E, N, V>* myArcFlagBuild;
    /// @brief The number of levels of the k-d tree partition of the network
    int myNumberOfLevels;
    /// @brief The number of arc flags per each edge
    int myNumberOfArcFlags;
#ifdef AFBL_DEBUG_LEVEL_0
    /// @brief The name of the arc flags file.
    // @note This is a CSV file for convenience/testing purposes
    const std::string myArcFlagsFileName;
#endif
    bool myAmClean;
};

// ===========================================================================
// method definitions
// ===========================================================================

template<class E, class N, class V>
AFBuilder<E, N, V>::~AFBuilder()
{
    delete myArcFlagBuild;
    delete myFlippedPartition;
    for (FlagInfo* flagInfo : myFlagInfos) {
        delete flagInfo;
    }
}

template<class E, class N, class V>
void AFBuilder<E, N, V>::reset()
{
    for (FlagInfo* flagInfo : myFlagInfos) {
        flagInfo->reset();
    }
    myAmClean = true;
}

template<class E, class N, class V>
std::vector<typename AFInfo<E>::FlagInfo*>& AFBuilder<E, N, V>::build(SUMOTime msTime, const V* const vehicle) {
    if (!myAmClean) {
        reset();
    }
    assert(myFlippedPartition);
    if (myFlippedPartition->isClean()) {
        myFlippedPartition->init(vehicle);
        myArcFlagBuild->setFlippedPartition(myFlippedPartition);
    }
    else {
        myFlippedPartition->reset(vehicle);
    }
    assert(myArcFlagBuild);
#ifdef AFBL_DEBUG_LEVEL_0
    bool fileExists = this->fileExists(myArcFlagsFileName);
    if (fileExists && myAmClean) {
        std::cout << "Loading arc flags from file " << myArcFlagsFileName << std::endl; 
        loadFlagsFromCsv(myArcFlagsFileName);
        std::cout << "Arc flags loaded." << std::endl;
    }
    else {
#endif
        myArcFlagBuild->init(msTime, vehicle, myFlagInfos);
#ifdef AFBL_DEBUG_LEVEL_0
    }
#endif
    delete myFlippedPartition;
    myFlippedPartition = nullptr;

#ifdef AFBL_DEBUG_LEVEL_0
    if (!fileExists) {
        std::cout << "Saving arc flags..." << std::endl;
        // save flag vectors in a CSV file (one column, flag vectors in the order of edges)
        saveFlagsToCsv(myArcFlagsFileName);
        std::cout << "Arc flags have been saved." << std::endl;
    }
#endif
    myAmClean = false;
    return myFlagInfos;
}

#ifdef AFBL_DEBUG_LEVEL_0
template<class E, class N, class V>
void AFBuilder<E, N, V>::saveFlagsToCsv(const std::string fileName)
{
    std::ofstream csvFile(fileName);
    for (FlagInfo* flagInfo : myFlagInfos) {
        if ((flagInfo->arcFlags).empty()) {
            // default flag is false / zero
            std::fill_n(std::back_inserter(flagInfo->arcFlags),
                myNumberOfArcFlags, false);
        }
        for (bool flag : flagInfo->arcFlags) {
            csvFile << flag;
        }
        csvFile << std::endl;
    }
    csvFile.close();
}

template<class E, class N, class V>
void AFBuilder<E, N, V>::loadFlagsFromCsv(const std::string fileName) {
    assert(myAmClean);
    std::string fileNameCopy = fileName;
    std::ifstream csvFile(fileNameCopy);
    std::string result;
    if (!csvFile.is_open()) {
        result = fileNameCopy.insert(0, "Could not open CSV file ");
        throw std::runtime_error(result);
    }
    for (FlagInfo* flagInfo : myFlagInfos) {
        (flagInfo->arcFlags).clear();
        std::fill_n(std::back_inserter(flagInfo->arcFlags),
            myNumberOfArcFlags, false);
        std::string line;
        if (std::getline(csvFile, line)) {
            if (line.empty()) {
                continue;
            }
            std::stringstream stringStream(line);
            std::string flagAsString(1, '\0');
            int pos = 0;
            while (stringStream.read(&flagAsString[0], 1)) {
                (flagInfo->arcFlags)[pos++] = (!flagAsString.compare("0") ? 0 : 1);
            }
        }
        else {
            result = fileNameCopy.insert(0, "CSV file ");
            throw std::runtime_error(result.append(" has not enough lines - wrong or corrupted file?"));
        }
    }
    myAmClean = false;
    csvFile.close();
}
#endif
