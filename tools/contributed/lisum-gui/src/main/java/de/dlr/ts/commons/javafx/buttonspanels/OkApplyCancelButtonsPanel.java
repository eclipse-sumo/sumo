/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2016-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    OkApplyCancelButtonsPanel.java
/// @author  Maximiliano Bottazzi
/// @date    2016
/// @version $Id$
///
//
/****************************************************************************/
package de.dlr.ts.commons.javafx.buttonspanels;

import javafx.scene.control.Button;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class OkApplyCancelButtonsPanel extends OkCancelButtonsPanel {

    private Button applyButton = new Button("Apply");

    /**
     *
     */
    public OkApplyCancelButtonsPanel() {
        addButton(1, applyButton);
    }

    /**
     *
     * @return
     */
    public Button getApplyButton() {
        return applyButton;
    }
}
