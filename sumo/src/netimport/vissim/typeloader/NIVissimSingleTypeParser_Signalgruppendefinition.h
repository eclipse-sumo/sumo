#ifndef NIVissimSingleTypeParser_Signalgruppendefinition_h
#define NIVissimSingleTypeParser_Signalgruppendefinition_h
/***************************************************************************
                          NIVissimSingleTypeParser_Signalgruppendefinition.h

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
// $Log$
// Revision 1.4  2005/04/27 12:24:39  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.3  2003/04/09 15:53:24  dkrajzew
// netconvert-changes: further work on Vissim-import, documentation added
//
// Revision 1.2  2003/04/07 12:17:12  dkrajzew
// further work on traffic lights import
//
// Revision 1.1  2003/02/07 11:08:42  dkrajzew
// Vissim import added (preview)
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include "../NIVissimLoader.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NIVissimSingleTypeParser_Signalgruppendefinition
 *
 */
class NIVissimSingleTypeParser_Signalgruppendefinition :
        public NIVissimLoader::VissimSingleTypeParser {
public:
    /// Constructor
    NIVissimSingleTypeParser_Signalgruppendefinition(NIVissimLoader &parent);

    /// Destructor
    ~NIVissimSingleTypeParser_Signalgruppendefinition();

    /// Parses the data type from the given stream
    bool parse(std::istream &from);

private:
    /// Parses a traffic light group which belongs to a fixed time traffic light
    bool parseFixedTime(int id, const std::string &name, int lsaid,
        std::istream &from);

    /// Parses a traffic light group which belongs to a VAS traffic light
    bool parseVAS(int id, const std::string &name, int lsaid,
        std::istream &from);

    /// Parses a traffic light group which belongs to a VSPLUS traffic light
    bool parseVSPLUS(int id, const std::string &name, int lsaid,
        std::istream &from);

    /// Parses a traffic light group which belongs to a TRENDS traffic light
    bool parseTRENDS(int id, const std::string &name, int lsaid,
        std::istream &from);

    /// Parses a traffic light group which belongs to a VAStraffic light
    bool parseVAP(int id, const std::string &name, int lsaid,
        std::istream &from);

    /// Parses a traffic light group which belongs to a TL traffic light
    bool parseTL(int id, const std::string &name, int lsaid,
        std::istream &from);

    /// Parses a traffic light group which belongs to a POS traffic light
    bool parsePOS(int id, const std::string &name, int lsaid,
        std::istream &from);

    /// Parses a traffic light group which belongs fixed time traffic light with an extern definition
    bool parseExternFixedTime(int id, const std::string &name, int lsaid,
        std::istream &from);

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
