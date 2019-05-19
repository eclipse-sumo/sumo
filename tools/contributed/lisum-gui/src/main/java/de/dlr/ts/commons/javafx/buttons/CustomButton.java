/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2014-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    CustomButton.java
/// @author  Maximiliano Bottazzi
/// @date    2014
/// @version $Id$
///
//
/****************************************************************************/
package de.dlr.ts.commons.javafx.buttons;

import javafx.scene.Node;
import javafx.scene.control.Button;
import javafx.scene.control.Tooltip;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class CustomButton extends Button {

    public CustomButton() {
    }

    public CustomButton(String text) {
        super(text);
    }

    public CustomButton(String text, Node graphic) {
        super(text, graphic);
    }

    public CustomButton(String imageName, String toolTipText) {
        //super("", IconsAndMarkers.instance.getIconImageView(imageName, 16));
        setTooltip(new Tooltip(toolTipText));
    }
}
