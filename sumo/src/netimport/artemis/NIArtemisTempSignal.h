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
// Revision 1.5  2003/06/05 11:44:51  dkrajzew
// class templates applied; documentation added
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
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
        char phaseName, double perc, int start);
    static void addConnectionPhases(const NBConnection &c,
        char startPhase, char endPhase, const std::string &group);
    static void close();
private:

    class PhaseDescription {
    public:
        PhaseDescription(double perc, size_t start);
        ~PhaseDescription();
    private:
        double myPercentage;
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
//#ifndef DISABLE_INLINE
//#include "NIArtemisTempSignal.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

