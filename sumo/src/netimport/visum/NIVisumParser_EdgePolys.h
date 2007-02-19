/****************************************************************************/
/// @file    NIVisumParser_EdgePolys.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 02. Nov 2004
/// @version $Id$
///
// Parser for visum-edge-geometries
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
#ifndef NIVisumParser_EdgePolys_h
#define NIVisumParser_EdgePolys_h
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


// ===========================================================================
// class declarations
// ===========================================================================
class NBNode;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIVisumParser_EdgePolys
 * Parses edges from their visum format.
 */
class NIVisumParser_EdgePolys :
            public NIVisumLoader::NIVisumSingleDataTypeParser
{
public:
    /// Constructor
    NIVisumParser_EdgePolys(NIVisumLoader &parent, NBNodeCont &nc,
                            const std::string &dataName);

    /// Destructor
    ~NIVisumParser_EdgePolys();

protected:
    /** @brief Parses a single edge using data from the inherited NamedColumnsParser. */
    void myDependentReport();

private:
    /// Checks whether the nodes are ok
    bool checkNodes(NBNode *from, NBNode *to) const;

private:
    NBNodeCont &myNodeCont;

};


#endif

/****************************************************************************/

