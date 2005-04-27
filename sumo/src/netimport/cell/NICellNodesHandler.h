#ifndef NICellNodesHandler_h
#define NICellNodesHandler_h
/***************************************************************************
                          NICellNodesHandler.h
             A LineHandler-derivate to load nodes form a cell-nodes-file
                             -------------------
    project              : SUMO
    begin                : Fri, 19 Jul 2002
    copyright            : (C) 2002 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.4  2005/04/27 12:24:35  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.3  2004/07/02 09:35:23  dkrajzew
// adaptation of current FastLane import
//
// Revision 1.2  2003/06/18 11:14:48  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.1  2003/02/07 11:10:56  dkrajzew
// names changed
//
// Revision 1.1  2002/10/16 15:40:04  dkrajzew
// initial commit for cell importing classes
//
// Revision 1.1  2002/07/25 08:41:45  dkrajzew
// Visum7.5 and Cell import added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <utils/importio/LineHandler.h>
#include <utils/common/FileErrorReporter.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * NICellNodesHandler
 * Being a LineHandler, this class retrieves each line from a LineReader
 * and parses these information assuming they contain node definitions
 * in Cell-format
 */
class NICellNodesHandler : public LineHandler,
                           public FileErrorReporter {
private:
public:
    /// constructor
    NICellNodesHandler(NBNodeCont &nc, const std::string &file);

    /// destructor
    ~NICellNodesHandler();

    /** implementation of the LineHandler-interface called by a LineReader
        interprets the retrieved information and stores it into the global
        NBNodeCont */
    bool report(const std::string &result);

protected:
    NBNodeCont &myNodeCont;

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
