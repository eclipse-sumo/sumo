/****************************************************************************/
/// @file    NIImporter_ArcView.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Importer for networks stored in ArcView-shape format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIImporter_ArcView_h
#define NIImporter_ArcView_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;
class OGRFeature;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIImporter_ArcView
 * @brief Importer for networks stored in ArcView-shape format
 *
 * The current importer works only if SUMO was compiled with GDAL-support.
 *  If not, an error message is generated.
 *
 * @todo reinsert import via shapelib
 */
class NIImporter_ArcView {
public:
    /** @brief Loads network definition from the assigned option and stores it in the given network builder
     *
     * If the option "arcview" is set, the file stored therein is read and
     *  the network definition stored therein is stored within the given network
     *  builder.
     *
     * If the option "arcview" is not set, this method simply returns.
     *
     * @param[in] oc The options to use
     * @param[in] nb The network builder to fill
     */
    static void loadNetwork(const OptionsCont &oc, NBNetBuilder &nb);


protected:
    /// Contructor
    NIImporter_ArcView(const OptionsCont &oc,
                     NBNodeCont &nc, NBEdgeCont &ec, NBTypeCont &tc,
                     const std::string &dbf_name, const std::string &shp_name,
                     bool speedInKMH);

    /// Destructor
    ~NIImporter_ArcView();

    /// loads the navtech-data
    bool load();

private:
#ifdef HAVE_GDAL
    /// parses the maximum speed allowed on the edge currently processed
    SUMOReal getSpeed(OGRFeature &f, const std::string &edgeid);

    /// parses the number of lanes of the edge currently processed
    unsigned int getLaneNo(OGRFeature &f,
                           const std::string &edgeid, SUMOReal speed);

    /// parses the priority of the edge currently processed
    int getPriority(OGRFeature &f, const std::string &edgeid);

    void checkSpread(NBEdge *e);
#endif

private:
    const OptionsCont &myOptions;

    std::string mySHPName;
    int myNameAddition;
    NBNodeCont &myNodeCont;
    NBEdgeCont &myEdgeCont;
    NBTypeCont &myTypeCont;
    bool mySpeedInKMH;
    int myRunningNodeID;

private:
    /// @brief Invalidated copy constructor.
    NIImporter_ArcView(const NIImporter_ArcView&);

    /// @brief Invalidated assignment operator.
    NIImporter_ArcView& operator=(const NIImporter_ArcView&);

};


#endif

/****************************************************************************/

