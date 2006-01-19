/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#include "DFDetector.h"
#include <fstream>

using namespace std;

DFDetector::DFDetector(const std::string &Id, const std::string &laneId,
                       SUMOReal pos, const dfdetector_type type)
    : myID(Id), myLaneID(laneId), myPosition(pos), myType(type)
{
}


DFDetector::~DFDetector()
{
}


void
DFDetector::setType(dfdetector_type type)
{
    myType = type;
}




DFDetectorCon::DFDetectorCon()
{
}


DFDetectorCon::~DFDetectorCon()
{
}

bool
DFDetectorCon::addDetector(const DFDetector & dfd )
{
	myDetectors.push_back(dfd);
    return true; // !!!
}


bool
DFDetectorCon::detectorsHaveCompleteTypes() const
{
    for(std::vector<DFDetector>::const_iterator i=myDetectors.begin(); i!=myDetectors.end(); ++i) {
        if((*i).getType()==TYPE_NOT_DEFINED) {
            return false;
        }
    }
    return true;
}


std::vector< DFDetector > &
DFDetectorCon::getDetectors()
{
    return myDetectors;
}


void
DFDetectorCon::save(const std::string &file) const
{
    ofstream strm(file.c_str());
    strm << "<detectors>" << endl;
    for(std::vector<DFDetector>::const_iterator i=myDetectors.begin(); i!=myDetectors.end(); ++i) {
        strm << "   <detector_definition id=\"" << (*i).getID()
            << "\" lane=\"" << (*i).getLaneID()
            << "\" pos=\"" << (*i).getPos();
        switch((*i).getType()) {
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
        default:
            throw 1;
        }
        strm << "/>" << endl;
    }
    strm << "</detectors>" << endl;
}


bool
DFDetectorCon::isDetector( std::string id )
{
	bool ret = false;
	for(std::vector<DFDetector>::const_iterator i=myDetectors.begin(); i!=myDetectors.end(); ++i) {
		if ( i->getID() == id )
			ret = true;
	}
	return ret;
}