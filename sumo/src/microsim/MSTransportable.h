/****************************************************************************/
/// @file    MSTransportable.h
/// @author  Michael Behrisch
/// @date    Tue, 21 Apr 2015
/// @version $Id$
///
// The common superclass for modelling transportable objects like persons and containers
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSTransportable_h
#define MSTransportable_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/vehicle/SUMOVehicleParameter.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicleType;


// ===========================================================================
// class definitions
// ===========================================================================
/**
  * @class MSTransportable
  *
  * The class holds a simulated moveable object
  */
class MSTransportable {
public:
    /// constructor
    MSTransportable(const SUMOVehicleParameter* pars, const MSVehicleType* vtype)
        : myParameter(pars), myVType(vtype) {}

    /// destructor
    virtual ~MSTransportable() {
        delete myParameter;
    }

    /// returns the id of the transportable
    inline const std::string& getID() const {
        return myParameter->id;
    }

    inline const SUMOVehicleParameter& getParameter() const {
        return *myParameter;
    }

    inline const MSVehicleType& getVehicleType() const {
        return *myVType;
    }

protected:
    /// the plan of the container
    const SUMOVehicleParameter* myParameter;

    /// @brief This container's type. (mainly used for drawing related information
    /// Note sure if it is really necessary
    const MSVehicleType* myVType;

    /// @brief Whether events shall be written
    bool myWriteEvents;

private:
    /// @brief Invalidated copy constructor.
    MSTransportable(const MSTransportable&);

    /// @brief Invalidated assignment operator.
    MSTransportable& operator=(const MSTransportable&);

};


#endif

/****************************************************************************/
