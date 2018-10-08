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

import javafx.scene.Node;
import javafx.scene.control.Button;
import javafx.scene.control.Tooltip;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class CustomButton extends Button
{

    public CustomButton()
    {
    }

    public CustomButton(String text)
    {
        super(text);
    }

    public CustomButton(String text, Node graphic)
    {
        super(text, graphic);
    }
    
    public CustomButton(String imageName, String toolTipText)
    {
        //super("", IconsAndMarkers.instance.getIconImageView(imageName, 16));
        setTooltip(new Tooltip(toolTipText));
    }
}
