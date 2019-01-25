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

import javafx.scene.control.Button;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class OkApplyCancelButtonsPanel extends OkCancelButtonsPanel
{

    private Button applyButton = new Button("Apply");
    
    /**
     * 
     */
    public OkApplyCancelButtonsPanel()
    {
        addButton(1, applyButton);
    }
    
    /**
     * 
     * @return 
     */
    public Button getApplyButton()
    {
        return applyButton;
    }
}
