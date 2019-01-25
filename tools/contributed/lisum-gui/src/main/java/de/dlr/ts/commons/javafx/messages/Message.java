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

import javafx.scene.control.Alert;
import javafx.scene.image.Image;
import javafx.stage.Stage;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public abstract class Message
{
    protected static String programName = null;
    protected static Image icon = null;
    protected Alert alert;
    
    
    /**
     * 
     * @param name
     */
    public static void setProgramName(String name)
    {
        Message.programName = name;
    }

    /**
     * 
     */
    protected final void init()
    {
        if(icon != null)
        {
            Stage st = (Stage)alert.getDialogPane().getScene().getWindow();
            st.getIcons().add(Message.icon);
        }
        
        if(programName != null)
            alert.setTitle(programName);
        else
            alert.setTitle("Information");
    }
    
    /**
     * 
     * @param icon 
     */
    public static void setIcon(Image icon)
    {
        Message.icon = icon;
    }
    
    /**
     * 
     */
    public final void show()
    {
        alert.showAndWait();
    }
}
