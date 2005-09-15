#ifndef NLBuilder_h
#define NLBuilder_h
/***************************************************************************
                          NLBuilder.h
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
// Revision 1.1  2005/09/15 12:04:36  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/09/09 12:58:18  dksumo
// new loading scheme implemented
//
// Revision 1.4  2005/08/01 13:00:34  dksumo
// further triggers added; loading and saving of mesostate added
//
// Revision 1.3  2005/04/26 07:43:21  dksumo
// SUMOTime inserted; level3 warnings patched; loading lane geometry into
//  microsim
//
// Revision 1.2  2004/11/22 12:33:28  dksumo
// tried to simplify the usage of detectors - merging mean lane data
//
// Revision 1.1  2004/10/22 12:50:20  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.9  2004/07/02 09:37:31  dkrajzew
// work on class derivation (for online-routing mainly)
//
// Revision 1.8  2004/01/12 14:46:21  dkrajzew
// handling of e2-detectors within the gui added
//
// Revision 1.7  2003/12/11 06:19:04  dkrajzew
// network loading and initialisation improved
//
// Revision 1.6  2003/12/04 13:18:23  dkrajzew
// handling of internal links added
//
// Revision 1.5  2003/10/22 15:47:44  dkrajzew
// we have to distinct between two teleporter versions now
//
// Revision 1.4  2003/08/18 12:41:40  dkrajzew
// xerces 2.2 and later compatibility patched
//
// Revision 1.3  2003/02/07 11:18:56  dkrajzew
// updated
//
// Revision 1.2  2002/10/17 10:32:41  dkrajzew
// sources and detectors joined with triggers to additional-files;
//  usage of standard SUMOSAXHandler instead of NLSAXHandler;
//  loading of triggers implemented
//
// Revision 1.1  2002/10/16 15:36:48  dkrajzew
// moved from ROOT/sumo/netload to ROOT/src/netload;
//  new format definition parseable in one step
//
// Revision 1.9  2002/07/31 17:34:51  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.10  2002/07/11 07:31:54  dkrajzew
// Option_FileName invented to allow relative path names within the
//  configuration files
//
// Revision 1.9  2002/07/02 08:19:08  dkrajzew
// Test for non-empty file option where a list of files is expected
//  moved to FileHelpers
//
// Revision 1.8  2002/06/11 14:39:25  dkrajzew
// windows eol removed
//
// Revision 1.7  2002/06/11 13:44:34  dkrajzew
// Windows eol removed
//
// Revision 1.6  2002/06/07 14:39:59  dkrajzew
// errors occured while building larger nets and adaption of new
//  netconverting methods debugged
//
// Revision 1.5  2002/05/14 04:54:25  dkrajzew
// Unexisting files are now catched independent to the Xerces-error
//  mechanism; error report generation moved to XMLConvert
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
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <map>
#include <vector>
#include <parsers/SAXParser.hpp>
#include <framework/XMLFormatter.hpp>
#include <sax2/DefaultHandler.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <microsim/MSNet.h>
#include "NLLoadFilter.h"
#include "NLGeomShapeBuilder.h"


/* =========================================================================
 * xerces 2.2 compatibility
 * ======================================================================= */
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSNet;
class NLContainer;
class MSEmitControl;
class MSJunctionLogic;
class MSDetectorControl;
class OptionsCont;
class NLNetHandler;
class NLEdgeControlBuilder;
class NLJunctionControlBuilder;
class NLDetectorBuilder;
class NLTriggerBuilder;
class MSRouteLoader;
class GNEImageProcWindow;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NLBuilder
 * The class is the main interface to load simulations.
 * It is a black-box where only the options and factories must be supplied
 * on the constructor call
 */
class NLBuilder {
public:
    /// constructor
    NLBuilder(const OptionsCont &oc, MSNet &net,
        NLEdgeControlBuilder &eb, NLJunctionControlBuilder &jb,
        NLDetectorBuilder &db, NLTriggerBuilder &tb,
        NLGeomShapeBuilder &sb);

    /// Destructor
    virtual ~NLBuilder();

    /// the net loading method
    virtual bool build();

    /// Closes the net building process
    bool buildNet(NLNetHandler &handler, GNEImageProcWindow &t);

protected:
    /// loads a described subpart form the given list of files
    bool load(const std::string &mmlWhat, NLLoadFilter what,
        const std::string &files,
        NLNetHandler &handler, SAX2XMLReader &parser);

    /// parses the files using the given initialised parser
    bool parse(const std::string &mmlWhat, const std::string &files,
        NLNetHandler &handler, SAX2XMLReader &parser);

    /// reports the process (done or failure)
    void subreport(const std::string &ok, const std::string &wrong) ;

    /// Closes the net building process
    bool buildNet(NLNetHandler &handler);

    /// Builds the route loader control
    MSRouteLoaderControl *buildRouteLoaderControl(const OptionsCont &oc);


protected:
    /// the options to get the names from
    const OptionsCont &m_pOptions;

    /// The edge control builder to use
    NLEdgeControlBuilder &myEdgeBuilder;

    /// The junction control builder to use
    NLJunctionControlBuilder &myJunctionBuilder;

    /// The detector control builder to use
    NLDetectorBuilder &myDetectorBuilder;

    /// The trigger control builder to use
    NLTriggerBuilder &myTriggerBuilder;

    /// The geometry shapes builder to use
    NLGeomShapeBuilder &myShapeBuilder;

    /// The net to fill
    MSNet &myNet;

private:
    /** invalidated copy operator */
    NLBuilder(const NLBuilder &s);

    /** invalidated assignment operator */
    NLBuilder &operator=(const NLBuilder &s);

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
