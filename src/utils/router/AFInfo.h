/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2023 German Aerospace Center (DLR) and others.
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
/// @file    AFInfo.h
/// @author  Ruediger Ebendt
/// @date    01.12.2023
///
// Definitions of informations associated with an edge for use in the arc flag 
// routing algorithm (Hilger et al.). In more detail, these informations are: 
// flag information for arc flag routing, a key for sorting the heap, a flag 
// indicating whether the edge has already been touched, labels for holding
// (tentative / final) distances to the boundary nodes
/****************************************************************************/
#pragma once
#include <config.h>
#include <unordered_set>

template<class E>
class AFInfo {
public:
    /**
     * @class FlagInfo
     *
     * It holds a pointer to the associated edge, and flag information 
     * for arc flag routing 
     */
    class FlagInfo {
    public:
        /** @brief Constructor
        * param[in] e The edge
        */
        FlagInfo(const E* const e) :
            edge(e)
        {
        }
        /** @brief Copy constructor
         * @param[in] other The other FLAG info instance to copy
         * @return This instance (i.e., the result of the copy)
         */
        FlagInfo& operator=(const FlagInfo& other)
        {
            // guard self assignment
            if (this == &other)
                return *this;
            edge = other.edge;
            arcFlags = other.arcFlags;
            return *this;
        }
        /// @brief Destructor
        virtual ~FlagInfo() {}
        /// @brief Reset the flag information
        virtual void reset() {
            arcFlags.clear();
        }
        /// @brief The current edge
        const E* const edge;
        /// @brief The arc flags
        std::vector<bool> arcFlags;
    }; // end of class FlagInfo declaration
    
    /**
     * @class ArcInfoBase
     *
     * Derived from FlagInfo. Therefore holds a pointer 
     * to the associated edge, and flag information for arc flag routing. 
     * Additionally, it holds a key for sorting the heap, and a flag 
     * indicating whether the edge has already been touched
     */
    class ArcInfoBase : public FlagInfo {
    public:
        /** @brief Constructor
         * param[in] e The edge
         */
        ArcInfoBase(const E* const e) :
            FlagInfo(e),
            key(std::numeric_limits<double>::max()),
            touched(false)
        {
        }
        /** @brief Copy constructor
         * @param[in] other The other ArcInfoBase instance to copy
         * @return This instance (i.e., the result of the copy)
         */
        ArcInfoBase& operator=(const ArcInfoBase& other)
        {
            // guard self assignment
            if (this == &other)
                return *this;
            static_cast<FlagInfo>(this) = static_cast<FlagInfo>(other);
            key = other.key;
            touched = other.touched;
            return *this;
        }
        ~ArcInfoBase() {}
        void reset() {
            // do nothing
        }
        /// @brief The arc flags
        std::vector<bool> arcFlags;
        /// @brief The key for sorting the heap
        double key;
        /// @brief The flag indicating whether the edge has already been touched or not
        bool touched;
    }; // end of class ArcInfoBase declaration

    /**
     * @class ArcInfo
     * 
     * Derived from ArcInfoBase. Therefore it tholds a 
     * pointer to the associated edge, flag information for arc flag routing, 
     * a key for sorting the heap, a flag indicating whether the edge has 
     * already been touched. Additionally, it has labels for holding 
     * (tentative/final) distances to the boundary nodes
     */
    class ArcInfo : public ArcInfoBase {
    public:
        /** @brief Constructor
         * param[in] e The edge
         */ 
        ArcInfo(const E* const e) :
            ArcInfoBase(e)
        {
        }
        /** @brief Copy constructor
          * @param[in] other The other arc info instance to copy
          * @return This instance (i.e., the result of the copy)
          */
        ArcInfo& operator=(const ArcInfo& other)
        {
            // guard self assignment
            if (this == &other)
                return *this;
            static_cast<ArcInfoBase>(this) = static_cast<ArcInfoBase>(other);
            effortsToBoundaryNodes = other.effortsToBoundaryNodes;
            return *this;
        }
        /// @brief Destructor
        ~ArcInfo() {}
        /// @brief Reset the arc information
        void reset() {
            // do nothing
        }
        /// @brief The efforts to boundary nodes
        std::vector<double> effortsToBoundaryNodes;
    }; // end of class ArcInfo declaration
};
