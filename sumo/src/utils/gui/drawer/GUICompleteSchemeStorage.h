#ifndef GUICompleteSchemeStorage_h
#define GUICompleteSchemeStorage_h

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <utils/gui/windows/GUISUMOAbstractView.h>

class GUICompleteSchemeStorage {
public:
    GUICompleteSchemeStorage() { }
    ~GUICompleteSchemeStorage() { }
    void add(const std::string &name, GUISUMOAbstractView::VisualizationSettings &scheme) {
        if(std::find(mySortedSchemeNames.begin(), mySortedSchemeNames.end(), name)==mySortedSchemeNames.end()) {
            mySortedSchemeNames.push_back(name);
        }
        mySettings[name] = scheme;
    }

    const GUISUMOAbstractView::VisualizationSettings &get(const std::string &name) const {
        return mySettings.find(name)->second;
    }

    bool contains(const std::string &name) const {
        return mySettings.find(name)!=mySettings.end();
    }

    const std::vector<std::string> getNames() const  {
        return mySortedSchemeNames;
    }

    void init();

protected:
    std::map<std::string, GUISUMOAbstractView::VisualizationSettings> mySettings;
    std::vector<std::string> mySortedSchemeNames;

};

extern GUICompleteSchemeStorage gSchemeStorage;


#endif
