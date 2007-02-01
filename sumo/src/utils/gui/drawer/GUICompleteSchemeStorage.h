/****************************************************************************/
/// @file    GUICompleteSchemeStorage.h
/// @author  unknown_author
/// @date    :find(mySortedSchemeNames.begin(), mySortedSchemeNames.end(), name)==mySortedSchemeNames.end()) {
/// @version $Id: $
///
// missing_desc
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUICompleteSchemeStorage_h
#define GUICompleteSchemeStorage_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <utils/gui/windows/GUISUMOAbstractView.h>

class GUICompleteSchemeStorage
{
public:
    GUICompleteSchemeStorage()
    { }
    ~GUICompleteSchemeStorage()
    { }
    void add(const std::string &name, GUISUMOAbstractView::VisualizationSettings &scheme)
    {
        if (std::find(mySortedSchemeNames.begin(), mySortedSchemeNames.end(), name)==mySortedSchemeNames.end()) {
            mySortedSchemeNames.push_back(name);
        }
        mySettings[name] = scheme;
    }

    GUISUMOAbstractView::VisualizationSettings &get(const std::string &name)
    {
        return mySettings.find(name)->second;
    }

    bool contains(const std::string &name) const
    {
        return mySettings.find(name)!=mySettings.end();
    }

    const std::vector<std::string> &getNames() const
    {
        return mySortedSchemeNames;
    }

    void init(const std::map<int, std::vector<RGBColor> > &vehColMap,
              const std::map<int, std::vector<RGBColor> > &laneColMap);

    const std::map<std::string, GUISUMOAbstractView::VisualizationSettings> &getItems() const
    {
        return mySettings;
    }

protected:
    std::map<std::string, GUISUMOAbstractView::VisualizationSettings> mySettings;
    std::vector<std::string> mySortedSchemeNames;

};

extern GUICompleteSchemeStorage gSchemeStorage;


#endif

/****************************************************************************/

