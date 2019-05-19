/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2016-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    RightArea.java
/// @author  Maximiliano Bottazzi
/// @date    2016
/// @version $Id$
///
//
/****************************************************************************/
package de.dlr.ts.commons.javafx.mainwindow.right;

import javafx.scene.Node;
import javafx.scene.layout.AnchorPane;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class RightArea {
    private AnchorPane anchorPane = new AnchorPane();


    /**
     *
     */
    public RightArea() {
        anchorPane.setStyle("-fx-background-color: #989898; -fx-background-color: #989898;");
    }

    public void setWidth(int width) {
        this.anchorPane.setPrefWidth(width);
    }

    /**
     *
     * @return
     */
    public Node getNode() {
        return anchorPane;
    }
}
