/****************************************************************************/
/// @file    NIArcView_Loader.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id:NIArcView_Loader.h 4701 2007-11-09 14:29:29Z dkrajzew $
///
// Importer for networks stored in ArcView-shape format
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
#ifndef NIArcView_Loader_h
#define NIArcView_Loader_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/common/FileErrorReporter.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;
class OGRFeature;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIArcView_Loader
 * @brief Importer for networks stored in ArcView-shape format
 *
 * The current importer works only if SUMO was compiled with GDAL-support.
 *  If not, an error message is generated.
 *
 * @todo reinsert import via shapelib
 */
class NIArcView_Loader :
            public FileErrorReporter
{
public:
    /// Contructor
    NIArcView_Loader(OptionsCont &oc,
                     NBNodeCont &nc, NBEdgeCont &ec, NBTypeCont &tc,
                     const std::string &dbf_name, const std::string &shp_name,
                     bool speedInKMH);

    /// Destructor
    ~NIArcView_Loader();

    /// loads the navtech-data
    bool load(OptionsCont &options);

private:
#ifdef HAVE_GDAL
    /// parses the maximum speed allowed on the edge currently processed
    SUMOReal getSpeed(OGRFeature &f, const std::string &edgeid);

    /// parses the number of lanes of the edge currently processed
    unsigned int getLaneNo(OGRFeature &f,
                     const std::string &edgeid, SUMOReal speed);

    /// parses the priority of the edge currently processed
    int getPriority(OGRFeature &f, const std::string &edgeid);
#endif

private:
    OptionsCont &myOptions;

    std::string mySHPName;
    int myNameAddition;
    NBNodeCont &myNodeCont;
    NBEdgeCont &myEdgeCont;
    NBTypeCont &myTypeCont;
    bool mySpeedInKMH;
    int myRunningNodeID;

private:
    /// @brief Invalidated copy constructor.
    NIArcView_Loader(const NIArcView_Loader&);

    /// @brief Invalidated assignment operator.
    NIArcView_Loader& operator=(const NIArcView_Loader&);

};


#endif

/****************************************************************************/

