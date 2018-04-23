/*
 * Copyright (C) 2016
 * Deutsches Zentrum fuer Luft- und Raumfahrt e.V.
 * Institut fuer Verkehrssystemtechnik
 * 
 * German Aerospace Center
 * Institute of Transportation Systems
 * 
 */
package de.dlr.ts.lisum.gui.misc;

import de.dlr.ts.commons.javafx.windows.AboutWindow;
import de.dlr.ts.lisum.gui.SystemProperties;
import javafx.scene.image.ImageView;
import javafx.stage.Window;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public final class LisumAboutWindow
{
    private final AboutWindow win;
    
    /**
     * 
     * @param owner
     */
    public LisumAboutWindow(Window owner)
    {
        ImageView iv = new ImageView(SystemProperties.getInstance().getMainIcon());
        iv.setScaleX(.8);
        iv.setScaleY(.8);
        
        win = new AboutWindow(owner);
        win.setIcon(iv);
        win.setSystemNameString(SystemProperties.getInstance().getSystemName());
        win.setCopyright("\u00A92017 DLR Institute of Transportation Systems");
        
        StringBuilder sb = new StringBuilder();
        sb.append("Product Version").append("\n");
        sb.append(SystemProperties.getInstance().getSystemName()).append("\n").append("\n");        
        
        sb.append("Contact: Maximiliano Bottazzi (maximiliano.bottazi@dlr.de)").append("\n");
        
        win.setLicense(sb.toString());
    }
    
    /**
     * 
     */
    public void show()
    {
        win.show();
    }
}
