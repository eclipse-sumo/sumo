/****************************************************************************/
/// @file    NIElmarEdgesHandler.h
/// @author  Daniel Krajzewicz
/// @date    Sun, 16 May 2004
/// @version $Id:NIElmarEdgesHandler.h 4701 2007-11-09 14:29:29Z dkrajzew $
///
// Importer of edges stored in split elmar format
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
#ifndef NIElmarEdgesHandler_h
#define NIElmarEdgesHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/importio/LineHandler.h>
#include <utils/common/FileErrorReporter.h>


// ===========================================================================
// class declarations
// ===========================================================================
/**
 * @class NIElmarEdgesHandler
 * @brief Importer of edges stored in split elmar format
 *
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
class NIElmarEdgesHandler : public LineHandler,
            public FileErrorReporter
{
public:
    /// constructor
    NIElmarEdgesHandler(NBNodeCont &nc, NBEdgeCont &ec,
                        const std::string &file) throw();

    /// destructor
    ~NIElmarEdgesHandler() throw();

    /** implementation of the LineHandler-interface called by a LineReader
        interprets the retrieved information and stores it into the global
        NBEdgeCont */
    bool report(const std::string &result) throw(ProcessError);

protected:
    NBNodeCont &myNodeCont;
    NBEdgeCont &myEdgeCont;

private:
    /// @brief Invalidated copy constructor.
    NIElmarEdgesHandler(const NIElmarEdgesHandler&);

    /// @brief Invalidated assignment operator.
    NIElmarEdgesHandler& operator=(const NIElmarEdgesHandler&);

};


#endif

/****************************************************************************/

