#include "GUIGlobals.h"
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

std::vector<size_t> gChosenObjects;


bool gfIsSelected(int type, size_t id)
{
    std::vector<size_t>::iterator i=
        find(gChosenObjects.begin(), gChosenObjects.end(), id);
    return i!=gChosenObjects.end();
}


void gfSelect(int type, size_t id)
{
    std::vector<size_t>::iterator i=
        find(gChosenObjects.begin(), gChosenObjects.end(), id);
    if(i==gChosenObjects.end()) {
        gChosenObjects.push_back(id);
    }
}


void gfDeselect(int type, size_t id)
{
    std::vector<size_t>::iterator i=
        find(gChosenObjects.begin(), gChosenObjects.end(), id);
    if(i!=gChosenObjects.end()) {
        gChosenObjects.erase(i);
    }
}


