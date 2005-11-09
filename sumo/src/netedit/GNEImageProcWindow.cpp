//---------------------------------------------------------------------------//
//                        GNEImageProcWindow.cpp
//  The map manipulation window
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Wed, 26 Jan 2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
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
// Revision 1.10  2005/11/09 06:41:08  dkrajzew
// problems on loading geometry items patched
//
// Revision 1.9  2005/10/17 09:04:48  dkrajzew
// building patched
//
// Revision 1.8  2005/10/10 11:59:53  dkrajzew
// debugging
//
// Revision 1.7  2005/10/07 11:38:33  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/23 06:01:19  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 12:03:02  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/05/04 08:37:26  dkrajzew
// ported to fox1.4
//
// Revision 1.3  2005/03/31 14:18:45  miguelliebe
// ConfigDialog upgrade(Bitmap scale)
//
// Revision 1.2  2005/01/31 09:25:20  dksumo
// added the possibility to save nodes and edges or the build network to netedit
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

#include <fx.h>
#include "GNEImageProcWindow.h"
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIcons.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include "ConfigDialog.h"
#include "InfoDialog.h"
#include <utils/common/ToString.h>
#include <netbuild/NBEdge.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/common/RandHelper.h>
#include <utils/common/DevHelper.h>
#include <utils/gui/events/GUIEvent_Message.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/MsgRetrievingFunction.h>
#include <sstream>
#include <gui/GUIEvent_SimulationLoaded.h>
#include <gui/GUIGlobals.h>

#include <netload/NLBuilder.h>
#include <guinetload/GUIHandler.h>
#include <guinetload/GUITriggerBuilder.h>
#include <guinetload/GUIDetectorBuilder.h>
#include <guinetload/GUIEdgeControlBuilder.h>
#include <guinetload/GUIJunctionControlBuilder.h>
#include <guinetload/GUIGeomShapeBuilder.h>
#include <guinetload/GUIHandler.h>
#include <guisim/GUIVehicleControl.h>
#include <guisim/GUINet.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>
#include <utils/gui/windows/GUIAppGlobals.h>
#include <sumo_only/SUMOFrame.h>
#include <utils/common/UtilExceptions.h>
#include <utils/xml/XMLBuildingExceptions.h>
#include <utils/common/XMLHelpers.h>
#include <xercesc/framework/MemBufInputSource.hpp>

#include <fstream>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

/* =========================================================================
 * statc value definitions
 * ======================================================================= */
static const char*  gMemBufId = "internal";



/* =========================================================================
 * FOX-declarations
 * ======================================================================= */
FXDEFMAP(GNEImageProcWindow) GNEImageProcWindowMap[]=
{
    //________Message_Type____________ID________________________Message_Handler________
//new Andreas (Anfang)
    FXMAPFUNC(SEL_COMMAND,  MID_EXTRACT_STREETS,  GNEImageProcWindow::onCmdExtractStreets),
    FXMAPFUNC(SEL_COMMAND,  MID_DILATION,  GNEImageProcWindow::onCmdDilate),
    FXMAPFUNC(SEL_COMMAND,  MID_EROSION,  GNEImageProcWindow::onCmdErode),
    FXMAPFUNC(SEL_COMMAND,  MID_OPENING,  GNEImageProcWindow::onCmdOpening),
    FXMAPFUNC(SEL_COMMAND,  MID_CLOSING,  GNEImageProcWindow::onCmdClosing),
    FXMAPFUNC(SEL_COMMAND,  MID_CLOSE_GAPS,  GNEImageProcWindow::onCmdCloseGaps),
    FXMAPFUNC(SEL_COMMAND,  MID_SKELETONIZE,  GNEImageProcWindow::onCmdSkeletonize),
    FXMAPFUNC(SEL_COMMAND,  MID_CREATE_GRAPH,  GNEImageProcWindow::onCmdCreateGraph),
    FXMAPFUNC(SEL_COMMAND,  MID_ERASE_STAINS,  GNEImageProcWindow::onCmdEraseStains),
    FXMAPFUNC(SEL_COMMAND,  MID_OPEN_BMP_DIALOG,  GNEImageProcWindow::onCmdShowBMPDialog),
//new Andreas (End)

//new Miguel (Anfang)
    FXMAPFUNC(SEL_COMMAND,  MID_SHOW_GRAPH_ON_EMPTY_BITMAP,  GNEImageProcWindow::onCmdShowGraphOnEmptyBitmap),
    FXMAPFUNC(SEL_COMMAND,  MID_SHOW_GRAPH_ON_ACTUAL_BITMAP,  GNEImageProcWindow::onCmdShowGraphOnActualBitmap),
    FXMAPFUNC(SEL_COMMAND,  MID_REDUCE_VERTEXES,  GNEImageProcWindow::onCmdReduceVertexes),
    FXMAPFUNC(SEL_COMMAND,  MID_REDUCE_VERTEXES_PLUS,  GNEImageProcWindow::onCmdReduceVertexesPlus),
    FXMAPFUNC(SEL_COMMAND,  MID_REDUCE_EDGES,  GNEImageProcWindow::onCmdReduceEdges),
    FXMAPFUNC(SEL_COMMAND,  MID_MERGE_VERTEXES,  GNEImageProcWindow::onCmdMergeVertexes),
    FXMAPFUNC(SEL_COMMAND,  MID_EXPORT_VERTEXES_XML,  GNEImageProcWindow::onCmdExportVertexesXML),
    FXMAPFUNC(SEL_COMMAND,  MID_EXPORT_EDGES_XML,  GNEImageProcWindow::onCmdExportEdgesXML),
//new Miguel (End)
    FXMAPFUNC(SEL_PAINT,             MID_MAP_IMAGEVIEWER,   GNEImageProcWindow::onPaint),
    FXMAPFUNC(SEL_LEFTBUTTONPRESS,   MID_MAP_IMAGEVIEWER,   GNEImageProcWindow::onMouseDown),
    FXMAPFUNC(SEL_LEFTBUTTONRELEASE, MID_MAP_IMAGEVIEWER,   GNEImageProcWindow::onMouseUp),
    FXMAPFUNC(SEL_MOTION,            MID_MAP_IMAGEVIEWER,   GNEImageProcWindow::onMouseMove),
};

// Object implementation
FXIMPLEMENT(GNEImageProcWindow, FXMDIChild, GNEImageProcWindowMap, ARRAYNUMBER(GNEImageProcWindowMap))


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GNEImageProcWindow::GNEImageProcWindow(GNEApplicationWindow *parent,
                                       NBNetBuilder *nb,
                                       FXMDIClient* p, FXMDIMenu *mdimenu,
                                       Image *img, bool extrFlag,
                                       const FXString& name,
                                       FXIcon* ic, FXPopup* pup, FXuint opts,
                                       FXint x,FXint y,FXint w,FXint h)
	: GUIGlChildWindow( p, mdimenu, name, ic),
    m_img(img), myGraphFlag(false), mySkelFlag(false), myExtrFlag(extrFlag),
    myParent(parent), myNetBuilder(nb)

{
    myErrorRetriever = new MsgRetrievingFunction<GNEImageProcWindow>(this,
        &GNEImageProcWindow::retrieveError);
    MsgHandler::getErrorInstance()->addRetriever(myErrorRetriever);
    myMessageRetriever = new MsgRetrievingFunction<GNEImageProcWindow>(this,
        &GNEImageProcWindow::retrieveMessage);
    MsgHandler::getMessageInstance()->addRetriever(myMessageRetriever);
    myWarningRetreiver = new MsgRetrievingFunction<GNEImageProcWindow>(this,
        &GNEImageProcWindow::retrieveWarning);
    MsgHandler::getWarningInstance()->addRetriever(myWarningRetreiver);

    myInfoDialog = new InfoDialog(this);
    myConfigDialog = new ConfigDialog(this);
    drawColor=FXRGB(255,0,0);


    setTracking();
    FXVerticalFrame *contentFrame =
        new FXVerticalFrame(this,
        FRAME_SUNKEN|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y,
        0,0,0,0,0,0,0,0);


    // build the tool bar
    myIMGToolBar = new FXToolBar(contentFrame,LAYOUT_SIDE_TOP|LAYOUT_FILL_X|FRAME_RAISED);
	new FXButton(myIMGToolBar,"\t\tOpen Bitmap Configuration Dialog.",
        GUIIconSubSys::getIcon(ICON_OPEN_BMP_DIALOG), this, MID_OPEN_BMP_DIALOG,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
	new FXToolBarGrip(myIMGToolBar,NULL,0,TOOLBARGRIP_SINGLE);
	new FXButton(myIMGToolBar,
        "\t\tExtracts streets (areas with one of the chosen colours).",
        GUIIconSubSys::getIcon(ICON_EXTRACT), this, MID_EXTRACT_STREETS,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
	new FXButton(myIMGToolBar,"\t\tPerforms a dilatation on the image.",
        GUIIconSubSys::getIcon(ICON_DILATE), this, MID_DILATION,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
	new FXButton(myIMGToolBar,"\t\tPerforms an erosion on the image.",
        GUIIconSubSys::getIcon(ICON_ERODE), this, MID_EROSION,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
	new FXButton(myIMGToolBar,"\t\tOpens morphologically (erode first, then dilate).",
        GUIIconSubSys::getIcon(ICON_OPENING), this, MID_OPENING,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
	new FXButton(myIMGToolBar,"\t\tCloses morphologically (dilate first, then close).",
        GUIIconSubSys::getIcon(ICON_CLOSING), this, MID_CLOSING,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
	new FXButton(myIMGToolBar,"\t\tMinimizes small white spots in black areas.",
        GUIIconSubSys::getIcon(ICON_CLOSE_GAPS), this, MID_CLOSE_GAPS,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
	new FXButton(myIMGToolBar,"\t\tErases black ´noise´.",
        GUIIconSubSys::getIcon(ICON_ERASE_STAINS), this, MID_ERASE_STAINS,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
	new FXButton(myIMGToolBar,"\t\tCreates a street´s skeleton (thin black lines in the middle of the street.",
        GUIIconSubSys::getIcon(ICON_SKELETONIZE), this, MID_SKELETONIZE,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
	new FXToolBarGrip(myIMGToolBar,NULL,0,TOOLBARGRIP_SINGLE);
	new FXButton(myIMGToolBar,"\t\tCreates a graph from the skeleton.",
		GUIIconSubSys::getIcon(ICON_CREATE_GRAPH), this, MID_CREATE_GRAPH,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
	new FXToolBarGrip(myIMGToolBar,NULL,0,TOOLBARGRIP_SINGLE);
	myPaintPop=new FXPopup(this,POPUP_VERTICAL);
	new FXButton(myPaintPop,"\t\tPaintbrush, very thin",
		GUIIconSubSys::getIcon(ICON_PAINTBRUSH1X), this, MID_OPEN_BMP_DIALOG,
		ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
	new FXButton(myPaintPop,"\t\tPaintbrush,  thin",
		GUIIconSubSys::getIcon(ICON_PAINTBRUSH2X), this, MID_OPEN_BMP_DIALOG,
		ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
	new FXButton(myPaintPop,"\t\tPaintbrush, normal",
		GUIIconSubSys::getIcon(ICON_PAINTBRUSH3X), this, MID_OPEN_BMP_DIALOG,
		ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
	new FXButton(myPaintPop,"\t\tPaintbrush, thick",
		GUIIconSubSys::getIcon(ICON_PAINTBRUSH4X), this, MID_OPEN_BMP_DIALOG,
		ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
	new FXButton(myPaintPop,"\t\tPaintbrush, very thick",
		GUIIconSubSys::getIcon(ICON_PAINTBRUSH5X), this, MID_OPEN_BMP_DIALOG,
		ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
	new FXMenuButton(myIMGToolBar,"&\t\tChoose brush size",
		GUIIconSubSys::getIcon(ICON_PAINTBRUSH1X),myPaintPop,
		MENUBUTTON_ATTACH_BOTH|MENUBUTTON_DOWN|MENUBUTTON_NOARROWS|LAYOUT_LEFT|
		ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);

	myRubberPop=new FXPopup(this,POPUP_VERTICAL);
	new FXButton(myRubberPop,"\t\tRubber, very thin",
		GUIIconSubSys::getIcon(ICON_RUBBER1X), this, MID_OPEN_BMP_DIALOG,
		ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
	new FXButton(myRubberPop,"\t\tRubber,  thin",
		GUIIconSubSys::getIcon(ICON_RUBBER2X), this, MID_OPEN_BMP_DIALOG,
		ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
	new FXButton(myRubberPop,"\t\tRubber, normal",
		GUIIconSubSys::getIcon(ICON_RUBBER3X), this, MID_OPEN_BMP_DIALOG,
		ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
	new FXButton(myRubberPop,"\t\tRubber, thick",
		GUIIconSubSys::getIcon(ICON_RUBBER4X), this, MID_OPEN_BMP_DIALOG,
		ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
	new FXButton(myRubberPop,"\t\tRubber, very thick",
		GUIIconSubSys::getIcon(ICON_RUBBER5X), this, MID_OPEN_BMP_DIALOG,
		ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
	new FXMenuButton(myIMGToolBar,"&\t\tChoose rubber size",
		GUIIconSubSys::getIcon(ICON_RUBBER1X),myRubberPop,
		MENUBUTTON_ATTACH_BOTH|MENUBUTTON_DOWN|MENUBUTTON_NOARROWS|LAYOUT_LEFT|
		ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);

    // build the image canvas
    myImageViewer = new FXImageView(contentFrame, 0, this, MID_MAP_IMAGEVIEWER,
        LAYOUT_FILL_X|LAYOUT_FILL_Y);
    myImageViewer->setBackColor(FXRGB(192, 192, 192));
    myImageViewer->setImage(m_img->GetFXImage());
}


GNEImageProcWindow::~GNEImageProcWindow()
{
    MsgHandler::getErrorInstance()->removeRetriever(myErrorRetriever);
    MsgHandler::getWarningInstance()->removeRetriever(myWarningRetreiver);
    MsgHandler::getMessageInstance()->removeRetriever(myMessageRetriever);
    delete myPaintPop;
    delete myRubberPop;
    delete myErrorRetriever;
    delete myMessageRetriever;
    delete myWarningRetreiver;
}


void
GNEImageProcWindow::create()
{
    FXMDIChild::create();
    myIMGToolBar->create();
    myInfoDialog->create();
    myConfigDialog->create();
    myImageViewer->create();
    myPaintPop->create();
    myRubberPop->create();
}


long
GNEImageProcWindow::onCmdExtractStreets(FXObject*,FXSelector,void*)
{
	if(myExtrFlag==false)
	{
		myExtrFlag=true;
        m_img->ExtractStreets();
		m_img->GetFXImage()->render();
        myImageViewer->update();
	}
    else {
		myInfoDialog->show();
    }
    return 1;
}

long
GNEImageProcWindow::onCmdErode(FXObject*,FXSelector,void*)
{
    m_img->Erode(myConfigDialog);
    m_img->GetFXImage()->render();
    myImageViewer->update();
    return 1;
}

long
GNEImageProcWindow::onCmdDilate(FXObject*,FXSelector,void*)
{
    m_img->Dilate(myConfigDialog);
    m_img->GetFXImage()->render();
    myImageViewer->update();
    return 1;
}


long
GNEImageProcWindow::onCmdOpening(FXObject*,FXSelector,void*)
{
    m_img->Opening(myConfigDialog);
    m_img->GetFXImage()->render();
    myImageViewer->update();
    return 1;
}

long
GNEImageProcWindow::onCmdClosing(FXObject*,FXSelector,void*)
{
    m_img->Closing(myConfigDialog);
    m_img->GetFXImage()->render();
    myImageViewer->update();
    return 1;
}

long
GNEImageProcWindow::onCmdCloseGaps(FXObject*,FXSelector,void*)
{
    m_img->CloseGaps();
    m_img->GetFXImage()->render();
    myImageViewer->update();
    return 1;
}

bool mySkelFlag=false;
long
GNEImageProcWindow::onCmdSkeletonize(FXObject*,FXSelector,void*)
{
	if(mySkelFlag==false)
	{
		mySkelFlag=true;
        m_img->CreateSkeleton();
		m_img->GetFXImage()->render();
        myImageViewer->update();
	}
    else {
		myInfoDialog->show();
    }
	return 1;
}

long
GNEImageProcWindow::onCmdEraseStains(FXObject*,FXSelector,void*)
{
    m_img->EraseStains(myConfigDialog);
    m_img->GetFXImage()->render();
    myImageViewer->update();
    return 1;
}




GUIEdgeControlBuilder *
buildEdgeBuilder()
{
    return new GUIEdgeControlBuilder(gIDStorage);
}


GUIVehicleControl*
buildVehicleControl()
{
    return new GUIVehicleControl();
}

int idbla = 0;
long
GNEImageProcWindow::onCmdCreateGraph(FXObject*,FXSelector,void*)
{
    if(myGraphFlag==false)
	{
		myGraphFlag=true;
		Graph leergraph;
        myGraph = leergraph;
		Graph* gr =new Graph();
		gr=&myGraph;
		if(m_img)
		{
            //
            OptionsCont &oc = OptionsSubSys::getOptions();
            oc.clear();
            myNetBuilder->insertNetBuildOptions(oc);
            DevHelper::insertDevOptions(oc);
            // add rand and dev options
            RandHelper::insertRandOptions(oc);
            oc.set("verbose", true);
            oc.set("no-node-removal", true);
            //
			gr=m_img->Tracking(gr,myConfigDialog);
			gr->MergeVertex();
			gr->Reduce_plus(myConfigDialog);
			m_img->GetFXImage()->render();
            myImageViewer->update();
			vector<Edge*> edges = gr->GetEArray();
			for(vector<Edge*>::iterator i=edges.begin(); i!=edges.end(); ++i) {
				Edge*e = *i;
				string name = "e" + toString<int>(idbla++);
				Position2D fromPos((SUMOReal) e->GetStartingVertex()->GetX(), (SUMOReal) -e->GetStartingVertex()->GetY());
				NBNode *fromNode = myNetBuilder->getNodeCont().retrieve(fromPos);
				if(fromNode==0) {
                    string fname = "n" + toString<int>(idbla++);
					fromNode = new NBNode(fname, fromPos);
                    myNetBuilder->getNodeCont().insert(fromNode);
				}
				Position2D toPos((SUMOReal) e->GetEndingVertex()->GetX(), (SUMOReal) -e->GetEndingVertex()->GetY());
				NBNode *toNode = myNetBuilder->getNodeCont().retrieve(toPos);
				if(toNode==0) {
                    string tname = "n" + toString<int>(idbla++);
					toNode = new NBNode(tname, toPos);
                    myNetBuilder->getNodeCont().insert(toNode);
				}
				if(fromNode!=toNode) {
					int lanes = 1;
					SUMOReal speed = (SUMOReal) 13.8;
					SUMOReal length = -1;
					NBEdge *edge = new NBEdge(name, name, fromNode, toNode,
						"stdtype", speed, lanes, length, -1);
                    myNetBuilder->getEdgeCont().insert(edge);
				}
			}
            //
            myNetBuilder->compute(oc);
            std::ostringstream strm;
            myNetBuilder->save(strm, oc);
            string description = strm.str();
            //
            OptionsSubSys::getOptions().clear();
            OptionsSubSys::guiInit(SUMOFrame::fillOptions, "hallo"/*!!!*/);
            OptionsCont &oc2 = OptionsSubSys::getOptions();
            GUINet *net = new GUINet(oc2.getInt("begin"), oc2.getInt("end"),
                new GUIVehicleControl());
            SUMOFrame::setMSGlobals(oc2);
            GUIEdgeControlBuilder *eb = buildEdgeBuilder();
            GUIJunctionControlBuilder jb(*net, oc2);
            GUIDetectorBuilder db(*net);
            GUITriggerBuilder tb;
            GUIGeomShapeBuilder sb(*net, gIDStorage);
            GUIHandler handler("", *net, db, tb, *eb, jb, sb);
            NLBuilder builder(oc2, *net, *eb, jb, db, tb, sb, handler);
            try {
                MsgHandler::getErrorInstance()->clear();
                MsgHandler::getWarningInstance()->clear();
                MsgHandler::getMessageInstance()->clear();
//                initDevices();
                SUMOFrame::setMSGlobals(oc);
                GUIHandler handler("", *net, db, tb, *eb, jb, sb);
                handler.setWanted(LOADFILTER_NET);
                // ... and the parser
                SAX2XMLReader* parser = XMLHelpers::getSAXReader(handler);
                MemBufInputSource* memBufIS =
                    new MemBufInputSource((const XMLByte*) description.c_str(),
                        description.length(), gMemBufId, false);
                parser->parse(*memBufIS);
                bool ok = true;
                if(!MsgHandler::getErrorInstance()->wasInformed()) {
                    ok = builder.buildNet(*this);
                } else {
                    delete net;
                    net = 0;
                }
                delete parser;
                RandHelper::initRandGlobal(oc);
                GUIEvent *e =
                    new GUIEvent_SimulationLoaded(
                        net, 0, 86400, string("hallo"/*!!!*/));
                myParent->handleEvent_SimulationLoaded(e);
            } catch (RandHelper &) {
            }
/*                string error = e.msg();
                MsgHandler::getErrorInstance()->inform(error);
                delete net;
                MSNet::clearAll();
                net = 0;*/
/*            } catch (XMLBuildingException &e) {
                string error = e.getMessage("", "");
                MsgHandler::getErrorInstance()->inform(error);
                delete net;
                MSNet::clearAll();
                net = 0;
            }*/
		}
    } else {
		myInfoDialog->show();
    }
    return 1;
}

long
GNEImageProcWindow::onCmdShowBMPDialog(FXObject*,FXSelector,void*)
{
    myConfigDialog->show();
    return 1;

}
/////////////////////////new Andreas (Ende)


//Noch zu ändern!!!!!!!!!!!!!!!!!!
/////////////////////////new Miguel (Anfang)
long
GNEImageProcWindow::onCmdShowGraphOnEmptyBitmap(FXObject*,FXSelector,void*)
{
    m_img->EmptyImage();
    m_img->DrawGraph(myGraph);
    m_img->GetFXImage()->render();
    myImageViewer->update();
    return 1;
}

long
GNEImageProcWindow::onCmdShowGraphOnActualBitmap(FXObject*,FXSelector,void*)
{
    m_img->DrawGraph(myGraph);
    m_img->GetFXImage()->render();
    myImageViewer->update();
    return 1;
}

long
GNEImageProcWindow::onCmdReduceVertexes(FXObject*,FXSelector,void*)
{
    return 1;

}

long
GNEImageProcWindow::onCmdReduceVertexesPlus(FXObject*,FXSelector,void*)
{
    myGraph.Reduce_plus(myConfigDialog);
    return 1;
}

long
GNEImageProcWindow::onCmdReduceEdges(FXObject*,FXSelector,void*)
{
    myGraph.Reduce_Edges();
    return 1;
}

long
GNEImageProcWindow::onCmdMergeVertexes(FXObject*,FXSelector,void*)
{
    myGraph.MergeVertex();
    return 1;
}

long
GNEImageProcWindow::onCmdExportVertexesXML(FXObject*,FXSelector,void*)
{
    myGraph.Export_Vertexes_XML();
    return 1;
}

long
GNEImageProcWindow::onCmdExportEdgesXML(FXObject*,FXSelector,void*)
{
    myGraph.Export_Edges_XML();
    myGraph.GetTraces(1,1000,myConfigDialog);
	return 1;
}

/////////////////////////new Miguel (Ende)

// Mouse button was pressed somewhere
long
GNEImageProcWindow::onMouseDown(FXObject*,FXSelector,void*)
{
//  myCanvas->grab();
    grab();

  // While the mouse is down, we'll draw lines
  mdflag=1;

  return 1;
}

// The mouse has moved, draw a line
long GNEImageProcWindow::onMouseMove(FXObject*, FXSelector, void* ptr)
{
  FXEvent *ev=(FXEvent*)ptr;

  // Draw
  if(mdflag==1)
  {

    // Get DC for the canvas
    FXDCWindow dc(myImageViewer);//(myCanvas);

    // Draw line
    dc.setLineWidth (1);
    dc.drawLine(ev->last_x, ev->last_y, ev->win_x, ev->win_y);
    m_img->DrawLine(ev->last_x,ev->last_y, ev->win_x, ev->win_y);

    dirty=1;
  }
  return 1;
}

// The mouse button was released again
long GNEImageProcWindow::onMouseUp(FXObject*,FXSelector,void* ptr)
{
  FXEvent *ev=(FXEvent*) ptr;
//  myCanvas->ungrab();
  ungrab();
  if(mdflag){
    FXDCWindow dc(myImageViewer);//(myCanvas);
    dc.setForeground(drawColor);
    dc.drawLine(ev->last_x, ev->last_y, ev->win_x, ev->win_y);
    m_img->DrawLine(ev->last_x,ev->last_y, ev->win_x, ev->win_y);

    // We have drawn something, so now the canvas is dirty
    dirty=1;

    // Mouse no longer down
    mdflag=0;
    }
  return 1;
}

// Paint the canvas
long GNEImageProcWindow::onPaint(FXObject*,FXSelector,void* ptr)
{
  FXDCWindow dc(myImageViewer);//(myCanvas);
  if(m_img)
  {
      m_img->GetFXImage()->render();
      dc.drawImage(m_img->GetFXImage(),0,0);
  }
  return 1;
  }



void
GNEImageProcWindow::retrieveMessage(const std::string &msg)
{
    GUIEvent *e = new GUIEvent_Message(MsgHandler::MT_MESSAGE, msg);
    myParent->handleEvent_Message(e);
}


void
GNEImageProcWindow::retrieveWarning(const std::string &msg)
{
    GUIEvent *e = new GUIEvent_Message(MsgHandler::MT_WARNING, msg);
    myParent->handleEvent_Message(e);
}


void
GNEImageProcWindow::retrieveError(const std::string &msg)
{
    GUIEvent *e = new GUIEvent_Message(MsgHandler::MT_ERROR, msg);
    myParent->handleEvent_Message(e);
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
