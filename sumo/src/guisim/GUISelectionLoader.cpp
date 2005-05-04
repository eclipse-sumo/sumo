/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#include <guisim/GUIEdge.h>
#include <microsim/MSLane.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include "GUISelectionLoader.h"
#include <fstream>

using namespace std;


void
GUISelectionLoader::loadSelection(const std::string &file)
{
    // ok, load all
    std::map<std::string, int> typeMap;
    typeMap["edge"] = GLO_EDGE;
    typeMap["induct loop"] = GLO_DETECTOR;
    typeMap["junction"] = GLO_JUNCTION;
    typeMap["speedtrigger"] = GLO_LANESPEEDTRIGGER;
    typeMap["lane"] = GLO_LANE;
    typeMap["tl-logic"] = GLO_TLLOGIC;
    typeMap["vehicle"] = GLO_VEHICLE;
    ifstream strm(file.c_str());
    while(strm.good()) {
        string name;
        strm >> name;
        if(name.length()==0) {
            continue;
        }
        size_t idx = name.find(':');
        if(idx!=string::npos) {
            string type = name.substr(0, idx);
            name = name.substr(idx+1);
            if(typeMap.find(type)==typeMap.end()) {
                // !!! inform user or something - the info does not fit to the pattern
                throw 1;
            }
            int itype = typeMap[type];
            int oid = -1;
            switch(itype) {
            case GLO_VEHICLE:
                {
                }
                break;
            case GLO_TLLOGIC:
                {
                }
                break;
            case GLO_DETECTOR:
                {
                }
                break;
            case GLO_EMITTER:
                {
                }
                break;
            case GLO_LANE:
                {
                    MSLane *l = MSLane::dictionary(name);
                    if(l!=0) {
                        oid = static_cast<const GUIEdge&>(l->edge()).getLaneGeometry(l).getGlID();
                    }
                }
                break;
            case GLO_EDGE:
                {
                }
                break;
            case GLO_JUNCTION:
                {
                }
                break;
            case GLO_LANESPEEDTRIGGER:
                {
                }
                break;
            }
            if(oid>=0) {
                gSelected.select(itype, oid, false);
            } else {
                // !!! inform user or something - the object was not found
            }
        } else {
            // !!! inform user or something - the info does not fit to the pattern
            throw 1;
        }
    }
}


GUISelectionLoader::GUISelectionLoader()
{
}


GUISelectionLoader::~GUISelectionLoader()
{
}
