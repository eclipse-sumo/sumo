/****************************************************************************
** Form implementation generated from reading ui file 'd:\projects\sumo\sumo_about.ui'
**
** Created: Mon Mar 10 14:41:46 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "QAboutSUMO.h"

#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

static const char* const image0_data[] = { 
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
".........####################a#a#a#a#a#.",
"........##############################..",
".......###.....###.......###......###...",
"......###......##.......###......###....",
".....###......##.......###......###.....",
"....###......##.......###......###......",
"...###......##a......a#a......###.......",
"..#####a#a#####a#a#a####a##a####........",
".#############################a.........",
"...........##.....##a...................",
"...........#a....a##....................",
"...........#a...###.....................",
"...........#a..###......................",
"...........#a.a##.......................",
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
"........................................"};

static const char* const image1_data[] = { 
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
"........................................"};


/* 
 *  Constructs a QAboutSUMO which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
QAboutSUMO::QAboutSUMO( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    QPixmap image0( ( const char** ) image0_data );
    QPixmap image1( ( const char** ) image1_data );
    if ( !name )
	setName( "QAboutSUMO" );
    resize( 271, 186 ); 
    setProperty( "caption", tr( "Form1" ) );

    PixmapLabel2 = new QLabel( this, "PixmapLabel2" );
    PixmapLabel2->setGeometry( QRect( 10, 10, 40, 40 ) ); 
    PixmapLabel2->setProperty( "pixmap", image0 );
    PixmapLabel2->setProperty( "scaledContents", QVariant( TRUE, 0 ) );

    PixmapLabel3 = new QLabel( this, "PixmapLabel3" );
    PixmapLabel3->setGeometry( QRect( 60, 10, 40, 40 ) ); 
    PixmapLabel3->setProperty( "pixmap", image1 );
    PixmapLabel3->setProperty( "scaledContents", QVariant( TRUE, 0 ) );

    TextLabel5 = new QLabel( this, "TextLabel5" );
    TextLabel5->setGeometry( QRect( 10, 60, 260, 30 ) ); 
    TextLabel5->setProperty( "text", tr( "A microscopic, multi-modal, open-source road traffic \nsimulation." ) );

    TextLabel2 = new QLabel( this, "TextLabel2" );
    TextLabel2->setGeometry( QRect( 10, 90, 230, 30 ) ); 
    TextLabel2->setProperty( "text", tr( "(c) Institute for Transport Research/DLR & ZAIK\n	2000-2003" ) );

    PushButton1 = new QPushButton( this, "PushButton1" );
    connect( PushButton1, SIGNAL(clicked()), this, SLOT(close()) );
    PushButton1->setGeometry( QRect( 90, 150, 93, 26 ) ); 
    PushButton1->setProperty( "text", tr( "OK" ) );

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 130, 10, 120, 20 ) ); 
    QFont TextLabel1_font(  TextLabel1->font() );
    TextLabel1_font.setFamily( "Arial" );
    TextLabel1_font.setPointSize( 16 );
    TextLabel1_font.setBold( TRUE );
    TextLabel1->setFont( TextLabel1_font ); 
    TextLabel1->setProperty( "text", tr( "SUMO v0.8" ) );

    TextLabel3 = new QLabel( this, "TextLabel3" );
    TextLabel3->setGeometry( QRect( 120, 30, 140, 20 ) ); 
    TextLabel3->setProperty( "text", tr( "Simulation of Urban MObility" ) );

    TextLabel4 = new QLabel( this, "TextLabel4" );
    TextLabel4->setGeometry( QRect( 70, 120, 133, 13 ) ); 
    TextLabel4->setProperty( "text", tr( "http://sumo.sourceforge.net" ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
QAboutSUMO::~QAboutSUMO()
{
    // no need to delete child widgets, Qt does it all for us
}

/*  
 *  Main event handler. Reimplemented to handle application
 *  font changes
 */
bool QAboutSUMO::event( QEvent* ev )
{
    bool ret = QDialog::event( ev ); 
    if ( ev->type() == QEvent::ApplicationFontChange ) {
	QFont TextLabel1_font(  TextLabel1->font() );
	TextLabel1_font.setFamily( "Arial" );
	TextLabel1_font.setPointSize( 16 );
	TextLabel1_font.setBold( TRUE );
	TextLabel1->setFont( TextLabel1_font ); 
    }
    return ret;
}

