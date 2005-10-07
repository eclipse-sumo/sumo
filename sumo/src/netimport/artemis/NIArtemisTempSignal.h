#ifndef NIArtemisTempSignal_h
#define NIArtemisTempSignal_h
//---------------------------------------------------------------------------//
//                        NIArtemisTempSignal.h -  ccc
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.9  2005/10/07 11:39:05  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.8  2005/09/23 06:01:53  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.7  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2005/04/27 12:24:25  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.5  2003/06/05 11:44:51  dkrajzew
// class templates applied; documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H


#include <string>
#include <map>
#include <netbuild/NBConnectionDefs.h>
#include <netbuild/NBConnection.h>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class NIArtemisTempSignal {
public:
    static void addPhase(const std::string &nodeid,
        char phaseName, SUMOReal perc, int start);
    static void addConnectionPhases(const NBConnection &c,
        char startPhase, char endPhase, const std::string &group);
    static void close();
private:

    class PhaseDescription {
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


    class ConnectionPhases {
    public:
        ConnectionPhases(char startPhase, char endPhase);
        ~ConnectionPhases();
    private:
        char myStartPhase, myEndPhase;
    };

    typedef std::map<NBConnection, ConnectionPhases*> ConnectionToPhases;

    static ConnectionToPhases myPhasesDict;

};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

