/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Verbindungsdefinition.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 18 Dec 2002
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <utils/geom/Position2DVector.h>
#include <utils/common/TplConvert.h>
#include "../NIVissimLoader.h"
#include "../tempstructs/NIVissimConnection.h"
#include "NIVissimSingleTypeParser_Verbindungsdefinition.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Verbindungsdefinition::NIVissimSingleTypeParser_Verbindungsdefinition(NIVissimLoader &parent)
        : NIVissimLoader::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Verbindungsdefinition::~NIVissimSingleTypeParser_Verbindungsdefinition() {}


bool
NIVissimSingleTypeParser_Verbindungsdefinition::parse(std::istream &from) {
    int id;
    from >> id; // type-checking is missing!
    string tag;
    // Read optional value "Name", skip optional value "Beschriftung"
    string name;
    while (tag!="von") {
        tag = overrideOptionalLabel(from);
        if (tag=="name") {
            name = readName(from);
        }
    }
    // Read the geometry information
    NIVissimExtendedEdgePoint from_def = readExtEdgePointDef(from);
    Position2DVector geom;
    tag = myRead(from); // "ueber"
    while (tag!="nach") {
        string x = myRead(from);
        string y = myRead(from);
        if (y!="nach") {
            geom.push_back_noDoublePos(
                Position2D(
                    TplConvert<char>::_2SUMOReal(x.c_str()),
                    TplConvert<char>::_2SUMOReal(y.c_str())
                ));
            tag = myRead(from);
            try {
                TplConvert<char>::_2SUMOReal(tag.c_str());
                tag = myRead(from);
            } catch (NumberFormatException &) {}
        } else {
            tag = y;
        }
    }
    NIVissimExtendedEdgePoint to_def = readExtEdgePointDef(from);
    // read some optional values until mandatory "Fahrzeugklassen" occures
    SUMOReal dxnothalt = 0;
    SUMOReal dxeinordnen = 0;
    SUMOReal zuschlag1, zuschlag2;
    zuschlag1 = zuschlag2 = 0;
    SUMOReal seglength = 0;
    tag = myRead(from);
    NIVissimConnection::Direction direction = NIVissimConnection::NIVC_DIR_ALL;
    while (tag!="fahrzeugklassen"&&tag!="sperrung"&&tag!="auswertung"&&tag!="DATAEND") {
        if (tag=="rechts") {
            direction = NIVissimConnection::NIVC_DIR_RIGHT;
        } else if (tag=="links") {
            direction = NIVissimConnection::NIVC_DIR_LEFT;
        } else if (tag=="alle") {
            direction = NIVissimConnection::NIVC_DIR_ALL;
        } else if (tag=="dxnothalt") {
            from >> dxnothalt; // type-checking is missing!
        } else if (tag=="dxeinordnen") {
            from >> dxeinordnen; // type-checking is missing!
        } else if (tag=="segment") {
            from >> tag;
            from >> seglength;
        }
        if (tag=="zuschlag") {
            from >> zuschlag1; // type-checking is missing!
            tag = readEndSecure(from);
            if (tag=="zuschlag") {
                from >> zuschlag2; // type-checking is missing!
                tag = readEndSecure(from, "auswertung");
            }
        } else {
            tag = readEndSecure(from, "auswertung");
        }
    }
    // read in allowed vehicle classes
    IntVector assignedVehicles;
    if (tag=="fahrzeugklassen") {
        tag = readEndSecure(from);
        while (tag!="DATAEND"&&tag!="sperrung"&&tag!="auswertung") {
            int classes = TplConvert<char>::_2int(tag.c_str());
            assignedVehicles.push_back(classes);
            tag = readEndSecure(from, "auswertung");
        }
    }
    // Read definitions of closed lanes
    NIVissimClosedLanesVector clv;
    if (tag!="DATAEND") {
        do {
            // check whether a next close lane definition can be found
            tag = readEndSecure(from);
            if (tag=="spur") {
                // get the lane number
//                from >> tag;
                int laneNo;
                from >> laneNo; // type-checking is missing!
                // get the list of assigned car classes
                IntVector assignedVehicles;
                tag = myRead(from);
                if (tag=="fahrzeugklassen") {
                    tag = myRead(from);
                }
                while (tag!="DATAEND"&&tag!="spur") {
                    int classes = TplConvert<char>::_2int(tag.c_str());
                    assignedVehicles.push_back(classes);
                    tag = readEndSecure(from);
                }
                // build and add the definition
                NIVissimClosedLaneDef *cld = new NIVissimClosedLaneDef(laneNo, assignedVehicles);
                clv.push_back(cld);
            }
        } while (tag!="DATAEND");
    }
    NIVissimConnection *c = new NIVissimConnection(id, name, from_def, to_def, geom,
            direction, dxnothalt, dxeinordnen, zuschlag1, zuschlag2, seglength,
            assignedVehicles, clv);

    if (!NIVissimConnection::dictionary(id, c)) {
        return false;
    }
    return true;
    //return NIVissimAbstractEdge::dictionary(id, c);
}



/****************************************************************************/

