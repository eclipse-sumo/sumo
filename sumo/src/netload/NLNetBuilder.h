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
// Revision 1.2  2002/10/17 10:32:41  dkrajzew
// sources and detectors joined with triggers to additional-files; usage of standard SUMOSAXHandler instead of NLSAXHandler; loading of triggers implemented
//
// Revision 1.1  2002/10/16 15:36:48  dkrajzew
// moved from ROOT/sumo/netload to ROOT/src/netload; new format definition parseable in one step
//
// Revision 1.9  2002/07/31 17:34:51  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.10  2002/07/11 07:31:54  dkrajzew
// Option_FileName invented to allow relative path names within the configuration files
//
// Revision 1.9  2002/07/02 08:19:08  dkrajzew
// Test for non-empty file option where a list of files is expected moved to FileHelpers
//
// Revision 1.8  2002/06/11 14:39:25  dkrajzew
// windows eol removed
//
// Revision 1.7  2002/06/11 13:44:34  dkrajzew
// Windows eol removed
//
// Revision 1.6  2002/06/07 14:39:59  dkrajzew
// errors occured while building larger nets and adaption of new netconverting methods debugged
//
// Revision 1.5  2002/05/14 04:54:25  dkrajzew
// Unexisting files are now catched independent to the Xerces-error mechanism; error report generation moved to XMLConvert
//
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
#include <microsim/MSNet.h>
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
class NLHandlerBuilder;

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
protected:
    /// the options to get the names from
    const OptionsCont &m_pOptions;
    /// the different data types
public:
    /// standard constructor
    NLNetBuilder(const OptionsCont &oc);
    /// standard destructor
    virtual ~NLNetBuilder();
    /// the net loading method
    MSNet *buildMSNet();
protected:
    /// counts the structures and preallocates them
    bool load(NLHandlerBuilder *handler, SAX2XMLReader &parser);
    /// loads a described subpart form the given list of files
    bool load(LoadFilter what, const std::string &files,
        NLHandlerBuilder *handler, SAX2XMLReader &parser);
    /// parses the files using the given initialised parser
    bool parse(const std::string &files,
        NLHandlerBuilder *handler, SAX2XMLReader &parser);
    /// returns the data name that accords to the given enum
    std::string getDataName(LoadFilter forWhat);
    /// reports the process (done or failure)
    void subreport(const std::string &ok, const std::string &wrong) ;
    /// prints the final report
    void report(const NLContainer &container);
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
