/****************************************************************************/
/// @file    GUIDialog_AboutSUMO.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 08.03.2004
/// @version $Id$
///
// The application's "About" - dialog
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include "GUIDialog_AboutSUMO.h"
#include <utils/gui/windows/GUIAppGlobals.h>
#include <utils/foxtools/FXLinkLabel.h>
#include <utils/gui/images/GUIIconSubSys.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// bitmap definitions
// ===========================================================================
static const char* dlr_icon[] = {
    "40 42 3 1",
    "# c #000000",
    "a c #333333",
    ". c #ffffff",
    "........................................",
    "........................................",
    "...........................#............",
    "..........................##............",
    ".........................###............",
    "........................####............",
    ".......................#####............",
    "......................###.##............",
    ".....................###..##............",
    "....................###...##............",
    "...................###....##............",
    "..................###.....##............",
    ".........##############################.",
    "........##############################..",
    ".......###.....###.......###......###...",
    "......###.....###.......###......###....",
    ".....###.....###.......###......###.....",
    "....###.....###.......###......###......",
    "...###.....###.......###......###.......",
    "..##############################........",
    ".##############################.........",
    "...........##.....###...................",
    "...........##....###....................",
    "...........##...###.....................",
    "...........##..###......................",
    "...........##.###.......................",
    "...........#####........................",
    "...........####.........................",
    "...........###..........................",
    "...........##...........................",
    "...........#...#####....##.....#####....",
    "...............######...##.....######...",
    "...............##..###..##.....##..##...",
    "...............##...##..##.....##..##...",
    "...............##...##..##.....#####....",
    "...............##...##..##.....#####....",
    "...............##..###..##.....##.###...",
    "...............######...#####..##..##...",
    "...............#####....#####..##..###..",
    "........................................",
    "........................................",
    "........................................"
};

static const char* zaik_icon[] = {
    "40 42 26 1",
    "k c #000033",
    "j c #000066",
    "s c #003333",
    "v c #003366",
    "i c #330033",
    "a c #330066",
    "t c #333333",
    "# c #333366",
    "w c #333399",
    "e c #663366",
    "p c #663399",
    "o c #666666",
    "d c #666699",
    "g c #996699",
    "b c #999999",
    "f c #9999cc",
    "u c #99cccc",
    "n c #cc99cc",
    "c c #cccccc",
    "h c #ccccff",
    "x c #ccffcc",
    "r c #ccffff",
    "m c #ffcccc",
    "l c #ffccff",
    "q c #ffffcc",
    ". c #ffffff",
    "........................................",
    "..######abc#########a###ad.ba#########..",
    "..#a#aefcg#a#a#aaechb#a##id.e#a#a#a#a#..",
    "..###ab...###a###b...de##jafdka##a####..",
    "..#a#ab..hdlcbfmfb..cdc.cbd#nf##a##a#a..",
    "..#####bfoab...hejgbe##pbcbf..f##a####..",
    "..##a#apb#f.daoc.d#f##a#jaac..ci##a#a#..",
    "..#a##jb.nqda#a#b.b.#a#####dccda#a####..",
    "..###aib.h#a###aad.l##a#a#ai#g#####a#a..",
    "..a#a##c.e##a####kd.ba###a###rea#a##a#..",
    "..ei#ab..#a##a#a#ad..dia###a#cda##a###..",
    "..cdj#hg.d##a####abnbc##a#a##lds#a##a#..",
    "..cbede#hbk###a#atuf#bp####a#.#a##a###..",
    "..#d..bjbca#a##a##ldjd..di##er##a##a##..",
    "..af...a#.oa#a##adc##c..ua#jgca##a##a#..",
    "..#oh.bbfcuatjta#clcfg..ga#acfa#a##a##..",
    "..#a#eadcl.cfnucc.cbe#dda#t#.o####a##a..",
    "..#####a##b.fbbn.daj#jbb#ajbca#a#a##a#..",
    "..##a#a##a#lbjafua###agc###lbva###a###..",
    "..#a####a###dfbd#a##a#dh#ifc#a###a##a#..",
    "..e#a#a##akac..c##a##idqab.da##a###a##..",
    "..c##i#ak#bbf..h#a#a###dd.da#a##a#a###..",
    "..ggcb#ofmhdefcdi###a#ncddi###a#####a#..",
    "..#...dlhd##a##a##atjb..ca##a##a#a#a##..",
    "..e...dd#ia##a#a#as#pb..c#a##a####a##a..",
    "..#bhbdmcfo######dnrcpucda#a##a#a##a##..",
    "..#a#a#wbchlcccc.cnd##aaivi##ji##a##a#..",
    "..######ja#odgdd###i#a##dbd#ebd#a##a##..",
    "..#a#a#a###aava#a#a####af..ab.ba##a###..",
    "..##a###a#a##a#####aa#atb.cjb.fi#a#a##..",
    "..####a#######a#aa#####a#d#ab.bj####a#..",
    "..#a#a##idlxlccb#bc..cejbccab.fab.c###..",
    "..#####a#dbfc..bp.hfc.uab.cab.bd..pa#a..",
    "..#a#a##a#aiu.h##o##d.cab.cjb.fc.dt###..",
    "..####a###jb.re#agc...caf.cab.l..#a#a#..",
    "..##a##a#ae..gjad..cc.cjb.cab.fc.b#a##..",
    "..#a##a###c.nvi#b.fkd.nif.cjb.bd..###a..",
    "..##a##aib..bdbpf.cpf.r#b.cab.fif.ca##..",
    "..###a###f.....bol...c.#b.cjb.ba#c.bi#..",
    "..a#a###a#######a#dd##da##############..",
    "..##t#a########a###i##a###a##a##a#####..",
    "........................................"
};


// ===========================================================================
// method definitions
// ===========================================================================
GUIDialog_AboutSUMO::GUIDialog_AboutSUMO(FXWindow* parent,  const char* name,
        int x, int y)
        : FXDialogBox(parent, name, DECOR_CLOSE|DECOR_TITLE, x, y, 0, 0)
{
    FXVerticalFrame *f1 =
        new FXVerticalFrame(this,
                            LAYOUT_TOP|FRAME_NONE|LAYOUT_FILL_X,
                            0,0,0,0, 0,0,1,1);
    // build icons
    FXHorizontalFrame *f2 =
        new FXHorizontalFrame(f1,
                              LAYOUT_TOP|LAYOUT_CENTER_X|FRAME_NONE, 0,0,0,0, 0, 0, 1, 1);
    myDLRIcon = new FXXPMIcon(gFXApp, dlr_icon);
    myZAIKIcon = new FXXPMIcon(gFXApp, zaik_icon);
    new FXButton(f2,"\tDLR\t.", myDLRIcon, 0, 0,
                 LAYOUT_CENTER_Y|TEXT_OVER_ICON, 5, 0, 40+5, 0,  0,0,0,0);
    new FXButton(f2,"\tZAIK\t.", myZAIKIcon, 0, 0,
                 LAYOUT_CENTER_Y|TEXT_OVER_ICON, 40+10, 0, 40+5, 0,  0,0,0,0);
    // "SUMO <VERSION>"
    FXVerticalFrame *f4 =
        new FXVerticalFrame(f2, FRAME_NONE, 0,0,0,0,   20,0,0,0);
    FXFont *fnt = new FXFont(gFXApp, "Arial", 18, FONTWEIGHT_BOLD);
    FXLabel *l = new FXLabel(f4, (FXString)"SUMO " + VERSION_STRING, 0,
                             LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_CENTER_X|LABEL_NORMAL,
                             0,0,0,0, 0,0,0,0);
    l->setFont(fnt);
    new FXLabel(f4, "Simulation of Urban MObility", 0,
                LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_CENTER_X|LABEL_NORMAL,
                0,0,0,0, 0,0,0,0);
    //
    // additional infos
    FXVerticalFrame *f3 =
        new FXVerticalFrame(f1,
                            FRAME_NONE,
                            0,0,0,0, 0,0,0,0);
    // copyright notice
    new FXLabel(f3, "A microscopic, multi-modal, open source",
                0, LABEL_NORMAL, 0,0,0,0, 0,0,0,0);
    new FXLabel(f3, "road traffic simulation.",
                0, LABEL_NORMAL, 0,0,0,0, 0,0,0,0);

    new FXLabel(f3, "(c) Institute of Transportation Research/DLR and ZAIK",
                0, LAYOUT_CENTER_X|JUSTIFY_CENTER_X|LABEL_NORMAL, 0,0,0,0, 0,0,5,0);
    new FXLabel(f3, "2000-2007",
                0, LAYOUT_CENTER_X|JUSTIFY_CENTER_X|LABEL_NORMAL, 0,0,0,0, 0,0,0,0);

    FXLinkLabel *link = new FXLinkLabel(f3, "http://sumo.sourceforge.net",
                                        0, LAYOUT_CENTER_X|JUSTIFY_CENTER_X|LABEL_NORMAL, 0,0,0,0, 5,5,5,5);
    link->setTipText("http://sumo.sourceforge.net");
    // ok-button
    new FXButton(f1,"OK\t\t", 0, this, ID_ACCEPT,
                 LAYOUT_FIX_WIDTH|LAYOUT_CENTER_X|JUSTIFY_CENTER_X|FRAME_THICK|FRAME_RAISED,
                 0, 0, 50, 30);
    setIcon(GUIIconSubSys::getIcon(ICON_APP));
}


void
GUIDialog_AboutSUMO::create()
{
    FXDialogBox::create();
    myDLRIcon->create();
    myZAIKIcon->create();
}


GUIDialog_AboutSUMO::~GUIDialog_AboutSUMO()
{
    delete myDLRIcon;
    delete myZAIKIcon;
}



/****************************************************************************/

