/****************************************************************************/
/// @file    NITigerLoader.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 29 Jun 2004
/// @version $Id:NITigerLoader.h 4701 2007-11-09 14:29:29Z dkrajzew $
///
// Importer for TIGER networks
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
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
 * @class NITigerLoader
 * @brief Importer for TIGER networks
 */
class NITigerLoader :
            public FileErrorReporter
{
public:
    /// constructor
    NITigerLoader(NBEdgeCont &ec, NBNodeCont &nc,
                  const std::string &file,
                  bool tryIgnoreNodePositions);

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
    bool myTryIgnoreNodePositions;

private:
    /// @brief Invalidated copy constructor.
    NITigerLoader(const NITigerLoader&);

    /// @brief Invalidated assignment operator.
    NITigerLoader& operator=(const NITigerLoader&);

};


#endif

/****************************************************************************/

