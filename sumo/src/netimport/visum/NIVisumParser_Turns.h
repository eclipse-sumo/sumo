#ifndef NIVisumParser_Turns_h
#define NIVisumParser_Turns_h
/***************************************************************************
                          NIVisumParser_Turns.h
			  Parser for turn descriptions stored in visum-files
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
// Revision 1.2  2005/04/27 12:24:42  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.1  2003/02/07 11:14:54  dkrajzew
// updated
//
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
#include <map>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NIVisumParser_Turns
 * This class parses turn descriptions, defining which turns are possible
 * at a certain junction from their visum-representation.
 */
class NIVisumParser_Turns :
        public NIVisumLoader::NIVisumSingleDataTypeParser {
public:
    /// Constructor
    NIVisumParser_Turns(NIVisumLoader &parent, NBNodeCont &nc,
        const std::string &dataName,
        NIVisumLoader::VSysTypeNames &vsystypes);

    /// Destructor
    ~NIVisumParser_Turns();

protected:
    /** @brief Parses a single turn definition using data from the inherited NamedColumnsParser. */
    void myDependentReport();

private:
    /** @brief checks whether a retrieved node exists;
        Reports an error if not */
    bool checkNode(NBNode *node, const std::string &type,
        const std::string &nodeTypeName);

    /** Returns the information whether the current turn is valid for the wished modality */
    bool isVehicleTurning();

private:
    /// a map of VSysTypes to the traffic type they represent
    NIVisumLoader::VSysTypeNames &usedVSysTypes;

    NBNodeCont &myNodeCont;

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
