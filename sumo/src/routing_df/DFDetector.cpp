/***************************************************************************
                          DFDetector.cpp
                          Class representing a detector within the DFROUTER
                             -------------------
    project              : SUMO
    begin                : Thu, 16.03.2006
    copyright            : (C) 2006 by DLR/IVF http://ivf.dlr.de/
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.17  2006/04/11 11:08:13  dkrajzew
// debugging
//
// Revision 1.16  2006/04/07 05:29:39  dkrajzew
// removed some warnings
//
// Revision 1.15  2006/04/05 05:35:26  dkrajzew
// further work on the dfrouter
//
// Revision 1.14  2006/03/28 06:17:18  dkrajzew
// extending the dfrouter by distance/length factors
//
// Revision 1.13  2006/03/27 07:32:15  dkrajzew
// some further work...
//
// Revision 1.12  2006/03/17 09:04:25  dkrajzew
// class-documentation added/patched
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "DFDetector.h"
#include <fstream>
#include <iostream>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <router/ROEdge.h>
#include "RODFEdge.h"
#include "DFRORouteDesc.h"
#include "DFRORouteCont.h"
#include "DFDetectorFlow.h"
#include <utils/helpers/RandomDistributor.h>
#include <utils/common/StdDefs.h>
#include <utils/geom/GeomHelper.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
DFDetector::DFDetector(const std::string &Id, const std::string &laneId,
                       SUMOReal pos, const dfdetector_type type)
    : myID(Id), myLaneID(laneId), myPosition(pos), myType(type), myRoutes(0)
{
}


DFDetector::DFDetector(const std::string &Id, const DFDetector &f)
    : myID(Id), myLaneID(f.myLaneID), myPosition(f.myPosition),
    myType(f.myType), myRoutes(0)
{
    if(f.myRoutes!=0) {
        myRoutes = new DFRORouteCont(*(f.myRoutes));
    }
}


DFDetector::~DFDetector()
{
	delete myRoutes;
}


void
DFDetector::setType(dfdetector_type type)
{
    myType = type;
}


SUMOReal
DFDetector::computeDistanceFactor(const DFRORouteDesc &rd) const
{
    SUMOReal distance = GeomHelper::distance(
        static_cast<RODFEdge*>(rd.edges2Pass[0])->getFromPosition(),
        static_cast<RODFEdge*>(rd.edges2Pass[rd.edges2Pass.size()-1])->getToPosition());
    SUMOReal length = 0;
    for(std::vector<ROEdge*>::const_iterator i=rd.edges2Pass.begin(); i!=rd.edges2Pass.end(); ++i) {
        length += (*i)->getLength();
    }
    return (distance/length);
}


void
DFDetector::buildDestinationDistribution(const DFDetectorCon &detectors,
                                         const DFDetectorFlows &flows,
										 SUMOTime startTime,
                                         SUMOTime endTime,
                                         SUMOTime stepOffset,
                                         std::map<size_t, RandomDistributor<size_t>* > &into) const
{
    std::vector<DFRORouteDesc*>::const_iterator ri;
    const std::vector<DFRORouteDesc*> &descs = myRoutes->get();
    /*
    std::map<const ROEdge *, int> lastDetNo;
    for(ri=descs.begin(); ri!=descs.end(); ++ri) {
        DFRORouteDesc *rd = *ri;
        rd->lastDetectorEdge
    }
*/
    const std::vector<FlowDef> &mflows = flows.getFlowDefs(myID);
    const std::map<ROEdge*, std::vector<ROEdge*> > &dets2Follow = myRoutes->getDets2Follow();
    // iterate through time (in output interval steps)
    int time;
    for(time=startTime; time<endTime; time+=stepOffset) {
        into[time] = new RandomDistributor<size_t>();
        // get the number of vehicles to emit
        const FlowDef &srcFD = mflows[(int) (time/stepOffset) - startTime];// !!! check stepOffset
        SUMOReal toEmit = (SUMOReal) (srcFD.qLKW + srcFD.qPKW);
        // iterate through the routes
        size_t index = 0;
        for(ri=descs.begin(); ri!=descs.end()&&toEmit>=0; ++ri, index++) {
            DFRORouteDesc *rd = *ri;

            SUMOReal distanceFactor = computeDistanceFactor(*rd);

            // the current probability is 1
            SUMOReal ovProb = 1.;
            bool hadMissing = false;
            // go thrught this route's edges
            std::vector<ROEdge*>::reverse_iterator i = rd->edges2Pass.rbegin() + 1;
            while(i!=rd->edges2Pass.rend()) {
                if((*i)->getNoFollowing()>1||hadMissing) {
                    if(dets2Follow.find(*i)!=dets2Follow.end()) {

                        const std::vector<ROEdge*> &possNext = dets2Follow.find(*i)->second;
                        ROEdge *my = 0;
                        SUMOReal sumFlows = 0;
                        SUMOReal myFlow = 0;
                        for(std::vector<ROEdge*>::const_iterator k=possNext.begin(); k!=possNext.end(); ++k) {
                            SUMOReal cflow = 0;
                            if(true) { // !!!
                                cflow = (SUMOReal) detectors.getAggFlowFor(*k, time, 30*60, flows);
                                /*
                                for(SUMOTime t2 = 0; t2<30*60&&t2+time<endTime; t2+=stepOffset) { // !!!
                                    SUMOReal tmpFlow = (SUMOReal) detectors.getFlowFor(*k, (t2+time/*!!! + desc->duration2Last/), flows);
                                    if(tmpFlow<0) {
                                        tmpFlow = 0;
                                    }
                                    cflow += tmpFlow;
                                }
                                */
                            } else {
                                cflow = (SUMOReal) detectors.getFlowFor(*k, (time/*!!! + desc->duration2Last*/), flows);
                            }
                            if(cflow<0) {
                                cflow = 0; // !!!
                            }
                            if(my==0&&find((*ri)->edges2Pass.begin(), (*ri)->edges2Pass.end(), *k)!=(*ri)->edges2Pass.end()) {
                                my = *k;
                                myFlow = cflow;
                            }
                            sumFlows += cflow;
                        }

                        SUMOReal cProb = 1.;
                        if(sumFlows!=0) {
                            cProb = (myFlow / sumFlows);
                        } else {
                            // myFlow should be of course==0 so let's keep the previous probability
                            cProb = 1.;
                        }
                        ovProb *= cProb;
                        hadMissing = false;
                    } else {
                        hadMissing = true;
                    }
                }
                ++i;
            }
            distanceFactor = distanceFactor * distanceFactor;
            ovProb *= (rd->factor * distanceFactor);
            into[time]->add(ovProb, index);
            (*ri)->overallProb += ovProb;
        }
    }
    /*
    const std::map< int, FlowDef > &mflows = flows.getFlowDefs(myID);
    const std::map<ROEdge*, std::vector<ROEdge*> > &dets2Follow = myRoutes->getDets2Follow();
    for(int time=startTime; time<endTime; time+=stepOffset) {
        if(mflows.find(time)==mflows.end()) {
            continue;
        }
        into[time] = new RandomDistributor<size_t>();
        FlowDef srcFD = mflows.find(time)->second;
        SUMOReal toEmit = (SUMOReal) (srcFD.qLKW + srcFD.qPKW);
        const std::vector<DFRORouteDesc*> &descs = myRoutes->get();
        std::vector<DFRORouteDesc*>::const_iterator ri;
        size_t index = 0;
        for(ri=descs.begin(); ri!=descs.end()&&toEmit>=0; ++ri, index++) {
            SUMOReal destFlow = toEmit;
            for(std::vector<ROEdge*>::iterator j=(*ri)->edges2Pass.begin(); j!=(*ri)->edges2Pass.end()&&destFlow>0; ++j) {
                if(dets2Follow.find(*j)!=dets2Follow.end()) {
                    // here we have a detector which flows divides on consecutives
                    const std::vector<ROEdge*> &possNext = dets2Follow.find(*j)->second;
                    ROEdge *my = 0;
                    SUMOReal sumFlows = 0;
                    SUMOReal myFlow = 0;
                    for(std::vector<ROEdge*>::const_iterator k=possNext.begin(); k!=possNext.end(); ++k) {
                        SUMOReal cflow = detectors.getFlowFor(*k, (time/*!!! + desc->duration2Last/), flows);
    if(myID=="12") {
        cout << myID << " " << cflow << " " << (*k)->getID() << endl;
    }
                        if(cflow<0) {
                            cflow = destFlow; // !!!
                        }
                        if(my==0&&find((*ri)->edges2Pass.begin(), (*ri)->edges2Pass.end(), *k)!=(*ri)->edges2Pass.end()) {
                            my = *k;
                            myFlow = cflow;
                        }
                        sumFlows += cflow;
    if(myID=="12") {
        if(my!=0) {
            cout << myID << " " << cflow << " " << my->getID() << endl;
        } else {
            cout << myID << " " << cflow << endl;
        }
        cout << myID << " " << destFlow << " " << sumFlows << " " << cflow << endl;
    }

                    }
                    if(my!=0&&sumFlows>0) {
                        destFlow = (myFlow / sumFlows) * destFlow;
                    } else {
                        destFlow = 0;
                    }
                }
            }
//            cout << myID << " " << destFlow << endl;
            into[time]->add(destFlow, index);
    if(myID=="12") {
        cout << "dest " << myID << " " << destFlow << endl;
    }
            (*ri)->overallProb += destFlow;
        }
    }
/*
        std::vector<size_t> unset;
        size_t index = 0;
        for(ri=descs.begin(); ri!=descs.end()&&toEmit>=0; ++ri, index++) {
            DFRORouteDesc *desc = (*ri);
            const ROEdge *lastEdge = desc->lastDetectorEdge;
            if(lastEdge!=0) {
                int flow = detectors.getFlowFor(lastEdge, (SUMOTime) (time + desc->duration2Last), flows);
                if(flow>=0) {
                    into[time]->add(flow, index);
                    toEmit -= flow;
                    continue;
                }
            }
            unset.push_back(index);
        }
        if(toEmit>=0) {
            std::vector<size_t>::const_iterator di;
            for(di=unset.begin(); di!=unset.end()&&toEmit>=0; ++di) {
                into[time]->add((SUMOReal) toEmit / (SUMOReal) unset.size(), *di);
            }
        }
    }
    /*
    myRoutes->sortByDistance();
    const std::map< int, FlowDef > &mflows = flows.getFlowDefs(myID);
    for(int time=startTime; time<endTime; time+=stepOffset) {
        if(mflows.find(time)==mflows.end()) {
            continue;
        }
        into[time] = new RandomDistributor<size_t>();
        FlowDef srcFD = mflows.find(time)->second;
        size_t toEmit = srcFD.qLKW + srcFD.qPKW;
        const std::vector<DFRORouteDesc*> &descs = myRoutes->get();
        std::vector<DFRORouteDesc*>::const_iterator ri;
        std::vector<size_t> unset;
        size_t index = 0;
        for(ri=descs.begin(); ri!=descs.end()&&toEmit>=0; ++ri, index++) {
            DFRORouteDesc *desc = (*ri);
            const ROEdge *lastEdge = desc->lastDetectorEdge;
            if(lastEdge!=0) {
                int flow = detectors.getFlowFor(lastEdge, (SUMOTime) (time + desc->duration2Last), flows);
                if(flow>=0) {
                    into[time]->add(flow, index);
                    toEmit -= flow;
                    continue;
                }
            }
            unset.push_back(index);
        }
        if(toEmit>=0) {
            std::vector<size_t>::const_iterator di;
            for(di=unset.begin(); di!=unset.end()&&toEmit>=0; ++di) {
                into[time]->add((SUMOReal) toEmit / (SUMOReal) unset.size(), *di);
            }
        }
    }

    /*
	RandomDistributor<size_t> *ret = new RandomDistributor<size_t>;
	ret->add(1, 0); // !!!
    */
//	return ret;
}

void
DFDetector::writeEmitterDefinition(const std::string &file,
                                   const DFDetectorCon &detectors,
								   const DFDetectorFlows &flows,
								   SUMOTime startTime, SUMOTime endTime,
								   SUMOTime stepOffset) const
{
	// write the definition
    cout << myID << endl;
	ofstream strm(file.c_str());
	if(!strm.good()) {
		MsgHandler::getErrorInstance()->inform("Could not open definition file '" + file + "'.");
		throw ProcessError();
	}
		// begin
	if(getType()==SOURCE_DETECTOR) {
		strm << "<triggeredsource>" << endl;
	} else {
		strm << "<calibrator>" << endl;
	}


        std::map<size_t, RandomDistributor<size_t>* > dists;
	if(flows.knows(myID)) {
        buildDestinationDistribution(detectors, flows, startTime, endTime, stepOffset, dists);
    }

		// routes
	{

		if(myRoutes!=0&&myRoutes->get().size()!=0) {
		    const std::vector<DFRORouteDesc*> &routes = myRoutes->get();
		    std::vector<DFRORouteDesc*>::const_iterator i;
            // compute the overall routes sum
            SUMOReal overallSum = 0;
            for(i=routes.begin(); i!=routes.end(); ++i) {
                overallSum += (*i)->overallProb;
            }

            int writtenRoutes = 0;
            if(overallSum!=0) {
		        // !!! check things about intervals
                // !!! optional
		        for(i=routes.begin(); i!=routes.end(); ++i) {
                    if((*i)->overallProb>0) {
                        strm << "   <routedistelem id=\"" << (*i)->routename << "\" probability=\"" << ((*i)->overallProb/overallSum) << "\"/>" << endl; // !!!
                        writtenRoutes++;
                    }
	    	    }
            }
            if(writtenRoutes==0) {
                strm << "   <routedistelem id=\"" << routes[0]->routename << "\" probability=\"1\"/>" << endl; // !!!
            }
		} else {
			cout << "Detector " << getID() << " has no routes!?" << endl;
			throw 1; //!!!
		}
	}
		// vehicle types
	{
	}
    if(myID=="12") {
        int bla = 0;
    }
		// emissions
	if(flows.knows(myID)) {
		// get the flows for this detector

        const std::vector<FlowDef> &mflows = flows.getFlowDefs(myID);
		// go through the simulation seconds
		for(int time=startTime; time<endTime; time+=stepOffset) {
			// get own (departure flow)
            if((int) mflows.size()<=(int) (time/stepOffset) - startTime) {
                cout << mflows.size() << ":" << (int) (time/stepOffset) - startTime << endl;
                throw 1;
            }
            const FlowDef &srcFD = mflows[(int) (time/stepOffset) - startTime]; // !!! check stepOffset
			// get flows at end
            RandomDistributor<size_t> *destDist = dists[time];
			// go through the cars
			size_t carNo = (size_t) (srcFD.qPKW + srcFD.qLKW);
			for(size_t car=0; car<carNo; ++car) {
				// get the vehicle parameter
				string type = "test";
                SUMOReal v;
                int destIndex = destDist->getOverallProb()>0 ? destDist->get() : -1;
//!!! micro srcIndex = srcDist.get();
//				std::vector<std::string> route = droutes[destIndex]->edges2Pass;
				if(srcFD.isLKW>1) {
					srcFD.isLKW = srcFD.isLKW - (SUMOReal) 1.;
//!!!		        	type = lkwTypes[vehSpeedDist.get()];
			        v = srcFD.vLKW;
				} else {
//!!!	    			type = pkwTypes[vehSpeedDist.get()];
                    v = srcFD.vPKW;
				}
				if(v<=0) {
					v = (SUMOReal) (100 / 3.6);
				} else if(v>=180) {
					v = (SUMOReal) (100 / 3.6);
				}

				// compute the departure time
				int ctime = (int) (time + ((SUMOReal) stepOffset * (SUMOReal) car / (SUMOReal) carNo));

				// write
				strm << "   <emit id=\"";
				if(getType()==SOURCE_DETECTOR) {
					strm << "emitter_" << myID;
				} else {
					strm << "calibrator_" << myID;
				}
				strm << "_" << ctime  << "\"" // !!! running
					<< " time=\"" << ctime << "\""
                    << " speed=\"" << v << "\"";
                if(destIndex>=0) {
                    strm << " route=\"" << myRoutes->get()[destIndex]->routename << "\""; // !!! optional
                }
                strm << " vehtype=\"" << type << "\"/>" << endl;
				srcFD.isLKW += srcFD.fLKW;
			}
			delete destDist;
		}
	}

	if(getType()==SOURCE_DETECTOR) {
		strm << "</triggeredsource>" << endl;
	} else {
		strm << "</calibrator>" << endl;
	}
}


void
DFDetector::addRoutes(DFRORouteCont *routes)
{
	delete myRoutes;
	myRoutes = routes;
}


bool
DFDetector::hasRoutes() const
{
	return myRoutes!=0&&myRoutes->get().size()!=0;
}


bool
DFDetector::writeRoutes(std::vector<std::string> &saved,
						std::ostream &os)
{
	if(myRoutes!=0) {
		return myRoutes->save(saved, "", os);
	}
	return false;
}


void
DFDetector::writeSingleSpeedTrigger(const std::string &file,
                                 const DFDetectorFlows &flows,
                                 SUMOTime startTime, SUMOTime endTime,
                                 SUMOTime stepOffset)
{
	ofstream strm(file.c_str());
	if(!strm.good()) {
		MsgHandler::getErrorInstance()->inform("Could not open file '" + file + "'.");
		throw ProcessError();
	}
	strm << "<vss>" << endl;
    const std::vector<FlowDef> &mflows = flows.getFlowDefs(myID);
    for(SUMOTime t=startTime; t<endTime; t+=stepOffset) {
        const FlowDef &srcFD = mflows[(int) (t/stepOffset) - startTime];// !!! check stepOffset
        SUMOReal speed = MAX2(srcFD.vLKW, srcFD.vPKW);
        if(speed==0) {
            speed = 200; // !!! no limit
        }
        speed = (SUMOReal) (speed / 3.6);
        strm << "   <step time=\"" << t << "\" speed=\"" << speed << "\"/>" << endl;
    }
	strm << "</vss>" << endl;
}










DFDetectorCon::DFDetectorCon()
{
}


DFDetectorCon::~DFDetectorCon()
{
}


bool
DFDetectorCon::addDetector(DFDetector *dfd )
{
	if(myDetectorMap.find(dfd->getID())!=myDetectorMap.end()) {
		return false;
	}
	myDetectorMap[dfd->getID()] = dfd;
	myDetectors.push_back(dfd);
    string edgeid = dfd->getLaneID().substr(0, dfd->getLaneID().rfind('_'));
    if(myDetectorEdgeMap.find(edgeid)==myDetectorEdgeMap.end()) {
        myDetectorEdgeMap[edgeid] = std::vector<DFDetector*>();
    }
    myDetectorEdgeMap[edgeid].push_back(dfd);
    return true; // !!!
}


bool
DFDetectorCon::detectorsHaveCompleteTypes() const
{
    for(std::vector<DFDetector*>::const_iterator i=myDetectors.begin(); i!=myDetectors.end(); ++i) {
        if((*i)->getType()==TYPE_NOT_DEFINED) {
            return false;
        }
    }
    return true;
}


bool
DFDetectorCon::detectorsHaveRoutes() const
{
    for(std::vector<DFDetector*>::const_iterator i=myDetectors.begin(); i!=myDetectors.end(); ++i) {
        if((*i)->hasRoutes()) {
            return true;
        }
    }
    return false;
}


const std::vector< DFDetector*> &
DFDetectorCon::getDetectors() const
{
    return myDetectors;
}


void
DFDetectorCon::save(const std::string &file) const
{
    ofstream strm(file.c_str());
	if(!strm.good()) {
		MsgHandler::getErrorInstance()->inform("The detector output file '" + file + "' could not be opened.");
		throw ProcessError();
	}
    strm << "<detectors>" << endl;
    for(std::vector<DFDetector*>::const_iterator i=myDetectors.begin(); i!=myDetectors.end(); ++i) {
        strm << "   <detector_definition id=\"" << (*i)->getID()
            << "\" lane=\"" << (*i)->getLaneID()
            << "\" pos=\"" << (*i)->getPos();
        switch((*i)->getType()) {
        case BETWEEN_DETECTOR:
            strm << "\" type=\"between\"";
            break;
        case SOURCE_DETECTOR:
            strm << "\" type=\"source\"";
            break;
        case HIGHWAY_SOURCE_DETECTOR:
            strm << "\" type=\"highway_source\"";
            break;
        case SINK_DETECTOR:
            strm << "\" type=\"sink\"";
            break;
		case DISCARDED_DETECTOR:
            strm << "\" type=\"discarded\"";
            break;
        default:
            throw 1;
        }
        strm << "/>" << endl;
    }
    strm << "</detectors>" << endl;
}


void
DFDetectorCon::saveAsPOIs(const std::string &file) const
{
    ofstream strm(file.c_str());
    strm << "<pois>" << endl;
    for(std::vector<DFDetector*>::const_iterator i=myDetectors.begin(); i!=myDetectors.end(); ++i) {
		strm << "   <poi id=\"" << (*i)->getID();
        switch((*i)->getType()) {
        case BETWEEN_DETECTOR:
            strm << "\" type=\"between_detector_position\" color=\"0,0,1\"";
            break;
        case SOURCE_DETECTOR:
            strm << "\" type=\"source_detector_position\" color=\"0,1,0\"";
            break;
        case HIGHWAY_SOURCE_DETECTOR:
            strm << "\" type=\"highway_source_detector_position\" color=\".5,1,.5\"";
            break;
        case SINK_DETECTOR:
            strm << "\" type=\"sink_detector_position\" color=\"1,0,0\"";
            break;
		case DISCARDED_DETECTOR:
            strm << "\" type=\"discarded_detector_position\" color=\".2,.2,.2\"";
            break;
        default:
            throw 1;
        }
        strm << " lane=\"" << (*i)->getLaneID()<< "\" pos=\""
			<< (*i)->getPos() << "\"/>" << endl;
    }
    strm << "</pois>" << endl;
}


void
DFDetectorCon::saveRoutes(const std::string &file) const
{
    ofstream strm(file.c_str());
	if(!strm.good()) {
		MsgHandler::getErrorInstance()->inform("The detector output file '" + file + "' could not be opened.");
		throw ProcessError();
	}
	std::vector<std::string> saved;
	strm << "<routes>" << endl;
	bool lastWasSaved = true;
    for(std::vector<DFDetector*>::const_iterator i=myDetectors.begin(); i!=myDetectors.end(); ++i) {
		if(lastWasSaved) {
			strm << endl;
		}
		lastWasSaved = (*i)->writeRoutes(saved, strm);
	}
	strm << endl;
	strm << "</routes>" << endl;
}


const DFDetector &
DFDetectorCon::getDetector(const std::string &id) const
{
	return *(myDetectorMap.find(id)->second);
}


/*
bool
DFDetectorCon::isDetector( std::string id )
{
	bool ret = false;
	for(std::vector<DFDetector*>::const_iterator i=myDetectors.begin(); i!=myDetectors.end(); ++i) {
		if ( (*i)->getID() == id )
			ret = true;
	}
	return ret;
}
*/


void
DFDetectorCon::writeEmitters(const std::string &file,
							 const DFDetectorFlows &flows,
							 SUMOTime startTime, SUMOTime endTime,
							 SUMOTime stepOffset, bool writeCalibrators)
{
	ofstream strm(file.c_str());
	if(!strm.good()) {
		MsgHandler::getErrorInstance()->inform("Could not open file '" + file + "'.");
		throw ProcessError();
	}
	strm << "<additional>" << endl;
	for(std::vector<DFDetector*>::const_iterator i=myDetectors.begin(); i!=myDetectors.end(); ++i) {
		DFDetector *det = *i;
		string defFileName;
		// write the declaration into the file
		if(det->getType()==SOURCE_DETECTOR) {
			defFileName = "emitter_" + det->getID() + ".def.xml";
			strm << "   <trigger id=\"source_" << det->getID()
				<< "\" objecttype=\"emitter\" "
				<< "pos=\"" << det->getPos() << "\" "
				<< "objectid=\"" << det->getLaneID() << "\" "
                << "friendly_pos=\"x\" " // !!!
				<< "file=\"" << defFileName << "\"/>" << endl;
		} else if(writeCalibrators) {
			defFileName = "calibrator_" + det->getID() + ".def.xml";
			strm << "   <trigger id=\"calibrator_" << det->getID()
				<< "\" objecttype=\"calibrator\" "
				<< "pos=\"" << det->getPos() << "\" "
				<< "objectid=\"" << det->getLaneID() << "\" "
                << "friendly_pos=\"x\" " // !!!
				<< "file=\"" << defFileName << "\"/>" << endl;
		}
		// check whethe the definition shall not be saved
		if(defFileName=="") {
			continue;
		}
		det->writeEmitterDefinition(defFileName, *this, flows,
			startTime, endTime, stepOffset);
	}
	strm << "</additional>" << endl;
}


int
DFDetectorCon::getFlowFor(const ROEdge *edge, SUMOTime time,
                          const DFDetectorFlows &) const
{
    SUMOReal stepOffset = 60; // !!!
    SUMOReal startTime = 0; // !!!
    assert(myDetectorEdgeMap.find(edge->getID())!=myDetectorEdgeMap.end());
    const std::vector<FlowDef> &flows = static_cast<const RODFEdge*>(edge)->getFlows();
    if(flows.size()!=0) {
        const FlowDef &srcFD = flows[(int) ((time/stepOffset) - startTime)];
        return (int) (MAX2(srcFD.qLKW, (SUMOReal) 0.) + MAX2(srcFD.qPKW, (SUMOReal) 0.));
    }
    /*
    const std::vector<DFDetector*> &detsOnEdge = myDetectorEdgeMap.find(edge->getID())->second;
    std::vector<DFDetector*>::const_iterator i;
    SUMOReal ret = 0;
    int counted = 0;
    for(i=detsOnEdge.begin(); i!=detsOnEdge.end(); ++i) {
        if(flows.knows((*i)->getID())) {
            const std::vector<FlowDef> &mflows = flows.getFlowDefs((*i)->getID());
            const FlowDef &srcFD = mflows[(int) (time/stepOffset) - startTime]; // !!! check stepOffset
            counted++; // !!! make a difference between pkws and lkws
            ret += (srcFD.qLKW + srcFD.qPKW);
        }
    }
    if(counted!=0) {
        return (int) (ret / (SUMOReal) counted);
    }
    */
    return -1;
}


int
DFDetectorCon::getAggFlowFor(const ROEdge *edge, SUMOTime time, SUMOTime period,
                             const DFDetectorFlows &) const
{
    SUMOReal stepOffset = 60; // !!!
    SUMOReal startTime = 0; // !!!
    assert(myDetectorEdgeMap.find(edge->getID())!=myDetectorEdgeMap.end());
    const std::vector<FlowDef> &flows = static_cast<const RODFEdge*>(edge)->getFlows();
    if(flows.size()!=0) {
        SUMOReal agg = 0;
        size_t beginIndex = (int) ((time/stepOffset) - startTime); // !!! falsch!!!
        for(SUMOTime t=0; t<period&&beginIndex<flows.size(); t+=(SUMOTime) stepOffset) {
            const FlowDef &srcFD = flows[beginIndex++];
            if(srcFD.qLKW>=0) {
                agg += srcFD.qLKW;
            }
            if(srcFD.qPKW>=0) {
                agg += srcFD.qPKW;
            }
        }
        return (int) agg;
    }
    return -1;
}


void
DFDetectorCon::writeSpeedTrigger(const std::string &file,
                                 const DFDetectorFlows &flows,
                                 SUMOTime startTime, SUMOTime endTime,
                                 SUMOTime stepOffset)
{
	ofstream strm(file.c_str());
	if(!strm.good()) {
		MsgHandler::getErrorInstance()->inform("Could not open file '" + file + "'.");
		throw ProcessError();
	}
	strm << "<additional>" << endl;
	for(std::vector<DFDetector*>::const_iterator i=myDetectors.begin(); i!=myDetectors.end(); ++i) {
		DFDetector *det = *i;
		// write the declaration into the file
		if(det->getType()==SINK_DETECTOR&&flows.knows(det->getID())) {
            string filename = "vss_" + det->getID() + ".def.xml";
            strm << "   <trigger id=\"vss_" << det->getID() << '\"'
                << " objecttype=\"lane\""
                << " objectid=\"" << det->getLaneID() << '\"'
                << " attr=\"speed\" "
                << " file=\"" << filename << "\"/>"
                << endl;
            det->writeSingleSpeedTrigger(filename, flows, startTime, endTime, stepOffset);
        }
    }
    strm << "</additional>" << endl;
}


void
DFDetectorCon::writeEndRerouterDetectors(const std::string &file)
{
	ofstream strm(file.c_str());
	if(!strm.good()) {
		MsgHandler::getErrorInstance()->inform("Could not open file '" + file + "'.");
		throw ProcessError();
	}
	strm << "<additional>" << endl;
	for(std::vector<DFDetector*>::const_iterator i=myDetectors.begin(); i!=myDetectors.end(); ++i) {
		DFDetector *det = *i;
		// write the declaration into the file
		if(det->getType()==SINK_DETECTOR) {
            strm << "   <trigger id=\"endrerouter_" << det->getID()
                << "\" objecttype=\"rerouter\" objectid=\"" <<
                det->getLaneID() << "\" attr=\"reroute\" pos=\"0\" file=\"endrerouter_"
                << det->getID() << ".def.xml\"/>" << endl;
        }
    }
    strm << "</additional>" << endl;
}


void
DFDetectorCon::writeValidationDetectors(const std::string &file,
                                        bool includeSources,
                                        bool singleFile, bool friendly)
{
	ofstream strm(file.c_str());
	if(!strm.good()) {
		MsgHandler::getErrorInstance()->inform("Could not open file '" + file + "'.");
		throw ProcessError();
	}
	strm << "<additional>" << endl;
	for(std::vector<DFDetector*>::const_iterator i=myDetectors.begin(); i!=myDetectors.end(); ++i) {
		DFDetector *det = *i;
		// write the declaration into the file
		if(det->getType()!=SOURCE_DETECTOR||includeSources) {
            SUMOReal pos = det->getPos();
            if(det->getType()==SOURCE_DETECTOR) {
                pos += 1;
            }
            strm << "   <detector id=\"validation_" << det->getID() << "\" "
                << "lane=\"" << det->getLaneID() << "\" "
                << "pos=\"" << pos << "\" "
                << "freq=\"60\" style=\"CSV\" ";
            if(friendly) {
                strm << "friendly_pos=\"x\" ";
            }
            if(!singleFile) {
                strm << "file=\"validation_det_" << det->getID() << ".xml\"/>" << endl;
            } else {
                strm << "file=\"validation_dets.xml\"/>" << endl;
            }
        }
    }
    strm << "</additional>" << endl;
}


void
DFDetectorCon::removeDetector(const std::string &id)
{
        //
        std::map<std::string, DFDetector*>::iterator ri1 = myDetectorMap.find(id);
        DFDetector *oldDet = (*ri1).second;
        myDetectorMap.erase(ri1);
        //
        std::vector<DFDetector*>::iterator ri2 =
            find(myDetectors.begin(), myDetectors.end(), oldDet);
        myDetectors.erase(ri2);
        //
        bool found = false;
        for(std::map<std::string, std::vector<DFDetector*> >::iterator rr3=myDetectorEdgeMap.begin(); !found&&rr3!=myDetectorEdgeMap.end(); ++rr3) {
            std::vector<DFDetector*> &dets = (*rr3).second;
            for(std::vector<DFDetector*>::iterator ri3=dets.begin(); !found&&ri3!=dets.end(); ++ri3) {
                if(*ri3==oldDet) {
                    found = true;
                    dets.erase(ri3);
                }
            }
        }
        delete oldDet;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


