/***************************************************************************
                          NIVissimSingleTypeParser_Verbindungsdefinition.cpp

                             -------------------
    begin                : Wed, 18 Dec 2002
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.8  2003/07/30 09:24:04  dkrajzew
// reinserted the needed but removed check
//
// Revision 1.7  2003/07/22 15:11:25  dkrajzew
// removed warnings
//
// Revision 1.6  2003/05/20 09:42:38  dkrajzew
// all data types implemented
//
// Revision 1.5  2003/03/26 12:17:14  dkrajzew
// further debugging/improvements of Vissim-import
//
// Revision 1.4  2003/03/20 16:32:24  dkrajzew
// windows eol removed
//
// Revision 1.3  2003/03/18 13:11:53  dkrajzew
// debugging
//
// Revision 1.2  2003/03/06 16:26:59  dkrajzew
// debugging
//
// Revision 1.1  2003/02/07 11:08:43  dkrajzew
// Vissim import added (preview)
//
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include <utils/geom/Position2DVector.h>
#include <utils/convert/TplConvert.h>
#include "../NIVissimLoader.h"
#include "../tempstructs/NIVissimConnection.h"
#include "NIVissimSingleTypeParser_Verbindungsdefinition.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVissimSingleTypeParser_Verbindungsdefinition::NIVissimSingleTypeParser_Verbindungsdefinition(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser_Verbindungsdefinition::~NIVissimSingleTypeParser_Verbindungsdefinition()
{
}


bool
NIVissimSingleTypeParser_Verbindungsdefinition::parse(std::istream &from)
{
	int id;
    from >> id; // type-checking is missing!
    string tag;
    // Read optional value "Name", skip optional value "Beschriftung"
    string name;
    while(tag!="von") {
        tag = overrideOptionalLabel(from);
        if(tag=="name") {
            name = readName(from);
        }
    }
    // Read the geometry information
    NIVissimExtendedEdgePoint from_def = readExtEdgePointDef(from);
    Position2DVector geom;
    tag = myRead(from); // "ueber"
    while(tag!="nach") {
        string x = myRead(from);
        string y = myRead(from);
        if(y!="nach") {
            geom.push_back(
                Position2D(
                    TplConvert<char>::_2float(x.c_str()),
                    TplConvert<char>::_2float(y.c_str())
                ));
            tag = myRead(from);
            try {
                double tmp = TplConvert<char>::_2float(tag.c_str());
                tag = myRead(from);
            } catch (NumberFormatException &e) {
            }
        } else {
            tag = y;
        }
    }
    NIVissimExtendedEdgePoint to_def = readExtEdgePointDef(from);
    // read some optional values until mandatory "Fahrzeugklassen" occures
    double dxnothalt = 0;
    double dxeinordnen = 0;
    double zuschlag1, zuschlag2;
    zuschlag1 = zuschlag2 = 0;
    double seglength = 0;
    tag = myRead(from);
    NIVissimConnection::Direction direction;
    while(tag!="fahrzeugklassen"&&tag!="sperrung"&&tag!="auswertung"&&tag!="DATAEND") {
        if(tag=="rechts") {
            direction = NIVissimConnection::NIVC_DIR_RIGHT;
        } else if(tag=="links") {
            direction = NIVissimConnection::NIVC_DIR_LEFT;
        } else if(tag=="alle") {
            direction = NIVissimConnection::NIVC_DIR_ALL;
        }
        else if(tag=="dxnothalt") {
            from >> dxnothalt; // type-checking is missing!
        } else if(tag=="dxeinordnen") {
            from >> dxeinordnen; // type-checking is missing!
        }
        else if(tag=="segment") {
            from >> tag;
            from >> seglength;
        }
        if(tag=="zuschlag") {
            from >> zuschlag1; // type-checking is missing!
            tag = readEndSecure(from);
            if(tag=="zuschlag") {
                from >> zuschlag2; // type-checking is missing!
                tag = readEndSecure(from, "auswertung");
            }
        } else {
            tag = readEndSecure(from, "auswertung");
        }
    }
    // read in allowed vehicle classes
    IntVector assignedVehicles;
    if(tag=="fahrzeugklassen") {
        tag = readEndSecure(from);
        while(tag!="DATAEND"&&tag!="sperrung"&&tag!="auswertung") {
            int classes = TplConvert<char>::_2int(tag.c_str());
            assignedVehicles.push_back(classes);
            tag = readEndSecure(from, "auswertung");
        }
    }
    // Read definitions of closed lanes
    NIVissimClosedLanesVector clv;
    if(tag!="DATAEND") {
        do {
            // check whether a next close lane definition can be found
            tag = readEndSecure(from);
            if(tag=="spur") {
                // get the lane number
//                from >> tag;
                int laneNo;
                from >> laneNo; // type-checking is missing!
                // get the list of assigned car classes
                IntVector assignedVehicles;
                tag = myRead(from);
                if(tag=="fahrzeugklassen") {
                    tag = myRead(from);
                }
                while(tag!="DATAEND"&&tag!="spur") {
                    int classes = TplConvert<char>::_2int(tag.c_str());
                    assignedVehicles.push_back(classes);
                    tag = readEndSecure(from);
                }
                // build and add the definition
                NIVissimClosedLaneDef *cld = new NIVissimClosedLaneDef(laneNo, assignedVehicles);
                clv.push_back(cld);
            }
        } while(tag!="DATAEND");
    }
    NIVissimConnection *c = new NIVissimConnection(id, name, from_def, to_def, geom,
        direction, dxnothalt, dxeinordnen, zuschlag1, zuschlag2, seglength,
        assignedVehicles, clv);

    if(!NIVissimConnection::dictionary(id, c)) {
        return false;
    }
    return true;
    //return NIVissimAbstractEdge::dictionary(id, c);
}

