/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#include "DFDetector.h"
#include <fstream>
#include <iostream>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <router/ROEdge.h>
#include "DFRORouteDesc.h"
#include "DFRORouteCont.h"
#include "DFDetectorFlow.h"
#include <utils/helpers/RandomDistributor.h>

using namespace std;

DFDetector::DFDetector(const std::string &Id, const std::string &laneId,
                       SUMOReal pos, const dfdetector_type type)
    : myID(Id), myLaneID(laneId), myPosition(pos), myType(type), myRoutes(0)
{
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


void
DFDetector::buildDestinationDistribution(const DFDetectorCon &detectors,
                                         const DFDetectorFlows &flows,
										 SUMOTime startTime,
                                         SUMOTime endTime,
                                         SUMOTime stepOffset,
                                         std::map<size_t, RandomDistributor<size_t>* > &into) const
{
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
		// routes
	{
		if(myRoutes!=0) {

		// !!! check things about intervals
        // !!! optional
		const std::vector<DFRORouteDesc*> &routes = myRoutes->get();
		std::vector<DFRORouteDesc*>::const_iterator i;
		for(i=routes.begin(); i!=routes.end(); ++i) {
			strm << "   <routedistelem id=\"" << (*i)->routename << "\" probability=\"" << 1 << "\"/>" << endl; // !!!
		}
		/*
                    if(haveEnd) {
                        for(j=droutes.begin(); j!=droutes.end(); ++j) {
                            float prob;
                            if((*j)->probab>=0) {
                                prob = (*j)->probab;
                            } else {
                                prob = no; // !!!?
                            }
                            if(prob!=0) {
                                strm2 << "      <route-dist id=\"" << (*j)->routename << "\" probability=\"" << prob << "\"/>" << endl;
                            }
                        }
                    } else {
                        strm2 << "      <route-dist id=\"" << droutes[droutes.size()-1]->routename << "\" probability=\"" << 1 << "\"/>" << endl;
                    }
					*/
		} else {
			cout << "Detector " << getID() << " has no routes!?" << endl;
			throw 1; //!!!
		}
	}
		// vehicle types
	{
	}
		// emissions
	if(flows.knows(myID)) {
		// get the flows for this detector
		const std::map< int, FlowDef > &mflows = flows.getFlowDefs(myID);
        std::map<size_t, RandomDistributor<size_t>* > dists;
        buildDestinationDistribution(detectors, flows, startTime, endTime, stepOffset, dists);
		// go through the simulation seconds
		for(int time=startTime; time<endTime; time+=stepOffset) {
            if(mflows.find(time)==mflows.end()) {
                continue;
            }
			// get own (departure flow)
			FlowDef srcFD = mflows.find(time)->second;
			// get flows at end
            RandomDistributor<size_t> *destDist = dists[time];
			// go through the cars
			size_t carNo = srcFD.qLKW + srcFD.qLKW;
			for(size_t car=0; car<carNo; ++car) {
				// get the vehicle parameter
				string type = "test";
                SUMOReal v;
				size_t destIndex = destDist->get();
//!!! micro srcIndex = srcDist.get();
//				std::vector<std::string> route = droutes[destIndex]->edges2Pass;
				if(srcFD.isLKW>1) {
					srcFD.isLKW = srcFD.isLKW - 1.;
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
				int ctime = time * stepOffset + ((double) stepOffset * (double) car / (double) carNo);

				// write
				strm << "   <emit id=\"";
				if(getType()==SOURCE_DETECTOR) {
					strm << "emitter_" << myID;
				} else {
					strm << "calibrator_" << myID;
				}
				strm << "_" << ctime  << "\"" // !!! running
					<< " time=\"" << ctime << "\""
                    << " speed=\"" << v << "\""
                    << " route=\"" << myRoutes->get()[destIndex]->routename << "\"" // !!! optional
                    << " vehtype=\"" << type << "\"/>" << endl;
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
				<< "file=\"" << defFileName << "\"/>" << endl;
		} else if(writeCalibrators) {
			defFileName = "calibrator_" + det->getID() + ".def.xml";
			strm << "   <trigger id=\"calibrator_" << det->getID()
				<< "\" objecttype=\"calibrator\" "
				<< "pos=\"" << det->getPos() << "\" "
				<< "objectid=\"" << det->getLaneID() << "\" "
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
                          const DFDetectorFlows &flows) const
{
    assert(myDetectorEdgeMap.find(edge->getID())!=myDetectorEdgeMap.end());
    const std::vector<DFDetector*> &detsOnEdge = myDetectorEdgeMap.find(edge->getID())->second;
    std::vector<DFDetector*>::const_iterator i;
    int ret = 0;
    int counted = 0;
    for(i=detsOnEdge.begin(); i!=detsOnEdge.end(); ++i) {
        if(flows.knows((*i)->getID(), time)) {
            const FlowDef &flow = flows.getFlowDef((*i)->getID(), time);
            counted++; // !!! make a difference between pkws and lkws
            ret += flow.qLKW + flow.qPKW;
        }
    }
    if(counted!=0) {
        return ret / counted;
    }
    return -1;
}



