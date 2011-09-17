/****************************************************************************/
/// @file    NWWriter_OpenDrive.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 04.05.2011
/// @version $Id: NWWriter_OpenDrive.h 10238 2011-05-05 20:14:59Z behrisch $
///
// Exporter writing networks using the MATSim format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NWWriter_OpenDrive_h
#define NWWriter_OpenDrive_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NBEdge;
class NBEdgeCont;
class NBNetBuilder;
class NBNode;
class NBNodeCont;
class NBTrafficLightLogicCont;
class NBTypeCont;
class OptionsCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NWWriter_OpenDrive
 */
class NWWriter_OpenDrive {
public:
    static void writeNetwork(const OptionsCont &oc, NBNetBuilder &nb);

};


#endif

/****************************************************************************/

