/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2014-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    OkApplyCancelMessage.java
/// @author  Maximiliano Bottazzi
/// @date    2014
/// @version $Id$
///
//
/****************************************************************************/
package de.dlr.ts.commons.javafx.messages;

import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.scene.control.Button;
import javafx.scene.layout.HBox;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class OkApplyCancelMessage extends HBox {
    public final Button okButton = new Button("Ok");
    public final Button applyButton = new Button("Apply");
    public final Button cancelButton = new Button("Cancel");

    public OkApplyCancelMessage() {
        this.getChildren().addAll(okButton, applyButton, cancelButton);
        this.setSpacing(10.);
    }

    /**
     *
     * @param action
     */
    public void okPressed(final Action action) {
        pressed(okButton, action);
    }

    public void applyPressed(final Action action) {
        pressed(applyButton, action);
    }

    public void cancelPressed(final Action action) {
        pressed(cancelButton, action);
    }

    /**
     *
     * @param button
     * @param action
     */
    private void pressed(final Button button, final Action action) {
        button.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {
                action.doIt();
            }
        });
    }

    /**
     *
     */
    public static interface Action {
        void doIt();
    }
}
