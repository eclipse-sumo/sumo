#ifndef NITigerLoader_h
#define NITigerLoader_h
/***************************************************************************
                          NITigerLoader.h
              A loader tiger-files
                             -------------------
    project              : SUMO
    begin                : Tue, 29 Jun 2004
    copyright            : (C) 2004 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.4  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/04/27 12:24:36  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.2  2004/11/23 10:23:53  dkrajzew
// debugging
//
// Revision 1.1  2004/07/02 09:34:38  dkrajzew
// elmar and tiger import added
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
#include <netbuild/NBCapacity2Lanes.h>
#include <utils/importio/LineHandler.h>
#include <utils/importio/LineReader.h>
#include <utils/importio/NamedColumnsParser.h>
#include <utils/common/FileErrorReporter.h>
#include <utils/geom/Position2DVector.h>


/* =========================================================================
 * class declaration
 * ======================================================================= */
class OptionsCont;
class NBNode;


/* =========================================================================
 * class declaration
 * ======================================================================= */
/**
 * NITigerLoader
 */
class NITigerLoader :
        public FileErrorReporter
{
public:
    /// constructor
    NITigerLoader(NBEdgeCont &ec, NBNodeCont &nc,
        const std::string &file);

    /// destructor
    ~NITigerLoader();

    /// starts the parsing
    void load(OptionsCont &options);

protected:
    Position2DVector convertShape(const std::vector<std::string> &sv);

    NBNode *getNode(const Position2D &p);

    std::string getType(const std::vector<std::string> &sv) const;

    double getSpeed(const std::string &type) const;

    int getLaneNo(const std::string &type) const;

protected:
    bool myWasSet;
    double myInitX, myInitY;
    NBEdgeCont &myEdgeCont;
    NBNodeCont &myNodeCont;

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
