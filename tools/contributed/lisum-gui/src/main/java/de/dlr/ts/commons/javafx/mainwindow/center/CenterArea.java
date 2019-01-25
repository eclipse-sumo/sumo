/*
 * Copyright (C) 2016
 * Deutsches Zentrum fuer Luft- und Raumfahrt e.V.
 * Institut fuer Verkehrssystemtechnik
 * 
 * German Aerospace Center
 * Institute of Transportation Systems
 * 
 */
package de.dlr.ts.commons.javafx.mainwindow.center;

import de.dlr.ts.commons.javafx.mainwindow.Colors;
import javafx.scene.Node;
import javafx.scene.layout.AnchorPane;


/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class CenterArea
{
    private AnchorPane anchorPane = new AnchorPane();
    
    
    /**
     * 
     */
    public CenterArea()
    {
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
    public void set(Node node)
    {
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
    public Node getNode()
    {
        return anchorPane;
    }
}
