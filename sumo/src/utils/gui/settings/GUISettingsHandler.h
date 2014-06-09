/****************************************************************************/
/// @file    GUISettingsHandler.h
/// @author  Michael Behrisch
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    Fri, 24. Apr 2009
/// @version $Id$
///
// The handler for parsing gui settings from xml.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUISettingsHandler_h
#define GUISettingsHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/RandomDistributor.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUISUMOAbstractView;
class Position;


// ===========================================================================
// class definitions
// ===========================================================================
/** @class GUISettingsHandler
 * @brief An XML-handler for visualisation schemes
 */
class GUISettingsHandler : public SUMOSAXHandler {
public:
    /** @brief Constructor
     * @param[in] file the file to parse
     */
    GUISettingsHandler(const std::string& content, bool isFile = true);


    /// @brief Destructor
    ~GUISettingsHandler();



    /// @name inherited from GenericSAXHandler
    //@{

    /** @brief Called on the opening of a tag
     * @param[in] element ID of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myStartElement
     */
    void myStartElement(int element, const SUMOSAXAttributes& attrs);
    //@}



    /** @brief Adds the parsed settings to the global list of settings
     * @return the name of the parsed settings
     */
    std::string addSettings(GUISUMOAbstractView* view = 0) const;


    /** @brief Sets the viewport which has been parsed
     * @param[in] parent the view for which the viewport has to be set
     */
    void setViewport(GUISUMOAbstractView* view) const;


    /** @brief Sets the viewport which has been parsed
     * @param[out] zoom Variable to store the loaded zoom into
     * @param[out] xoff Variable to store the loaded x-offset into
     * @param[out] yoff Variable to store the loaded y-offset into
     */
    void setViewport(Position& lookFrom, Position& lookAt) const;


    /** @brief Makes a snapshot if it has been parsed
     * @param[in] parent the view which needs to be shot
     * @todo Please describe why the snapshots are only set if no other existed before (see code)
     */
    void setSnapshots(GUISUMOAbstractView* view) const;


    /** @brief Returns whether any decals have been parsed
     * @return whether decals have been parsed
     */
    bool hasDecals() const;


    /** @brief Returns the parsed decals
     * @return the parsed decals
     */
    const std::vector<GUISUMOAbstractView::Decal>& getDecals() const;


    /** @brief Returns the parsed delay
     * @return the parsed delay
     */
    SUMOReal getDelay() const;


    /** @brief Returns the parsed breakpoints
     * @return the parsed breakpoints
     */
    const std::vector<SUMOTime>& getBreakpoints() const {
        return myBreakpoints;
    }


    /// @brief loads breakpoints from the specified file
    static std::vector<SUMOTime> loadBreakpoints(const std::string& file);


    /** @brief Returns the parsed view type
     * @return the parsed view type
     */
    const std::string& getViewType() const {
        return myViewType;
    }

    RandomDistributor<std::string> getEventDistribution(const std::string& id);
    SUMOReal getJamSoundTime() {
        return myJamSoundTime;
    }

private:
    /// @brief The settings to fill
    GUIVisualizationSettings mySettings;

    /// @brief The view type (osg, opengl, default) loaded
    std::string myViewType;

    /// @brief The delay loaded
    SUMOReal myDelay;

    /// @brief The viewport loaded, zoom is stored in z coordinate
    Position myLookFrom;

    /// @brief The point to look at, only needed for osg view
    Position myLookAt;

    /// @brief mappig of time steps to filenames for potential snapshots
    std::map<SUMOTime, std::string> mySnapshots;

    /// @brief The decals list to fill
    std::vector<GUISUMOAbstractView::Decal> myDecals;

    /// @brief The last color scheme category (edges or vehicles)
    int myCurrentColorer;

    /// @brief The current color scheme
    GUIColorScheme* myCurrentScheme;

    /// @brief The parsed breakpoints
    std::vector<SUMOTime> myBreakpoints;

    /// @brief The parsed event distributions
    std::map<std::string, RandomDistributor<std::string> > myEventDistributions;
    SUMOReal myJamSoundTime;

private:

    /// @brief parse combined settings of bool, size and color
    GUIVisualizationTextSettings parseTextSettings(
        const std::string& prefix, const SUMOSAXAttributes& attrs,
        GUIVisualizationTextSettings defaults);

};

#endif

/****************************************************************************/
