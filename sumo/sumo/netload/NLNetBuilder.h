#ifndef NLNetBuilder_h
#define NLNetBuilder_h
/***************************************************************************
                          NLNetBuilder.h
			  Container for MSNet during its building
                             -------------------
    project              : SUMO
    begin                : Mon, 9 Jul 2001
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
// Revision 1.1  2002/04/08 07:21:24  traffic
// Initial revision
//
// Revision 2.0  2002/02/14 14:43:24  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.3  2002/02/13 15:40:45  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:10  traffic
// moved from netbuild
//
// Revision 1.5  2001/08/16 12:53:59  traffic
// further exception handling (now validated) and new comments
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <map>
#include <parsers/SAXParser.hpp>
#include <framework/XMLFormatter.hpp>
#include <sax/HandlerBase.hpp>

/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSNet;
class NLContainer;
class MSEmitControl;
class MSJunctionLogic;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * NLNetBuilder
 * The class is the main interface to load simulations.
 * It is a black-box where only the filename of the simulation to load must be
 * supplied and the net is returned.
 * It is assumed that the simulation is stored in a XML-file.
 */
class NLNetBuilder {
public:
    /// information if the parser should check the file
    static bool                     check;
    /// information if the parser should perform a more verbose parsing
    static bool                     verbose;
private:
    /// information about the text type
    static const char*              encodingName;
public:
    /// standard constructor
    NLNetBuilder(bool doCheck, bool printAll);
    /// standard destructor
    ~NLNetBuilder();
    /// loads a complete simulation specified by its name
    MSNet *loadNet(const char *path, const char *junctionsfolder);
    /// loads the routes of a simulation
    MSEmitControl *loadVehicles(const char *path);
    /// reports whether the current subpart was decoded properly
    void subreport(char *ok, char *wrong);
    /// reports statistics and errors
    void report(NLContainer *container);
private:
    /// performs the parsing of a XML-file using the specified handlers
    int parse(const char *path, const char *msg1, HandlerBase **handlerlist, int step);
    /** returns the matching net-parsing handler for the current step for 
        all steps to do before the junctions can be parsed */
    HandlerBase **getNetHandlerPreJunctions(NLContainer *container);
    /** returns the matching net-parsing handler for the current step for 
        all steps to do adter the junctions were parsed */
    HandlerBase **getNetHandlerPostJunctions(NLContainer *container);
    /// returns the matching route-parsing handler for the current step
    HandlerBase **getVehiclesHandler(NLContainer *container);
private:
    /** invalid copy operator */
    NLNetBuilder(const NLNetBuilder &s);
    /** invalid assignment operator */
    NLNetBuilder &operator=(const NLNetBuilder &s);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NLNetBuilder.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
