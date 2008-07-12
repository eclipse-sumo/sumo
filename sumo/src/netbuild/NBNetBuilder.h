/****************************************************************************/
/// @file    NBNetBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//	The instance responsible for building networks
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
#ifndef NBNetBuilder_h
#define NBNetBuilder_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include "NBEdgeCont.h"
#include "NBTypeCont.h"
#include "NBNodeCont.h"
#include "NBTrafficLightLogicCont.h"
#include "NBJunctionLogicCont.h"
#include "NBDistrictCont.h"

class OptionsCont;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBNetBuilder
 */
class NBNetBuilder
{
public:
    /// Constructor
    NBNetBuilder();

    /// Destructor
    ~NBNetBuilder();

    void buildLoaded();

    static void insertNetBuildOptions(OptionsCont &oc);

    void preCheckOptions(OptionsCont &oc);

    /**
     * @brief computes the structures
     * the order of the computation steps is not variable!!!
     */
    void compute(OptionsCont &oc);

    /** saves the net */
    bool save(OutputDevice &device, OptionsCont &oc);

    NBEdgeCont &getEdgeCont();
    NBNodeCont &getNodeCont();
    NBTypeCont &getTypeCont();
    NBTrafficLightLogicCont &getTLLogicCont();
    NBJunctionLogicCont &getJunctionLogicCont();
    NBDistrictCont &getDistrictCont();


protected:

    void inform(int &step, const std::string &about);





    void initJoinedEdgesInformation();

    bool saveMap(const std::string &path);

protected:
    NBEdgeCont myEdgeCont;
    NBNodeCont myNodeCont;
    NBTypeCont myTypeCont;
    NBTrafficLightLogicCont myTLLCont;
    NBJunctionLogicCont myJunctionLogicCont;
    NBDistrictCont myDistrictCont;

};


#endif

/****************************************************************************/

