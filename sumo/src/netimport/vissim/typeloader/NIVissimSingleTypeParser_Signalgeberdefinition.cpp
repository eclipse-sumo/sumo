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
// Revision 1.14  2005/10/07 11:40:30  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.13  2005/09/23 06:02:58  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.12  2005/04/27 12:24:39  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.11  2004/11/23 10:23:53  dkrajzew
// debugging
//
// Revision 1.10  2003/11/11 08:24:52  dkrajzew
// debug values removed
//
// Revision 1.9  2003/07/07 08:29:54  dkrajzew
// Warnings are now reported to the MsgHandler
//
// Revision 1.8  2003/06/18 11:35:30  dkrajzew
// message subsystem changes applied and some further work done; seems to be stable but is not perfect, yet
//
// Revision 1.7  2003/05/20 09:42:37  dkrajzew
// all data types implemented
//
// Revision 1.6  2003/04/16 09:59:52  dkrajzew
// further work on Vissim-import
//
// Revision 1.5  2003/04/09 15:53:23  dkrajzew
// netconvert-changes: further work on Vissim-import, documentation added
//
// Revision 1.4  2003/04/07 12:17:11  dkrajzew
// further work on traffic lights import
//
// Revision 1.3  2003/03/20 16:32:24  dkrajzew
// windows eol removed
//
// Revision 1.2  2003/03/06 16:26:58  dkrajzew
// debugging
//
// Revision 1.1  2003/02/07 11:08:43  dkrajzew
// Vissim import added (preview)
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <cassert>
#include <iostream>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/IntVector.h>
#include "../NIVissimLoader.h"
#include "../tempstructs/NIVissimTL.h"
#include "NIVissimSingleTypeParser_Signalgeberdefinition.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


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
    int lsaid;
    IntVector groupids;
    if(tag=="lsa") {
        int groupid;
        from >> lsaid; // type-checking is missing!
        from >> tag; // "Gruppe"
        do {
            from >> groupid;
            groupids.push_back(groupid);
            tag = myRead(from);
        } while(tag=="oder");
        //
    } else {
        from >> tag; // strecke
        WRITE_WARNING("Omitting unknown traffic light!!!");
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
    IntVector assignedVehicleTypes = parseAssignedVehicleTypes(from, "N/A");
    //
    NIVissimTL *tl = NIVissimTL::dictionary(lsaid);
    NIVissimTL::NIVissimTLSignal *signal =
        new NIVissimTL::NIVissimTLSignal(lsaid, id, name, groupids, edgeid,
            laneno, (SUMOReal) position, assignedVehicleTypes);
    if(!NIVissimTL::NIVissimTLSignal::dictionary(lsaid, id, signal)) {
        throw 1; // !!!
    }
    return true;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

