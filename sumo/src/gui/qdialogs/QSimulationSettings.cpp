/****************************************************************************
** Form implementation generated from reading ui file 'SimulationSettings.ui'
**
** Created: Tue Aug 19 11:19:33 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "QSimulationSettings.h"

#include <string>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qtabwidget.h>
#include <qtoolbutton.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qfiledialog.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsSaveStorage.h>
#include <utils/common/StringTokenizer.h>
#include <utils/convert/TplConvert.h>
#include <utils/convert/ToString.h>
#include <qmessagebox.h>

using namespace std;

/*
 *  Constructs a QSimulationSettings which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
QSimulationSettings::QSimulationSettings( QWidget* parent)
    : QDialog( parent, 0, TRUE, 0 )
{
	setName( "QSimulationSettings" );
    resize( 301, 195 );
    setProperty( "sizePolicy", QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, sizePolicy().hasHeightForWidth() ) );
    setProperty( "minimumSize", QSize( 301, 195 ) );
    setProperty( "maximumSize", QSize( 301, 195 ) );
    setProperty( "caption", tr( "Simulation Settings" ) );

    PushButton9 = new QPushButton( this, "PushButton9" );
    PushButton9->setGeometry( QRect( 40, 160, 80, 26 ) );
    PushButton9->setProperty( "text", tr( "OK" ) );

    PushButton10 = new QPushButton( this, "PushButton10" );
    PushButton10->setGeometry( QRect( 170, 160, 80, 26 ) );
    PushButton10->setProperty( "text", tr( "Cancel" ) );

    TabWidget2 = new QTabWidget( this, "TabWidget2" );
    TabWidget2->setGeometry( QRect( 10, 0, 280, 150 ) );
    TabWidget2->setProperty( "sizePolicy", QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, TabWidget2->sizePolicy().hasHeightForWidth() ) );

    tab = new QWidget( TabWidget2, "tab" );
    TextLabel2 = new QLabel( tab, "TextLabel2" );
    TextLabel2->setGeometry( QRect( 8, 47, 100, 20 ) );
    TextLabel2->setProperty( "text", tr( "Simulation Begin:" ) );
    BeginSpin = new QSpinBox( tab, "BeginSpin" );
    BeginSpin->setGeometry( QRect( 110, 50, 54, 21 ) );
    QWhatsThis::add(  BeginSpin, tr( "Sets the time the simulation begins at" ) );
    BeginSpin->setValue(getIntOptionSecure("b"));
    BeginSpin->setMinValue(0);
    BeginSpin->setMaxValue(186400);
    TextLabel3 = new QLabel( tab, "TextLabel3" );
    TextLabel3->setGeometry( QRect( 8, 77, 80, 20 ) );
    TextLabel3->setProperty( "text", tr( "Simulation End:" ) );
    EndSpin = new QSpinBox( tab, "EndSpin" );
    EndSpin->setGeometry( QRect( 110, 80, 54, 21 ) );
    EndSpin->setMinValue(0);
    EndSpin->setMaxValue(186400);
    QWhatsThis::add(  EndSpin, tr( "Sets the time the simulation ends at" ) );
    EndSpin->setValue(getIntOptionSecure("e"));
    TextLabel1 = new QLabel( tab, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 10, 10, 100, 20 ) );
    TextLabel1->setProperty( "text", tr( "Configuration Name:" ) );
    ConfigurationNameEdit = new QLineEdit( tab, "ConfigurationNameEdit" );
    ConfigurationNameEdit->setGeometry( QRect( 110, 10, 130, 22 ) );
    ConfigurationNameEdit->setText(getStringOptionSecure("c").c_str());
    ConfigurationNameChooser = new QToolButton( tab, "ConfigurationNameChooser" );
    ConfigurationNameChooser->setGeometry( QRect( 250, 10, 20, 20 ) );
    ConfigurationNameChooser->setProperty( "text", tr( "..." ) );

    TabWidget2->insertTab( tab, tr( "Main Settings" ) );
    tab_2 = new QWidget( TabWidget2, "tab_2" );
    TextLabel3_2 = new QLabel( tab_2, "TextLabel3_2" );
    TextLabel3_2->setGeometry( QRect( 10, 70, 70, 20 ) );
    TextLabel3_2->setProperty( "text", tr( "Additional file" ) );
    TextLabel2_2 = new QLabel( tab_2, "TextLabel2_2" );
    TextLabel2_2->setGeometry( QRect( 10, 40, 70, 20 ) );
    TextLabel2_2->setProperty( "text", tr( "Routes file" ) );
    TextLabel1_2 = new QLabel( tab_2, "TextLabel1_2" );
    TextLabel1_2->setGeometry( QRect( 10, 10, 70, 20 ) );
    TextLabel1_2->setProperty( "text", tr( "Network file" ) );
    NetworkNameEdit = new QLineEdit( tab_2, "NetworkNameEdit" );
    NetworkNameEdit->setGeometry( QRect( 80, 10, 150, 22 ) );
    NetworkNameEdit->setText(getStringOptionSecure("n").c_str());
    NetworkNameChooser = new QToolButton( tab_2, "NetworkNameChooser" );
    NetworkNameChooser->setGeometry( QRect( 240, 10, 20, 20 ) );
    NetworkNameChooser->setProperty( "text", tr( "..." ) );
    RoutesNameEdit = new QLineEdit( tab_2, "RoutesNameEdit" );
    RoutesNameEdit->setGeometry( QRect( 80, 40, 150, 22 ) );
    RoutesNameEdit->setText(getStringOptionSecure("r").c_str());
    TabWidget2->insertTab( tab_2, tr( "Input File Settings" ) );
    RoutesNameChooser = new QToolButton( tab_2, "RoutesNameChooser" );
    RoutesNameChooser->setGeometry( QRect( 240, 40, 20, 20 ) );
    RoutesNameChooser->setProperty( "text", tr( "..." ) );
    AdditionalNameEdit = new QLineEdit( tab_2, "AdditionalNameEdit" );
    AdditionalNameEdit->setGeometry( QRect( 80, 70, 150, 22 ) );
    AdditionalNameEdit->setText(getStringOptionSecure("a").c_str());
    AdditionalNameChooser = new QToolButton( tab_2, "AdditionalNameChooser" );
    AdditionalNameChooser->setGeometry( QRect( 240, 70, 20, 20 ) );
    AdditionalNameChooser->setProperty( "text", tr( "..." ) );

    tab_3 = new QWidget( TabWidget2, "tab_3" );
    TextLabel4_2 = new QLabel( tab_3, "TextLabel4_2" );
    TextLabel4_2->setGeometry( QRect( 10, 10, 90, 20 ) );
    TextLabel4_2->setProperty( "text", tr( "Raw output name" ) );
    TextLabel6 = new QLabel( tab_3, "TextLabel6" );
    TextLabel6->setGeometry( QRect( 10, 80, 80, 20 ) );
    TextLabel6->setProperty( "text", tr( "Dump intervals" ) );
    TextLabel5 = new QLabel( tab_3, "TextLabel5" );
    TextLabel5->setGeometry( QRect( 10, 50, 110, 20 ) );
    TextLabel5->setProperty( "text", tr( "Dump basename" ) );
    DumpNameEdit = new QLineEdit( tab_3, "DumpNameEdit" );
    DumpNameEdit->setGeometry( QRect( 110, 50, 120, 22 ) );
    DumpNameEdit->setText(getStringOptionSecure("dump-basename").c_str());
    DumpBaseChooser = new QToolButton( tab_3, "DumpBaseChooser" );
    DumpBaseChooser->setGeometry( QRect( 240, 50, 20, 20 ) );
    DumpBaseChooser->setProperty( "text", tr( "..." ) );
    DumpIntervalsEdit = new QLineEdit( tab_3, "DumpIntervalsEdit" );
    DumpIntervalsEdit->setGeometry( QRect( 110, 80, 120, 22 ) );
    DumpNameEdit->setText(getStringOptionSecure("dump-intervals").c_str());
    RawNameEdit = new QLineEdit( tab_3, "RawNameEdit" );
    RawNameEdit->setGeometry( QRect( 110, 10, 120, 22 ) );
    RawNameEdit->setText(getStringOptionSecure("o").c_str());
    RawNameChooser = new QToolButton( tab_3, "RawNameChooser" );
    RawNameChooser->setGeometry( QRect( 240, 10, 20, 20 ) );
    RawNameChooser->setProperty( "text", tr( "..." ) );
    TabWidget2->insertTab( tab_3, tr( "Output File Settings" ) );

    tab_4 = new QWidget( TabWidget2, "tab_4" );
    PushButton3 = new QPushButton( tab_4, "PushButton3" );
    PushButton3->setGeometry( QRect( 10, 40, 93, 26 ) );
    PushButton3->setProperty( "text", tr( "Edit Actions" ) );
    EnableActions = new QCheckBox( tab_4, "EnableActions" );
    EnableActions->setGeometry( QRect( 10, 10, 100, 20 ) );
    EnableActions->setProperty( "text", tr( "Enable Actions" ) );
    TabWidget2->insertTab( tab_4, tr( "Actions" ) );
    // signals and slots connections
    connect( PushButton9, SIGNAL( clicked() ), this, SLOT( pressedOK() ) );
    connect( PushButton10, SIGNAL( clicked() ), this, SLOT( pressedCancel() ) );
    connect( ConfigurationNameChooser, SIGNAL( clicked() ), this, SLOT( pressedConfigChooser() ) );
    connect( NetworkNameChooser, SIGNAL( clicked() ), this, SLOT( pressedNetworkChooser() ) );
    connect( RoutesNameChooser, SIGNAL( clicked() ), this, SLOT( pressedRoutesChooser() ) );
    connect( AdditionalNameChooser, SIGNAL( clicked() ), this, SLOT( pressedAdditionalChooser() ) );
    connect( RawNameChooser, SIGNAL( clicked() ), this, SLOT( pressedRawChooser() ) );
    connect( DumpBaseChooser, SIGNAL( clicked() ), this, SLOT( pressedDumpChooser() ) );
    connect( PushButton3, SIGNAL( clicked() ), this, SLOT( pressedEditActions() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
QSimulationSettings::~QSimulationSettings()
{
}

void
QSimulationSettings::pressedDumpChooser()
{
    QString fn =
        QFileDialog::getOpenFileName( QString::null, QString("*.*"), this );
    if ( fn.isEmpty() ) {
        return;
    }
    DumpNameEdit->setText(fn);
}


void
QSimulationSettings::pressedAdditionalChooser()
{
    QString fn =
        QFileDialog::getOpenFileName( QString::null, QString("*.add.xml"), this );
    if ( fn.isEmpty() ) {
        return;
    }
    AdditionalNameEdit->setText(fn);
}


void
QSimulationSettings::pressedCancel()
{
    close();
}


void
QSimulationSettings::pressedConfigChooser()
{
    QString fn =
        QFileDialog::getSaveFileName( QString::null, QString("*.sumo.cfg"), this );
    if ( fn.isEmpty() ) {
        return;
    }
    ConfigurationNameEdit->setText(fn);
}


void
QSimulationSettings::pressedEditActions()
{
    qWarning( "QSimulationSettings::pressedEditActions(): Not implemented yet!" );
}


void
QSimulationSettings::pressedNetworkChooser()
{
    QString fn =
        QFileDialog::getOpenFileName( QString::null, QString("*.net.xml"), this );
    if ( fn.isEmpty() ) {
        return;
    }
    NetworkNameEdit->setText(fn);
}


void
QSimulationSettings::pressedOK()
{
    // check whether something must be done
    if(changed()) {
        // check the inputs
        if(!validate()) {
            return;
        }
        // try to save new settings
        if(!procSaveSettings()) {
            return;
        }
    }
    close();
}


void
QSimulationSettings::pressedRawChooser()
{
    QString fn =
        QFileDialog::getOpenFileName( QString::null, QString("*.out.xml"), this );
    if ( fn.isEmpty() ) {
        return;
    }
    RawNameEdit->setText(fn);
}


void
QSimulationSettings::pressedRoutesChooser()
{
    QString fn =
        QFileDialog::getOpenFileName( QString::null, QString("*.rou.xml"), this );
    if ( fn.isEmpty() ) {
        return;
    }
    RoutesNameEdit->setText(fn);
}



bool
QSimulationSettings::changed()
{
    OptionsCont &oc = OptionsSubSys::getOptions();
    return
        changed("b", BeginSpin->value()) ||
        changed("e", EndSpin->value()) ||
        changed("c", string(ConfigurationNameEdit->text().ascii())) ||
        changed("n", string(NetworkNameEdit->text().ascii())) ||
        changed("o", string(RawNameEdit->text().ascii())) ||
        changed("r", string(RoutesNameEdit->text().ascii())) ||
        changed("a", string(AdditionalNameEdit->text().ascii())) ||
        changed("dump-basename", string(DumpNameEdit->text().ascii())) ||
        changed("dump-intervals", string(DumpIntervalsEdit->text().ascii()));
}


bool
QSimulationSettings::changed(const std::string &name, int val)
{
    int old = getIntOptionSecure(name);
    return old!=val;
}


bool
QSimulationSettings::changed(const std::string &name,
                             const std::string &val)
{
    string old = getStringOptionSecure(name);
    return old!=val;
}




bool
QSimulationSettings::validate()
{
    // check begin/end
    if(BeginSpin->value()>=EndSpin->value()) {
        QMessageBox *myBox = new QMessageBox("False parameter!",
            "The begin value of a simulation should be smaller than his end.",
            QMessageBox::Warning,
            QMessageBox::Ok | QMessageBox::Default,
            QMessageBox::NoButton, QMessageBox::NoButton);
        return false;
    }
    // check dump interval
    string di = string(DumpIntervalsEdit->text().ascii());
    if(di.length()!=0) {
        StringTokenizer st(di, ";");
        while(st.hasNext()) {
            try {
                int t = TplConvert<char>::_2int(st.next().c_str());
            } catch (NumberFormatException e) {
                QMessageBox *myBox = new QMessageBox("False parameter!",
                    "The dump intervals should be a list of integers separated by ';'.",
                    QMessageBox::Warning,
                    QMessageBox::Ok | QMessageBox::Default,
                    QMessageBox::NoButton, QMessageBox::NoButton);
                return false;
            }
        }
    }
    return true;
}


bool
QSimulationSettings::procSaveSettings()
{
    OptionsCont &oc = OptionsSubSys::getOptions();
    if(string(ConfigurationNameEdit->text().ascii())==oc.getString("c")) {
        // check whether the user wants to overwrite an existing config
        QMessageBox mb( "Configuration changed!",
            "Do you want to overwrite the existing configuration?\n"
            "Pressing 'No' will discard the changes!",
            QMessageBox::Information,
            QMessageBox::Yes | QMessageBox::Default,
            QMessageBox::No,
            QMessageBox::Cancel | QMessageBox::Escape );
/*    mb.setButtonText( QMessageBox::Yes, "Yes" );
    mb.setButtonText( QMessageBox::No, "No" );*/
        switch( mb.exec() ) {
        case QMessageBox::Yes:
            saveNewConfig();
            break;
        case QMessageBox::No:
            return true;
            break;
        case QMessageBox::Cancel:
            return false;
            break;
        }
    }
    return true;
}



std::string
QSimulationSettings::getStringOptionSecure(const std::string &name)
{
    OptionsCont &oc = OptionsSubSys::getOptions();
    if(!oc.isSet(name)) {
        return "";
    }
    return oc.getString(name);
}


int
QSimulationSettings::getIntOptionSecure(const std::string &name)
{
    OptionsCont &oc = OptionsSubSys::getOptions();
    if(!oc.isSet(name)) {
        return 0;
    }
    return oc.getInt(name);
}


bool
QSimulationSettings::saveNewConfig()
{
    OptionsSaveStorage os("files;simulation");
    os.insert("files.net-files", string(NetworkNameEdit->text().ascii()));
    os.insert("files.route-files", string(RoutesNameEdit->text().ascii()));
    os.insert("files.additional-files", string(AdditionalNameEdit->text().ascii()));
    os.insert("files.output-file", string(RawNameEdit->text().ascii()));
    os.insert("files.dump-intervals", string(DumpIntervalsEdit->text().ascii()));
    os.insert("files.dump-basename", string(DumpNameEdit->text().ascii()));

    os.insert("simulation.begin", toString<int>(BeginSpin->value()));
    os.insert("simulation.end", toString<int>(EndSpin->value()));

    return os.save(string(ConfigurationNameEdit->text().ascii()));
}

