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
public class OkCancelButtonsPanel extends ButtonsPane
{
    protected Button okButton = new Button("OK");
    protected Button cancelButton = new Button("Cancel");

    
    /**
     * 
     */
    public OkCancelButtonsPanel()
    {
        addButton(cancelButton);
        addButton(okButton);        
    }    
    
    /**
     * 
     * @return 
     */
    public Button getOkButton()
    {
        return okButton;
    }

    /**
     * 
     * @return 
     */
    public Button getCancelButton()
    {
        return cancelButton;
    }
}
