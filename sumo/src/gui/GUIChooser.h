#ifndef GUIChooser_h
#define GUIChooser_h
//---------------------------------------------------------------------------//
//                        GUIChooser.h -
//  Class for the window that allows to choose a street, junction or vehicle
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
// Revision 1.2  2003/02/07 10:34:14  dkrajzew
// files updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <qmainwindow.h>
#include <string>
#include <vector>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUISUMOView;
class QHBoxLayout;
class QListBox;


/* =========================================================================
 * class definition
 * ======================================================================= */
/**
 * Instances of this class are windows that display the list of instances
 * from a given artifact like vehicles, edges or junctions and allow
 * one of their items
 */
class GUIChooser : public QMainWindow
{
    /// is a q-object
    Q_OBJECT
public:
    /**
     * ChooseableArtifact
     * Enumeration to differ to show the list of which atrifact
     * By now, one can display edges, junctions and vehicles
     */
    enum ChooseableArtifact {
        /// display junctions
        CHOOSEABLE_ARTIFACT_JUNCTIONS,
        /// display edges
        CHOOSEABLE_ARTIFACT_EDGES,
        /// display vehicles
        CHOOSEABLE_ARTIFACT_VEHICLES
    };

public:
    /// constructor
    GUIChooser(GUISUMOView *parent, ChooseableArtifact type,
        std::vector<std::string> &names);

    /// destructor
    ~GUIChooser();

    /// returns the id of the chosen artifact; 0 if none was chosen
    std::string getID() const;

    /// catched as a message is reported to the parent of this window
    void closeEvent ( QCloseEvent * );

public slots:
    /// called when the user presses the "Ok"-button
    void pressedOK();

    /// called when the user presses the "Cancel"-button
    void pressedCancel();

private:
    /// builds the list of artifact ids
    void buildList(QHBoxLayout *vbox, std::vector<std::string> &names);

    /// builds the buttons
    void buildButtons(QHBoxLayout *vbox);

private:
    /// the QListBox that holds the ids
    QListBox *_list;

    /// the chosen id
    std::string _id;

    /// the artifact to choose
    ChooseableArtifact _type;

    /// the parent window
    GUISUMOView *_parent;
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIChooser.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

