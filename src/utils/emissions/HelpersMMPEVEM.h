/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    HelpersMMPEVEM.h
/// @author  Kevin Badalian (badalian_k@mmp.rwth-aachen.de)
/// @date    2021-10
///
// The MMP's emission model for electric vehicles.
// If you use this model for academic research, you are highly encouraged to
// cite our paper "Accurate physics-based modeling of electric vehicle energy
// consumption in the SUMO traffic microsimulator"
// (DOI: 10.1109/ITSC48978.2021.9564463).
// Teaching and Research Area Mechatronics in Mobile Propulsion (MMP), RWTH Aachen
/****************************************************************************/


#pragma once


#include <utils/emissions/PollutantsInterface.h>
#include <utils/emissions/EnergyParams.h>

#include <map>




/**
 * \class HelpersMMPEVEM
 * \brief This helper class allows the PollutantsInterface to load and use
 *        different MMPEVEMs.
 */
class HelpersMMPEVEM : public PollutantsInterface::Helper {
private:
    static const int MMPEVEM_BASE = 5 << 16;


public:
    /**
     * \brief Constructor
     */
    HelpersMMPEVEM();

    /**
     * \brief Compute the amount of emitted pollutants for an emission class in a
     *        given state.
     *
     * This method returns 0 for all emission types but electric power
     * consumption.
     *
     * \param[in] c An emission class
     * \param[in] e An emission type
     * \param[in] v Current vehicle velocity [m/s]
     * \param[in] a Current acceleration of the vehicle [m/s^2]
     * \param[in] slope Slope of the road at the vehicle's current position [deg]
     *
     * \returns The electric power consumption [Wh/s] or 0 for all other emission
     *          types
     */
    double compute(const SUMOEmissionClass /* c */,
                   const PollutantsInterface::EmissionType e, const double v,
                   const double a, const double slope,
                   const EnergyParams* ptr_energyParams) const;
};

