/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NBSign.h
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Nov 2012
/// @version $Id$
///
// A class representing a street sign
/****************************************************************************/
#ifndef NBSign_h
#define NBSign_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <utils/common/StringBijection.h>

// ===========================================================================
// class declarations
// ===========================================================================
class NBEdge;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBSign
 * @brief A class representing a single street sign
 */
class NBSign {
public:

    enum SignType {
        SIGN_TYPE_SPEED,
        SIGN_TYPE_YIELD,
        SIGN_TYPE_STOP,
        SIGN_TYPE_ALLWAY_STOP,
        SIGN_TYPE_ON_RAMP,
        SIGN_TYPE_PRIORITY,
        SIGN_TYPE_RIGHT_BEFORE_LEFT,
        SIGN_TYPE_ROUNDABOUT,
        SIGN_TYPE_RAIL_CROSSING,
        SIGN_TYPE_SLOPE,
        SIGN_TYPE_CITY,
        SIGN_TYPE_INFO // terminator
    };

    /** @brief Constructor with id, and position
     *
     * @param[in] type The type of the sign
     * @param[in] offset The offset of the sign from the start of its edge
     * @param[in] label The (optional) label (for SPEED, SLOPE etc)
     */
    NBSign(SignType type, double offset, const std::string label = "");


    /// @brief Destructor
    ~NBSign();

    /// @brief write into device as POI positioned relative to the given edge
    void writeAsPOI(OutputDevice& into, const NBEdge* edge) const;


private:
    /// @brief the type of the sign
    SignType myType;

    /// @brief The offset of the sign from the start of its edge
    double myOffset;

    /// @brief The (optional) label (for SPEED, SLOPE etc)
    std::string myLabel;

    static StringBijection<SignType> SignTypeStrings;
    static StringBijection<SignType> SignTypeColors;
};


#endif

/****************************************************************************/

