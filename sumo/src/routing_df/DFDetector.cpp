/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#include "DFDetector.h"
#include <fstream>

using namespace std;

DFDetector::DFDetector(): id(0),laneid(0),pos(0),type(BETWEEN_DETECTOR)
{}

DFDetector::~DFDetector()
{}


DFDetectorCon::DFDetectorCon()
{}

DFDetectorCon::~DFDetectorCon()
{
}

void
DFDetectorCon::addDetector(const DFDetector & dfd )
{
	myDetectors.push_back(dfd);
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
        case SINK_DETEKTOR:
            strm << "\" type=\"sink\"";
            break;
        default:
            throw 1;
        }
        strm << "/>" << endl;
    }
    strm << "</detectors>" << endl;
}
