/*
 * Copyright (C) 2016
 * Deutsches Zentrum fuer Luft- und Raumfahrt e.V.
 * Institut fuer Verkehrssystemtechnik
 * 
 * German Aerospace Center
 * Institute of Transportation Systems
 * 
 */
package de.dlr.ts.commons.javafx.mainwindow.top;

import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.scene.control.Button;
import javafx.scene.control.Separator;
import javafx.scene.control.ToolBar;
import javafx.scene.control.Tooltip;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
class TestToolBar extends ToolBar
{
    
    private Button openSimulationButton = new Button("TestButton");
    {
        openSimulationButton.setOnAction(new EventHandler<ActionEvent>()
        {
            @Override
            public void handle(ActionEvent event)
            {
                System.out.println("Mamala");
            }
        });
                
        openSimulationButton.setTooltip(new Tooltip("Open simulation"));
    }

    public TestToolBar()
    {
        getItems().addAll(new Separator(), openSimulationButton);
    }
    
    
}
