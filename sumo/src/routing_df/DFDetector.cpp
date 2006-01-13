#include "DFDetector.h"

DFDetector::DFDetector(): id(0),laneid(0),pos(0),type(BETWEEN_DETECTOR)
{}

DFDetector::~DFDetector()
{}


DFDetectorCon::DFDetectorCon()
{}

DFDetectorCon::~DFDetectorCon()
{
	_detmap.clear();
}

void
DFDetectorCon::addDetector(const int &id, const DFDetector & dfd )
{
	_detmap[id] = dfd;
}

