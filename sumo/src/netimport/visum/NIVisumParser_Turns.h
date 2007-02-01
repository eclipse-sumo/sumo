/****************************************************************************/
/// @file    NIVisumParser_Turns.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 14 Nov 2002
/// @version $Id: $
///
// Parser for turn descriptions stored in visum-files
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
#ifndef NIVisumParser_Turns_h
#define NIVisumParser_Turns_h
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
#include <map>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIVisumParser_Turns
 * This class parses turn descriptions, defining which turns are possible
 * at a certain junction from their visum-representation.
 */
class NIVisumParser_Turns :
            public NIVisumLoader::NIVisumSingleDataTypeParser
{
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
    /** Returns the information whether the current turn is valid for the wished modality */
    bool isVehicleTurning();

private:
    /// a map of VSysTypes to the traffic type they represent
    NIVisumLoader::VSysTypeNames &usedVSysTypes;

    NBNodeCont &myNodeCont;

};


#endif

/****************************************************************************/

