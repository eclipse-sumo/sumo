#ifndef NIVissimTL_h
#define NIVissimTL_h
//---------------------------------------------------------------------------//
//                        NIVissimTL.h -  ccc
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
// Revision 1.7  2003/06/16 08:01:57  dkrajzew
// further work on Vissim-import
//
// Revision 1.6  2003/06/05 11:46:57  dkrajzew
// class templates applied; documentation added
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include <map>
#include <string>
#include <vector>
#include <utils/geom/Position2DVector.h>
#include <utils/geom/AbstractPoly.h>
#include <utils/common/IntVector.h>
#include <utils/common/DoubleVector.h>
#include "NIVissimBoundedClusterObject.h"

/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBTrafficLightDefinition;

class NIVissimTL
      /*  : public NIVissimBoundedClusterObject */{
public:
    NIVissimTL(int id, const std::string &type, const std::string &name,
        double absdur, double offset);
    ~NIVissimTL();
//    void computeBounding();
    std::string getType() const;
    int getID() const;

public:
    static bool dictionary(int id, const std::string &type,
        const std::string &name, double absdur, double offset);
    static bool dictionary(int id, NIVissimTL *o);
    static NIVissimTL *dictionary(int id);
//    static IntVector getWithin(const AbstractPoly &poly, double offset);
    static void clearDict();
    static bool dict_SetSignals();

public:
    class NIVissimTLSignal;
    class NIVissimTLSignalGroup;
    typedef std::map<int, NIVissimTLSignal *> SSignalDictType;
    typedef std::map<int, NIVissimTLSignalGroup *> SGroupDictType;
    typedef std::map<int, SSignalDictType> SignalDictType;
    typedef std::map<int, SGroupDictType> GroupDictType;

/**
 *
 */
    class NIVissimTLSignal {
    public:
        NIVissimTLSignal(int lsaid, int id, const std::string &name,
            const IntVector &groupids, int edgeid, int laneno,
            double position, const IntVector &assignedVehicleTypes);
        ~NIVissimTLSignal();
        bool isWithin(const Position2DVector &poly) const;
        Position2D getPosition() const;
        bool addTo(NBTrafficLightDefinition *node) const;

    public:
        static bool dictionary(int lsaid, int id, NIVissimTLSignal *o);
        static NIVissimTLSignal *dictionary(int lsaid, int id);
        static void clearDict();
        static SSignalDictType getSignalsFor(int tlid);

    protected:
        int myLSA;
        int myID;
        std::string myName;
        IntVector myGroupIDs;
        int myEdgeID;
        int myLane;
        double myPosition;
        IntVector myVehicleTypes;
        static SignalDictType myDict;
    };

    class NIVissimTLSignalGroup {
    public:
        NIVissimTLSignalGroup(int lsaid, int id, const std::string &name,
            bool isGreenBegin, const DoubleVector &times,
            double tredyellow, double tyellow);
        ~NIVissimTLSignalGroup();
        bool addTo(NBTrafficLightDefinition *node) const;
    public:
        static bool dictionary(int lsaid, int id, NIVissimTLSignalGroup *o);
        static NIVissimTLSignalGroup *dictionary(int lsaid, int id);
        static void clearDict();
        static SGroupDictType getGroupsFor(int tlid);

    private:
        int myLSA;
        int myID;
        std::string myName;
        DoubleVector myTimes;
        bool myFirstIsRed;
        double myTRedYellow, myTYellow;
        static GroupDictType myDict;
    };

protected:
    int myID;
    std::string myName;
    double myAbsDuration;
    double myOffset;
    NIVissimTLSignalGroup *myCurrentGroup;
    std::string myType;
private:
    typedef std::map<int, NIVissimTL*> DictType;
    static DictType myDict;
};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NIVissimTL.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

