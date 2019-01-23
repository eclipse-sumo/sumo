/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimTL.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
#ifndef NIVissimTL_h
#define NIVissimTL_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


#include <map>
#include <string>
#include <vector>
#include <utils/geom/PositionVector.h>
#include <utils/geom/AbstractPoly.h>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NBTrafficLightLogicCont;
class NBLoadedTLDef;
class NBEdgeCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIVissimTL
 */
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
//    static std::vector<int> getWithin(const AbstractPoly &poly, double offset);
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
        NIVissimTLSignal(int id, const std::string& name,
                         const std::vector<int>& groupids, int edgeid, int laneno,
                         double position, const std::vector<int>& assignedVehicleTypes);
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
        int myID;
        std::string myName;
        std::vector<int> myGroupIDs;
        int myEdgeID;
        int myLane;
        double myPosition;
        std::vector<int> myVehicleTypes;
        static SignalDictType myDict;
    };

    class NIVissimTLSignalGroup {
    public:
        NIVissimTLSignalGroup(int id, const std::string& name,
                              bool isGreenBegin, const std::vector<double>& times,
                              SUMOTime tredyellow, SUMOTime tyellow);
        ~NIVissimTLSignalGroup();
        bool addTo(NBLoadedTLDef* node) const;
    public:
        static bool dictionary(int lsaid, int id, NIVissimTLSignalGroup* o);
        static NIVissimTLSignalGroup* dictionary(int lsaid, int id);
        static void clearDict();
        static SGroupDictType getGroupsFor(int tlid);

    private:
        int myID;
        std::string myName;
        std::vector<double> myTimes;
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

