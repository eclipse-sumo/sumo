#ifndef NIArtemisParser_SignalGroups_h
#define NIArtemisParser_SignalGroups_h
/***************************************************************************
                          NIArtemisParser_SignalGroups.h
                             -------------------
    project              : SUMO
    begin                : Mon, 10 Feb 2003
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
// Revision 1.5  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/04/27 12:24:25  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.3  2003/03/17 14:18:58  dkrajzew
// Windows eol removed
//
// Revision 1.2  2003/03/12 16:44:47  dkrajzew
// further work on artemis-import
//
// Revision 1.1  2003/03/03 15:00:36  dkrajzew
// initial commit for artemis-import files
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

#include "NIArtemisLoader.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NIArtemisParser_SignalGroups
 */
class NIArtemisParser_SignalGroups :
        public NIArtemisLoader::NIArtemisSingleDataTypeParser {
public:
    /// Constructor
    NIArtemisParser_SignalGroups(NBNodeCont &nc, NIArtemisLoader &parent,
        const std::string &dataName);

    /// Destructor
    ~NIArtemisParser_SignalGroups();

protected:
    /** @brief Parses a single modality type name using data from the inherited NamedColumnsParser. */
    void myDependentReport();

    /** @brief build the loaded signals
        needs data from "Signals" and "Signal Phases", too */
    void myClose();

protected:
    NBNodeCont &myNodeCont;

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
