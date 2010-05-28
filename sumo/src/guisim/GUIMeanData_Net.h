/****************************************************************************/
/// @file    GUIMeanData_Net.h
/// @author  Michael Behrisch
/// @date    Wed, 27.05.2010
/// @version $Id: GUIMeanData_Net.h 8236 2010-02-10 11:16:41Z behrisch $
///
// GUI-version of the meandata collector
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIMeanData_Net_h
#define GUIMeanData_Net_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <set>
#include <microsim/output/MSMeanData_Net.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIMainWindow;
class GUISUMOAbstractView;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIMeanData_Net
 * @brief GUI-version of the meandata collector
 *
 * This gui-version of a bus-stop extends MSMeanData_Net by methods for visual
 *  tracking of individual values.
 *
 * @see MSMeanData_Net
 */
class GUIMeanData_Net : public MSMeanData_Net {
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the detector
     * @param[in] dumpBegin Begin time of dump
     * @param[in] dumpEnd End time of dump
     * @param[in] useLanes Information whether lane-based or edge-based dump shall be generated
     * @param[in] withEmpty Information whether empty lanes/edges shall be written
     * @param[in] trackVehicles Information whether vehicles shall be tracked
     * @param[in] maxTravelTime the maximum travel time to output
     * @param[in] minSamples the minimum number of sample seconds before the values are valid
     * @param[in] haltSpeed the maximum speed to consider a vehicle waiting
     * @param[in] vTypes the set of vehicle types to consider
     */
    GUIMeanData_Net(const std::string &id,
                    const SUMOTime dumpBegin, const SUMOTime dumpEnd,
                    const bool useLanes, const bool withEmpty,
                    const bool trackVehicles,
                    const SUMOReal maxTravelTime, const SUMOReal minSamples,
                    const SUMOReal haltSpeed, const std::set<std::string> vTypes) throw();


    /** @brief Destructor */
    ~GUIMeanData_Net() throw();


    /** @brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow *getParameterWindow(GUIMainWindow &app,
            GUISUMOAbstractView &parent) throw();


    /** @brief Returns the total number of entered vehicles
     *
     * @return The total number of entered vehicles
     */
    unsigned int getTotalEntered() throw();


private:
    /// @brief The rotation of the sign
    SUMOReal myFGSignRot;


};


#endif

/****************************************************************************/
