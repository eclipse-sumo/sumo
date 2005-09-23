#ifndef NIVIssimElements_h
#define NIVIssimElements_h
//---------------------------------------------------------------------------//
//                        NIVissimElements.h -  ccc
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.4  2005/09/23 06:02:56  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.3  2005/04/27 12:24:36  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.2  2003/06/05 11:46:54  dkrajzew
// class templates applied; documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H


enum NIVissimElement
{
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
    VE_Fahrverhaltendefinition,
    VE_Fahrtverlaufdateien,
    VE_Emission,
    VE_Einheitendefinition,
    VE_Streckentypdefinition,
    VE_Kantensperrung,
    VE_DUMMY
};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

