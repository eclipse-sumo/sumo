/****************************************************************************
** Form interface generated from reading ui file 'ApplicationSettings.ui'
**
** Created: Mon Aug 18 17:19:19 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef QAPPLICATIONSETTINGS_H
#define QAPPLICATIONSETTINGS_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QCheckBox;
class QPushButton;

class QApplicationSettings : public QDialog
{
    Q_OBJECT

public:
    QApplicationSettings( QWidget* parent, bool &quitOnEnd);
    ~QApplicationSettings();

    QCheckBox* CheckBox1;
    QPushButton* PushButton2;
    QPushButton* PushButton1;

public slots:
    virtual void pressedCancel();
    virtual void pressedOK();
private:
    bool &myAppQuitOnEnd;
};

#endif // QAPPLICATIONSETTINGS_H
