#ifndef NIArcView_Loader_h
#define NIArcView_Loader_h
//---------------------------------------------------------------------------//
//                        NIArcView_Loader.h -
//  The loader of arcview-files
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.12  2005/11/14 09:51:17  dkrajzew
// allowed further information to be stored in arcview-files
//
// Revision 1.11  2005/10/07 11:38:54  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.10  2005/09/23 06:01:43  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.9  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.8  2005/04/27 12:24:24  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.7  2004/08/02 12:43:07  dkrajzew
// got rid of the shapelib-interface; conversion of geocoordinates added
//
// Revision 1.6  2004/01/12 15:53:00  dkrajzew
// work on code style
//
// Revision 1.5  2003/12/04 16:53:53  dkrajzew
// native ArcView-importer by ericnicolay added
//
// Revision 1.4  2003/07/22 15:11:24  dkrajzew
// removed warnings
//
// Revision 1.3  2003/06/18 11:34:25  dkrajzew
// the arcview-import should be more stable nw when dealing with false tables
//
// Revision 1.2  2003/06/05 11:44:14  dkrajzew
// class templates applied; documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/common/FileErrorReporter.h>
#include <utils/importio/LineHandler.h>
#include <utils/importio/LineReader.h>
#include <utils/importio/NamedColumnsParser.h>
#include "shapereader.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class OptionsCont;


/* =========================================================================
 * class definitions
 * ======================================================================= */
class NIArcView_Loader :
    public FileErrorReporter/*,
    public LineHandler*/
{
public:
    /// Contructor
    NIArcView_Loader(NBNodeCont &nc, NBEdgeCont &ec,
        const std::string &dbf_name, const std::string &shp_name,
        bool speedInKMH);

    /// Destructor
    ~NIArcView_Loader();

    /// loads the navtech-data
    void load(OptionsCont &options);

private:
    bool parseBin();
    /// parses a non-first line (a data line)
    bool parseLine(const std::string &line);

    /// parses the maximum speed allowed on the edge currently processed
    SUMOReal getSpeed(const std::string &edgeid);

    /// parses the number of lanes of the edge currently processed
    size_t getLaneNo(const std::string &edgeid, SUMOReal speed);

    /// parses the length of the edge currently processed
    SUMOReal getLength(/*const std::string &edgeid,*/
        const Position2D &from_pos, const Position2D &to_pos);

    /// parses the priority of the edge currently processed
    int getPriority(const std::string &edgeid);

    std::string getStringSecure(const std::string &which);


private:
    /// parser of the dbf-file columns
    NamedColumnsParser myColumnsParser;

    std::string myDBFName;
    std::string mySHPName;
    shapereader myBinShapeReader;
    int myNameAddition;
    NBNodeCont &myNodeCont;
    NBEdgeCont &myEdgeCont;
    bool mySpeedInKMH;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

