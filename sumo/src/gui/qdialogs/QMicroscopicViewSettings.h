/****************************************************************************
** Form interface generated from reading ui file 'MicroscopicViewSettings.ui'
**
** Created: Mon Aug 18 17:19:19 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef QMICROSCOPICVIEWSETTINGS_H
#define QMICROSCOPICVIEWSETTINGS_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QCheckBox;
class QComboBox;
class QLabel;
class QPushButton;
class QTabWidget;
class QWidget;

class QMicroscopicViewSettings : public QDialog
{
    Q_OBJECT

public:
    QMicroscopicViewSettings( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~QMicroscopicViewSettings();

    QPushButton* PushButton15;
    QPushButton* PushButton16;
    QTabWidget* TabWidget5;
    QWidget* tab;
    QCheckBox* CheckBox5;
    QLabel* TextLabel11;
    QComboBox* ComboBox1;
    QWidget* tab_2;
    QCheckBox* CheckBox7;
    QCheckBox* CheckBox6;
    QWidget* tab_3;
    QLabel* TextLabel12;
    QComboBox* ComboBox2;
    QWidget* tab_4;
    QLabel* TextLabel13;
    QComboBox* ComboBox3;

};

#endif // QMICROSCOPICVIEWSETTINGS_H
