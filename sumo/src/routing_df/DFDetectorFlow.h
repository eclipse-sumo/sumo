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

#include <map>
#include <string>
#include <vector>
//#include <routing_df/DFDetector.h>

struct FlowDef {
    // Anzahl Fahrzeuge innerhalb der beschriebenen Zeit
    float qPKW;     
    // Anzahl PKWs innerhalb der beschriebenen Zeit
    float qLKW;
    // Durchschnittsgeschwindigkeit der LKWs in der b. Zeit
    float vLKW;
    // Durchschnittsgeschwindigkeit der PKWs in der b. Zeit
    float vPKW;
    // Anfangszeitpunkt der beschriebenen Zeit (in s?)
    int time;
    // Name des Detektors
    std::string det;
    // Verhältnis LKW/PKW (qKFZ!=0 ? (qLKW / qKFZ) : 0;)
    float fLKW;
    // mit 0 initialisieren
    float isLKW;
};

class DFDetectorFlows
{
public:
	DFDetectorFlows();
	~DFDetectorFlows();
	void addFlow(std::string detector_id, int timestamp, FlowDef fd );
	FlowDef getFlowDef( std::string det_id, int timestamp);
	std::map< int, FlowDef > &getFlowDefs( std::string id );
protected:
    std::map<std::string, std::map<int, FlowDef> > myCurrFlows;
  
};



#endif