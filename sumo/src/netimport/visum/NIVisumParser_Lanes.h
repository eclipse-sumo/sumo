/****************************************************************************/
/// @file    NIVisumParser_Lanes.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 23 Mar 2006
/// @version $Id: $
///
// Parser for visum-lanes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIVisumParser_Lanes_h
#define NIVisumParser_Lanes_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "NIVisumLoader.h"
#include <netbuild/NBEdge.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIVisumParser_Lanes
 * Parses connectors from a visum-file. Connectors are used to connect districts
 * and the "real" road network. Both types (ingoing and outgoing) are supported.
 */
class NIVisumParser_Lanes :
            public NIVisumLoader::NIVisumSingleDataTypeParser
{
public:
    /// Constructor
    NIVisumParser_Lanes(NIVisumLoader &parent,
                        NBNodeCont &nc, NBEdgeCont &ec, NBDistrictCont &dc,
                        const std::string &dataName);

    /// Destructor
    ~NIVisumParser_Lanes();

protected:
    /** @brief Parses data of a single connector;
        Values are stored within the inherited NamedColumnsParser */
    void myDependentReport();

private:
    NBNodeCont &myNodeCont;
    NBEdgeCont &myEdgeCont;
    NBDistrictCont &myDistrictCont;

};


#endif

/****************************************************************************/

