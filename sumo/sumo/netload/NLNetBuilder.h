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
// Revision 1.4  2002/04/24 10:32:05  dkrajzew
// Unfound files are now only reported once
//
// Revision 1.3  2002/04/17 11:18:47  dkrajzew
// windows-newlines removed
//
// Revision 1.2  2002/04/15 07:07:56  dkrajzew
// new loading paradigm implemented
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
#include <vector>
#include <parsers/SAXParser.hpp>
#include <framework/XMLFormatter.hpp>
#include <sax2/DefaultHandler.hpp>
#include "../microsim/MSNet.h"
#include "NLLoadFilter.h"

/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSNet;
class NLContainer;
class MSEmitControl;
class MSJunctionLogic;
class MSDetectorControl;
class OptionsCont;
class SAX2XMLReader;
class NLSAXHandler;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * NLNetBuilder
 * The class is the main interface to load simulations.
 * It is a black-box where only the options must be supplied on the
 * constructor call
 * It is assumed that the simulation is stored in a XML-file.
 */
class NLNetBuilder {
private:
    /// the options to get the names from
    const OptionsCont &m_pOptions;
    /// the different data types
public:
    /// standard constructor
    NLNetBuilder(const OptionsCont &oc);
    /// standard destructor
    ~NLNetBuilder();
    /// the net loading method
    MSNet *build();
private:
    /// counts the structures and preallocates them
    bool count(NLContainer &container, SAX2XMLReader &parser);
    /// counts the structures and preallocates them
    void load(NLContainer &container, SAX2XMLReader &parser);
    /// loads a described subpart form the given list of files
    void load(LoadFilter what, std::string files, NLContainer &cont, SAX2XMLReader &parser);
    /// prepares the parser for processing using the current handler
    void prepareParser(SAX2XMLReader &parser, NLSAXHandler *handler, int step);
    /// parses the files using the given initialised parser
    bool parse(const std::string &files, SAX2XMLReader &parser);
    /// returns the data name that accords to the given enum
    std::string getDataName(LoadFilter forWhat);
    /// returns the list of handlers needed to parse the given data type
    std::vector<NLSAXHandler*> getHandler(LoadFilter forWhat, NLContainer &container);
    /// reports the process (done or failure)
    void subreport(const std::string &ok, const std::string &wrong) ;
    /// prints the final report
    void report(const NLContainer &container);
    /// returns false when only ':' are supplied (no filename)
    bool checkFilenames(const std::string &files);
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
