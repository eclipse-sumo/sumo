#ifndef GUIGlobals_h
#define GUIGlobals_h

#include <vector>
#include <string>
#include <fx.h>
#include <gui/GUIGlObjectStorage.h>
#include <gui/GUIAddWeightsStorage.h>

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


extern bool gAllowTextures;


/// Information whether not to display the information about a simulation's end
extern bool gSuppressEndInfo;

/// Structure holding the used network
extern GUISimInfo *gSimInfo;


extern std::string gCurrentFolder;



extern float gAggregationRememberingFactor;
extern FXApp *gFXApp;


    /** @brief A container for numerical ids of objects
        in order to make them grippable by openGL */
extern GUIGlObjectStorage gIDStorage;


extern GUIAddWeightsStorage gAddWeightsStorage;
extern std::vector<int> gBreakpoints;


#endif

