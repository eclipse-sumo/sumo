/*
 * Copyright (C) 2016
 * Deutsches Zentrum fuer Luft- und Raumfahrt e.V.
 * Institut fuer Verkehrssystemtechnik
 * 
 * German Aerospace Center
 * Institute of Transportation Systems
 * 
 */
package de.dlr.ts.commons.javafx.messages;

import java.util.Optional;
import javafx.scene.control.Alert;
import javafx.scene.control.ButtonType;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class ConfirmationMessage extends Message
{
    
    public ConfirmationMessage(String text)
    {
        alert = new Alert(Alert.AlertType.CONFIRMATION);
        alert.setHeaderText(null);
        alert.setContentText(text);
        init();
    }
    
    /**
     * 
     * @param header 
     */
    public void setHeader(String header)
    {
        alert.setHeaderText(header);
    }
    
    /**
     * 
     * @return 
     */
    public Optional<ButtonType> showAndWait()
    {
        /*
        if (result.get() == ButtonType.OK){
            // ... user chose OK
        } else {
            // ... user chose CANCEL or closed the dialog
        }
        */
        
        return alert.showAndWait();
    }
}
