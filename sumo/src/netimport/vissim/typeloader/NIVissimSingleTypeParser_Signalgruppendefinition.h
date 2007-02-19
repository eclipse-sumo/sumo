/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Signalgruppendefinition.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 18 Dec 2002
/// @version $Id$
///
//
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
#ifndef NIVissimSingleTypeParser_Signalgruppendefinition_h
#define NIVissimSingleTypeParser_Signalgruppendefinition_h
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

#include <iostream>
#include "../NIVissimLoader.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIVissimSingleTypeParser_Signalgruppendefinition
 *
 */
class NIVissimSingleTypeParser_Signalgruppendefinition :
            public NIVissimLoader::VissimSingleTypeParser
{
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


#endif

/****************************************************************************/

