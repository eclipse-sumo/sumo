#ifndef GUIGlobals_h
#define GUIGlobals_h

#include <vector>
#include <fx.h>

class GUINet;


struct GUISimInfo {
    GUISimInfo(GUINet &netArg) : net(netArg) { }
    GUINet &net;
};

/// the window shall be closed when the simulation has ended
extern bool gQuitOnEnd;

/// the gui loads and starts a simulation at the beginning
extern bool gStartAtBegin;

/// Information whether aggregated views are allowed
extern bool gAllowAggregated;

/// Information whether time-floating views are allowed
extern bool gAllowAggregatedFloating;

/// Information whether not to display the information about a simulation's end
extern bool gSuppressEndInfo;

/// Structure holding the used network
extern GUISimInfo *gSimInfo;

/// list of chosen items
extern std::vector<size_t> gChosenObjects;



extern float gAggregationRememberingFactor;
extern FXApp *gFXApp;


bool gfIsSelected(int type, size_t id);

void gfSelect(int type, size_t id);

void gfDeselect(int type, size_t id);


#endif

