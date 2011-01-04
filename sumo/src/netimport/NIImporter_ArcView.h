/****************************************************************************/
/// @file    NIImporter_ArcView.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Importer for networks stored in ArcView-shape format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
    /** @brief Loads content of the optionally given ArcView Shape files
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
    /** @brief Constructor
     * @param[in] oc Options container to read options from
     * @param[in] nc The node container to store nodes into
     * @param[in] ec The edge container to store edges into
     * @param[in] tc The type container to get edge types from
     * @param[in] dbf_name The name of the according database file
     * @param[in] shp_name The name of the according shape file
     * @param[in] speedInKMH Whether the speed shall be assumed to be given in km/h
     */
    NIImporter_ArcView(const OptionsCont &oc,
                       NBNodeCont &nc, NBEdgeCont &ec, NBTypeCont &tc,
                       const std::string &dbf_name, const std::string &shp_name,
                       bool speedInKMH);

    /// @brief Destructor
    ~NIImporter_ArcView();


    /** @brief Loads the shape files
     */
    void load();


private:
#ifdef HAVE_GDAL
    /** @brief Parses the maximum speed allowed on the edge currently processed
     * @param[in] f The entry to read the speed from
     * @param[in] edgeid The id of the edge for error output
     */
    SUMOReal getSpeed(OGRFeature &f, const std::string &edgeid);


    /** @brief Parses the number of lanes of the edge currently processed
     * @param[in] f The entry to read the lane number from
     * @param[in] edgeid The id of the edge for error output
     * @param[in] speed The edge's speed used to help determinig the edge's lane number
     */
    unsigned int getLaneNo(OGRFeature &f,
                           const std::string &edgeid, SUMOReal speed);

    /** @brief Parses the priority of the edge currently processed
     * @param[in] f The entry to read the priority from
     * @param[in] edgeid The id of the edge for error output
     */
    int getPriority(OGRFeature &f, const std::string &edgeid);


    /** @brief Checks whether the lane spread shall be changed
     *
     * If for the given edge an edge into the vice direction is already
     *  stored, both edges' lane spread functions are set to LANESPREAD_RIGHT.
     *
     * @param[in] e The edge to check
     */
    void checkSpread(NBEdge *e);
#endif

private:
    /// @brief The options to use
    const OptionsCont &myOptions;

    /// @brief The name of the shape file
    std::string mySHPName;

    /// @brief A running number to assure unique edge ids
    int myNameAddition;

    /// @brief The container to add nodes to
    NBNodeCont &myNodeCont;

    /// @brief The container to add edges to
    NBEdgeCont &myEdgeCont;

    /// @brief The container to get the types from
    NBTypeCont &myTypeCont;

    /// @brief Whether the speed is given in km/h
    bool mySpeedInKMH;

    /// @brief A running number to assure unique node ids
    int myRunningNodeID;


private:
    /// @brief Invalidated copy constructor.
    NIImporter_ArcView(const NIImporter_ArcView&);

    /// @brief Invalidated assignment operator.
    NIImporter_ArcView& operator=(const NIImporter_ArcView&);

};


#endif

/****************************************************************************/

