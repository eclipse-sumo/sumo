#ifndef NIVisumParser_Connectors_h
#define NIVisumParser_Connectors_h
/***************************************************************************
                          NIVisumParser_Connectors.h
			  Parser for visum-connectors
                             -------------------
    project              : SUMO
    begin                : Thu, 14 Nov 2002
    copyright            : (C) 2002 by DLR/IVF http://ivf.dlr.de/
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
// $Log$
// Revision 1.3  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/04/27 12:24:41  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.1  2003/02/07 11:14:54  dkrajzew
// updated
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "NIVisumLoader.h"
#include <netbuild/NBEdge.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NIVisumParser_Connectors
 * Parses connectors from a visum-file. Connectors are used to connect districts
 * and the "real" road network. Both types (ingoing and outgoing) are supported.
 */
class NIVisumParser_Connectors :
        public NIVisumLoader::NIVisumSingleDataTypeParser {
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

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:

