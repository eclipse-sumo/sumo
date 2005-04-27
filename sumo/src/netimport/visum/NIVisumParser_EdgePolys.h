#ifndef NIVisumParser_EdgePolys_h
#define NIVisumParser_EdgePolys_h
/***************************************************************************
                          NIVisumParser_EdgePolys.h
              Parser for visum-edge-geometries
                             -------------------
    project              : SUMO
    begin                : Tue, 02. Nov 2004
    copyright            : (C) 2004 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.2  2005/04/27 12:24:41  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.1  2004/11/23 10:24:54  dkrajzew
// added the possibility to read Visum geometries
//
// Revision 1.1  2004/11/22 12:47:11  dksumo
// added the possibility to parse visum-geometries
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include "NIVisumLoader.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBNode;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NIVisumParser_EdgePolys
 * Parses edges from their visum format.
 */
class NIVisumParser_EdgePolys :
        public NIVisumLoader::NIVisumSingleDataTypeParser {
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


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:

