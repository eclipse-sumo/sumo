/*
 * Copyright (C) 2014
 * Deutsches Zentrum fuer Luft- und Raumfahrt e.V.
 * Institut fuer Verkehrssystemtechnik
 * 
 * German Aerospace Center
 * Institute of Transportation Systems
 * 
 */
package de.dlr.ts.commons.javafx.mainwindow.bottom;

import javafx.geometry.Pos;
import javafx.scene.Node;
import javafx.scene.control.Label;
import javafx.scene.layout.AnchorPane;
import javafx.scene.text.Font;


/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class StatusBar
{    
    private AnchorPane root = new AnchorPane();
    private Label rightLabel = new Label();

    
    
    /**
     * 
     */
    public StatusBar()
    {
        root.setPrefHeight(25);
        //mainPane.setStyle("-fx-border-color: #909090; -fx-border-insets: 2.;");
        //mainPane.setStyle("-fx-border-color: #00FF00;");
        //-fx-background-color: #FFFFFF; 
        
        /**
         * 
         */
        rightLabel.setPrefWidth(150);
        rightLabel.setAlignment(Pos.CENTER_RIGHT);        
        rightLabel.setFont(new Font(14));
        
        AnchorPane.setTopAnchor(rightLabel, 2.);
        AnchorPane.setRightAnchor(rightLabel, 12.);        
        AnchorPane.setBottomAnchor(rightLabel, 2.);
        root.getChildren().add(rightLabel);
        
    }

    /**
     * 
     * @return 
     */
    public Node getNode()
    {
        return root;
    }
    
    /**
     * 
     * @return 
     */
    public Label getRightLabel()
    {
        return rightLabel;
    }
    
}
