/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#include <fx.h>
#include "GNEImageProcWindow.h"
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIcons.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include "ConfigDialog.h"
#include "InfoDialog.h"
#include <utils/convert/ToString.h>
#include <netbuild/NBEdge.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/common/RandHelper.h>
#include <utils/gui/events/GUIEvent_Message.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/MsgRetrievingFunction.h>
#include <sstream>
#include <gui/GUIEvent_SimulationLoaded.h>

#include <guinetload/GUINetBuilder.h>
#include <guinetload/GUIEdgeControlBuilder.h>
#include <guinetload/GUIJunctionControlBuilder.h>
#include <guisim/GUIVehicleControl.h>
#include <guisim/GUINet.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>
#include <utils/gui/windows/GUIAppGlobals.h>
#include <sumo_only/SUMOFrame.h>
#include <utils/common/UtilExceptions.h>
#include <utils/xml/XMLBuildingExceptions.h>

#include <fstream>


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
                                       FXMDIClient* p, FXMDIMenu *mdimenu,
                                       Image *img, bool extrFlag,
                                       const FXString& name,
                                       FXIcon* ic, FXPopup* pup, FXuint opts,
                                       FXint x,FXint y,FXint w,FXint h)
    : GUIGlChildWindow( p, mdimenu, name, ic),
    m_img(img), myGraphFlag(false), mySkelFlag(false), myExtrFlag(extrFlag),
    myParent(parent)

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
    new FXToolBarGrip(myIMGToolBar,NULL,0,TOOLBARGRIP_SEPARATOR);
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
    new FXToolBarGrip(myIMGToolBar,NULL,0,TOOLBARGRIP_SEPARATOR);
    new FXButton(myIMGToolBar,"\t\tCreates a graph from the skeleton.",
        GUIIconSubSys::getIcon(ICON_CREATE_GRAPH), this, MID_CREATE_GRAPH,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    new FXToolBarGrip(myIMGToolBar,NULL,0,TOOLBARGRIP_SEPARATOR);
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
    /*
    FXHorizontalFrame *canvasFrame =
        new FXHorizontalFrame(contentFrame,
        FRAME_SUNKEN|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y,
        0,0,0,0,0,0,0,0);
        */
    myImageViewer = new FXImageView(contentFrame, 0, this, MID_MAP_IMAGEVIEWER,
        LAYOUT_FILL_X|LAYOUT_FILL_Y);
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
//    myIMGToolBarDrag->create();
//    myImageMenu->create();
//    myGraphMenu->create();
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
        /*
        FXDCWindow dc(myCanvas);
        if(m_img)
        {
            m_img->ExtractStreets();
            m_img->GetFXImage()->render();
            dc.drawImage(m_img->GetFXImage(),0,0);
        }
        */
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
    /*
    FXDCWindow dc(myCanvas);
    if(m_img)
    {
        m_img->Erode(dialog);
        m_img->GetFXImage()->render();
        dc.drawImage(m_img->GetFXImage(),0,0);
    }
    */
    return 1;
}

long
GNEImageProcWindow::onCmdDilate(FXObject*,FXSelector,void*)
{
    m_img->Dilate(myConfigDialog);
    m_img->GetFXImage()->render();
    myImageViewer->update();
    /*
    FXDCWindow dc(myCanvas);
    if(m_img)
    {
        m_img->Dilate(dialog);
        m_img->GetFXImage()->render();
        dc.drawImage(m_img->GetFXImage(),0,0);
    }
    */
    return 1;
}


long
GNEImageProcWindow::onCmdOpening(FXObject*,FXSelector,void*)
{
    m_img->Opening(myConfigDialog);
    m_img->GetFXImage()->render();
    myImageViewer->update();
    /*
    FXDCWindow dc(myCanvas);
    if(m_img)
    {
        m_img->Opening(dialog);
        m_img->GetFXImage()->render();
        dc.drawImage(m_img->GetFXImage(),0,0);
    }
    */
    return 1;
}

long
GNEImageProcWindow::onCmdClosing(FXObject*,FXSelector,void*)
{
    m_img->Closing(myConfigDialog);
    m_img->GetFXImage()->render();
    myImageViewer->update();
    /*
    FXDCWindow dc(myCanvas);
    if(m_img)
    {
        m_img->Closing(dialog);
        m_img->GetFXImage()->render();
        dc.drawImage(m_img->GetFXImage(),0,0);
    }
    */
    return 1;
}

long
GNEImageProcWindow::onCmdCloseGaps(FXObject*,FXSelector,void*)
{
    m_img->CloseGaps();
    m_img->GetFXImage()->render();
    myImageViewer->update();
    /*
    FXDCWindow dc(myCanvas);
    if(m_img)
    {
        m_img->CloseGaps();
        m_img->GetFXImage()->render();
        dc.drawImage(m_img->GetFXImage(),0,0);
    }
    */
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

        /*
        FXDCWindow dc(myCanvas);
        if(m_img)
        {
            m_img->CreateSkeleton();
            m_img->GetFXImage()->render();
            dc.drawImage(m_img->GetFXImage(),0,0);
        }
        */
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
    /*
    FXDCWindow dc(myCanvas);
    if(m_img)
    {
        m_img->EraseStains(dialog);
        m_img->GetFXImage()->render();
        dc.drawImage(m_img->GetFXImage(),0,0);
    }
    */
    return 1;
}




GUIEdgeControlBuilder *
buildEdgeBuilder()
{
    return new GUIEdgeControlBuilder(gIDStorage);
}


GUINetBuilder *
buildNetBuilder(const OptionsCont &oc,
                               NLEdgeControlBuilder &eb,
                               NLJunctionControlBuilder &jb,
                               bool allowAggregatedViews)
{
    return new GUINetBuilder(oc, eb, jb, true/*gAllowAggregated!!!!*/);
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
//      FXDCWindow dc(myCanvas);
        if(m_img)
        {
            //
            OptionsCont &oc = OptionsSubSys::getOptions();
            oc.clear();
            NBNetBuilder::insertNetBuildOptions(oc);
            RandHelper::insertRandOptions(oc);
            oc.set("verbose", true);
            oc.set("no-node-removal", true);
            //
            gr=m_img->Tracking(gr,myConfigDialog);
            gr->MergeVertex();
            gr->Reduce_plus(myConfigDialog);
            m_img->GetFXImage()->render();
            myImageViewer->update();
//          dc.drawImage(m_img->GetFXImage(),0,0);
            vector<Edge*> edges = gr->GetEArray();
            for(vector<Edge*>::iterator i=edges.begin(); i!=edges.end(); ++i) {
                Edge*e = *i;
                string name = "e" + toString<int>(idbla++);
                Position2D fromPos(e->GetStartingVertex()->GetX(), e->GetStartingVertex()->GetY());
                NBNode *fromNode = NBNodeCont::retrieve(fromPos);
                if(fromNode==0) {
                    string fname = "n" + toString<int>(idbla++);
                    fromNode = new NBNode(fname, fromPos);
                    NBNodeCont::insert(fromNode);
                }
                Position2D toPos(e->GetEndingVertex()->GetX(), e->GetEndingVertex()->GetY());
                NBNode *toNode = NBNodeCont::retrieve(toPos);
                if(toNode==0) {
                    string tname = "n" + toString<int>(idbla++);
                    toNode = new NBNode(tname, toPos);
                    NBNodeCont::insert(toNode);
                }
                if(fromNode!=toNode) {
                    int lanes = 1;
                    double speed = 13.8;
                    double length = -1;
                    NBEdge *edge = new NBEdge(name, name, fromNode, toNode,
                        "stdtype", speed, lanes, length, -1);
                    NBEdgeCont::insert(edge);
                }
            }
            //
            NBNetBuilder nb;
            nb.compute(oc);
            std::ostringstream strm;
            nb.save(strm, oc);
            // !!!
            string description = strm.str();
            ofstream bla("d:\\test.net.xml");
            bla << description;
            bla.close();
            // !!!
            //
            OptionsSubSys::getOptions().clear();
            OptionsSubSys::guiInit(SUMOFrame::fillOptions, "hallo"/*!!!*/);
            GUIEdgeControlBuilder *eb = buildEdgeBuilder();
            GUIJunctionControlBuilder jb;
            GUINetBuilder *builder = buildNetBuilder(oc, *eb, jb, false/*gAllowAggregated*/);
            GUINet *net = 0;
            try {
                MsgHandler::getErrorInstance()->clear();
                MsgHandler::getWarningInstance()->clear();
                MsgHandler::getMessageInstance()->clear();
//                initDevices();
                SUMOFrame::setMSGlobals(oc);
                net = static_cast<GUINet*>(
                    builder->buildNetworkFromDescription(
                        buildVehicleControl(), description/*!!!strm.str()*/));
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
    /*
    FXDCWindow dc(myCanvas);
    if(m_img)
    {
        m_img->EmptyImage();
        m_img->DrawGraph(graph);
        m_img->GetFXImage()->render();
        dc.drawImage(m_img->GetFXImage(),0,0);
    }
    */
    return 1;
}

long
GNEImageProcWindow::onCmdShowGraphOnActualBitmap(FXObject*,FXSelector,void*)
{
    m_img->DrawGraph(myGraph);
    m_img->GetFXImage()->render();
    myImageViewer->update();
/*
    FXDCWindow dc(myCanvas);
    if(m_img)
    {
        m_img->DrawGraph(graph);
        m_img->GetFXImage()->render();
        dc.drawImage(m_img->GetFXImage(),0,0);
    }
    */
    return 1;
}

long
GNEImageProcWindow::onCmdReduceVertexes(FXObject*,FXSelector,void*)
{
    /*
    graph.Reduce();
    */
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
    myGraph.GetTraces(1,1000);
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


