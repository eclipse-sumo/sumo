/****************************************************************************/
/// @file    NIVissimTL.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIVissimTL_h
#define NIVissimTL_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


#include <map>
#include <string>
#include <vector>
#include <utils/geom/PositionVector.h>
#include <utils/geom/AbstractPoly.h>
#include <utils/common/SUMOTime.h>


class NBTrafficLightLogicCont;



// ===========================================================================
// class declarations
// ===========================================================================
class NBLoadedTLDef;
class NBEdgeCont;

class NIVissimTL {
public:
    NIVissimTL(int id, const std::string& type, const std::string& name,
               SUMOTime absdur, SUMOTime offset);
    ~NIVissimTL();
//    void computeBounding();
    std::string getType() const;
    int getID() const;

public:
    static bool dictionary(int id, const std::string& type,
                           const std::string& name, SUMOTime absdur, SUMOTime offset);
    static bool dictionary(int id, NIVissimTL* o);
    static NIVissimTL* dictionary(int id);
//    static std::vector<int> getWithin(const AbstractPoly &poly, SUMOReal offset);
    static void clearDict();
    static bool dict_SetSignals(NBTrafficLightLogicCont& tlc,
                                NBEdgeCont& ec);

public:
    class NIVissimTLSignal;
    class NIVissimTLSignalGroup;
    typedef std::map<int, NIVissimTLSignal*> SSignalDictType;
    typedef std::map<int, NIVissimTLSignalGroup*> SGroupDictType;
    typedef std::map<int, SSignalDictType> SignalDictType;
    typedef std::map<int, SGroupDictType> GroupDictType;

    /**
     *
     */
    class NIVissimTLSignal {
    public:
        NIVissimTLSignal(int lsaid, int id, const std::string& name,
                         const std::vector<int>& groupids, int edgeid, int laneno,
                         SUMOReal position, const std::vector<int>& assignedVehicleTypes);
        ~NIVissimTLSignal();
        bool isWithin(const PositionVector& poly) const;
        Position getPosition() const;
        bool addTo(NBEdgeCont& ec, NBLoadedTLDef* node) const;

    public:
        static bool dictionary(int lsaid, int id, NIVissimTLSignal* o);
        static NIVissimTLSignal* dictionary(int lsaid, int id);
        static void clearDict();
        static SSignalDictType getSignalsFor(int tlid);

    protected:
        int myLSA;
        int myID;
        std::string myName;
        std::vector<int> myGroupIDs;
        int myEdgeID;
        int myLane;
        SUMOReal myPosition;
        std::vector<int> myVehicleTypes;
        static SignalDictType myDict;
    };

    class NIVissimTLSignalGroup {
    public:
        NIVissimTLSignalGroup(int lsaid, int id, const std::string& name,
                              bool isGreenBegin, const std::vector<SUMOReal>& times,
                              SUMOTime tredyellow, SUMOTime tyellow);
        ~NIVissimTLSignalGroup();
        bool addTo(NBLoadedTLDef* node) const;
    public:
        static bool dictionary(int lsaid, int id, NIVissimTLSignalGroup* o);
        static NIVissimTLSignalGroup* dictionary(int lsaid, int id);
        static void clearDict();
        static SGroupDictType getGroupsFor(int tlid);

    private:
        int myLSA;
        int myID;
        std::string myName;
        std::vector<SUMOReal> myTimes;
        bool myFirstIsRed;
        SUMOTime myTRedYellow, myTYellow;
        static GroupDictType myDict;
    };

protected:
    int myID;
    std::string myName;
    SUMOTime myAbsDuration;
    SUMOTime myOffset;
    NIVissimTLSignalGroup* myCurrentGroup;
    std::string myType;
private:
    typedef std::map<int, NIVissimTL*> DictType;
    static DictType myDict;
};


#endif

/****************************************************************************/

