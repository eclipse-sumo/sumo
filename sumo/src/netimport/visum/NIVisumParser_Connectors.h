/****************************************************************************/
/// @file    NIVisumParser_Connectors.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 14 Nov 2002
/// @version $Id: $
///
// Parser for visum-connectors
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
#ifndef NIVisumParser_Connectors_h
#define NIVisumParser_Connectors_h
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
 * @class NIVisumParser_Connectors
 * Parses connectors from a visum-file. Connectors are used to connect districts
 * and the "real" road network. Both types (ingoing and outgoing) are supported.
 */
class NIVisumParser_Connectors :
            public NIVisumLoader::NIVisumSingleDataTypeParser
{
public:
    /// Constructor
    NIVisumParser_Connectors(NIVisumLoader &parent,
                             NBNodeCont &nc, NBEdgeCont &ec, NBTypeCont &tc, NBDistrictCont &dc,
                             const std::string &dataName);

    /// Destructor
    ~NIVisumParser_Connectors();

protected:
    /** @brief Parses data of a single connector;
        Values are stored within the inherited NamedColumnsParser */
    void myDependentReport();

private:
    /// Builds a node which belongs to a district - is not a part of the "real" road network
    NBNode *buildDistrictNode(const std::string &id, NBNode *dest,
                              NBEdge::EdgeBasicFunction dir);

private:
    NBNodeCont &myNodeCont;
    NBEdgeCont &myEdgeCont;
    NBTypeCont &myTypeCont;
    NBDistrictCont &myDistrictCont;

};


#endif

/****************************************************************************/

