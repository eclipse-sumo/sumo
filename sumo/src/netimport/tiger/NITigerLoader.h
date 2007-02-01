/****************************************************************************/
/// @file    NITigerLoader.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 29 Jun 2004
/// @version $Id: $
///
// A loader tiger-files
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
#ifndef NITigerLoader_h
#define NITigerLoader_h
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
#include <map>
#include <netbuild/NBCapacity2Lanes.h>
#include <utils/importio/LineHandler.h>
#include <utils/importio/LineReader.h>
#include <utils/importio/NamedColumnsParser.h>
#include <utils/common/FileErrorReporter.h>
#include <utils/geom/Position2DVector.h>


// ===========================================================================
// class declaration
// ===========================================================================
class OptionsCont;
class NBNode;


// ===========================================================================
// class declaration
// ===========================================================================
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

    SUMOReal getSpeed(const std::string &type) const;

    int getLaneNo(const std::string &type) const;

protected:
    bool myWasSet;
    SUMOReal myInitX, myInitY;
    NBEdgeCont &myEdgeCont;
    NBNodeCont &myNodeCont;

};


#endif

/****************************************************************************/

