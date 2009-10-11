/****************************************************************************/
/// @file    GUISettingsHandler.h
/// @author  Michael Behrisch
/// @date    Fri, 24. Apr 2009
/// @version $Id$
///
// The handler for parsing gui settings from xml.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/XMLSubSys.h>


// ===========================================================================
// class definitions
// ===========================================================================
/** @class GUISettingsHandler
 * @brief An XML-handler for visualisation schemes
 */
class GUISettingsHandler : public SUMOSAXHandler {
public:
    /** @brief Constructor
     *
     * @param[in] file the file to parse
     */
    GUISettingsHandler(const std::string &content, bool isFile=true) throw();

    /// @brief Destructor
    ~GUISettingsHandler() throw();


    /// @name inherited from GenericSAXHandler
    //@{

    /** @brief Called on the opening of a tag
     *
     * @param[in] element ID of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myStartElement
     */
    void myStartElement(SumoXMLTag element, const SUMOSAXAttributes &attrs) throw(ProcessError);
    //@}

    /** @brief Adds the parsed settings to the global list of settings
     *
     * @return the name of the parsed settings
     */
    std::string addSettings(GUISUMOAbstractView* view=0) throw();

    /** @brief Sets the viewport which has been parsed
     *
     * @param[in] parent the view for which the viewport has to be set
     */
    void setViewport(GUISUMOAbstractView* view) throw();

    /** @brief Makes a snapshot if it has been parsed
     *
     * @param[in] parent the view which needs to be shot
     */
    void setSnapshots(GUISUMOAbstractView* view) throw();

    /** @brief Returns whether any decals have been parsed
     *
     * @return whether decals have been parsed
     */
    bool hasDecals() throw();

    /** @brief Returns the parsed decals
     *
     * @return the parsed decals
     */
    std::vector<GUISUMOAbstractView::Decal>& getDecals() throw();

private:
    /// @brief The settings to fill
    GUIVisualizationSettings mySettings;
    /// @brief The viewport loaded
    SUMOReal myZoom, myXPos, myYPos;
    /// @brief mappig of time steps to filenames for potential snapshots
    std::map<SUMOTime, std::string> mySnapshots;
    /// @brief The decals list to fill
    std::vector<GUISUMOAbstractView::Decal> myDecals;
    /// @brief The last color scheme category (edges or vehicles)
    SumoXMLTag myCurrentColorer;
    /// @brief The current color scheme
    GUIColorScheme* myCurrentScheme;
    /// @brief The current color scheme entry position
    unsigned int myCurrentSchemePos;

};

#endif

/****************************************************************************/
