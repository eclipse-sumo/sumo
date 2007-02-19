/****************************************************************************/
/// @file    NICellEdgesHandler.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 19 Jul 2002
/// @version $Id$
///
// A LineHandler-derivate to load edges form a cell-edges-file
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
#ifndef NICellEdgesHandler_h
#define NICellEdgesHandler_h
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

#include <string>
#include <utils/importio/LineHandler.h>
#include <netbuild/NBCapacity2Lanes.h>
#include <utils/common/FileErrorReporter.h>

// ===========================================================================
// class declarations
// ===========================================================================
/**
 * NICellEdgesHandler
 * Being a LineHandler, this class retrieves each line from a LineReader
 * and parses these information assuming they contain edge definitions
 * in Cell-format
 */
// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class NICellEdgesHandler : public LineHandler,
            public FileErrorReporter
{
public:
    /// constructor
    NICellEdgesHandler(NBNodeCont &nc, NBEdgeCont &ec, NBTypeCont &tc,
                       const std::string &file, NBCapacity2Lanes capacity2Lanes);

    /// destructor
    ~NICellEdgesHandler();

    /** implementation of the LineHandler-interface called by a LineReader
        interprets the retrieved information and stores it into the global
        NBEdgeCont */
    bool report(const std::string &result);

protected:
    NBNodeCont &myNodeCont;
    NBEdgeCont &myEdgeCont;
    NBTypeCont &myTypeCont;

private:
    /// the converter for converting capacities to laneno
    NBCapacity2Lanes _capacity2Lanes;

};


#endif

/****************************************************************************/

