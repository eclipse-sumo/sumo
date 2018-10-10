/*
 * Copyright (C) 2016
 * Deutsches Zentrum fuer Luft- und Raumfahrt e.V.
 * Institut fuer Verkehrssystemtechnik
 * 
 * German Aerospace Center
 * Institute of Transportation Systems
 * 
 */
package de.dlr.ts.commons.javafx.mainwindow.bottom;

import javafx.scene.control.TextArea;
import javafx.scene.layout.AnchorPane;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class DebugScreen extends AnchorPane
{    
    private TextArea textArea = new TextArea();


    /**
     * 
     */
    public DebugScreen()
    {
    }
    
    /**
     * 
     */
    public void init()
    {
        this.getChildren().add(textArea);
        AnchorPane.setBottomAnchor(textArea, 0.);
        AnchorPane.setTopAnchor(textArea, 0.);
        AnchorPane.setRightAnchor(textArea, 0.);
        AnchorPane.setLeftAnchor(textArea, 0.);
    }
}
