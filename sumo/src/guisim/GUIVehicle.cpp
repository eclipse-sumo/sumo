//---------------------------------------------------------------------------//
//                        GUIVehicle.cpp -
//  A MSVehicle extended by some values for usage within the gui
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.34  2005/12/01 07:33:44  dkrajzew
// introducing bus stops: eased building vehicles; vehicles may now have nested elements
//
// Revision 1.33  2005/10/07 11:37:17  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.32  2005/09/22 13:39:35  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.31  2005/09/15 11:06:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.30  2005/05/04 08:05:24  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.29  2005/02/01 10:10:39  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.28  2005/01/27 14:20:56  dkrajzew
// code beautifying
//
// Revision 1.27  2004/12/16 12:20:09  dkrajzew
// debugging
//
// Revision 1.26  2004/11/24 08:46:43  dkrajzew
// recent changes applied
//
// Revision 1.25  2004/07/02 08:54:11  dkrajzew
// some design issues
//
// Revision 1.24  2004/04/02 11:20:35  dkrajzew
// changes needed to visualise the selection status
//
// Revision 1.23  2004/03/19 12:57:55  dkrajzew
// porting to FOX
//
// Revision 1.22  2004/01/26 15:53:21  dkrajzew
// added some yet unset display variables
//
// Revision 1.21  2004/01/26 07:00:50  dkrajzew
// reinserted the building of repeating vehicles
//
// Revision 1.20  2003/11/20 13:06:30  dkrajzew
// loading and using of predefined vehicle colors added
//
// Revision 1.19  2003/11/12 13:59:04  dkrajzew
// redesigned some classes by changing them to templates
//
// Revision 1.18  2003/11/11 08:11:05  dkrajzew
// logging (value passing) moved from utils to microsim
//
// Revision 1.17  2003/10/31 08:02:31  dkrajzew
// hope to have patched false usage of RAND_MAX when using gcc
//
// Revision 1.16  2003/10/22 07:07:06  dkrajzew
// patching of lane states on force vehicle removal added
//
// Revision 1.15  2003/08/14 13:47:44  dkrajzew
// false usage of function-pointers patched; false inclusion of
//  .moc-files removed
//
// Revision 1.14  2003/08/04 11:35:52  dkrajzew
// only GUIVehicles need a color definition; process of building cars changed
//
// Revision 1.13  2003/07/30 08:54:14  dkrajzew
// the network is capable to display the networks state, now
//
// Revision 1.12  2003/07/22 14:59:27  dkrajzew
// changes due to new detector handling
//
// Revision 1.11  2003/06/18 12:54:19  dkrajzew
// has to reapply a changed setting of table parameter
//
// Revision 1.10  2003/06/18 11:30:26  dkrajzew
// debug outputs now use a DEBUG_OUT macro instead of cout; this shall ease
//  the search for further couts which must be redirected to the messaaging
//  subsystem
//
// Revision 1.9  2003/06/06 10:29:24  dkrajzew
// new subfolder holding popup-menus was added due to link-dependencies under
//  linux; QGLObjectPopupMenu*-classes were moved to "popup"
//
// Revision 1.8  2003/06/05 06:29:50  dkrajzew
// first tries to build under linux: warnings removed; moc-files included
//  Makefiles added
//
// Revision 1.7  2003/05/20 09:26:57  dkrajzew
// data retrieval for new views added
//
// Revision 1.6  2003/04/14 08:27:17  dkrajzew
// new globject concept implemented
//
// Revision 1.5  2003/04/09 15:32:28  dkrajzew
// periodical vehicles must have a period over zero now to be reasserted
//
// Revision 1.4  2003/03/20 17:31:41  dkrajzew
// StringUtils moved from utils/importio to utils/common
//
// Revision 1.3  2003/03/20 16:19:28  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.2  2003/02/07 10:39:17  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <cmath>
#include <vector>
#include <string>
#include <utils/common/StringUtils.h>
#include <microsim/MSVehicle.h>
#include "GUINet.h"
#include "GUIVehicle.h"
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <microsim/logging/CastingFunctionBinding.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/MSVehicleControl.h>
#include <utils/foxtools/MFXMenuHeader.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <gui/GUIViewTraffic.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member variables
 * ======================================================================= */
RGBColor GUIVehicle::_laneChangeColor1;
RGBColor GUIVehicle::_laneChangeColor2;


/* =========================================================================
 * FOX callback mapping
 * ======================================================================= */
FXDEFMAP(GUIVehicle::GUIVehiclePopupMenu) GUIVehiclePopupMenuMap[]=
{
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_ALLROUTES, GUIVehicle::GUIVehiclePopupMenu::onCmdShowAllRoutes),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_ALLROUTES, GUIVehicle::GUIVehiclePopupMenu::onCmdHideAllRoutes),
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_CURRENTROUTE, GUIVehicle::GUIVehiclePopupMenu::onCmdShowCurrentRoute),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_CURRENTROUTE, GUIVehicle::GUIVehiclePopupMenu::onCmdHideCurrentRoute),
};

// Object implementation
FXIMPLEMENT(GUIVehicle::GUIVehiclePopupMenu, GUIGLObjectPopupMenu, GUIVehiclePopupMenuMap, ARRAYNUMBER(GUIVehiclePopupMenuMap))


/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * GUIVehicle::GUIVehiclePopupMenu - methods
 * ----------------------------------------------------------------------- */
GUIVehicle::GUIVehiclePopupMenu::GUIVehiclePopupMenu(
        GUIMainWindow &app, GUISUMOAbstractView &parent,
        GUIGlObject &o)
    : GUIGLObjectPopupMenu(app, parent, o)
{
}


GUIVehicle::GUIVehiclePopupMenu::~GUIVehiclePopupMenu()
{
}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdShowAllRoutes(FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_VEHICLE);
    static_cast<GUIViewTraffic*>(myParent)->showRoute(
        static_cast<GUIVehicle*>(myObject), -1);
    return 1;
}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdHideAllRoutes(FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_VEHICLE);
    static_cast<GUIViewTraffic*>(myParent)->hideRoute(
        static_cast<GUIVehicle*>(myObject), -1);
    return 1;
}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdShowCurrentRoute(FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_VEHICLE);
    static_cast<GUIViewTraffic*>(myParent)->showRoute(
        static_cast<GUIVehicle*>(myObject), 0);
    return 1;
}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdHideCurrentRoute(FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_VEHICLE);
    static_cast<GUIViewTraffic*>(myParent)->hideRoute(
        static_cast<GUIVehicle*>(myObject), 0);
    return 1;
}

//ofstream stepOut("step.txt");

#ifdef NETWORKING_BLA
ofstream networking_endOut("end.txt");
ofstream networking_stepOut("step.txt");
ofstream networking_knownOut("known.txt");
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>
#endif

/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIVehicle::GUIVehicle( GUIGlObjectStorage &idStorage,
                       std::string id, MSRoute* route,
                       SUMOTime departTime,
                       const MSVehicleType* type,
                       size_t noMeanData,
                       int repNo, int repOffset)
    : MSVehicle(id, route, departTime, type, noMeanData, repNo, repOffset),
    GUIGlObject(idStorage, string("vehicle:")+id)
#ifdef NETWORKING_BLA
    ,networking_globalConns(0)
#endif
{
    // compute both random colors
    //  color1
    long prod = 1;
    for(size_t i=0; i<id.length(); i++) {
        prod *= (int) id.at(i);
        if(prod>(1<<24)) {
            prod /= 128;
        }
    }
    _randomColor1 = RGBColor(
        (SUMOReal) (256-(prod & 255)) / (SUMOReal) 255,
        (SUMOReal) (256-((prod>>8) & 255)) / (SUMOReal) 255,
        (SUMOReal) (256-((prod>>16) & 255)) / (SUMOReal) 255);
    // color2
    _randomColor2 = RGBColor(
        (SUMOReal)rand() / ( static_cast<SUMOReal>(RAND_MAX) + 1),
        (SUMOReal)rand() / ( static_cast<SUMOReal>(RAND_MAX) + 1),
        (SUMOReal)rand() / ( static_cast<SUMOReal>(RAND_MAX) + 1));
    // lane change color (static!!!)
    _laneChangeColor1 = RGBColor(1, 1, 1);
    _laneChangeColor2 = RGBColor((SUMOReal) 0.7, (SUMOReal) 0.7, (SUMOReal) 0.7);

#ifdef NETWORKING_BLA
    SUMOReal prob = OptionsSubSys::getOptions().getFloat("device");
    if(prob>(SUMOReal) rand() / (SUMOReal) RAND_MAX) {
        networking_HaveDevice = true;
    } else {
        networking_HaveDevice = false;
    }
    if(id==OptionsSubSys::getOptions().getString("knownveh")) {
        networking_HaveDevice = true;
    }

	networking_myLaneEmitTime = std::vector<int>(route->size(), -1);
	networking_EntryTime = -1;
#endif
}


#ifdef NETWORKING_BLA

    class networking_ByEdgeFinder {
    private:
        /// The wished index vector size
        MSEdge *edge;

    public:
        /** constructor */
        explicit networking_ByEdgeFinder(MSEdge *e)
            : edge(e) { }

        /** the comparing function */
        bool operator() (const GUIVehicle::networking_EdgeTimeInformation &p) {
            return p.edge==edge;
        }

    };

#endif
GUIVehicle::~GUIVehicle()
{
#ifdef NETWORKING_BLA
    if(networking_HaveDevice) {
		int known = 0;
		SUMOReal mmax = 0;
		SUMOReal mmin = 0;
		SUMOReal meanTimeDist = 0;
		SUMOReal mmax2 = 0;
		SUMOReal mmin2 = 0;
		SUMOReal meanTimeDist2 = 0;
		int i = 0;

        for(i=0; i<myRoute->size()-1; i++) {
            SUMOTime time = -1;
/*            std::vector<networking_EdgeTimeInformation>::iterator j;
            j = find_if(networking_myKnownEdges.begin(), networking_myKnownEdges.end(),
                networking_ByEdgeFinder((*myRoute)[i]));
            if(j!=networking_myKnownEdges.end()) {*/
                time = networking_myLaneEmitTime[i] ;
				if(time!=-1) {
					SUMOReal tmp = networking_myLaneEntryTime[i] - (SUMOReal) time;
					SUMOReal tmp2 = (SUMOReal) time;
					if(known==0) {
						mmin = tmp;
						mmax = tmp;
						mmin2 = tmp2;
						mmax2 = tmp2;
					} else {
						mmin = mmin < tmp ? mmin : tmp;
						mmax = mmax > tmp ? mmax : tmp;
						mmin2 = mmin2 < tmp2 ? mmin2 : tmp2;
						mmax2 = mmax2 > tmp2 ? mmax2 : tmp2;
					}
	                known++;
					meanTimeDist += networking_myLaneEntryTime[i] - (SUMOReal) time;
					meanTimeDist2 += tmp2;
				}
//            }
        }


		if(known!=0) {
			meanTimeDist /= (SUMOReal) known;
			meanTimeDist2 /= (SUMOReal) known;
		} else {
			meanTimeDist = 0;
			meanTimeDist2 = 0;
		}

        networking_endOut
            << id() << ";" << MSNet::globaltime << ";"
            << networking_myKnownEdges.size() << ";"
			<< networking_globalConns << ";"
			<< networking_mySeenGlobal.size() << ";"
			<< known << ";" << meanTimeDist << ";" << mmin << ";" << mmax
			<< ";" << networking_EntryTime << ";"
			<< meanTimeDist2 << ";" << mmin2 << ";" << mmax2
			<< endl;

    }
#endif
}


std::vector<std::string>
GUIVehicle::getNames()
{
    std::vector<std::string> ret;
    ret.reserve(MSVehicle::myDict.size());
    for(MSVehicle::DictType::iterator i=MSVehicle::myDict.begin();
        i!=MSVehicle::myDict.end(); i++) {
        MSVehicle *veh = (*i).second;
        if(veh->running()) {
            ret.push_back((*i).first);
        }
    }
    return ret;
}


std::vector<size_t>
GUIVehicle::getIDs()
{
    std::vector<size_t> ret;
    ret.reserve(MSVehicle::myDict.size());
    for(MSVehicle::DictType::iterator i=MSVehicle::myDict.begin();
        i!=MSVehicle::myDict.end(); i++) {
        MSVehicle *veh = (*i).second;
        if(veh->running()) {
            ret.push_back(static_cast<GUIVehicle*>((*i).second)->getGlID());
        }
    }
    return ret;
}


const RGBColor &
GUIVehicle::getDefinedColor() const
{
    return RGBColor(1, 1, 1); // !!!
}


const RGBColor &
GUIVehicle::getRandomColor1() const
{
    return _randomColor1;
}


const RGBColor &
GUIVehicle::getRandomColor2() const
{
    return _randomColor2;
}


int
GUIVehicle::getPassedColor() const
{
    int passed = 255 - myLastLaneChangeOffset;
    if(passed<128) {
        passed = 128;
    }
    return passed;
}


const RGBColor &
GUIVehicle::getLaneChangeColor2() const
{
    if(myLastLaneChangeOffset==0) {
        return _laneChangeColor1;
    } else {
        return _laneChangeColor2;
    }
}


MSVehicle *
GUIVehicle::getNextPeriodical() const
{
    // check whether another one shall be repated
    if(myRepetitionNumber<=0) {
        return 0;
    }
    return MSNet::getInstance()->getVehicleControl().buildVehicle(
        StringUtils::version1(myID), myRoute, myDesiredDepart+myPeriod,
        myType, myRepetitionNumber-1, myPeriod);
}


GUIGLObjectPopupMenu *
GUIVehicle::getPopUpMenu(GUIMainWindow &app,
                         GUISUMOAbstractView &parent)
{
    GUIGLObjectPopupMenu *ret = new GUIVehiclePopupMenu(app, parent, *this);
    new MFXMenuHeader(ret, app.getBoldFont(), getFullName().c_str(), 0, 0, 0);
    new FXMenuSeparator(ret);
    //
    new FXMenuCommand(ret, "Center",
        GUIIconSubSys::getIcon(ICON_RECENTERVIEW), ret, MID_CENTER);
    new FXMenuSeparator(ret);
    //
    if(gSelected.isSelected(GLO_VEHICLE, getGlID())) {
        new FXMenuCommand(ret, "Remove From Selected",
            GUIIconSubSys::getIcon(ICON_FLAG_MINUS), ret, MID_REMOVESELECT);
    } else {
        new FXMenuCommand(ret, "Add To Selected",
            GUIIconSubSys::getIcon(ICON_FLAG_PLUS), ret, MID_ADDSELECT);
    }
    if(static_cast<GUIViewTraffic&>(parent).amShowingRouteFor(this, 0)) {
        new FXMenuCommand(ret, "Hide Current Route", 0, ret, MID_HIDE_CURRENTROUTE);
    } else {
        new FXMenuCommand(ret, "Show Current Route", 0, ret, MID_SHOW_CURRENTROUTE);
    }
    if(static_cast<GUIViewTraffic&>(parent).amShowingRouteFor(this, -1)) {
        new FXMenuCommand(ret, "Hide All Routes", 0, ret, MID_HIDE_ALLROUTES);
    } else {
        new FXMenuCommand(ret, "Show All Routes", 0, ret, MID_SHOW_ALLROUTES);
    }
    new FXMenuSeparator(ret);
    //
    new FXMenuCommand(ret, "Show Parameter",
        GUIIconSubSys::getIcon(ICON_APP_TABLE), ret, MID_SHOWPARS);
    return ret;
}


GUIParameterTableWindow *
GUIVehicle::getParameterWindow(GUIMainWindow &app,
                               GUISUMOAbstractView &parent)
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this, 9);
    // add items
    ret->mkItem("type [NAME]", false, myType->id());
    ret->mkItem("left same route [#]", false, (SUMOReal) getRepetitionNo());
    ret->mkItem("emission period [s]", false, (SUMOReal) getPeriod());
    ret->mkItem("waiting time [s]", true,
        new CastingFunctionBinding<MSVehicle, SUMOReal, size_t>(
            this, &MSVehicle::getWaitingTime));
    ret->mkItem("last lane change [s]", true,
        new CastingFunctionBinding<GUIVehicle, SUMOReal, size_t>(
        this, &GUIVehicle::getLastLaneChangeOffset));
    ret->mkItem("desired depart [s]", false, (SUMOReal) getDesiredDepart());
    ret->mkItem("position [m]", true,
        new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::pos));
    ret->mkItem("speed [m/s]", true,
        new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::speed));
    // close building
    ret->closeBuilding();
    return ret;
}



GUIGlObjectType
GUIVehicle::getType() const
{
    return GLO_VEHICLE;
}


std::string
GUIVehicle::microsimID() const
{
    return id();
}


bool
GUIVehicle::active() const
{
    return running();
}


void
GUIVehicle::setRemoved()
{
    myLane = 0;
}


int
GUIVehicle::getRepetitionNo() const
{
    return myRepetitionNumber;
}


int
GUIVehicle::getPeriod() const
{
    return myPeriod;
}


size_t
GUIVehicle::getLastLaneChangeOffset() const
{
    return myLastLaneChangeOffset;
}


size_t
GUIVehicle::getDesiredDepart() const
{
    return myDesiredDepart;
}


Boundary
GUIVehicle::getCenteringBoundary() const
{
	throw 1;
}

// !!! 4 UniDortmund #ifdef NETWORKING_BLA
#include "time.h"

void
px2gps(SUMOReal x, SUMOReal y, int scale, int gkr, int gkh, SUMOReal &lat, SUMOReal &lon)
{
    //Berechnet zu x,y-Koordinaten die GK-Koordinaten
    //bei zwei gegebenen GPS-Eckpunkten(der betrachteten Karte)
    SUMOReal rm, e2, c, bI, bII, bf, co, g2, g1, t, fa, dl, gb, gl;
    int mKen;
	SUMOReal gkx=gkr+x*scale;//2601000.25+x*2;
    SUMOReal gky=gkh+x*scale;//5711999.75-y*2;
    const SUMOReal rho = (SUMOReal) (180/3.1415926535897932384626433832795);
    e2 = (SUMOReal) 0.0067192188;
    c = (SUMOReal) 6398786.849;
    mKen = (int) (gkx / 1000000);
    rm = gkx - mKen * 1000000 - 500000;
    bI = gky / (SUMOReal) 10000855.7646;
    bII = bI * bI;
    bf = (SUMOReal) 325632.08677 * bI *(((((((SUMOReal) 0.00000562025 * bII - (SUMOReal) 0.00004363980) * bII + (SUMOReal) 0.00022976983) * bII - (SUMOReal) 0.00113566119) * bII + (SUMOReal) 0.00424914906) * bII - (SUMOReal) 0.00831729565) * bII + 1);
    bf = bf / 3600 / rho;
    co = cos(bf);
    g2 = e2 * (co * co);
    g1 = c / sqrt(1 + g2);
    t = sin(bf) / cos(bf); // tan(bf)
    fa = rm / g1;
    gb = bf - fa * fa * t * (1 + g2) / 2 + fa * fa * fa * fa * t * (5 + 3 * t * t + 6 * g2 - 6 * g2 * t * t) / 24;
    gb = gb * rho;
    dl = fa - fa * fa * fa * (1 + 2 * t * t + g2) / 6 + fa * fa * fa * fa * fa * (1 + 28 * t * t + 24 * t * t * t * t) / 120;
    gl = dl * rho / co + mKen * 3;
    lat = gb;
    lon = gl;
}

void
GUIVehicle::networking_Begin()
{
	char buffer2 [100];
	char buffer3 [100];
	time_t rawtime;
	tm* ptm;
    SUMOReal mylat, mylon;

    int scale = 1;
	int gkr=1000;
	int gkh=1000;


    time(&rawtime);
	ptm=gmtime(&rawtime);
    //ptemp=pfad[m];
    Position2D pos =
        static_cast<GUINet*>(MSNet::getInstance())->getVehiclePosition(id());
	px2gps(pos.x(), pos.y(), scale, gkr, gkh, mylat, mylon);
//	mylat=ptemp->GetGPSLat()*100;
//	mylon=ptemp->GetGPSLon()*100;
	int mylat1=(int)mylat;
	int mylon1=(int)mylon;
	long mylat2=(long)((mylat-(int)mylat)*1000000);
	long mylon2=(long)((mylon-(int)mylon)*1000000);

//    stepOut << id() << "," << MSNet::getInstance()->getCurrentTimeStep() << ",";
    sprintf(buffer2,"$GPRMC,%02d%02d%02d,A,%04d.%06d,N,%05d.%06d,E,,,%02d%02d%d,,\n",
        ptm->tm_hour+1,ptm->tm_min,ptm->tm_sec,
        mylat1,mylat2,mylon1,mylon2,
        ptm->tm_mday,ptm->tm_mon+1,ptm->tm_year+1900);
//    stepOut << buffer2;

//    stepOut << id() << "," << MSNet::getInstance()->getCurrentTimeStep() << ",";
	sprintf(buffer3,"$GPGGA,%02d%02d%02d,%04d.%06d,N,%05d.%06d,E,,,,0.0,,,,,\n",
        ptm->tm_hour+1,ptm->tm_min,ptm->tm_sec,
        mylat1,mylat2,mylon1,mylon2);
//	stepOut << buffer3;
        /*
	//Miguel!!!
	FXSlider* my1=myDialog->getMapscaleSlider();
	FXTextField* my2=myDialog->getGKRTextField();
	FXTextField* my3=myDialog->getGKHTextField();
	int scale=my1->getValue();
	FXint myFXint1=FXIntVal(my2->getText(),10);
	FXint myFXint2=FXIntVal(my3->getText(),10);
	int gkr=myFXint1;
	int gkh=myFXint2;
	///Miguel!!!
	char buffer1 [100];
	char buffer2 [100];
	char buffer3 [100];
	SUMOReal mylat;
	SUMOReal mylon;
	int mylat1;
	int mylon1;
	long mylat2;
	long mylon2;
	time_t rawtime;
	tm* ptm;
	Vertex* ptemp;

	for (int l=1; l<=cars; l++)
	{
		sprintf(buffer1,"Trace%2d.txt",l);
		FILE* Traces = fopen(buffer1,"w");
		Drive(fuel);
		pfad = GetPfadArray();
		for (int m=0; m<(int) pfad.size(); m++)
		{
			ptemp=pfad[m];
			time(&rawtime);
			ptm=gmtime(&rawtime);
			ptemp->px2gps(scale,gkr,gkh);
			mylat=ptemp->GetGPSLat()*100;
			mylon=ptemp->GetGPSLon()*100;
			mylat1=(int)mylat;
			mylon1=(int)mylon;
			mylat2=(long)((mylat-(int)mylat)*1000000);
			mylon2=(long)((mylon-(int)mylon)*1000000);
			//So soll es aussehen
			//$GPRMC,163156,A,5152.681389,N,00745.598541,E,,,26102004,,
			//$GPGGA,163156,5152.681389,N,00745.598541,E,,,,0.0,,,,,

			sprintf(buffer2,"$GPRMC,%02d%02d%02d,A,%04d.%06d,N,%05d.%06d,E,,,%02d%02d%d,,\n",ptm->tm_hour+1,ptm->tm_min,ptm->tm_sec,mylat1,mylat2,mylon1,mylon2,ptm->tm_mday,ptm->tm_mon+1,ptm->tm_year+1900);
			fputs(buffer2,Traces);
			sprintf(buffer3,"$GPGGA,%02d%02d%02d,%04d.%06d,N,%05d.%06d,E,,,,0.0,,,,,\n",ptm->tm_hour+1,ptm->tm_min,ptm->tm_sec,mylat1,mylat2,mylon1,mylon2);
			fputs(buffer3,Traces);
		}
		fclose(Traces);

	}
*/
    /* !!! 4 UniDortmund
    if(id()==OptionsSubSys::getOptions().getString("knownveh")) {
		networking_EdgeTimeInformation ei;
		ei.edge = (MSEdge*) &(myLane->edge());
		ei.time = MSNet::globaltime;
		ei.val = -1;
		addEdgeTimeInfo(ei);
	}
    networking_mySeenThisTime.clear();
    !!! 4 UniDortmund */

}

#ifdef NETWORKING_BLA// !!! 4 UniDortmund

void
GUIVehicle::addEdgeTimeInfo(const GUIVehicle::networking_EdgeTimeInformation &ei)
{
        std::vector<networking_EdgeTimeInformation>::iterator j;
        j = find_if(networking_myKnownEdges.begin(), networking_myKnownEdges.end(),
            networking_ByEdgeFinder(ei.edge));
        if(j==networking_myKnownEdges.end()) {
            networking_myKnownEdges.push_back(ei);
        } else {
            if((*j).time>ei.time) {
                (*j).time = ei.time;
            }
        }


}


void
GUIVehicle::networking_KnowsAbout(GUIVehicle *v2, MSNet::Time t)
{
	networking_globalConns++;
    networking_mySeenThisTime.push_back(v2);
    const std::vector<networking_EdgeTimeInformation> &info
        = v2->networking_GetKnownEdges();

    std::vector<networking_EdgeTimeInformation>::const_iterator i;

    for(i=info.begin(); i!=info.end(); ++i) {
		addEdgeTimeInfo(*i);
		if(myRoute->find((*i).edge)==myRoute->end()) {
			continue;
		}

		bool over = false;
		for(int i2=0; i2<myRoute->size(); i2++) {
			if((*i).edge==(*myRoute)[i2]) {
				if(networking_myLaneEmitTime[i2]==-1&&over) {
					networking_myLaneEmitTime[i2] = (*i).time;
				}
				/*
				if(networking_myLaneEmitTime[i2]>ei.time&&over) {
					networking_myLaneEmitTime[i2] = ei.time;
				}
				*/
			}
			if(&(myLane->edge())==(*myRoute)[i2]) {
				over = true;
			}
		}
    }

		networking_mySeenGlobal.insert(v2);
}


const std::vector<GUIVehicle::networking_EdgeTimeInformation> &
GUIVehicle::networking_GetKnownEdges()
{
    return networking_myKnownEdges;
}


void
GUIVehicle::networking_End()
{
    networking_stepOut
        << id() << ";" << MSNet::globaltime << ";"
        << networking_myKnownEdges.size() << ";"
		<< networking_mySeenThisTime.size() << endl;
    if(id()==OptionsSubSys::getOptions().getString("knownveh")) {
        for(int i=0; i<myRoute->size(); i++) {
            SUMOTime time = -1;
            std::vector<networking_EdgeTimeInformation>::iterator j;
            j = find_if(networking_myKnownEdges.begin(), networking_myKnownEdges.end(),
                networking_ByEdgeFinder((*myRoute)[i]));
            if(j!=networking_myKnownEdges.end()) {
                time = MSNet::globaltime - (*j).time;
            }
            networking_knownOut
                << time << ";";
        }
        networking_knownOut << endl;
    }
}



bool
GUIVehicle::networking_hasDevice()
{
    return networking_HaveDevice;
}


void GUIVehicle::enterLaneAtMove( MSLane* enteredLane, SUMOReal driven )
{
	networking_EdgeTimeInformation ei;
	ei.edge = (MSEdge*) &(enteredLane->edge());
	ei.time = MSNet::globaltime;
	ei.val = -1;
	addEdgeTimeInfo(ei);
networking_myLaneEntryTime.push_back(MSNet::globaltime);
	MSVehicle::enterLaneAtMove( enteredLane, driven );
	if(networking_EntryTime==-1) {
		networking_EntryTime = MSNet::globaltime;
	}
}

void GUIVehicle::enterLaneAtEmit( MSLane* enteredLane )
{
	networking_EdgeTimeInformation ei;
	ei.edge = (MSEdge*) &(enteredLane->edge());
	ei.time = MSNet::globaltime;
	ei.val = -1;
	addEdgeTimeInfo(ei);
	networking_myLaneEntryTime.push_back(MSNet::globaltime);
	MSVehicle::enterLaneAtEmit(enteredLane);
	if(networking_EntryTime==-1) {
		networking_EntryTime = MSNet::globaltime;
	}

}

#endif


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


