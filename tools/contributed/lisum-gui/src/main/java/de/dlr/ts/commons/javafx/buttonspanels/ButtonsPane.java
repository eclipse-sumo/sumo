/*
 * Copyright (C) 2016
 * Deutsches Zentrum fuer Luft- und Raumfahrt e.V.
 * Institut fuer Verkehrssystemtechnik
 * 
 * German Aerospace Center
 * Institute of Transportation Systems
 * 
 */
package de.dlr.ts.commons.javafx.buttonspanels;

import javafx.geometry.Orientation;
import javafx.geometry.Pos;
import javafx.scene.Node;
import javafx.scene.control.Button;
import javafx.scene.control.Separator;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.FlowPane;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class ButtonsPane
{
    protected FlowPane buttonsFlowPane = new FlowPane(Orientation.HORIZONTAL);
    
    protected Separator separator = new Separator(Orientation.HORIZONTAL);
    protected AnchorPane mainAnchorPane = new AnchorPane(separator, buttonsFlowPane);
    {        
        mainAnchorPane.setPrefHeight(40.);
        AnchorPane.setTopAnchor(separator, 0.);
        AnchorPane.setLeftAnchor(separator, 4.);
        AnchorPane.setRightAnchor(separator, 4.);

        //buttonsPane.setPrefHeight(26);
        AnchorPane.setTopAnchor(buttonsFlowPane, 5.);
        AnchorPane.setRightAnchor(buttonsFlowPane, 10.);
        AnchorPane.setLeftAnchor(buttonsFlowPane, 10.);
        AnchorPane.setBottomAnchor(buttonsFlowPane, 5.);
        
        buttonsFlowPane.setAlignment(Pos.CENTER_RIGHT);
        buttonsFlowPane.setHgap(10.);
    }
    
    /**
     * 
     * @return 
     */
    public double getHeight()
    {
        return 40.;
    }
    
    /**
     * 
     * @param button 
     */
    protected final void addButton(Button button)
    {
        addButton(0, button);
    }
    
    /**
     * 
     * @param pos 
     */
    public void setAlignment(Pos pos)
    {
        buttonsFlowPane.setAlignment(pos);
    }
    
    /**
     * 
     * @param index
     * @param button 
     */
    protected final void addButton(int index, Button button)
    {
        buttonsFlowPane.getChildren().add(index, button);
        button.setPrefWidth(80);
        button.setPrefHeight(26);
    }
    
    /**
     * 
     * @return 
     */
    public final Node getNode()
    {
        return mainAnchorPane;
    }
}
