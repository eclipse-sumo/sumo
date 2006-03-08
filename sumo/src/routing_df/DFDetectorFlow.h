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
    SUMOReal qPKW;
    // Number of heavy duty vehicles that passed within the described time
    SUMOReal qLKW;
    // Mean velocity of passenger vehicles within the described time
    SUMOReal vPKW;
    // Mean velocity of heavy duty vehicles within the described time
    SUMOReal vLKW;
    // begin time (in s)
//    int time;
    // probability for having a heavy duty vehicle(qKFZ!=0 ? (qLKW / qKFZ) : 0;)
    SUMOReal fLKW;
    // initialise with 0
    mutable SUMOReal isLKW;
    //
    bool firstSet;
};

class DFDetectorFlows
{
public:
	DFDetectorFlows(SUMOTime startTime, SUMOTime endTime,
        SUMOTime stepOffset);
	~DFDetectorFlows();
	void addFlow(const std::string &detector_id, int timestamp,
        const FlowDef &fd );
	//const FlowDef &getFlowDef(const std::string &det_id, SUMOTime timestamp) const;
	//const std::map< int, FlowDef > &getFlowDefs( const std::string &id ) const;

    const std::vector<FlowDef> &getFlowDefs( const std::string &id ) const;
    bool knows( const std::string &det_id ) const;
    bool knows( const std::string &det_id, SUMOTime time ) const;
    //void buildFastAccess(SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset);
protected:
//    std::map<std::string, std::map<SUMOTime, FlowDef> > myCurrentFlows;
    std::map<std::string, std::vector<FlowDef> > myFastAccessFlows;
    SUMOTime myBeginTime, myEndTime, myStepOffset;

};



#endif