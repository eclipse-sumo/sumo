/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Signalgruppendefinition.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 18 Dec 2002
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIVissimSingleTypeParser_Signalgruppendefinition_h
#define NIVissimSingleTypeParser_Signalgruppendefinition_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include "../NIImporter_Vissim.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIVissimSingleTypeParser_Signalgruppendefinition
 *
 */
class NIVissimSingleTypeParser_Signalgruppendefinition :
    public NIImporter_Vissim::VissimSingleTypeParser {
public:
    /// Constructor
    NIVissimSingleTypeParser_Signalgruppendefinition(NIImporter_Vissim& parent);

    /// Destructor
    ~NIVissimSingleTypeParser_Signalgruppendefinition();

    /// Parses the data type from the given stream
    bool parse(std::istream& from);

private:
    /// Parses a traffic light group which belongs to a fixed time traffic light
    bool parseFixedTime(int id, const std::string& name, int lsaid,
                        std::istream& from);

    /// Parses a traffic light group which belongs to a VAS traffic light
    bool parseVAS(int id, const std::string& name, int lsaid,
                  std::istream& from);

    /// Parses a traffic light group which belongs to a VSPLUS traffic light
    bool parseVSPLUS(int id, const std::string& name, int lsaid,
                     std::istream& from);

    /// Parses a traffic light group which belongs to a TRENDS traffic light
    bool parseTRENDS(int id, const std::string& name, int lsaid,
                     std::istream& from);

    /// Parses a traffic light group which belongs to a VAStraffic light
    bool parseVAP(int id, const std::string& name, int lsaid,
                  std::istream& from);

    /// Parses a traffic light group which belongs to a TL traffic light
    bool parseTL(int id, const std::string& name, int lsaid,
                 std::istream& from);

    /// Parses a traffic light group which belongs to a POS traffic light
    bool parsePOS(int id, const std::string& name, int lsaid,
                  std::istream& from);

    /// Parses a traffic light group which belongs fixed time traffic light with an extern definition
    bool parseExternFixedTime(int id, const std::string& name, int lsaid,
                              std::istream& from);

};


#endif

/****************************************************************************/

