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
// Revision 1.6  2003/04/16 09:59:53  dkrajzew
// further work on Vissim-import
//
// Revision 1.5  2003/04/09 15:53:24  dkrajzew
// netconvert-changes: further work on Vissim-import, documentation added
//
// Revision 1.4  2003/04/07 12:17:11  dkrajzew
// further work on traffic lights import
//
// Revision 1.3  2003/04/01 15:24:44  dkrajzew
// parsing of parking places patched
//
// Revision 1.2  2003/03/26 12:17:14  dkrajzew
// further debugging/improvements of Vissim-import
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
    NIVissimTL *tl = NIVissimTL::dictionary(lsaid);
    if(tl==0) {
        SErrorHandler::add(
            string("A traffic light group with an unknown traffic light occured."));
        SErrorHandler::add(
            string("  Group-ID: ") + toString<int>(id)
            );
        SErrorHandler::add(
            string("  TrafficLight-ID: ") + toString<int>(lsaid)
            );
        return false;
    }
    string type = tl->getType();
    if(type=="festzeit") {
        return parseFixedTime(id, name, lsaid, from);
    } if(type=="festzeit_fake") {
        return parseExternFixedTime(id, name, lsaid, from);
    } if(type=="vas") {
        return parseVAS(id, name, lsaid, from);
    } if(type=="vsplus") {
        return parseVSPLUS(id, name, lsaid, from);
    } if(type=="trends") {
        return parseTRENDS(id, name, lsaid, from);
    } if(type=="vap") {
        return parseVAP(id, name, lsaid, from);
    } if(type=="tl") {
        return parseTL(id, name, lsaid, from);
    } if(type=="pos") {
        return parsePOS(id, name, lsaid, from);
    }
    SErrorHandler::add(
        string("Unsupported LSA-Type '") + type + string("' occured."));
    return false;
}


bool
NIVissimSingleTypeParser_Signalgruppendefinition::parseFixedTime(
        int id, const std::string &name, int lsaid, std::istream &from)
{
    //
    bool isGreenBegin;
    DoubleVector times;
    string tag = myRead(from);
    if(tag=="dauergruen") {
        isGreenBegin = true;
        from >> tag;
    } else if(tag=="dauerrot") {
        isGreenBegin = false;
        from >> tag;
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
    if(!NIVissimTL::NIVissimTLSignalGroup::dictionary(lsaid, id, group)) {
        throw 1; // !!!
    }
    return true;
}


bool
NIVissimSingleTypeParser_Signalgruppendefinition::parseVAS(
        int id, const std::string &name, int lsaid, std::istream &from)
{
    cout << "Warning: VAS traffic lights are not supported." << endl;
    string tag;
    while(tag!="detektoren") {
        tag = myRead(from);
    }
    return true;
}


bool
NIVissimSingleTypeParser_Signalgruppendefinition::parseVSPLUS(
        int id, const std::string &name, int lsaid, std::istream &from)
{
    cout << "Warning: VSPLUS traffic lights are not supported." << endl;
    return true;
}


bool
NIVissimSingleTypeParser_Signalgruppendefinition::parseTRENDS(
        int id, const std::string &name, int lsaid, std::istream &from)
{
    cout << "Warning: TRENDS traffic lights are not supported." << endl;
    return true;
}


bool
NIVissimSingleTypeParser_Signalgruppendefinition::parseVAP(
        int id, const std::string &name, int lsaid, std::istream &from)
{
    cout << "Warning: VAS traffic lights are not supported." << endl;
    return true;
}


bool
NIVissimSingleTypeParser_Signalgruppendefinition::parseTL(
        int id, const std::string &name, int lsaid, std::istream &from)
{
    cout << "Warning: VAS traffic lights are not supported." << endl;
    return true;
}


bool
NIVissimSingleTypeParser_Signalgruppendefinition::parsePOS(
        int id, const std::string &name, int lsaid, std::istream &from)
{
    cout << "Warning: VAS traffic lights are not supported." << endl;
    return true;
}


bool
NIVissimSingleTypeParser_Signalgruppendefinition::parseExternFixedTime(
        int id, const std::string &name, int lsaid, std::istream &from)
{
    cout << "Warning: VAS traffic lights are not supported." << endl;
    return true;
}



