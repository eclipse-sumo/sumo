/*
 * Copyright (C) 2014
 * Deutsches Zentrum fuer Luft- und Raumfahrt e.V.
 * Institut fuer Verkehrssystemtechnik
 * 
 * German Aerospace Center
 * Institute of Transportation Systems
 * 
 */
package de.dlr.ts.commons.javafx.buttons;

//import de.dlr.ts.display.Commons;
import javafx.beans.value.ObservableValue;
import javafx.scene.Node;
import javafx.scene.control.ToggleButton;
import javafx.scene.control.Tooltip;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class CustomToggleButton extends ToggleButton
{

    public CustomToggleButton()
    {
    }

    public CustomToggleButton(String text)
    {
        super(text);
    }

    public CustomToggleButton(String text, Node graphic)
    {
        super(text, graphic);
    }
    
    public CustomToggleButton(Node graphic)
    {
        super("", graphic);
    }
  
    /**
     * 
     * @param imageName
     * @param toolTipText 
     */
    public CustomToggleButton(String imageName, String toolTipText)
    {
       // super("", IconsAndMarkers.instance.getIconImageView(imageName, 16));
        setTooltip(new Tooltip(toolTipText));
    }
    
    
    {
        this.selectedProperty().addListener((ObservableValue<? extends Boolean> observable, Boolean oldValue, Boolean newValue) ->
        {
            /*
            if(newValue)
                getGraphic().setEffect(Commons.instance.toggleButtonEffect);
            else
                getGraphic().setEffect(null);
            */
        });
    }
}
