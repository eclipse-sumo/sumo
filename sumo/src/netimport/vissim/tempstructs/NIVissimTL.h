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
// Revision 1.11  2005/09/23 06:02:57  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.10  2005/04/27 12:24:37  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.9  2003/07/07 08:28:48  dkrajzew
// adapted the importer to the new node type description; some further work
//
// Revision 1.8  2003/06/18 11:35:29  dkrajzew
// message subsystem changes applied and some further work done; seems to be stable but is not perfect, yet
//
// Revision 1.7  2003/06/16 08:01:57  dkrajzew
// further work on Vissim-import
//
// Revision 1.6  2003/06/05 11:46:57  dkrajzew
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


#include <map>
#include <string>
#include <vector>
#include <utils/geom/Position2DVector.h>
#include <utils/geom/AbstractPoly.h>
#include <utils/common/IntVector.h>
#include <utils/common/DoubleVector.h>
#include <utils/common/SUMOTime.h>


class NBTrafficLightLogicCont;



/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBLoadedTLDef;
class NBEdgeCont;

class NIVissimTL
      /*  : public NIVissimBoundedClusterObject */{
public:
    NIVissimTL(int id, const std::string &type, const std::string &name,
        SUMOTime absdur, SUMOTime offset);
    ~NIVissimTL();
//    void computeBounding();
    std::string getType() const;
    int getID() const;

public:
    static bool dictionary(int id, const std::string &type,
        const std::string &name, SUMOTime absdur, SUMOTime offset);
    static bool dictionary(int id, NIVissimTL *o);
    static NIVissimTL *dictionary(int id);
//    static IntVector getWithin(const AbstractPoly &poly, SUMOReal offset);
    static void clearDict();
    static bool dict_SetSignals(NBTrafficLightLogicCont &tlc,
        NBEdgeCont &ec);

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
            SUMOReal position, const IntVector &assignedVehicleTypes);
        ~NIVissimTLSignal();
        bool isWithin(const Position2DVector &poly) const;
        Position2D getPosition() const;
        bool addTo(NBEdgeCont &ec, NBLoadedTLDef *node) const;

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
        SUMOReal myPosition;
        IntVector myVehicleTypes;
        static SignalDictType myDict;
    };

    class NIVissimTLSignalGroup {
    public:
        NIVissimTLSignalGroup(int lsaid, int id, const std::string &name,
            bool isGreenBegin, const DoubleVector &times,
            SUMOTime tredyellow, SUMOTime tyellow);
        ~NIVissimTLSignalGroup();
        bool addTo(NBLoadedTLDef *node) const;
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
        SUMOTime myTRedYellow, myTYellow;
        static GroupDictType myDict;
    };

protected:
    int myID;
    std::string myName;
    SUMOTime myAbsDuration;
    SUMOTime myOffset;
    NIVissimTLSignalGroup *myCurrentGroup;
    std::string myType;
private:
    typedef std::map<int, NIVissimTL*> DictType;
    static DictType myDict;
};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

