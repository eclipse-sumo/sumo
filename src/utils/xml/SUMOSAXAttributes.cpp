/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2007-2023 German Aerospace Center (DLR) and others.
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
/// @file    SUMOSAXAttributes.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Fri, 30 Mar 2007
///
// Encapsulated SAX-Attributes
/****************************************************************************/
#include <config.h>

#include <string>
#include <iostream>
#include <sstream>
#include <utils/common/MsgHandler.h>
#include <utils/common/RGBColor.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/StringUtils.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/PositionVector.h>
#include "SUMOSAXAttributes.h"


// ===========================================================================
// static members
// ===========================================================================
const std::string SUMOSAXAttributes::ENCODING = " encoding=\"UTF-8\"";


// ===========================================================================
// method definitions
// ===========================================================================
SUMOSAXAttributes::SUMOSAXAttributes(const std::string& objectType):
    myObjectType(objectType) {}


const std::string invalid_return<std::string>::value = "";
template<>
std::string SUMOSAXAttributes::fromString(const std::string& value) const {
    if (value == "") {
        throw EmptyData();
    }
    return value;
}


SUMOTime
SUMOSAXAttributes::getSUMOTimeReporting(int attr, const char* objectid,
                                        bool& ok, bool report) const {
    try {
        bool isPresent = true;
        const std::string& val = getString(attr, &isPresent);
        if (!isPresent) {
            if (report) {
                emitUngivenError(getName(attr), objectid);
            }
            ok = false;
            return -1;
        }
        return string2time(val);
    } catch (EmptyData&) {
        if (report) {
            emitEmptyError(getName(attr), objectid);
        }
    } catch (ProcessError&) {
        if (report) {
            emitFormatError(getName(attr), "is not a valid time value", objectid);
        }
    }
    ok = false;
    return -1;
}


SUMOTime
SUMOSAXAttributes::getPeriod(const char* objectid, bool& ok, bool report) const {
    int attr = SUMO_ATTR_PERIOD;
    try {
        bool isPresent = true;
        const std::string& val = getString(attr, &isPresent);
        if (!isPresent) {
            // try 'freq' as alias for 'period'
            attr = SUMO_ATTR_FREQUENCY;
            isPresent = true;
            const std::string& valFreq = getString(attr, &isPresent);
            if (!isPresent) {
                if (report) {
                    emitUngivenError(getName(SUMO_ATTR_PERIOD), objectid);
                }
                ok = false;
                return -1;
            }
            return string2time(valFreq);
        }
        return string2time(val);
    } catch (EmptyData&) {
        if (report) {
            emitEmptyError(getName(attr), objectid);
        }
    } catch (ProcessError&) {
        if (report) {
            emitFormatError(getName(attr), "is not a valid time value", objectid);
        }
    }
    ok = false;
    return -1;
}


SUMOTime
SUMOSAXAttributes::getOptSUMOTimeReporting(int attr, const char* objectid,
        bool& ok, SUMOTime defaultValue, bool report) const {
    try {
        bool isPresent = true;
        const std::string& val = getString(attr, &isPresent);
        if (!isPresent) {
            return defaultValue;
        }
        return string2time(val);
    } catch (EmptyData&) {
        if (report) {
            emitEmptyError(getName(attr), objectid);
        }
    } catch (ProcessError&) {
        if (report) {
            emitFormatError(getName(attr), "is not a valid time value", objectid);
        }
    }
    ok = false;
    return -1;
}


SUMOTime
SUMOSAXAttributes::getOptPeriod(const char* objectid, bool& ok, SUMOTime defaultValue, bool report) const {
    int attr = SUMO_ATTR_PERIOD;
    try {
        bool isPresent = true;
        const std::string& val = getString(attr, &isPresent);
        if (!isPresent) {
            // try 'freq' as alias for 'period'
            attr = SUMO_ATTR_FREQUENCY;
            isPresent = true;
            const std::string& valFreq = getString(attr, &isPresent);
            if (!isPresent) {
                return defaultValue;
            }
            return string2time(valFreq);
        }
        return string2time(val);
    } catch (EmptyData&) {
        if (report) {
            emitEmptyError(getName(attr), objectid);
        }
    } catch (ProcessError&) {
        if (report) {
            emitFormatError(getName(attr), "is not a valid time value", objectid);
        }
    }
    ok = false;
    return -1;
}


void
SUMOSAXAttributes::emitUngivenError(const std::string& attrname, const char* objectid) const {
    std::ostringstream oss;
    oss << "Attribute '" << attrname << "' is missing in definition of ";
    if (objectid == nullptr || objectid[0] == 0) {
        oss << "a " << myObjectType;
    } else {
        oss << myObjectType << " '" << objectid << "'";
    }
    oss << ".";
    WRITE_ERROR(oss.str());
}


void
SUMOSAXAttributes::emitEmptyError(const std::string& attrname, const char* objectid) const {
    std::ostringstream oss;
    oss << "Attribute '" << attrname << "' in definition of ";
    if (objectid == nullptr || objectid[0] == 0) {
        oss << "a " << myObjectType;
    } else {
        oss << myObjectType << " '" << objectid << "'";
    }
    oss << " is empty.";
    WRITE_ERROR(oss.str());
}


void
SUMOSAXAttributes::emitFormatError(const std::string& attrname, const std::string& type, const char* objectid) const {
    std::ostringstream oss;
    oss << "Attribute '" << attrname << "' in definition of ";
    if (objectid == nullptr || objectid[0] == 0) {
        oss << "a " << myObjectType;
    } else {
        oss << myObjectType << " '" << objectid << "'";
    }
    oss << " " << type << ".";
    WRITE_ERROR(oss.str());
}


const int invalid_return<int>::value = -1;
template<>
int SUMOSAXAttributes::fromString(const std::string& value) const {
    return StringUtils::toInt(value);
}


const long long int invalid_return<long long int>::value = -1;
template<>
long long int SUMOSAXAttributes::fromString(const std::string& value) const {
    return StringUtils::toLong(value);
}


const double invalid_return<double>::value = -1;
template<>
double SUMOSAXAttributes::fromString(const std::string& value) const {
    return StringUtils::toDouble(value);
}


const bool invalid_return<bool>::value = false;
template<>
bool SUMOSAXAttributes::fromString(const std::string& value) const {
    return StringUtils::toBool(value);
}


const RGBColor invalid_return<RGBColor>::value = RGBColor();
template<>
RGBColor SUMOSAXAttributes::fromString(const std::string& value) const {
    return RGBColor::parseColor(value);
}


const Position invalid_return<Position>::value = Position();
template<>
Position SUMOSAXAttributes::fromString(const std::string& value) const {
    StringTokenizer st(value);
    // check StringTokenizer
    while (st.hasNext()) {
        // obtain position
        StringTokenizer pos(st.next(), ",");
        // check that position has X-Y or X-Y-Z
        if ((pos.size() != 2) && (pos.size() != 3)) {
            throw FormatException("is not a valid position");
        }
        // obtain x and y
        double x = StringUtils::toDouble(pos.next());
        double y = StringUtils::toDouble(pos.next());
        // check if return a X-Y or a X-Y-Z Position
        if (pos.size() == 2) {
            return Position(x, y);
        } else {
            // obtain z
            double z = StringUtils::toDouble(pos.next());
            return Position(x, y, z);
        }
    }
    // empty positions aren't allowed
    throw FormatException("is not a valid position");
}


const PositionVector invalid_return<PositionVector>::value = PositionVector();
template<>
PositionVector SUMOSAXAttributes::fromString(const std::string& value) const {
    StringTokenizer st(value);
    PositionVector shape;
    while (st.hasNext()) {
        StringTokenizer pos(st.next(), ",");
        if (pos.size() != 2 && pos.size() != 3) {
            throw FormatException("is not a valid list of positions");
        }
        double x = StringUtils::toDouble(pos.next());
        double y = StringUtils::toDouble(pos.next());
        if (pos.size() == 2) {
            shape.push_back(Position(x, y));
        } else {
            double z = StringUtils::toDouble(pos.next());
            shape.push_back(Position(x, y, z));
        }
    }
    return shape;
}


const Boundary invalid_return<Boundary>::value = Boundary();
template<>
Boundary SUMOSAXAttributes::fromString(const std::string& value) const {
    StringTokenizer st(value, ",");
    if (st.size() != 4) {
        throw FormatException("is not a valid boundary");
    }
    const double xmin = StringUtils::toDouble(st.next());
    const double ymin = StringUtils::toDouble(st.next());
    const double xmax = StringUtils::toDouble(st.next());
    const double ymax = StringUtils::toDouble(st.next());
    return Boundary(xmin, ymin, xmax, ymax);
}


const SumoXMLEdgeFunc invalid_return<SumoXMLEdgeFunc>::value = SumoXMLEdgeFunc::NORMAL;
template<>
SumoXMLEdgeFunc SUMOSAXAttributes::fromString(const std::string& value) const {
    if (SUMOXMLDefinitions::EdgeFunctions.hasString(value)) {
        return SUMOXMLDefinitions::EdgeFunctions.get(value);
    }
    throw FormatException("is not a valid edge function");
}


const SumoXMLNodeType invalid_return<SumoXMLNodeType>::value = SumoXMLNodeType::UNKNOWN;
template<>
SumoXMLNodeType SUMOSAXAttributes::fromString(const std::string& value) const {
    if (SUMOXMLDefinitions::NodeTypes.hasString(value)) {
        return SUMOXMLDefinitions::NodeTypes.get(value);
    }
    throw FormatException("is not a valid node type");
}


const RightOfWay invalid_return<RightOfWay>::value = RightOfWay::DEFAULT;
template<>
RightOfWay SUMOSAXAttributes::fromString(const std::string& value) const {
    if (SUMOXMLDefinitions::RightOfWayValues.hasString(value)) {
        return SUMOXMLDefinitions::RightOfWayValues.get(value);
    }
    throw FormatException("is not a valid right of way value");
}


const FringeType invalid_return<FringeType>::value = FringeType::DEFAULT;
template<>
FringeType SUMOSAXAttributes::fromString(const std::string& value) const {
    if (SUMOXMLDefinitions::FringeTypeValues.hasString(value)) {
        return SUMOXMLDefinitions::FringeTypeValues.get(value);
    }
    throw FormatException("is not a valid fringe type");
}


const ParkingType invalid_return<ParkingType>::value = ParkingType::ONROAD;
template<>
ParkingType SUMOSAXAttributes::fromString(const std::string& value) const {
    if (value == toString(ParkingType::OPPORTUNISTIC)) {
        return ParkingType::OPPORTUNISTIC;
    } else {
        return StringUtils::toBool(value) ? ParkingType::OFFROAD : ParkingType::ONROAD;
    }
}


const std::vector<std::string> invalid_return<std::vector<std::string> >::value = std::vector<std::string>();
template<>
std::vector<std::string> SUMOSAXAttributes::fromString(const std::string& value) const {
    const std::vector<std::string>& ret = StringTokenizer(value).getVector();
    if (ret.empty()) {
        throw EmptyData();
    }
    return ret;
}


const std::vector<int> invalid_return<std::vector<int> >::value = std::vector<int>();
template<>
std::vector<int> SUMOSAXAttributes::fromString(const std::string& value) const {
    const std::vector<std::string>& tmp = StringTokenizer(value).getVector();
    if (tmp.empty()) {
        throw EmptyData();
    }
    std::vector<int> ret;
    for (const std::string& s : tmp) {
        ret.push_back(StringUtils::toInt(s));
    }
    return ret;
}


/****************************************************************************/
