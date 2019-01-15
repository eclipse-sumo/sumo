/*
 * Copyright (C) 2016
 * Deutsches Zentrum fuer Luft- und Raumfahrt e.V.
 * Institut fuer Verkehrssystemtechnik
 * 
 * German Aerospace Center
 * Institute of Transportation Systems
 * 
 */
package de.dlr.ts.commons.javafx.mainwindow.right;

import javafx.scene.Node;
import javafx.scene.layout.AnchorPane;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class RightArea
{
    private AnchorPane anchorPane = new AnchorPane();
    
    
    /**
     * 
     */
    public RightArea()
    {
        anchorPane.setStyle("-fx-background-color: #989898; -fx-background-color: #989898;");
    }
    
    public void setWidth(int width)
    {
        this.anchorPane.setPrefWidth(width);
    } 
    
    /**
     * 
     * @return 
     */
    public Node getNode()
    {
        return anchorPane;
    }
}
