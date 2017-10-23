/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
/****************************************************************************/
/// @file    GNECalibratorEdge.h
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2017
/// @version $Id$
///
//
/****************************************************************************/
#ifndef GNECalibratorEdge_h
#define GNECalibratorEdge_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNECalibrator.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class GNECalibratorEdge
* class for represent Calibratos in netedit
*/
class GNECalibratorEdge : public GNECalibrator {
public:

    /**@brief Constructor
    * @param[in] id The storage of gl-ids to get the one for this lane representation from
    * @param[in] edge Edge of this calibrator belongs
    * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
    * @param[in] pos position of the calibrator on the edge (Currently not used)
    * @param[in] frequency the aggregation interval in which to calibrate the flows
    * @param[in] output The output file for writing calibrator information
    * @param[in] calibratorRoutes vector with the calibratorRoutes of calibrator
    * @param[in] calibratorFlows vector with the calibratorFlows of calibrator
    * @param[in] calibratorVehicleTypes vector with the CalibratorVehicleType of calibrator
    */
    GNECalibratorEdge(const std::string& id, GNEEdge* edge, GNEViewNet* viewNet, double pos, double frequency, const std::string& output,
                      const std::vector<GNECalibratorRoute>& calibratorRoutes, const std::vector<GNECalibratorFlow*>& calibratorFlows,
                      const std::vector<GNECalibratorVehicleType>& calibratorVehicleTypes);

    /// @brief Destructor
    ~GNECalibratorEdge();

private:
    /// @brief Invalidated copy constructor.
    GNECalibratorEdge(const GNECalibratorEdge&) = delete;

    /// @brief Invalidated assignment operator.
    GNECalibratorEdge& operator=(const GNECalibratorEdge&) = delete;
};

#endif
/****************************************************************************/
