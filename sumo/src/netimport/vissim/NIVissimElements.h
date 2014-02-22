/****************************************************************************/
/// @file    NIVissimElements.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
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
#ifndef NIVissimElements_h
#define NIVissimElements_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


enum NIVissimElement {
    VE_Kennungszeile,
    VE_Startzufallszahl,
    VE_Simdauer,
    VE_Startuhrzeit,
    VE_SimRate,
    VE_Zeitschrittfaktor,
    VE_Linksverkehr,
    VE_DynUml,
    VE_Stauparameterdefinition,
    VE_Gelbverhaltendefinition,
    VE_Streckendefinition,
    VE_Verbindungsdefinition,
    VE_Richtungsentscheidungsdefinition,
    VE_Routenentscheidungsdefinition,
    VE_VWunschentscheidungsdefinition,
    VE_Langsamfahrbereichdefinition,
    VE_Zuflussdefinition,
    VE_Fahrzeugtypdefinition,
    VE_Fahrzeugklassendefinition,
    VE_Verkehrszusammensetzungsdefinition,
    VE_Geschwindigkeitsverteilungsdefinition,
    VE_Laengenverteilungsdefinition,
    VE_Zeitenverteilungsdefinition,
    VE_Baujahrverteilungsdefinition,
    VE_Laufleistungsverteilungsdefinition,
    VE_Massenverteilungsdefinition,
    VE_Leistungsverteilungsdefinition,
    VE_Maxbeschleunigungskurvedefinition,
    VE_Wunschbeschleunigungskurvedefinition,
    VE_Maxverzoegerungskurvedefinition,
    VE_Wunschverzoegerungskurvedefinition,
    VE_Querverkehrsstoerungsdefinition,
    VE_Lichtsignalanlagendefinition,
    VE_Signalgruppendefinition,
    VE_Signalgeberdefinition,
    VE_LSAKopplungdefinition,
    VE_Detektorendefinition,
    VE_Haltestellendefinition,
    VE_Liniendefinition,
    VE_Stopschilddefinition,
    VE_Messungsdefinition,
    VE_Reisezeitmessungsdefinition,
    VE_Verlustzeitmessungsdefinition,
    VE_Querschnittsmessungsdefinition,
    VE_Stauzaehlerdefinition,
    VE_Auswertungsdefinition,
    VE_Fensterdefinition,
    VE_Gefahrenwarnsystemdefinition,
    VE_Parkplatzdefinition,
    VE_Knotendefinition,
    VE_TEAPACdefinition,
    VE_Netzobjektdefinition,
    VE_Richtungspfeildefinition,
    VE_Rautedefinition,
    VE_Fahrverhaltendefinition,
    VE_Fahrtverlaufdateien,
    VE_Emission,
    VE_Einheitendefinition,
    VE_Streckentypdefinition,
    VE_Kantensperrung,
    VE_DUMMY
};


#endif

/****************************************************************************/

