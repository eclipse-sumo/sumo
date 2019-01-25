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

import javafx.scene.control.Menu;
import javafx.scene.control.MenuItem;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyCodeCombination;
import javafx.scene.input.KeyCombination;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class EditMenu extends Menu
{
    private MenuItem cutMenuItem;
    private MenuItem copyMenuItem;
    private MenuItem pasteMenuItem;
    
    
    /**
     * 
     */
    EditMenu()
    {
        this.setText("_Edit");
        this.setAccelerator(KeyCombination.keyCombination("SHORTCUT+E"));
        
        cutMenuItem = new MenuItem("Cut");
        cutMenuItem.setDisable(true);
        cutMenuItem.setAccelerator(new KeyCodeCombination(KeyCode.X, KeyCombination.CONTROL_DOWN));
        
        copyMenuItem = new MenuItem("Copy");
        copyMenuItem.setDisable(true);
        copyMenuItem.setAccelerator(new KeyCodeCombination(KeyCode.C, KeyCombination.CONTROL_DOWN));
        
        pasteMenuItem = new MenuItem("Paste");
        pasteMenuItem.setDisable(true);
        pasteMenuItem.setAccelerator(new KeyCodeCombination(KeyCode.V, KeyCombination.CONTROL_DOWN));
        
        getItems().addAll(cutMenuItem, copyMenuItem, pasteMenuItem);
    }

    public MenuItem getCopyMenuItem()
    {
        return copyMenuItem;
    }

    public MenuItem getCutMenuItem()
    {
        return cutMenuItem;
    }

    public MenuItem getPasteMenuItem()
    {
        return pasteMenuItem;
    }
    
}
