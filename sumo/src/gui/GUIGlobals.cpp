#include "GUIGlobals.h"
#include <gui/GUIGlObjectStorage.h>
#include <gui/GUIGlobalSelection.h>

#include <algorithm>

using namespace std;

bool gQuitOnEnd;
bool gStartAtBegin;
bool gAllowAggregated;
bool gAllowAggregatedFloating;
bool gAllowTextures;
bool gSuppressEndInfo;
float gAggregationRememberingFactor;

GUISimInfo *gSimInfo = 0;

FXApp *gFXApp = 0;

std::string gCurrentFolder;


GUISelectedStorage gSelected;

    /** @brief A container for numerical ids of objects
        in order to make them grippable by openGL */
GUIGlObjectStorage gIDStorage;

GUIAddWeightsStorage gAddWeightsStorage;

std::vector<int> gBreakpoints;