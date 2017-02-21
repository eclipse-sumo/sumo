/****************************************************************************/
/// @file    GNEDialog_AllowDisallow.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2017
/// @version $Id$
///
/// Dialog for edit rerouters
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
#ifndef GNEDialog_AllowDisallow_h
#define GNEDialog_AllowDisallow_h

// ===========================================================================
// included modules
// ===========================================================================

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/SUMOVehicleClass.h>

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEDialog_AllowDisallow
 * @brief Dialog for edit rerouters
 */
class GNEDialog_AllowDisallow : public FXDialogBox {
    /// @brief FOX-declaration
    FXDECLARE(GNEDialog_AllowDisallow)

public:
    /// @brief Constructor
    GNEDialog_AllowDisallow(FXApp* app, std::string *allow);

    /// @brief destructor
    ~GNEDialog_AllowDisallow();

    /// @name FOX-callbacks
    /// @{
    /// @brief event when user press a enable/disable button
    long onCmdValueChanged(FXObject*, FXSelector, void*);

    /// @brief event when user press select all VClasses button
    long onCmdSelectAll(FXObject*, FXSelector, void*);

    /// @brief event when user press unselect all VClasses button
    long onCmdUnselectAll(FXObject*, FXSelector, void*);

    /// @brief event when user press select only non road button
    long onCmdSelectOnlyNonRoad(FXObject*, FXSelector, void*);

    /// @brief event after press accept button
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief event after press cancel button
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief event after press reset button
    long onCmdReset(FXObject*, FXSelector, void*);
    /// @}

protected:
    /// @brief FOX needs this
    GNEDialog_AllowDisallow() {}

    /// @brief pointer to current allowed vehicles
    std::string *myAllow;
    
    /// @brief copy of current allowed vehicles
    std::string myCopyOfAllow;

    /// @brief select all VClass
    FXButton* mySelectAllVClassButton;

    /// @brief unselect all VCLass
    FXButton* myUnselectAllVClassButton;

    /// @brief select only non-road vehicles
    FXButton* mySelectOnlyNonRoadVClassButton;

    /// @brief accept button
    FXButton* myAcceptButton;

    /// @brief cancel button
    FXButton* myCancelButton;

    /// @brief cancel button
    FXButton* myResetButton;

    /// @brief map with the buttons for every VClass
    std::map<SUMOVehicleClass, std::pair<FXButton*, FXLabel*> > myVClassMap;

private:
    /// @brief Invalidated copy constructor.
    GNEDialog_AllowDisallow(const GNEDialog_AllowDisallow&);

    /// @brief Invalidated assignment operator.
    GNEDialog_AllowDisallow& operator=(const GNEDialog_AllowDisallow&);

    /// @name build VClass element
    /// @{
    /// @brief build VClass of type private
	void buildVClassPrivate(FXVerticalFrame *contentsFrame);

    /// @brief build VClass of type emergency
    void buildVClassEmergency(FXVerticalFrame *contentsFrame);

    /// @brief build VClass of type authority
    void buildVClassAuthority(FXVerticalFrame *contentsFrame);

    /// @brief build VClass of type army
    void buildVClassArmy(FXVerticalFrame *contentsFrame);

    /// @brief build VClass of type vip
    void buildVClassVip(FXVerticalFrame *contentsFrame);

    /// @brief build VClass of type passenger
    void buildVClassPassenger(FXVerticalFrame *contentsFrame);

    /// @brief build VClass of type hov
    void buildVClassHov(FXVerticalFrame *contentsFrame);

    /// @brief build VClass of type taxi
    void buildVClassTaxi(FXVerticalFrame *contentsFrame);

    /// @brief build VClass of type bus
    void buildVClassBus(FXVerticalFrame *contentsFrame);

    /// @brief build VClass of type coach
    void buildVClassCoach(FXVerticalFrame *contentsFrame);

    /// @brief build VClass of type delivery
    void buildVClassDelivery(FXVerticalFrame *contentsFrame);

    /// @brief build VClass of type truck
    void buildVClassTruck(FXVerticalFrame *contentsFrame);

    /// @brief build VClass of type trailer
    void buildVClassTrailer(FXVerticalFrame *contentsFrame);

    /// @brief build VClass of type tram
    void buildVClassTram(FXVerticalFrame *contentsFrame);

    /// @brief build VClass of type rail urban
    void buildVClassRailUrban(FXVerticalFrame *contentsFrame);

    /// @brief build VClass of type rail
    void buildVClassRail(FXVerticalFrame *contentsFrame);

    /// @brief build VClass of type rail electric
    void buildVClassRailElectric(FXVerticalFrame *contentsFrame);

    /// @brief build VClass of type motorcylce
    void buildVClassMotorcycle(FXVerticalFrame *contentsFrame);

    /// @brief build VClass of type moped
    void buildVClassMoped(FXVerticalFrame *contentsFrame);

    /// @brief build VClass of type bicycle
    void buildVClassBicycle(FXVerticalFrame *contentsFrame);

    /// @brief build VClass of type pedestrian
    void buildVClassPedestrian(FXVerticalFrame *contentsFrame);

    /// @brief build VClass of type electric vehicle
    void buildVClassEVehicle(FXVerticalFrame *contentsFrame);

    /// @brief build VClass of type ship
    void buildVClassShip(FXVerticalFrame *contentsFrame);

    /// @brief build VClass of type custom 1
    void buildVClassCustom1(FXVerticalFrame *contentsFrame);

    /// @brief build VClass of type custom 2
    void buildVClassCustom2(FXVerticalFrame *contentsFrame);
    /// @}
};

#endif
