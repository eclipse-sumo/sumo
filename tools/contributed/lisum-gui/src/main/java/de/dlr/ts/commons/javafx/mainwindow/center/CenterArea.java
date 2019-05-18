/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2016-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    CenterArea.java
/// @author  Maximiliano Bottazzi
/// @date    2016
/// @version $Id$
///
//
/****************************************************************************/
package de.dlr.ts.commons.javafx.mainwindow.center;

import de.dlr.ts.commons.javafx.mainwindow.Colors;
import javafx.scene.Node;
import javafx.scene.layout.AnchorPane;


/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class CenterArea {
    private AnchorPane anchorPane = new AnchorPane();


    /**
     *
     */
    public CenterArea() {
        String col = Colors.getInstance().DARK_GRAY;

        anchorPane.setStyle("-fx-border-color: " + col + "; -fx-border-insets: 4;");


        /**
        AnchorPane.setTopAnchor(anchorPane, 2.);
        AnchorPane.setBottomAnchor(anchorPane, 2.);
        AnchorPane.setLeftAnchor(anchorPane, 2.);
        AnchorPane.setRightAnchor(anchorPane, 2.);
        BorderPane.setMargin(anchorPane, new Insets(3.));
        **/
    }

    /**
     *
     * @param node
     */
    public void set(Node node) {
        AnchorPane.setTopAnchor(node, 0.);
        AnchorPane.setBottomAnchor(node, 0.);
        AnchorPane.setLeftAnchor(node, 0.);
        AnchorPane.setRightAnchor(node, 0.);

        anchorPane.getChildren().add(node);
    }

    /**
     *
     * @return
     */
    public Node getNode() {
        return anchorPane;
    }
}
