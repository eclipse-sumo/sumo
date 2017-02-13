/****************************************************************************/
/// @file    GNEFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
/// @version $Id$
///
/// Abstract class for lateral frames in NetEdit
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNEFrame_h
#define GNEFrame_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBTrafficLightLogic.h>
#include <utils/foxtools/FXRealSpinDial.h>
#include <utils/gui/div/GUISelectedStorage.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>

// ===========================================================================
// class declarations
// ===========================================================================
class GNEViewNet;
class GNEUndoList;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEFrame
 * Abstract class for lateral frames in NetEdit
 */
class GNEFrame : public FXVerticalFrame {
public:
    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     * @brief frameLabel label of the frame
     */
    GNEFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet, const std::string& frameLabel);

    /// @brief destructor
    ~GNEFrame();

    /**@brief show Frame
     * @note some GNEFrames needs a re-implementation
     */
    virtual void show();

    /**@brief hide Frame
     * @note some GNEFrames needs a re-implementation
     */
    virtual void hide();

    /// @brief set width of GNEFrame
    void setFrameWidth(int width);

    /// @brief get view net
    GNEViewNet* getViewNet() const;

    /// @brief get the label for the frame's header
    FXLabel* getFrameHeaderLabel() const;

    /// @brief get font of the header's frame
    FXFont* getFrameHeaderFont() const;

protected:
    /// @brief FOX needs this
    GNEFrame() {}

    /// @brief @brief the window to inform when the tls is modfied
    GNEViewNet* myViewNet;

    /// @brief Font for the Header
    FXFont* myFrameHeaderFont;

    /// @brief the label for the frame's header
    FXLabel* myFrameHeaderLabel;

    /// @brief Vertical frame that holds all widgets of frame
    FXVerticalFrame* myContentFrame;

    /// @brief fame for header elements
    FXHorizontalFrame* myHeaderFrame;

    /// @brief fame for left header elements
    FXHorizontalFrame* myHeaderLeftFrame;

    /// @brief fame for right header elements
    FXHorizontalFrame* myHeaderRightFrame;

private:
    /// @brief scroll windows that holds the content frame
    FXScrollWindow* myScrollWindowsContents;

    /// @brief Invalidated copy constructor.
    GNEFrame(const GNEFrame&);

    /// @brief Invalidated assignment operator.
    GNEFrame& operator=(const GNEFrame&);
};


#endif

/****************************************************************************/
