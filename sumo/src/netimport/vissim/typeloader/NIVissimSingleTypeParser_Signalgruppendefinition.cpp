/***************************************************************************
                          NIVissimSingleTypeParser_Signalgruppendefinition.cpp

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
#include <utils/common/DoubleVector.h>
#include "../NIVissimLoader.h"
#include "../tempstructs/NIVissimTL.h"
#include "NIVissimSingleTypeParser_Signalgruppendefinition.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVissimSingleTypeParser_Signalgruppendefinition::NIVissimSingleTypeParser_Signalgruppendefinition(NIVissimLoader &parent)
	: NIVissimLoader::VissimSingleTypeParser(parent)
{
}


NIVissimSingleTypeParser_Signalgruppendefinition::~NIVissimSingleTypeParser_Signalgruppendefinition()
{
}


bool
NIVissimSingleTypeParser_Signalgruppendefinition::parse(std::istream &from)
{
    //
	int id;
    from >> id; // !!!
    //
    string tag;
    tag = myRead(from);
    string name;
    if(tag=="name") {
        name = readName(from);
        tag = myRead(from);
    }
    //
    int lsaid;
    from >> lsaid;
    //
    bool isGreenBegin;
    DoubleVector times;
    tag = myRead(from);
    if(tag=="dauergruen") {
        isGreenBegin = true;
    } else if(tag=="dauerrot") {
        isGreenBegin = false;
    } else {
        // the first phase will be red
        isGreenBegin = false;
        while(tag=="rotende"||tag=="gruenanfang") {
            double point;
            from >> point; // !!!
            times.push_back(point);
            from >> tag;
            from >> point; // !!!
            times.push_back(point);
            tag = myRead(from);
        }
    }
    //
    double tredyellow, tyellow;
    from >> tredyellow;
    from >> tag;
    from >> tyellow;
    NIVissimTL::NIVissimTLSignalGroup *group =
        new NIVissimTL::NIVissimTLSignalGroup(
            lsaid, id, name, isGreenBegin, times, tredyellow, tyellow);
    assert(NIVissimTL::NIVissimTLSignalGroup::dictionary(lsaid, id, group));
    return true;
}


