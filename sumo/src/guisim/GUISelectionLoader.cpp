//---------------------------------------------------------------------------//
//                        GUISelectionLoader.cpp -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 29.05.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

// $Log$
// Revision 1.5  2005/10/07 11:37:17  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/15 11:06:37  dkrajzew
// LARGE CODE RECHECK
//
//
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

#include <guisim/GUIEdge.h>
#include <microsim/MSLane.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include "GUISelectionLoader.h"
#include <fstream>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

using namespace std;

/* =========================================================================
 * member method definitions
 * ======================================================================= */
void
GUISelectionLoader::loadSelection(const std::string &file)
{
    // ok, load all
    std::map<std::string, int> typeMap;
    typeMap["edge"] = GLO_EDGE;
    typeMap["induct loop"] = GLO_DETECTOR;
    typeMap["junction"] = GLO_JUNCTION;
    typeMap["speedtrigger"] = GLO_TRIGGER;
    typeMap["lane"] = GLO_LANE;
    typeMap["tl-logic"] = GLO_TLLOGIC;
    typeMap["vehicle"] = GLO_VEHICLE;
    ifstream strm(file.c_str());
    while(strm.good()) {
        string name;
        strm >> name;
        if(name.length()==0) {
            continue;
        }
        size_t idx = name.find(':');
        if(idx!=string::npos) {
            string type = name.substr(0, idx);
            name = name.substr(idx+1);
            if(typeMap.find(type)==typeMap.end()) {
                // !!! inform user or something - the info does not fit to the pattern
                throw 1;
            }
            int itype = typeMap[type];
            int oid = -1;
            switch(itype) {
            case GLO_VEHICLE:
                {
                }
                break;
            case GLO_TLLOGIC:
                {
                }
                break;
            case GLO_DETECTOR:
                {
                }
                break;
            case GLO_EMITTER:
                {
                }
                break;
            case GLO_LANE:
                {
                    MSLane *l = MSLane::dictionary(name);
                    if(l!=0) {
                        oid = static_cast<const GUIEdge&>(l->edge()).getLaneGeometry(l).getGlID();
                    }
                }
                break;
            case GLO_EDGE:
                {
                }
                break;
            case GLO_JUNCTION:
                {
                }
                break;
            case GLO_TRIGGER:
                {
                }
                break;
            }
            if(oid>=0) {
                gSelected.select(itype, oid, false);
            } else {
                // !!! inform user or something - the object was not found
            }
        } else {
            // !!! inform user or something - the info does not fit to the pattern
            throw 1;
        }
    }
}


GUISelectionLoader::GUISelectionLoader()
{
}


GUISelectionLoader::~GUISelectionLoader()
{
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
