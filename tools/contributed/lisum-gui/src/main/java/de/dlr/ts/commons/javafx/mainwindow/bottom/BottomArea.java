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

import javafx.scene.Node;
import javafx.scene.layout.AnchorPane;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class BottomArea
{
    //private DebugScreen debugScreen;
    private StatusBar statusBar = new StatusBar();
    private AnchorPane anchorPane = new AnchorPane();

    
    /**
     * 
     */
    public BottomArea()
    {
        /*
        debugScreen = new DebugScreen();
        debugScreen.init();
        debugScreen.setStyle("-fx-background-color: #989898; -fx-background-color: #989898;");
        
        //centerAnchorPane.getChildren().add(debugScreen);
        //AnchorPane.setTopAnchor(debugScreen, 500.);
        debugScreen.setPrefHeight(100);
        
        AnchorPane.setRightAnchor(debugScreen, 0.);
        AnchorPane.setLeftAnchor(debugScreen, 0.);
        AnchorPane.setBottomAnchor(debugScreen, 27.);
        */
        
        /**
         * 
         */
        anchorPane.getChildren().add(statusBar.getNode());
        
        //AnchorPane.setTopAnchor(bottomBar.getNode(), 0.);
        AnchorPane.setRightAnchor(statusBar.getNode(), 0.);
        AnchorPane.setLeftAnchor(statusBar.getNode(), 0.);
        AnchorPane.setBottomAnchor(statusBar.getNode(), 0.);
    }
    
    /**
     * 
     * @return 
     */
    public Node getNode()
    {
        return anchorPane;
    }

    /**
     * 
     * @return 
     */
    public StatusBar getStatusBar()
    {
        return statusBar;
    }
    
}
