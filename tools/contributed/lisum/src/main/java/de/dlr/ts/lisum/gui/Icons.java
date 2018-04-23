/*
 * Copyright (C) 2014
 * Deutsches Zentrum fuer Luft- und Raumfahrt e.V.
 * Institut fuer Verkehrssystemtechnik
 * 
 * German Aerospace Center
 * Institute of Transportation Systems
 * 
 */
package de.dlr.ts.lisum.gui;

import javafx.scene.image.Image;
import javafx.scene.image.ImageView;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public final class Icons
{
    private static final Icons INSTANCE = new Icons();
    
    /**
     * 
     * @return 
     */
    public static Icons getInstance()
    {
        return INSTANCE;
    }
    
    /**
     * 
     */
    public Icons()
    {
    }
    
    /**
     * 
     * @param name
     * @param size
     * @return 
     */
    public Image getIconImage(String name, int size)
    {
        String _size;
        
        if(size == 16)
            _size = "16x16";
        else
            _size = "24x24";
        
        return new Image(getClass().getResourceAsStream("/images/icons/" + _size + "/" + name + ".png"));
    }
    
    /**
     * 
     */
    public void init()
    {
        
    }
    
    /**
     * 
     * @param name
     * @param size
     * @return 
     */
    public ImageView getIconImageView(String name, int size)
    {
        return new ImageView(getIconImage(name, size));
    }
    
    /**
     * 
     * @param name
     * @param size
     * @param z
     * @return 
     */
    public ImageView getIconImageView(String name, int size, int z )
    {
        ImageView iv = new ImageView(getIconImage(name, size));
        iv.setEffect(null);
        
        return new ImageView(getIconImage(name, size));
    }
    
    /**
     * 
     * @param name 
     * @return  
     */
    public Image getMarkerImage(String name)
    {
        return new Image(getClass().getResourceAsStream("/images/markers/" + name + ".png"));
    }
    
    
    /**
     * 
     * @param name
     * @return 
     */
    public ImageView getMarkerImageView(String name)
    {
        return new ImageView(getMarkerImage(name));
    }
    
}
