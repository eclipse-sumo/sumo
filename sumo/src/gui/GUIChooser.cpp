#include <string>
#include <vector>
#include <qmainwindow.h>
#include <qlistbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qstring.h>
#include "GUISUMOView.h"
#include "GUIChooser.h"

using namespace std;

GUIChooser::GUIChooser(GUISUMOView *parent, ChooseableArtifact type,
                       std::vector<std::string> &names)
    : QMainWindow(0, (string("Chooser")).c_str(), WType_Modal),
    _type(type), _parent(parent)
{
    setFixedSize(300, 300);
    QHBoxLayout *hbox = new QHBoxLayout( this );
    hbox->setSpacing(4);
    // build the list
    buildList(hbox, names);
    // build the buttons
    buildButtons(hbox);
}


GUIChooser::~GUIChooser()
{
}

void
GUIChooser::buildList(QHBoxLayout *hbox, std::vector<std::string> &names)
{
    _list = new QListBox(this, "items", 0);
    hbox->addWidget(_list);
    for(std::vector<std::string>::iterator
        i=names.begin(); i!=names.end(); i++) {
        _list->insertItem(QString((*i).c_str()));
    }
}

void
GUIChooser::buildButtons(QHBoxLayout *hbox)
{
    // build the layout
    QGridLayout *layout = new QGridLayout( this, 3, 1, 4, 4 );
    hbox->addItem(layout);
    // build the "OK"-button
    QPushButton *button = new QPushButton(QString("OK"), this, "OK");
    connect( button, SIGNAL( clicked() ), this, SLOT( pressedOK() ) );
    layout->addWidget( button, 0, 0 );
    // build the "Cancel"-button
    button = new QPushButton(QString("Cancel"), this, "Cancel");
    connect( button, SIGNAL( clicked() ), this, SLOT( pressedCancel() ) );
    layout->addWidget( button, 1, 0 );
    // add the spacer
}


void
GUIChooser::pressedOK()
{
    _id = string(_list->currentText().ascii());
    close(FALSE);
}


void
GUIChooser::pressedCancel()
{
    _id = "";
    close(FALSE);
}


void
GUIChooser::closeEvent ( QCloseEvent *e )
{
    if(_id.length()!=0) {
        _parent->setView(_type, _id);
    }
    QMainWindow::closeEvent(e);
}


std::string
GUIChooser::getID() const
{
    return _id;
}
