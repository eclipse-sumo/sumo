//---------------------------------------------------------------------------//
//                        MSMeanData_Net_Utils.cpp -
//  Utilities for building the lanes' mean data output
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 10.05.2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.8  2005/09/22 13:45:52  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.7  2005/09/15 11:08:51  dkrajzew
// LARGE CODE RECHECK
//
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

#include "MSMeanData_Net.h"
#include "MSMeanData_Net_Utils.h"
#include <cassert>
#include <algorithm>
#include <set>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <microsim/MSEdgeControl.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/iodevices/SharedOutputDevices.h>
#include <microsim/output/MSDetector2File.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
MSMeanData_Net_Cont
MSMeanData_Net_Utils::buildList(MSDetector2File &det2file,
                                MSEdgeControl &ec,
                                std::vector<int> dumpMeanDataIntervalls,
                                std::string baseNameDumpFiles,
                                std::vector<int> laneDumpMeanDataIntervalls,
                                std::string baseNameLaneDumpFiles,
                                const std::vector<int> &dumpBegins,
                                const std::vector<int> &dumpEnds)
{
    MSMeanData_Net_Cont ret;
    if ( dumpMeanDataIntervalls.size() > 0 ) {
        MSMeanData_Net_Cont tmp =
            buildList(det2file, ec,
                dumpMeanDataIntervalls, baseNameDumpFiles,
                dumpBegins, dumpEnds, false);
        copy(tmp.begin(), tmp.end(), back_inserter(ret));
    }
    if ( laneDumpMeanDataIntervalls.size() > 0 ) {
        MSMeanData_Net_Cont tmp =
            buildList(det2file, ec,
                laneDumpMeanDataIntervalls, baseNameLaneDumpFiles,
                dumpBegins, dumpEnds, true);
        copy(tmp.begin(), tmp.end(), back_inserter(ret));
    }
    return ret;
}


MSMeanData_Net_Cont
MSMeanData_Net_Utils::buildList(MSDetector2File &det2file,
                                MSEdgeControl &ec,
                                std::vector<int> dumpMeanDataIntervalls,
                                std::string baseNameDumpFiles,
                                const std::vector<int> &dumpBegins,
                                const std::vector<int> &dumpEnds,
                                bool useLanes)
{
    MSMeanData_Net_Cont ret;
    if ( dumpMeanDataIntervalls.size() > 0 ) {
        dumpMeanDataIntervalls = buildUniqueList(dumpMeanDataIntervalls);
        sort( dumpMeanDataIntervalls.begin(),
              dumpMeanDataIntervalls.end() );

        // Prepare MeanData container, e.g. assign intervals and open files.
        for ( std::vector<int>::iterator it =
                  dumpMeanDataIntervalls.begin();
              it != dumpMeanDataIntervalls.end(); ++it ) {

            string fileName   = baseNameDumpFiles + "_" + toString( *it ) +
                string(".xml");
            OutputDevice* dev =
                SharedOutputDevices::getInstance()->getOutputDevice( fileName );
            if( dev==0 ) {
                MsgHandler::getErrorInstance()->inform(
                    string("The following file containing aggregated values could not been build:\n")
                    + fileName);
                throw ProcessError();
            }
            // Write xml-comment
            dev->getOStream() << "<!--\n"
                "- noVehContrib is the number of vehicles have been on the lane for\n"
                "  at least one timestep during the current intervall.\n"
                "  They contribute to speed, speedsquare and density.\n"
                "  They may not have passed the entire lane.\n"
                "- noVehEntireLane is the number of vehicles that have passed the\n"
                "  entire lane and left the lane during the current intervall. They\n"
                "  may have started their journey on this lane in a previous intervall.\n"
                "  Only those vehicles contribute to traveltime. \n"
                "- noVehEntered is the number of vehicles that entered this lane\n"
                "  during the current intervall either by move, emit or lanechange.\n"
                "  Note that noVehEntered might be high if vehicles are emitted on\n"
                "  this lane.\n"
                "- noVehLeft is the number of vehicles that left this lane during\n"
                "  the current intervall by move.\n"
                "- traveltime [s]\n"
                "  If noVehContrib==0 then traveltime is set to laneLength / laneMaxSpeed. \n"
                "  If noVehContrib!=0 && noVehEntireLane==0 then traveltime is set to\n"
                "  laneLength / speed.\n"
                "  Else traveltime is calculated from the data of the vehicles that\n"
                "  passed the entire lane.\n"
                "- speed [m/s]\n"
                "  If noVehContrib==0 then speed is set to laneMaxSpeed.\n"
                "- speedsquare [(m/s)^2]\n"
                "  If noVehContrib==0 then speedsquare is set to -1.\n"
                "- density [veh/km]\n"
                "  If noVehContrib==0 then density is set to 0.\n"
                "-->\n" ;

            MSMeanData_Net *det =
                new MSMeanData_Net( *it, ret.size(), ec,
                    dumpBegins, dumpEnds, useLanes, true);
            ret.push_back( det );
            det2file.addDetectorAndInterval(det, dev, *it);
        }
    }
    return ret;
}


std::vector<int>
MSMeanData_Net_Utils::buildUniqueList(
            std::vector<int> dumpMeanDataIntervalls)
{
    vector<int> ret;
    set<int> u;
    copy(dumpMeanDataIntervalls.begin(), dumpMeanDataIntervalls.end(),
        inserter(u, u.begin()));
    if(dumpMeanDataIntervalls.size()!=u.size()) {
        WRITE_WARNING("Removed duplicate dump-intervalls");
    }
    copy(u.begin(), u.end(), back_inserter(ret));
    return ret;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
