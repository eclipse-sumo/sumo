/****************************************************************************/
/// @file    NIArtemisTempSignal.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
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
#ifndef NIArtemisTempSignal_h
#define NIArtemisTempSignal_h


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
#include <netbuild/NBConnectionDefs.h>
#include <netbuild/NBConnection.h>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class NIArtemisTempSignal
{
public:
    static void addPhase(const std::string &nodeid,
                         char phaseName, SUMOReal perc, int start);
    static void addConnectionPhases(const NBConnection &c,
                                    char startPhase, char endPhase, const std::string &group);
    static void close();
private:

    class PhaseDescription
    {
    public:
        PhaseDescription(SUMOReal perc, size_t start);
        ~PhaseDescription();
    private:
        SUMOReal myPercentage;
        size_t myBegin;
    };


    typedef std::map<char, PhaseDescription*> NameToDescMap;
    typedef std::map<std::string, NameToDescMap> NodeToDescsMap;

    static NodeToDescsMap myDescDict;


    class ConnectionPhases
    {
    public:
        ConnectionPhases(char startPhase, char endPhase);
        ~ConnectionPhases();
    private:
        char myStartPhase, myEndPhase;
    };

    typedef std::map<NBConnection, ConnectionPhases*> ConnectionToPhases;

    static ConnectionToPhases myPhasesDict;

};


#endif

/****************************************************************************/

