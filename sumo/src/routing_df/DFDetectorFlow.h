#ifndef DFDETECTOR_FLOW_h
#define DFDETECTOR_FLOW_h

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <utils/common/SUMOTime.h>
#include <map>
#include <string>
#include <vector>
//#include <routing_df/DFDetector.h>

struct FlowDef {
    // Number of passenger vehicles that passed within the described time
    float qPKW;
    // Number of heavy duty vehicles that passed within the described time
    float qLKW;
    // Mean velocity of passenger vehicles within the described time
    float vPKW;
    // Mean velocity of heavy duty vehicles within the described time
    float vLKW;
    // begin time (in s)
    int time;
    // probability for having a heavy duty vehicle(qKFZ!=0 ? (qLKW / qKFZ) : 0;)
    float fLKW;
    // initialise with 0
    float isLKW;
};

class DFDetectorFlows
{
public:
	DFDetectorFlows();
	~DFDetectorFlows();
	void addFlow(std::string detector_id, int timestamp, FlowDef fd );
	const FlowDef &getFlowDef(const std::string &det_id, SUMOTime timestamp) const;
	const std::map< int, FlowDef > &getFlowDefs( const std::string &id ) const;
    bool knows( const std::string &det_id ) const;
    bool knows( const std::string &det_id, SUMOTime time ) const;
protected:
    std::map<std::string, std::map<SUMOTime, FlowDef> > myCurrFlows;

};



#endif