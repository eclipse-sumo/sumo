/***************************************************************************
                          NIVissimSingleTypeParser_Signalgeberdefinition.cpp

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
// Revision 1.2  2003/03/06 16:26:58  dkrajzew
// debugging
//
// Revision 1.1  2003/02/07 11:08:43  dkrajzew
// Vissim import added (preview)
//
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <cassert>
#include <iostream>
#include <utils/convert/TplConvert.h>
#include <utils/convert/ToString.h>
#include <utils/common/SErrorHandler.h>
#include <utils/common/IntVector.h>
#include "../NIVissimLoader.h"
#include "../tempstructs/NIVissimTL.h"
#include "NIVissimSingleTypeParser_Signalgeberdefinition.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVissimSingleTypeParser_Signalgeberdefinition::NIVissimSingleTypeParser_Signalgeberdefinition(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser_Signalgeberdefinition::~NIVissimSingleTypeParser_Signalgeberdefinition()
{
}


bool
NIVissimSingleTypeParser_Signalgeberdefinition::parse(std::istream &from)
{
    //
    int id;
    from >> id;
    //
    string tag, name;
    tag = myRead(from);
    if(tag=="name") {
        name = readName(from);
        tag = myRead(from);
    }
    // skip optional "Beschriftung"
    tag = overrideOptionalLabel(from, tag);
    //
    int vwished = -1;
    int lsaid, groupid;
    if(tag=="lsa") {
        from >> lsaid; // !!!
        from >> tag; // "Gruppe"
        from >> groupid;
        tag = myRead(from);
        if(tag=="oder") {
            cout << "NIVissimSingleTypeParser_Signalgeberdefinition: multiplicity" << endl;
            return true;
        }
        //
    } else {
        from >> tag;
        from >> vwished;
        cout << "Omitting unknown traffic light!!!" << endl;
        return true;
    }
    //
    from >> tag;
    int edgeid;
    from >> edgeid;

    from >> tag;
    int laneno;
    from >> laneno;

    from >> tag;
    int position;
    from >> position;
    //
    while(tag!="fahrzeugklassen") {
        tag = myRead(from);
    }
    IntVector assignedVehicleTypes = parseAssignedVehicleTypes(from, "BLA");
    //
    NIVissimTL::NIVissimTLSignal *signal =
        new NIVissimTL::NIVissimTLSignal(lsaid, id, name, groupid, edgeid,
            laneno, position, assignedVehicleTypes);
    assert(NIVissimTL::NIVissimTLSignal::dictionary(lsaid, id, signal));
    return true;
}


