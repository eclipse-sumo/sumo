/***************************************************************************
                          PCVisum.cpp
    A reader of polygons stored in visum-format
                             -------------------
    project              : SUMO
    subproject           : PolyConvert
    begin                : Mon, 05 Dec 2005
    copyright            : (C) 2005 by DLR http://ivf.dlr.de/
    author               : Danilo Boyom
    email                : Danilot.Tete-Boyom@dlr.de
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
// Revision 1.4  2007/01/08 14:43:58  dkrajzew
// code beautifying; prliminary import for Visum points added
//
// Revision 1.3  2006/11/17 11:14:34  dkrajzew
// warnings removed
//
// Revision 1.2  2006/08/02 10:27:21  dkrajzew
// building under Linux patched
//
// Revision 1.1  2006/08/01 07:52:47  dkrajzew
// polyconvert added
//
// Revision 1.1  2006/03/27 07:22:27  dksumo
// initial checkin
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

#include <string>
#include <map>
#include <math.h>
#include <fstream>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <polyconvert/PCPolyContainer.h>
#include "PCVisum.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method defintions
 * ======================================================================= */
PCVisum::PCVisum(PCPolyContainer &toFill)
	: myCont(toFill)
{
}


PCVisum::~PCVisum()
{
}


void
PCVisum::load(OptionsCont &oc)
{
    std::string file = oc.getString("visum-file");
    ifstream out(file.c_str());
    if(!out) {
        MsgHandler::getErrorInstance()->inform("Can not open visum-file '" + file + "'.");
        throw ProcessError();
    }
    // Polygon's Attributes
    std::string id, index, xKoord, yKoord;
    Position2DVector vec;

    std::string buff, rest;
    std::string tab = "\t";
    int l = 0;

    while(out.good()) {
        getline(out,buff);

        if(buff.find("$BEZIRKPOLY") != string::npos){

            while(buff.find("*") == string::npos){
                l++;
                getline(out,buff);
                if(buff.find("*") == string::npos){
                    continue;
                }// in the order to not read the last line

                id = buff.substr(0,buff.find(";"));
                rest = buff.substr(buff.find(";")+1, buff.length());
                index = rest.substr(0,rest.find(";"));
                rest = rest.substr(rest.find(";")+1, rest.length());
                xKoord = rest.substr(0,rest.find(";"));
                rest = rest.substr(rest.find(";")+1, rest.length());
                yKoord = rest.substr(0,rest.find(";"));
                Position2D pos2D((SUMOReal) atof(xKoord.c_str()), (SUMOReal) atof(yKoord.c_str()));
                vec.push_back(pos2D);
                throw 1;
                    /*!!!
					Polygon2D *poly = new Polygon2D(id, atoi(index.c_str()), vec);
					vec.clear();
					myCont.insert(id.append(index),poly);
					*/
			  }
		}
	}
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



